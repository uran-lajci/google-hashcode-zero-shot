#include <bits/stdc++.h>
using namespace std;

struct Item {
    double d;      // marginal gain per MB
    long long g;   // marginal gain (ms * requests)
    int v, c;
    int ver;
};

struct Cmp {
    bool operator()(const Item& a, const Item& b) const {
        if (a.d != b.d) return a.d < b.d;          // max-heap on density
        if (a.v != b.v) return a.v > b.v;          // deterministic tie-break
        return a.c > b.c;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E, R, C;
    long long X;
    cin >> V >> E >> R >> C >> X;

    vector<int> S(V);
    for (int i = 0; i < V; i++) cin >> S[i];

    vector<int> LD(E);
    vector<vector<pair<int, int>>> epc(E);   // (cache, latency)
    vector<int> Lmat((size_t)C * E, -1);     // Lmat[c*E+e] = latency or -1

    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        for (int k = 0; k < K; k++) {
            int c, lc;
            cin >> c >> lc;
            epc[e].push_back({c, lc});
            Lmat[(size_t)c * E + e] = lc;
        }
    }

    vector<long long> cnt((size_t)V * E, 0);
    for (int r = 0; r < R; r++) {
        int v, e;
        long long n;
        cin >> v >> e >> n;
        cnt[(size_t)v * E + e] += n;
    }

    // Aggregated requests per video: (endpoint, count)
    vector<vector<pair<int, long long>>> req(V);
    for (int v = 0; v < V; v++)
        for (int e = 0; e < E; e++) {
            long long x = cnt[(size_t)v * E + e];
            if (x > 0) req[v].push_back({e, x});
        }
    vector<long long>().swap(cnt);

    // Current best latency per (video, endpoint), initially data center latency
    vector<int> best((size_t)V * E);
    for (int v = 0; v < V; v++)
        for (int e = 0; e < E; e++)
            best[(size_t)v * E + e] = LD[e];

    // Initial marginal gains for every (video, cache) pair
    vector<long long> gain((size_t)V * C, 0);
    for (int v = 0; v < V; v++) {
        for (auto& [e, n] : req[v]) {
            for (auto& [c, lc] : epc[e]) {
                gain[(size_t)v * C + c] += n * (long long)(LD[e] - lc);
            }
        }
    }

    priority_queue<Item, vector<Item>, Cmp> pq;
    for (int v = 0; v < V; v++)
        for (int c = 0; c < C; c++) {
            long long g = gain[(size_t)v * C + c];
            if (g > 0) {
                pq.push({(double)g / S[v], g, v, c, 0});
            }
        }
    vector<long long>().swap(gain);

    vector<long long> used(C, 0);
    vector<vector<int>> placed(C);
    int curVer = 0;
    long long ops = 0;
    const long long BUDGET = 1000000000LL;

    while (!pq.empty()) {
        if (ops > BUDGET) break;
        Item it = pq.top();
        pq.pop();
        int v = it.v, c = it.c;

        if (it.ver == curVer) {
            // Fresh value: accept if it fits (capacity only shrinks, so discard otherwise)
            if (used[c] + S[v] <= X && it.g > 0) {
                used[c] += S[v];
                placed[c].push_back(v);
                curVer++;
                for (auto& [e, n] : req[v]) {
                    int lc = Lmat[(size_t)c * E + e];
                    if (lc >= 0) {
                        int& b = best[(size_t)v * E + e];
                        if (lc < b) b = lc;
                    }
                }
            }
        } else {
            // Stale: recompute marginal gain
            long long m = 0;
            for (auto& [e, n] : req[v]) {
                int lc = Lmat[(size_t)c * E + e];
                int b = best[(size_t)v * E + e];
                if (lc >= 0 && lc < b) m += n * (long long)(b - lc);
            }
            ops += (long long)req[v].size();
            if (m > 0) {
                pq.push({(double)m / S[v], m, v, c, curVer});
            }
        }
    }

    // Output
    int nonEmpty = 0;
    for (int c = 0; c < C; c++)
        if (!placed[c].empty()) nonEmpty++;

    string out;
    out += to_string(nonEmpty) + "\n";
    for (int c = 0; c < C; c++) {
        if (placed[c].empty()) continue;
        out += to_string(c);
        for (int v : placed[c]) {
            out += ' ';
            out += to_string(v);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
