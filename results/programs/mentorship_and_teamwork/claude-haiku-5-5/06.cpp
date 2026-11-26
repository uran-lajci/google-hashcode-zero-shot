#include <bits/stdc++.h>
using namespace std;

int C, P;
vector<string> cname, pname;
vector<long long> Dd, Ss, Bb;
vector<vector<pair<int,int>>> roles; // (skill id, level)
vector<unordered_map<int,int>> origLv;
vector<vector<int>> origHold;

struct Result {
    long long score = 0;
    vector<pair<int, vector<int>>> sched; // project index, contributor per role
};

static int getLv(const unordered_map<int,int>& m, int s) {
    auto it = m.find(s);
    return it == m.end() ? 0 : it->second;
}

Result runSim(const vector<int>& order, chrono::steady_clock::time_point deadline) {
    vector<unordered_map<int,int>> lv = origLv;
    vector<vector<int>> hold = origHold;
    vector<long long> freeAt(C, 0);
    vector<int> stamp(C, -1);
    int trial = 0;
    vector<char> started(P, 0);
    priority_queue<long long, vector<long long>, greater<long long>> pq;
    Result res;
    long long t = 0;
    bool timeUp = false;

    while (!timeUp) {
        if (chrono::steady_clock::now() > deadline) break;
        int freeCount = 0;
        for (int c = 0; c < C; c++) if (freeAt[c] <= t) freeCount++;

        for (int p : order) {
            if (chrono::steady_clock::now() > deadline) { timeUp = true; break; }
            if (started[p]) continue;
            int R = (int)roles[p].size();
            if (R > freeCount) continue;
            long long fin = t + Dd[p] - 1;
            long long pen = max(0LL, fin + 1 - Bb[p]);
            if (Ss[p] - pen <= 0) continue;

            trial++;
            vector<int> who(R, -1);
            vector<int> ord(R);
            for (int i = 0; i < R; i++) ord[i] = i;
            sort(ord.begin(), ord.end(), [&](int a, int b) {
                return roles[p][a].second > roles[p][b].second;
            });

            // Pass 1: direct fits (skill level >= required), lowest sufficient level.
            for (int r : ord) {
                int s = roles[p][r].first, L = roles[p][r].second;
                int best = -1, bl = INT_MAX;
                for (int c : hold[s]) {
                    if (freeAt[c] > t || stamp[c] == trial) continue;
                    int l = getLv(lv[c], s);
                    if (l >= L && l < bl) { bl = l; best = c; }
                }
                if (best >= 0) { who[r] = best; stamp[best] = trial; }
            }

            // Pass 2: mentored roles (skill exactly one level below, mentor present).
            bool ok = true;
            for (int r : ord) {
                if (who[r] >= 0) continue;
                int s = roles[p][r].first, L = roles[p][r].second;
                bool mentor = false;
                for (int r2 = 0; r2 < R && !mentor; r2++) {
                    if (who[r2] >= 0 && getLv(lv[who[r2]], s) >= L) mentor = true;
                }
                if (!mentor) { ok = false; break; }
                int cand = -1;
                if (L - 1 == 0) {
                    for (int c = 0; c < C; c++) {
                        if (freeAt[c] > t || stamp[c] == trial) continue;
                        if (lv[c].find(s) != lv[c].end()) continue;
                        cand = c; break;
                    }
                } else {
                    for (int c : hold[s]) {
                        if (freeAt[c] > t || stamp[c] == trial) continue;
                        if (getLv(lv[c], s) == L - 1) { cand = c; break; }
                    }
                }
                if (cand < 0) { ok = false; break; }
                who[r] = cand; stamp[cand] = trial;
            }
            if (!ok) continue;

            // Commit.
            started[p] = 1;
            res.score += Ss[p] - pen;
            freeCount -= R;
            for (int r = 0; r < R; r++) {
                int c = who[r];
                int s = roles[p][r].first, L = roles[p][r].second;
                auto it = lv[c].find(s);
                int cur = (it == lv[c].end()) ? 0 : it->second;
                if (L >= cur) {
                    if (it == lv[c].end()) {
                        lv[c][s] = cur + 1;
                        hold[s].push_back(c);
                    } else {
                        it->second = cur + 1;
                    }
                }
                freeAt[c] = t + Dd[p];
            }
            pq.push(t + Dd[p]);
            res.sched.push_back({p, who});
        }
        if (timeUp) break;
        while (!pq.empty() && pq.top() <= t) pq.pop();
        if (pq.empty()) break;
        t = pq.top();
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> C >> P;
    cname.resize(C);
    origLv.assign(C, {});
    origHold.clear();
    unordered_map<string,int> sid;
    auto getId = [&](const string& name) {
        auto it = sid.find(name);
        if (it != sid.end()) return it->second;
        int id = (int)sid.size();
        sid[name] = id;
        return id;
    };
    for (int c = 0; c < C; c++) {
        int n;
        cin >> cname[c] >> n;
        for (int i = 0; i < n; i++) {
            string sk; int l;
            cin >> sk >> l;
            int s = getId(sk);
            origLv[c][s] = l;
        }
    }
    int numSkills = (int)sid.size();
    origHold.assign(numSkills, {});
    for (int c = 0; c < C; c++)
        for (auto& kv : origLv[c]) origHold[kv.first].push_back(c);

    pname.resize(P);
    Dd.resize(P); Ss.resize(P); Bb.resize(P);
    roles.resize(P);
    for (int p = 0; p < P; p++) {
        int R;
        cin >> pname[p] >> Dd[p] >> Ss[p] >> Bb[p] >> R;
        roles[p].resize(R);
        for (int r = 0; r < R; r++) {
            string sk; int l;
            cin >> sk >> l;
            roles[p][r] = {getId(sk), l};
        }
    }
    // Skills may appear only in projects; make sure hold has room.
    if ((int)origHold.size() < (int)sid.size()) origHold.resize(sid.size());

    auto start = chrono::steady_clock::now();
    auto dl1 = start + chrono::milliseconds(3500);
    auto dl2 = start + chrono::milliseconds(7000);

    // Order 1: score per day, descending.
    vector<int> ord1(P);
    for (int i = 0; i < P; i++) ord1[i] = i;
    sort(ord1.begin(), ord1.end(), [&](int a, int b) {
        long long l = Ss[a] * Dd[b], r = Ss[b] * Dd[a];
        if (l != r) return l > r;
        if (Bb[a] != Bb[b]) return Bb[a] < Bb[b];
        return a < b;
    });
    // Order 2: raw score, descending.
    vector<int> ord2(P);
    for (int i = 0; i < P; i++) ord2[i] = i;
    sort(ord2.begin(), ord2.end(), [&](int a, int b) {
        if (Ss[a] != Ss[b]) return Ss[a] > Ss[b];
        if (Dd[a] != Dd[b]) return Dd[a] < Dd[b];
        return a < b;
    });

    Result best = runSim(ord1, dl1);
    Result r2 = runSim(ord2, dl2);
    if (r2.score > best.score) best = r2;

    cout << best.sched.size() << "\n";
    for (auto& e : best.sched) {
        int p = e.first;
        cout << pname[p] << "\n";
        for (size_t r = 0; r < e.second.size(); r++) {
            if (r) cout << " ";
            cout << cname[e.second[r]];
        }
        cout << "\n";
    }
    return 0;
}
