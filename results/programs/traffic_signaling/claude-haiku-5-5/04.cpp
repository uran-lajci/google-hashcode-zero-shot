#include <bits/stdc++.h>
using namespace std;

int D, I, S, V, F;
vector<int> Bs, Es, Ls;
vector<string> names;
vector<vector<int>> paths;

// Per-intersection compiled schedule for fast green-light lookup.
struct Compiled {
    vector<vector<int>> order;        // street ids in cycle order
    vector<vector<long long>> cum;    // cumulative inclusive end times
    vector<long long> cyc;            // cycle length
};

Compiled compile(const vector<vector<pair<int,int>>>& sched) {
    Compiled c;
    c.order.assign(I, {});
    c.cum.assign(I, {});
    c.cyc.assign(I, 0);
    for (int i = 0; i < I; i++) {
        long long acc = 0;
        for (auto& pr : sched[i]) {
            acc += pr.second;
            c.order[i].push_back(pr.first);
            c.cum[i].push_back(acc);
        }
        c.cyc[i] = acc;
    }
    return c;
}

inline int greenStreet(const Compiled& c, int i, int t) {
    if (c.cum[i].empty()) return -1;
    long long off = t % c.cyc[i];
    int k = upper_bound(c.cum[i].begin(), c.cum[i].end(), off) - c.cum[i].begin();
    return c.order[i][k];
}

// Simulates the whole city and returns the total score.
long long simulate(const vector<vector<pair<int,int>>>& sched) {
    Compiled cp = compile(sched);
    int nc = paths.size();
    vector<int> pos(nc, 0);
    vector<vector<int>> arr(D + 1);
    for (int c = 0; c < nc; c++) arr[0].push_back(c);

    vector<vector<int>> q(S);
    vector<int> head(S, 0);
    vector<int> qsz(I, 0);
    vector<char> inAct(I, 0);
    vector<int> act;
    long long score = 0;

    for (int t = 0; t <= D; t++) {
        vector<int> cur;
        cur.swap(arr[t]);
        for (int c : cur) {
            int s = paths[c][pos[c]];
            if (pos[c] == (int)paths[c].size() - 1) {
                score += F + (D - t);
                continue;
            }
            q[s].push_back(c);
            int i = Es[s];
            qsz[i]++;
            if (!inAct[i]) { inAct[i] = 1; act.push_back(i); }
        }
        size_t w = 0;
        for (size_t idx = 0; idx < act.size(); idx++) {
            int i = act[idx];
            int g = greenStreet(cp, i, t);
            if (g >= 0 && head[g] < (int)q[g].size()) {
                int c = q[g][head[g]++];
                qsz[i]--;
                pos[c]++;
                int ns = paths[c][pos[c]];
                long long ta = (long long)t + Ls[ns];
                if (ta <= D) arr[ta].push_back(c);
            }
            if (qsz[i] > 0) act[w++] = i;
            else inAct[i] = 0;
        }
        act.resize(w);
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> D >> I >> S >> V >> F;
    Bs.resize(S); Es.resize(S); Ls.resize(S); names.resize(S);
    unordered_map<string, int> id;
    id.reserve(S * 2);
    for (int s = 0; s < S; s++) {
        cin >> Bs[s] >> Es[s] >> names[s] >> Ls[s];
        id[names[s]] = s;
    }
    paths.resize(V);
    for (int v = 0; v < V; v++) {
        int p;
        cin >> p;
        paths[v].resize(p);
        for (int k = 0; k < p; k++) {
            string nm;
            cin >> nm;
            paths[v][k] = id[nm];
        }
    }

    // Free-flow statistics: how many cars use each street as a queueing street,
    // and the earliest free-flow time they reach its end.
    const long long INF = LLONG_MAX / 4;
    vector<long long> cnt(S, 0), firstArr(S, INF);
    for (int v = 0; v < V; v++) {
        long long t = 0;
        int p = paths[v].size();
        for (int k = 0; k < p - 1; k++) {
            int s = paths[v][k];
            if (k > 0) t += Ls[s];
            cnt[s]++;
            firstArr[s] = min(firstArr[s], t);
        }
    }

    // Incoming useful streets per intersection.
    vector<vector<int>> inc(I);
    for (int s = 0; s < S; s++) {
        if (cnt[s] > 0) inc[Es[s]].push_back(s);
    }

    vector<vector<pair<int,int>>> best(I);
    long long bestScore = 0;

    const double alphas[] = {0.25, 0.5, 1.0, 2.0, 4.0};
    for (int mode = 0; mode < 2; mode++) {
        for (double alpha : alphas) {
            vector<vector<pair<int,int>>> sched(I);
            for (int i = 0; i < I; i++) {
                if (inc[i].empty()) continue;
                vector<int> st = inc[i];
                if (mode == 0) {
                    sort(st.begin(), st.end(), [&](int a, int b) {
                        if (firstArr[a] != firstArr[b]) return firstArr[a] < firstArr[b];
                        return a < b;
                    });
                } else {
                    sort(st.begin(), st.end(), [&](int a, int b) {
                        if (firstArr[a] != firstArr[b]) return firstArr[a] < firstArr[b];
                        return a < b;
                    });
                    stable_sort(st.begin(), st.end(), [&](int a, int b) {
                        return cnt[a] > cnt[b];
                    });
                }
                for (int s : st) {
                    long long T = llround(alpha * (double)cnt[s]);
                    if (T < 1) T = 1;
                    if (T > D) T = D;
                    sched[i].push_back({s, (int)T});
                }
            }
            long long sc = simulate(sched);
            if (sc > bestScore) {
                bestScore = sc;
                best = sched;
            }
        }
    }

    // Output submission.
    int A = 0;
    for (int i = 0; i < I; i++) if (!best[i].empty()) A++;
    string out;
    out += to_string(A) + "\n";
    for (int i = 0; i < I; i++) {
        if (best[i].empty()) continue;
        out += to_string(i) + "\n";
        out += to_string(best[i].size()) + "\n";
        for (auto& pr : best[i]) {
            out += names[pr.first];
            out += ' ';
            out += to_string(pr.second);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
