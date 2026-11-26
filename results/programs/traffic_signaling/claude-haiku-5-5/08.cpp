#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> sB, sE, sL;
vector<string> sName;
vector<vector<int>> paths;
vector<long long> cnt, firstArr;
vector<int> off_, dur_, cyc_, endI;

// Exact simulation of the statement's rules with the current off_/dur_/cyc_.
long long simulate() {
    long long score = 0;
    vector<vector<int>> q(S);
    vector<int> head(S, 0);
    vector<char> inAct(S, 0);
    vector<int> act;
    vector<int> pos(V, 0);
    vector<vector<pair<int, int>>> arr(D + 1);  // (car, index k: car at end of paths[c][k])

    for (int c = 0; c < V; c++) {
        int s = paths[c][0];
        q[s].push_back(c);
        if (!inAct[s]) { inAct[s] = 1; act.push_back(s); }
    }

    for (int t = 0; t <= D; t++) {
        // 1) arrivals at end of streets at time t
        for (auto &pr : arr[t]) {
            int c = pr.first, k = pr.second;
            pos[c] = k;
            if (k == (int)paths[c].size() - 1) {
                score += F + (D - t);
            } else {
                int s = paths[c][k];
                q[s].push_back(c);
                if (!inAct[s]) { inAct[s] = 1; act.push_back(s); }
            }
        }
        arr[t].clear();
        arr[t].shrink_to_fit();

        // 2) crossings at green lights
        for (int s : act) {
            if (head[s] >= (int)q[s].size()) continue;
            int E = endI[s];
            if (cyc_[E] == 0 || dur_[s] == 0) continue;
            int p = t % cyc_[E];
            if (p >= off_[s] && p < off_[s] + dur_[s]) {
                int c = q[s][head[s]++];
                int k = pos[c] + 1;
                int ns = paths[c][k];
                long long ta = (long long)t + sL[ns];
                if (ta <= D) arr[ta].push_back({c, k});
            }
        }

        // 3) compact active list
        size_t w = 0;
        for (int s : act) {
            if (head[s] < (int)q[s].size()) act[w++] = s;
            else inAct[s] = 0;
        }
        act.resize(w);
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> D >> I >> S >> V >> F;
    sB.resize(S); sE.resize(S); sL.resize(S); sName.resize(S);
    endI.resize(S);
    unordered_map<string, int> id;
    id.reserve(S * 2);
    for (int s = 0; s < S; s++) {
        cin >> sB[s] >> sE[s] >> sName[s] >> sL[s];
        endI[s] = sE[s];
        id[sName[s]] = s;
    }
    paths.resize(V);
    for (int c = 0; c < V; c++) {
        int P;
        cin >> P;
        paths[c].resize(P);
        for (int k = 0; k < P; k++) {
            string nm;
            cin >> nm;
            paths[c][k] = id[nm];
        }
    }

    cnt.assign(S, 0);
    firstArr.assign(S, LLONG_MAX / 4);
    for (int c = 0; c < V; c++) {
        int P = paths[c].size();
        long long time = 0;
        for (int k = 0; k + 1 < P; k++) {
            int s = paths[c][k];
            cnt[s]++;
            firstArr[s] = min(firstArr[s], time);
            time += sL[paths[c][k + 1]];
        }
    }

    vector<vector<int>> byInt(I);
    for (int s = 0; s < S; s++)
        if (cnt[s] > 0) byInt[sE[s]].push_back(s);

    off_.assign(S, 0);
    dur_.assign(S, 0);
    cyc_.assign(I, 0);

    auto build = [&](int mode, double scale) {
        fill(dur_.begin(), dur_.end(), 0);
        fill(off_.begin(), off_.end(), 0);
        fill(cyc_.begin(), cyc_.end(), 0);
        vector<vector<int>> orders(I);
        for (int i = 0; i < I; i++) {
            vector<int> v = byInt[i];
            if (v.empty()) continue;
            if (mode == 0)
                sort(v.begin(), v.end(), [&](int a, int b) {
                    if (firstArr[a] != firstArr[b]) return firstArr[a] < firstArr[b];
                    return a < b;
                });
            else
                sort(v.begin(), v.end(), [&](int a, int b) {
                    if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
                    return a < b;
                });
            int cur = 0;
            for (int s : v) {
                long long T = llround(scale * (double)cnt[s]);
                if (T < 1) T = 1;
                if (T > D) T = D;
                dur_[s] = (int)T;
                off_[s] = cur;
                cur += (int)T;
            }
            cyc_[i] = cur;
            orders[i] = v;
        }
        return orders;
    };

    long long bestScore = -1;
    vector<vector<int>> bestOrders;
    vector<int> bestDur;

    const double scales[] = {1.0, 0.5, 0.25, 2.0, 4.0, 0.125};
    for (int mode = 0; mode < 2; mode++) {
        for (double sc : scales) {
            vector<vector<int>> orders = build(mode, sc);
            long long sc_val = simulate();
            if (sc_val > bestScore) {
                bestScore = sc_val;
                bestOrders = orders;
                bestDur = dur_;
            }
        }
    }

    // Output best submission
    int A = 0;
    for (int i = 0; i < I; i++)
        if (!bestOrders[i].empty()) A++;

    string out;
    out += to_string(A) + "\n";
    for (int i = 0; i < I; i++) {
        if (bestOrders[i].empty()) continue;
        out += to_string(i) + "\n";
        out += to_string(bestOrders[i].size()) + "\n";
        for (int s : bestOrders[i]) {
            out += sName[s];
            out += ' ';
            out += to_string(bestDur[s]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
