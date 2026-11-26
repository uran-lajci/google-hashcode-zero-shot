#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> SB, SE, SL;
vector<string> names;
unordered_map<string, int> sid;
vector<vector<int>> paths;

// schedule: for each intersection, list of (street, T)
typedef vector<vector<pair<int, int>>> Sched;

long long simulate(const Sched& sch) {
    // prepare green lookup
    vector<int> C(I, 0);
    vector<vector<int>> pref(I), st(I);
    for (int v = 0; v < I; v++) {
        if (sch[v].empty()) continue;
        int acc = 0;
        for (auto& pr : sch[v]) {
            acc += pr.second;
            pref[v].push_back(acc);
            st[v].push_back(pr.first);
        }
        C[v] = acc;
    }
    auto green = [&](int v, int t) -> int {
        if (C[v] == 0) return -1;
        int r = t % C[v];
        int idx = int(upper_bound(pref[v].begin(), pref[v].end(), r) - pref[v].begin());
        return st[v][idx];
    };

    vector<deque<int>> q(S);
    vector<int> qc(I, 0);
    set<int> act;
    vector<int> pos(V, 0);
    vector<vector<int>> bucket(D + 1);
    for (int c = 0; c < V; c++) {
        int s = paths[c][0];
        q[s].push_back(c);
        int v = SE[s];
        if (qc[v]++ == 0) act.insert(v);
    }
    long long score = 0;
    vector<int> cur;
    for (int t = 0; t < D; t++) {
        auto& b = bucket[t];
        if (!b.empty()) {
            sort(b.begin(), b.end());
            for (int c : b) {
                int s = paths[c][pos[c]];
                q[s].push_back(c);
                int v = SE[s];
                if (qc[v]++ == 0) act.insert(v);
            }
            vector<int>().swap(b);
        }
        cur.assign(act.begin(), act.end());
        for (int v : cur) {
            int g = green(v, t);
            if (g < 0) continue;
            if (q[g].empty()) continue;
            int c = q[g].front();
            q[g].pop_front();
            qc[v]--;
            if (qc[v] == 0) act.erase(v);
            pos[c]++;
            int ns = paths[c][pos[c]];
            int arr = t + SL[ns];
            if (pos[c] == (int)paths[c].size() - 1) {
                if (arr <= D) score += F + (D - arr);
            } else if (arr < D) {
                bucket[arr].push_back(c);
            }
        }
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long Dl, Il, Sl, Vl, Fl;
    cin >> Dl >> Il >> Sl >> Vl >> Fl;
    D = (int)Dl; I = (int)Il; S = (int)Sl; V = (int)Vl; F = Fl;
    SB.resize(S); SE.resize(S); SL.resize(S); names.resize(S);
    for (int s = 0; s < S; s++) {
        cin >> SB[s] >> SE[s] >> names[s] >> SL[s];
        sid[names[s]] = s;
    }
    paths.resize(V);
    for (int c = 0; c < V; c++) {
        int P;
        cin >> P;
        paths[c].resize(P);
        for (int k = 0; k < P; k++) {
            string nm;
            cin >> nm;
            paths[c][k] = sid[nm];
        }
    }

    // statistics
    const int INF = INT_MAX;
    vector<int> cnt(S, 0), first(S, INF);
    for (int c = 0; c < V; c++) {
        int P = (int)paths[c].size();
        long long acc = 0;
        for (int k = 0; k < P; k++) {
            int s = paths[c][k];
            if (k > 0) acc += SL[s];
            if (k < P - 1) {
                cnt[s]++;
                if (acc < D && acc < first[s]) first[s] = (int)acc;
            }
        }
    }
    vector<vector<int>> inc(I);
    for (int s = 0; s < S; s++) {
        if (cnt[s] > 0 && first[s] < D) inc[SE[s]].push_back(s);
    }

    Sched best(I);
    long long bestScore = -1;
    bool haveBest = false;

    // variants
    vector<double> scales = {1.0, 0.5, 2.0, 0.25, 0.0};  // 0.0 => uniform T=1
    for (int mode = 0; mode < 2; mode++) {
        for (double scale : scales) {
            Sched cand(I);
            for (int v = 0; v < I; v++) {
                if (inc[v].empty()) continue;
                vector<int> lst = inc[v];
                if (mode == 0) {
                    sort(lst.begin(), lst.end(), [&](int a, int b) {
                        if (first[a] != first[b]) return first[a] < first[b];
                        return a < b;
                    });
                } else {
                    sort(lst.begin(), lst.end(), [&](int a, int b) {
                        if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
                        return a < b;
                    });
                }
                for (int s : lst) {
                    int T;
                    if (scale == 0.0) T = 1;
                    else {
                        long long x = llround(cnt[s] * scale);
                        if (x < 1) x = 1;
                        if (x > D) x = D;
                        T = (int)x;
                    }
                    cand[v].push_back({s, T});
                }
            }
            long long sc = simulate(cand);
            if (!haveBest || sc > bestScore) {
                haveBest = true;
                bestScore = sc;
                best = cand;
            }
        }
    }

    // output
    int A = 0;
    for (int v = 0; v < I; v++) if (!best[v].empty()) A++;
    string out;
    out += to_string(A) + "\n";
    for (int v = 0; v < I; v++) {
        if (best[v].empty()) continue;
        out += to_string(v) + "\n";
        out += to_string(best[v].size()) + "\n";
        for (auto& pr : best[v]) {
            out += names[pr.first];
            out += ' ';
            out += to_string(pr.second);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
