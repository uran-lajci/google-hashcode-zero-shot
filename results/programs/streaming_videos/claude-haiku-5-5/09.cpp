#include <bits/stdc++.h>
using namespace std;

struct Item {
    double d;
    int v;
    int c;
    bool operator<(const Item& o) const {
        if (d != o.d) return d < o.d;
        if (v != o.v) return v > o.v;
        return c > o.c;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E, R, C, X;
    cin >> V >> E >> R >> C >> X;

    vector<int> S(V);
    for (int i = 0; i < V; ++i) cin >> S[i];

    vector<int> LD(E);
    vector<vector<pair<int, int>>> conn(E);         // (cache, latency)
    vector<int> Lm((size_t)E * C, -1);              // Lm[e*C+c] = latency or -1
    for (int e = 0; e < E; ++e) {
        int K;
        cin >> LD[e] >> K;
        conn[e].reserve(K);
        for (int k = 0; k < K; ++k) {
            int c, L;
            cin >> c >> L;
            conn[e].push_back({c, L});
            Lm[(size_t)e * C + c] = L;
        }
    }

    // rq[v] = list of (endpoint, count)
    vector<vector<pair<int, int>>> rq(V);
    for (int i = 0; i < R; ++i) {
        int v, e, n;
        cin >> v >> e >> n;
        rq[v].push_back({e, n});
    }

    // best[v*E+e] = current best latency for video v at endpoint e
    vector<int> best((size_t)V * E);
    for (int v = 0; v < V; ++v)
        for (int e = 0; e < E; ++e)
            best[(size_t)v * E + e] = LD[e];

    // benefit[v*C+c] = total time saved if video v is added to cache c
    vector<long long> benefit((size_t)V * C, 0);
    for (int v = 0; v < V; ++v) {
        for (auto [e, n] : rq[v]) {
            for (auto [c, L] : conn[e]) {
                long long gain = (long long)n * (LD[e] - L);
                if (gain > 0) benefit[(size_t)v * C + c] += gain;
            }
        }
    }

    priority_queue<Item> pq;
    for (int v = 0; v < V; ++v) {
        for (int c = 0; c < C; ++c) {
            long long b = benefit[(size_t)v * C + c];
            if (b > 0) pq.push({b / (double)S[v], v, c});
        }
    }

    vector<long long> used(C, 0);
    vector<vector<int>> cacheVideos(C);

    while (!pq.empty()) {
        Item t = pq.top();
        pq.pop();
        int v = t.v, c = t.c;
        long long b = benefit[(size_t)v * C + c];
        if (b <= 0) continue;
        if (used[c] + S[v] > X) continue;
        double cur = b / (double)S[v];
        if (cur != t.d) {
            pq.push({cur, v, c});
            continue;
        }

        // Accept (v, c)
        used[c] += S[v];
        cacheVideos[c].push_back(v);

        for (auto [e, n] : rq[v]) {
            int Lc = Lm[(size_t)e * C + c];
            if (Lc < 0) continue;
            int oldB = best[(size_t)v * E + e];
            if (Lc >= oldB) continue;
            int newB = Lc;
            best[(size_t)v * E + e] = newB;
            for (auto [c2, L2] : conn[e]) {
                long long before = max(0, oldB - L2);
                long long after = max(0, newB - L2);
                benefit[(size_t)v * C + c2] += (long long)n * (after - before);
            }
        }
    }

    int cnt = 0;
    for (int c = 0; c < C; ++c)
        if (!cacheVideos[c].empty()) ++cnt;

    string out;
    out += to_string(cnt) + "\n";
    for (int c = 0; c < C; ++c) {
        if (cacheVideos[c].empty()) continue;
        out += to_string(c);
        for (int v : cacheVideos[c]) {
            out += ' ';
            out += to_string(v);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
