#include <bits/stdc++.h>
using namespace std;

struct Contrib {
    string name;
    vector<pair<int,int>> sk; // (skillId, level), sorted by skillId
};

struct Proj {
    string name;
    int D, S, B, R;
    vector<pair<int,int>> roles; // (skillId, level)
};

struct Result {
    long long score = 0;
    vector<pair<int, vector<int>>> sched;
};

static chrono::steady_clock::time_point gStart;

double elapsedSec() {
    return chrono::duration<double>(chrono::steady_clock::now() - gStart).count();
}

Result runSim(const vector<Contrib>& C, const vector<Proj>& P, const vector<int>& ord,
              int K, double deadlineSec) {
    int n = C.size();
    vector<vector<pair<int,int>>> cur(n);
    for (int i = 0; i < n; i++) cur[i] = C[i].sk;

    auto getLevel = [&](int c, int k) -> int {
        const auto& v = cur[c];
        auto it = lower_bound(v.begin(), v.end(), make_pair(k, INT_MIN));
        if (it != v.end() && it->first == k) return it->second;
        return 0;
    };
    auto setLevelPlusOne = [&](int c, int k, int lvl) {
        auto& v = cur[c];
        auto it = lower_bound(v.begin(), v.end(), make_pair(k, INT_MIN));
        if (it != v.end() && it->first == k) it->second = lvl + 1;
        else v.insert(it, make_pair(k, lvl + 1));
    };

    vector<set<pair<int,int>>> bySkill(K); // (level, contributor)
    set<int> avail;
    auto insertAvail = [&](int c) {
        avail.insert(c);
        for (auto& [k, l] : cur[c]) bySkill[k].insert({l, c});
    };
    auto removeAvail = [&](int c) {
        avail.erase(c);
        for (auto& [k, l] : cur[c]) bySkill[k].erase({l, c});
    };

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> rel;
    for (int c = 0; c < n; c++) insertAvail(c);

    vector<int> taken;
    auto isTaken = [&](int c) {
        return find(taken.begin(), taken.end(), c) != taken.end();
    };

    // Tries to staff project p with currently available contributors.
    auto staff = [&](int p, vector<int>& assign) -> bool {
        const Proj& pr = P[p];
        int R = pr.R;
        assign.assign(R, -1);
        taken.clear();
        vector<int> rord(R);
        iota(rord.begin(), rord.end(), 0);
        sort(rord.begin(), rord.end(), [&](int a, int b) {
            if (pr.roles[a].second != pr.roles[b].second)
                return pr.roles[a].second > pr.roles[b].second;
            return a < b;
        });
        // Pass A: fully qualified roles, best fit.
        for (int r : rord) {
            int k = pr.roles[r].first, L = pr.roles[r].second;
            auto it = bySkill[k].lower_bound({L, INT_MIN});
            for (; it != bySkill[k].end(); ++it) {
                if (!isTaken(it->second)) {
                    assign[r] = it->second;
                    taken.push_back(it->second);
                    break;
                }
            }
        }
        // Pass B: mentored roles (level exactly L-1, with a mentor of level >= L).
        for (int r : rord) {
            if (assign[r] != -1) continue;
            int k = pr.roles[r].first, L = pr.roles[r].second;
            bool hasMentor = false;
            for (int c : taken) {
                if (getLevel(c, k) >= L) { hasMentor = true; break; }
            }
            if (!hasMentor) continue;
            int cand = -1;
            if (L - 1 == 0) {
                int scanned = 0;
                for (int c : avail) {
                    if (++scanned > 2000) break;
                    if (!isTaken(c) && getLevel(c, k) == 0) { cand = c; break; }
                }
            } else {
                auto it = bySkill[k].lower_bound({L - 1, INT_MIN});
                for (; it != bySkill[k].end() && it->first == L - 1; ++it) {
                    if (!isTaken(it->second)) { cand = it->second; break; }
                }
            }
            if (cand >= 0) {
                assign[r] = cand;
                taken.push_back(cand);
            }
        }
        for (int r = 0; r < R; r++) if (assign[r] == -1) return false;
        return true;
    };

    Result res;
    vector<int> active = ord;
    long long t = 0;
    vector<int> assign;

    while (true) {
        while (!rel.empty() && rel.top().first <= t) {
            insertAvail(rel.top().second);
            rel.pop();
        }
        vector<int> next;
        next.reserve(active.size());
        size_t i = 0;
        for (; i < active.size(); ++i) {
            if (avail.empty()) break;
            int p = active[i];
            const Proj& pr = P[p];
            long long pen = max(0LL, t + pr.D - pr.B);
            long long val = max(0LL, pr.S - pen);
            if (val <= 0) continue; // penalty only grows with time: drop forever
            if (staff(p, assign)) {
                // commit
                for (int c : assign) removeAvail(c);
                for (int r = 0; r < pr.R; r++) {
                    int c = assign[r];
                    int k = pr.roles[r].first, L = pr.roles[r].second;
                    int lvl = getLevel(c, k);
                    if (L >= lvl) setLevelPlusOne(c, k, lvl);
                }
                for (int c : assign) rel.push({(int)(t + pr.D), c});
                res.score += val;
                res.sched.push_back({p, assign});
            } else {
                next.push_back(p);
            }
        }
        for (; i < active.size(); ++i) next.push_back(active[i]);
        active.swap(next);

        if (rel.empty() || active.empty()) break;
        if (elapsedSec() > deadlineSec) break;
        t = rel.top().first;
    }
    return res;
}

