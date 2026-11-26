#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startT = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startT).count();
    };

    int C, P;
    cin >> C >> P;

    unordered_map<string, int> sid;
    auto getId = [&](const string& s) {
        auto it = sid.find(s);
        if (it != sid.end()) return it->second;
        int id = (int)sid.size();
        sid[s] = id;
        return id;
    };

    vector<string> cname(C);
    vector<vector<pair<int, int>>> lv(C);
    vector<vector<int>> holders;

    for (int c = 0; c < C; c++) {
        cin >> cname[c];
        int N;
        cin >> N;
        for (int i = 0; i < N; i++) {
            string s;
            int l;
            cin >> s >> l;
            int id = getId(s);
            if ((int)holders.size() <= id) holders.resize(id + 1);
            lv[c].push_back({id, l});
            holders[id].push_back(c);
        }
        sort(lv[c].begin(), lv[c].end());
    }

    vector<string> pname(P);
    vector<int> D(P), S(P), B(P);
    vector<vector<int>> rs(P), rl(P);
    for (int p = 0; p < P; p++) {
        int R;
        cin >> pname[p] >> D[p] >> S[p] >> B[p] >> R;
        rs[p].resize(R);
        rl[p].resize(R);
        for (int r = 0; r < R; r++) {
            string s;
            int l;
            cin >> s >> l;
            int id = getId(s);
            if ((int)holders.size() <= id) holders.resize(id + 1);
            rs[p][r] = id;
            rl[p][r] = l;
        }
    }

    auto getLv = [&](int c, int x) -> int {
        const auto& v = lv[c];
        auto it = lower_bound(v.begin(), v.end(), make_pair(x, INT_MIN));
        if (it != v.end() && it->first == x) return it->second;
        return 0;
    };
    auto addLv = [&](int c, int x) {
        auto& v = lv[c];
        auto it = lower_bound(v.begin(), v.end(), make_pair(x, INT_MIN));
        if (it != v.end() && it->first == x) {
            it->second++;
        } else {
            v.insert(it, {x, 1});
            holders[x].push_back(c);
        }
    };

    vector<int> order(P);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        double da = (double)S[a] / D[a], db = (double)S[b] / D[b];
        if (da != db) return da > db;
        return B[a] < B[b];
    });

    const int CAP = 2000;
    const int EXAM = 500;

    vector<char> isFree(C, 1), inList(C, 1);
    vector<int> freeList(C);
    iota(freeList.begin(), freeList.end(), 0);
    int freeCount = C;

    vector<int> usedSt(C, 0);
    int stampCnt = 0;

    auto tryStaff = [&](int p, vector<int>& asg) -> bool {
        int R = rl[p].size();
        asg.assign(R, -1);
        ++stampCnt;
        // Direct fits
        for (int r = 0; r < R; r++) {
            int X = rs[p][r], L = rl[p][r];
            int best = -1, bl = INT_MAX, scanned = 0;
            for (int c : holders[X]) {
                if (++scanned > CAP) break;
                if (!isFree[c] || usedSt[c] == stampCnt) continue;
                int v = getLv(c, X);
                if (v >= L && v < bl) {
                    best = c;
                    bl = v;
                    if (v == L) break;
                }
            }
            if (best >= 0) {
                asg[r] = best;
                usedSt[best] = stampCnt;
            }
        }
        // Mentored fits
        bool changed = true;
        while (changed) {
            changed = false;
            for (int r = 0; r < R; r++) {
                if (asg[r] >= 0) continue;
                int X = rs[p][r], L = rl[p][r];
                bool hasMentor = false;
                for (int j = 0; j < R; j++) {
                    if (j != r && asg[j] >= 0 && getLv(asg[j], X) >= L) {
                        hasMentor = true;
                        break;
                    }
                }
                if (!hasMentor) continue;
                int cand = -1, scanned = 0;
                if (L - 1 == 0) {
                    for (int c : freeList) {
                        if (++scanned > CAP) break;
                        if (!isFree[c] || usedSt[c] == stampCnt) continue;
                        if (getLv(c, X) == 0) {
                            cand = c;
                            break;
                        }
                    }
                } else {
                    for (int c : holders[X]) {
                        if (++scanned > CAP) break;
                        if (!isFree[c] || usedSt[c] == stampCnt) continue;
                        if (getLv(c, X) == L - 1) {
                            cand = c;
                            break;
                        }
                    }
                }
                if (cand >= 0) {
                    asg[r] = cand;
                    usedSt[cand] = stampCnt;
                    changed = true;
                }
            }
        }
        for (int r = 0; r < R; r++)
            if (asg[r] < 0) return false;
        return true;
    };

    vector<char> started(P, 0);
    vector<vector<int>> projAsg(P);
    vector<int> schedule;
    int deadCount = 0;
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    long long t = 0;

    while (true) {
        if (elapsed() > 8.5) break;

        // Compact freeList
        {
            size_t w = 0;
            for (size_t i = 0; i < freeList.size(); i++) {
                int c = freeList[i];
                if (isFree[c]) freeList[w++] = c;
                else inList[c] = 0;
            }
            freeList.resize(w);
        }

        if (freeCount > 0) {
            int examined = 0;
            vector<int> asg;
            for (int idx = 0; idx < (int)order.size(); idx++) {
                int p = order[idx];
                if (started[p]) continue;
                if (++examined > EXAM) break;
                if (freeCount < (int)rl[p].size()) continue;
                long long end = t + D[p];
                long long val = (long long)S[p] - max(0LL, end - B[p]);
                if (val <= 0) continue;
                if (!tryStaff(p, asg)) continue;

                started[p] = 1;
                deadCount++;
                projAsg[p] = asg;
                int R = rl[p].size();
                for (int r = 0; r < R; r++) {
                    int c = asg[r];
                    isFree[c] = 0;
                    freeCount--;
                }
                for (int r = 0; r < R; r++) {
                    int c = asg[r];
                    int X = rs[p][r], L = rl[p][r];
                    int cur = getLv(c, X);
                    if (L >= cur) addLv(c, X);
                }
                schedule.push_back(p);
                pq.push({end, p});
                if (freeCount == 0) break;
            }
        }

        if (deadCount > (int)order.size() / 2) {
            vector<int> no;
            no.reserve(order.size());
            for (int p : order)
                if (!started[p]) no.push_back(p);
            order.swap(no);
            deadCount = 0;
        }

        if (pq.empty()) break;
        t = pq.top().first;
        while (!pq.empty() && pq.top().first == t) {
            int p = pq.top().second;
            pq.pop();
            for (int c : projAsg[p]) {
                isFree[c] = 1;
                freeCount++;
                if (!inList[c]) {
                    inList[c] = 1;
                    freeList.push_back(c);
                }
            }
        }
    }

    string out;
    out += to_string(schedule.size()) + "\n";
    for (int p : schedule) {
        out += pname[p];
        out += "\n";
        for (size_t r = 0; r < projAsg[p].size(); r++) {
            if (r) out += " ";
            out += cname[projAsg[p][r]];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
