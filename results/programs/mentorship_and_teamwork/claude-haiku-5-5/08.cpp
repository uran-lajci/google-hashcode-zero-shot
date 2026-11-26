#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Con {
    string name;
    vector<pair<int,int>> sk; // (skillId, level)
};

struct Proj {
    string name;
    ll D, S, B;
    int R;
    vector<int> sk, lv;
};

typedef vector<pair<int, vector<int>>> Submission;

pair<ll, Submission> runOrder(const vector<int>& order,
                              const vector<Con>& cons,
                              const vector<Proj>& prj,
                              int numSkills) {
    int C = cons.size();
    vector<unordered_map<int,int>> lvl(C);
    vector<vector<int>> holders(numSkills);
    for (int c = 0; c < C; c++) {
        for (auto &pr : cons[c].sk) {
            lvl[c][pr.first] = pr.second;
            if (pr.second >= 1) holders[pr.first].push_back(c);
        }
    }
    vector<ll> freeT(C, 0);
    set<pair<ll,int>> byFree;
    for (int c = 0; c < C; c++) byFree.insert({0LL, c});

    vector<int> used(C, 0);
    int stamp = 0;
    ll total = 0;
    Submission sub;

    auto getLevel = [&](int c, int s) -> int {
        auto it = lvl[c].find(s);
        return it == lvl[c].end() ? 0 : it->second;
    };

    for (int p : order) {
        const Proj &pr = prj[p];
        int R = pr.R;
        stamp++;
        vector<int> idx(R);
        iota(idx.begin(), idx.end(), 0);
        stable_sort(idx.begin(), idx.end(), [&](int a, int b) {
            return pr.lv[a] > pr.lv[b];
        });
        vector<int> asg(R, -1);
        vector<char> direct(R, 0);
        vector<int> pend;

        // Phase 1: direct qualified roles
        for (int r : idx) {
            int s = pr.sk[r], L = pr.lv[r];
            int best = -1;
            ll bf = LLONG_MAX;
            int bl = INT_MAX;
            for (int c : holders[s]) {
                if (used[c] == stamp) continue;
                int lv = getLevel(c, s);
                if (lv < L) continue;
                if (freeT[c] < bf || (freeT[c] == bf && lv < bl)) {
                    bf = freeT[c];
                    bl = lv;
                    best = c;
                }
            }
            if (best == -1) {
                pend.push_back(r);
            } else {
                asg[r] = best;
                direct[r] = 1;
                used[best] = stamp;
            }
        }

        // Phase 2: mentees at exactly one level below
        bool ok = true;
        for (int r : pend) {
            int s = pr.sk[r], L = pr.lv[r];
            bool has = false;
            for (int j = 0; j < R && !has; j++) {
                if (direct[j] && pr.sk[j] == s && getLevel(asg[j], s) >= L)
                    has = true;
            }
            if (!has) { ok = false; break; }

            int best = -1;
            if (L == 1) {
                for (auto &it : byFree) {
                    int c = it.second;
                    if (used[c] == stamp) continue;
                    if (getLevel(c, s) == 0) { best = c; break; }
                }
            } else {
                ll bf = LLONG_MAX;
                for (int c : holders[s]) {
                    if (used[c] == stamp) continue;
                    if (getLevel(c, s) == L - 1 && freeT[c] < bf) {
                        bf = freeT[c];
                        best = c;
                    }
                }
            }
            if (best == -1) { ok = false; break; }
            asg[r] = best;
            used[best] = stamp;
        }
        if (!ok) continue;

        ll start = 0;
        for (int r = 0; r < R; r++) start = max(start, freeT[asg[r]]);
        ll end = start + pr.D;
        ll late = max(0LL, end - pr.B);
        ll score = pr.S - late;
        if (score <= 0) continue;

        // Commit
        for (int r = 0; r < R; r++) {
            int c = asg[r];
            int s = pr.sk[r], L = pr.lv[r];
            int lv = getLevel(c, s);
            if (lv <= L) {
                if (lv == 0) holders[s].push_back(c);
                lvl[c][s] = lv + 1;
            }
            byFree.erase({freeT[c], c});
            freeT[c] = end;
            byFree.insert({end, c});
        }
        total += score;
        sub.push_back({p, asg});
    }
    return {total, sub};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, P;
    cin >> C >> P;
    unordered_map<string,int> skillId;
    auto getSkill = [&](const string &name) -> int {
        auto it = skillId.find(name);
        if (it != skillId.end()) return it->second;
        int id = skillId.size();
        skillId[name] = id;
        return id;
    };

    vector<Con> cons(C);
    for (int i = 0; i < C; i++) {
        int N;
        cin >> cons[i].name >> N;
        for (int k = 0; k < N; k++) {
            string sn; int l;
            cin >> sn >> l;
            cons[i].sk.push_back({getSkill(sn), l});
        }
    }

    vector<Proj> prj(P);
    for (int i = 0; i < P; i++) {
        int R;
        cin >> prj[i].name >> prj[i].D >> prj[i].S >> prj[i].B >> R;
        prj[i].R = R;
        prj[i].sk.resize(R);
        prj[i].lv.resize(R);
        for (int k = 0; k < R; k++) {
            string sn; int l;
            cin >> sn >> l;
            prj[i].sk[k] = getSkill(sn);
            prj[i].lv[k] = l;
        }
    }
    int numSkills = skillId.size();

    // Three orderings
    vector<vector<int>> orders(3);
    for (int i = 0; i < P; i++) {
        orders[0].push_back(i);
        orders[1].push_back(i);
        orders[2].push_back(i);
    }
    sort(orders[0].begin(), orders[0].end(), [&](int a, int b) {
        ll l = prj[a].S * prj[b].D, r = prj[b].S * prj[a].D;
        if (l != r) return l > r;
        return a < b;
    });
    sort(orders[1].begin(), orders[1].end(), [&](int a, int b) {
        if (prj[a].S != prj[b].S) return prj[a].S > prj[b].S;
        return a < b;
    });
    sort(orders[2].begin(), orders[2].end(), [&](int a, int b) {
        if (prj[a].B != prj[b].B) return prj[a].B < prj[b].B;
        return a < b;
    });

    ll bestScore = -1;
    Submission bestSub;
    for (auto &ord : orders) {
        auto res = runOrder(ord, cons, prj, numSkills);
        if (res.first > bestScore) {
            bestScore = res.first;
            bestSub = res.second;
        }
    }

    cout << bestSub.size() << "\n";
    for (auto &entry : bestSub) {
        int p = entry.first;
        cout << prj[p].name << "\n";
        const vector<int> &asg = entry.second;
        for (size_t r = 0; r < asg.size(); r++) {
            if (r) cout << " ";
            cout << cons[asg[r]].name;
        }
        cout << "\n";
    }
    return 0;
}