int main() {
    gStart = chrono::steady_clock::now();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, P;
    cin >> C >> P;
    unordered_map<string, int> skillId;
    auto getId = [&](const string& s) {
        auto it = skillId.find(s);
        if (it != skillId.end()) return it->second;
        int id = skillId.size();
        skillId[s] = id;
        return id;
    };

    vector<Contrib> contribs(C);
    for (int i = 0; i < C; i++) {
        int N;
        cin >> contribs[i].name >> N;
        for (int j = 0; j < N; j++) {
            string s; int l;
            cin >> s >> l;
            contribs[i].sk.push_back({getId(s), l});
        }
        sort(contribs[i].sk.begin(), contribs[i].sk.end());
    }

    vector<Proj> projs(P);
    for (int i = 0; i < P; i++) {
        Proj& pr = projs[i];
        cin >> pr.name >> pr.D >> pr.S >> pr.B >> pr.R;
        pr.roles.resize(pr.R);
        for (int r = 0; r < pr.R; r++) {
            string s; int l;
            cin >> s >> l;
            pr.roles[r] = {getId(s), l};
        }
    }

    int K = max(1, (int)skillId.size());

    // Three priority orderings.
    vector<vector<int>> orders(3, vector<int>(P));
    for (int v = 0; v < 3; v++) iota(orders[v].begin(), orders[v].end(), 0);
    vector<double> k0(P), k1(P), k2(P);
    for (int i = 0; i < P; i++) {
        k0[i] = (double)projs[i].S / ((double)projs[i].D * projs[i].R);
        k1[i] = (double)projs[i].S;
        k2[i] = (double)projs[i].S / projs[i].D;
    }
    auto makeCmp = [&](const vector<double>& key) {
        return [&key](int a, int b) {
            if (key[a] != key[b]) return key[a] > key[b];
            return a < b;
        };
    };
    sort(orders[0].begin(), orders[0].end(), makeCmp(k0));
    sort(orders[1].begin(), orders[1].end(), makeCmp(k1));
    sort(orders[2].begin(), orders[2].end(), makeCmp(k2));

    Result best;
    bool haveBest = false;
    for (int v = 0; v < 3; v++) {
        double deadline = 2.5 * (v + 1);
        Result r = runSim(contribs, projs, orders[v], K, deadline);
        if (!haveBest || r.score > best.score) {
            best = std::move(r);
            haveBest = true;
        }
    }

    string out;
    out += to_string(best.sched.size()) + "\n";
    for (auto& [p, assign] : best.sched) {
        out += projs[p].name + "\n";
        for (size_t r = 0; r < assign.size(); r++) {
            if (r) out += " ";
            out += contribs[assign[r]].name;
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
