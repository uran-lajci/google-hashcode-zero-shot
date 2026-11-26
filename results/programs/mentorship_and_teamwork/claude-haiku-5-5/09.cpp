#include <bits/stdc++.h>
using namespace std;

struct Proj {
    string name;
    long long D, S, B;
    vector<pair<int,int>> roles; // skill id, level
    vector<int> order;           // role indices sorted by level desc
    double pri;
};

int C, P;
vector<string> cname;
vector<vector<pair<int,int>>> sk;     // per contributor: sorted (skillId, level)
vector<vector<int>> skillUsers;       // per skill: contributors having it (may grow)
vector<Proj> projs;
vector<char> isFree;
vector<int> freeVec;
vector<int> stampArr;
int curStamp = 0;

const int SCAN_CAP = 500;
const int FAIL_CAP = 50;

int getLevel(int c, int X) {
    const auto& v = sk[c];
    auto it = lower_bound(v.begin(), v.end(), make_pair(X, -1));
    if (it != v.end() && it->first == X) return it->second;
    return 0;
}

void bump(int c, int X, int L) {
    auto& v = sk[c];
    auto it = lower_bound(v.begin(), v.end(), make_pair(X, -1));
    if (it != v.end() && it->first == X) {
        if (it->second <= L) it->second++;
    } else {
        // level 0 -> 1 (always <= L since L >= 1)
        v.insert(it, make_pair(X, 1));
        skillUsers[X].push_back(c);
    }
}

bool alive(const Proj& p, long long t) {
    long long lateness = max(0LL, t + p.D - p.B);
    return p.S - lateness > 0;
}

bool staff(int pi, long long t, vector<int>& asg) {
    const Proj& pr = projs[pi];
    int R = pr.roles.size();
    asg.assign(R, -1);
    curStamp++;
    // pass 1: direct assignments
    for (int r : pr.order) {
        int X = pr.roles[r].first, L = pr.roles[r].second;
        int best = -1, bl = INT_MAX, examined = 0;
        for (int c : skillUsers[X]) {
            if (++examined > SCAN_CAP) break;
            if (!isFree[c] || stampArr[c] == curStamp) continue;
            int lev = getLevel(c, X);
            if (lev >= L && lev < bl) {
                bl = lev; best = c;
                if (lev == L) break;
            }
        }
        if (best != -1) {
            asg[r] = best;
            stampArr[best] = curStamp;
        }
    }
    // pass 2: mentees
    for (int r : pr.order) {
        if (asg[r] != -1) continue;
        int X = pr.roles[r].first, L = pr.roles[r].second;
        bool mentor = false;
        for (int r2 = 0; r2 < R; r2++) {
            if (asg[r2] != -1 && getLevel(asg[r2], X) >= L) { mentor = true; break; }
        }
        if (!mentor) return false;
        int pick = -1, examined = 0;
        if (L == 1) {
            for (int c : freeVec) {
                if (++examined > SCAN_CAP) break;
                if (!isFree[c] || stampArr[c] == curStamp) continue;
                if (getLevel(c, X) == 0) { pick = c; break; }
            }
        } else {
            for (int c : skillUsers[X]) {
                if (++examined > SCAN_CAP) break;
                if (!isFree[c] || stampArr[c] == curStamp) continue;
                if (getLevel(c, X) == L - 1) { pick = c; break; }
            }
        }
        if (pick == -1) return false;
        asg[r] = pick;
        stampArr[pick] = curStamp;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startTime = chrono::steady_clock::now();

    cin >> C >> P;
    unordered_map<string,int> skId;
    auto getSk = [&](const string& s) -> int {
        auto it = skId.find(s);
        if (it != skId.end()) return it->second;
        int id = skId.size();
        skId.emplace(s, id);
        skillUsers.emplace_back();
        return id;
    };

    cname.resize(C);
    sk.resize(C);
    for (int c = 0; c < C; c++) {
        int N;
        cin >> cname[c] >> N;
        for (int i = 0; i < N; i++) {
            string s; int L;
            cin >> s >> L;
            int id = getSk(s);
            sk[c].push_back({id, L});
            skillUsers[id].push_back(c);
        }
        sort(sk[c].begin(), sk[c].end());
    }

    projs.resize(P);
    for (int p = 0; p < P; p++) {
        Proj& pr = projs[p];
        int R;
        cin >> pr.name >> pr.D >> pr.S >> pr.B >> R;
        pr.roles.resize(R);
        for (int r = 0; r < R; r++) {
            string s; int L;
            cin >> s >> L;
            pr.roles[r] = {getSk(s), L};
        }
        pr.order.resize(R);
        for (int r = 0; r < R; r++) pr.order[r] = r;
        stable_sort(pr.order.begin(), pr.order.end(), [&](int a, int b) {
            return pr.roles[a].second > pr.roles[b].second;
        });
        pr.pri = (double)pr.S / (double)(pr.D * R);
    }

    vector<int> ord(P);
    iota(ord.begin(), ord.end(), 0);
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) {
        return projs[a].pri > projs[b].pri;
    });

    isFree.assign(C, 1);
    freeVec.resize(C);
    iota(freeVec.begin(), freeVec.end(), 0);
    stampArr.assign(C, 0);

    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> busy;
    vector<char> done(P, 0);
    int doneCount = 0;
    vector<int> rem = ord;
    vector<pair<int, vector<int>>> results;
    long long total = 0;
    long long t = 0;
    vector<int> asg;

    while (true) {
        double elapsed = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
        if (elapsed > 8.0) break;

        // attempt staffing at time t
        int fails = 0;
        for (size_t i = 0; i < rem.size() && fails < FAIL_CAP; i++) {
            int p = rem[i];
            if (done[p]) continue;
            if (!alive(projs[p], t)) { done[p] = 1; doneCount++; continue; }
            if (staff(p, t, asg)) {
                const Proj& pr = projs[p];
                int R = pr.roles.size();
                for (int r = 0; r < R; r++) {
                    int c = asg[r];
                    bump(c, pr.roles[r].first, pr.roles[r].second);
                    isFree[c] = 0;
                    busy.push({t + pr.D, c});
                }
                total += pr.S - max(0LL, t + pr.D - pr.B);
                done[p] = 1;
                doneCount++;
                results.push_back({p, asg});
            } else {
                fails++;
            }
        }
        if (doneCount > (int)rem.size() / 2) {
            vector<int> nr;
            nr.reserve(rem.size() - doneCount);
            for (int p : rem) if (!done[p]) nr.push_back(p);
            rem.swap(nr);
            doneCount = 0;
        }
        if ((int)rem.size() == 0) break;

        // compact free list
        {
            vector<int> nf;
            nf.reserve(freeVec.size());
            for (int c : freeVec) if (isFree[c]) nf.push_back(c);
            freeVec.swap(nf);
        }

        // advance time
        if (busy.empty()) break;
        t = busy.top().first;
        while (!busy.empty() && busy.top().first <= t) {
            int c = busy.top().second;
            busy.pop();
            isFree[c] = 1;
            freeVec.push_back(c);
        }
    }

    // output
    string out;
    out += to_string(results.size()) + "\n";
    for (auto& rs : results) {
        const Proj& pr = projs[rs.first];
        out += pr.name;
        out += "\n";
        for (size_t r = 0; r < rs.second.size(); r++) {
            if (r) out += " ";
            out += cname[rs.second[r]];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
