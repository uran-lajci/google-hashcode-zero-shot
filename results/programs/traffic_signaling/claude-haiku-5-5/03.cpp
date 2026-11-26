#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> Bv, Ev, Lv;
vector<string> nameV;
vector<vector<int>> path;
vector<int> wcnt;
vector<int> firstArr;

// schedule: for each intersection, list of (street, T)
using Sched = vector<vector<pair<int,int>>>;

long long simulate(const Sched& sch) {
    vector<vector<int>> cumv(I), sidv(I);
    vector<int> Cyc(I, 0);
    for (int X = 0; X < I; X++) {
        int acc = 0;
        for (auto& pr : sch[X]) {
            acc += pr.second;
            cumv[X].push_back(acc);
            sidv[X].push_back(pr.first);
        }
        Cyc[X] = acc;
    }
    auto green = [&](int X, int t) -> int {
        if (cumv[X].empty()) return -1;
        int r = t % Cyc[X];
        int idx = upper_bound(cumv[X].begin(), cumv[X].end(), r) - cumv[X].begin();
        return sidv[X][idx];
    };

    vector<deque<pair<int,int>>> q(S);
    vector<int> pos(V, 0);
    vector<char> inAct(S, 0);
    vector<int> active;
    for (int c = 0; c < V; c++) {
        int s = path[c][0];
        q[s].push_back({c, 0});
        if (!inAct[s]) { inAct[s] = 1; active.push_back(s); }
    }

    long long score = 0;
    vector<int> pending;
    for (int t = 0; t < D; t++) {
        pending.clear();
        for (int s : active) {
            if (q[s].empty()) continue;
            int X = Ev[s];
            if (green(X, t) != s) continue;
            if (q[s].front().second > t) continue;
            int c = q[s].front().first;
            q[s].pop_front();
            int k = pos[c];
            int nxt = path[c][k + 1];
            int arr = t + Lv[nxt];
            if (k + 1 == (int)path[c].size() - 1) {
                if (arr <= D) score += F + (D - arr);
            } else {
                pos[c] = k + 1;
                q[nxt].push_back({c, arr});
                if (!inAct[nxt]) {
                    inAct[nxt] = 1;
                    pending.push_back(nxt);
                }
            }
        }
        vector<int> na;
        na.reserve(active.size() + pending.size());
        for (int s : active) {
            if (q[s].empty()) inAct[s] = 0;
            else na.push_back(s);
        }
        for (int s : pending) {
            if (q[s].empty()) inAct[s] = 0;
            else na.push_back(s);
        }
        active.swap(na);
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> D >> I >> S >> V >> F;
    Bv.resize(S); Ev.resize(S); Lv.resize(S); nameV.resize(S);
    unordered_map<string,int> id;
    id.reserve(S * 2 + 1);
    for (int s = 0; s < S; s++) {
        cin >> Bv[s] >> Ev[s] >> nameV[s] >> Lv[s];
        id[nameV[s]] = s;
    }
    path.assign(V, {});
    for (int c = 0; c < V; c++) {
        int P;
        cin >> P;
        path[c].resize(P);
        for (int k = 0; k < P; k++) {
            string nm;
            cin >> nm;
            path[c][k] = id[nm];
        }
    }

    wcnt.assign(S, 0);
    firstArr.assign(S, INT_MAX);
    for (int c = 0; c < V; c++) {
        long long a = 0;
        int P = path[c].size();
        for (int k = 0; k + 1 < P; k++) {
            int s = path[c][k];
            wcnt[s]++;
            if (a < firstArr[s]) firstArr[s] = (int)min<long long>(a, INT_MAX);
            a += Lv[path[c][k + 1]];
        }
    }

    vector<vector<int>> inc(I);
    for (int s = 0; s < S; s++) {
        if (wcnt[s] > 0) inc[Ev[s]].push_back(s);
    }

    auto build = [&](int orderMode, long long div) -> Sched {
        Sched sch(I);
        for (int X = 0; X < I; X++) {
            if (inc[X].empty()) continue;
            vector<int> v = inc[X];
            if (orderMode == 0) {
                sort(v.begin(), v.end(), [&](int a, int b) {
                    if (firstArr[a] != firstArr[b]) return firstArr[a] < firstArr[b];
                    return a < b;
                });
            } else {
                sort(v.begin(), v.end(), [&](int a, int b) {
                    if (wcnt[a] != wcnt[b]) return wcnt[a] > wcnt[b];
                    return a < b;
                });
            }
            for (int s : v) {
                long long T;
                if (div >= 1000000000LL) T = 1;
                else T = (wcnt[s] + div - 1) / div;
                if (T < 1) T = 1;
                if (T > D) T = D;
                sch[X].push_back({s, (int)T});
            }
        }
        return sch;
    };

    Sched best;
    long long bestScore = -1;
    vector<long long> divs = {1000000000LL, 1, 2, 3, 4, 8, 16, 32};
    for (int orderMode = 0; orderMode < 2; orderMode++) {
        for (long long d : divs) {
            Sched sch = build(orderMode, d);
            long long sc = simulate(sch);
            if (sc > bestScore) {
                bestScore = sc;
                best = sch;
            }
        }
    }

    int cntSched = 0;
    for (int X = 0; X < I; X++) if (!best[X].empty()) cntSched++;

    ostringstream out;
    out << cntSched << "\n";
    for (int X = 0; X < I; X++) {
        if (best[X].empty()) continue;
        out << X << "\n";
        out << best[X].size() << "\n";
        for (auto& pr : best[X]) {
            out << nameV[pr.first] << " " << pr.second << "\n";
        }
    }
    cout << out.str();
    return 0;
}
