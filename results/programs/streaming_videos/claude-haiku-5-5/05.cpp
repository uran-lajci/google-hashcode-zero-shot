#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E, R, C, X;
    cin >> V >> E >> R >> C >> X;

    vector<int> S(V);
    for (int i = 0; i < V; i++) cin >> S[i];

    vector<int> LD(E);
    // lat[c*E + e] = latency from cache c to endpoint e, -1 if not connected
    vector<int> lat((size_t)C * E, -1);
    vector<vector<int>> cacheEnds(C);
    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        for (int k = 0; k < K; k++) {
            int c, Lc;
            cin >> c >> Lc;
            lat[(size_t)c * E + e] = Lc;
            cacheEnds[c].push_back(e);
        }
    }

    vector<vector<pair<int,int>>> reqs(V);   // per video: (endpoint, count)
    vector<vector<pair<int,int>>> ereq(E);   // per endpoint: (video, count)
    for (int r = 0; r < R; r++) {
        int v, e, n;
        cin >> v >> e >> n;
        reqs[v].push_back({e, n});
        ereq[e].push_back({v, n});
    }

    // best[v*E + e] = current best latency for video v at endpoint e
    vector<int> best((size_t)V * E);
    for (int v = 0; v < V; v++)
        for (int e = 0; e < E; e++)
            best[(size_t)v * E + e] = LD[e];

    // Initial marginal gains gain[v*C + c]
    vector<long long> gain((size_t)V * C, 0);
    for (int c = 0; c < C; c++) {
        for (int e : cacheEnds[c]) {
            long long d = LD[e] - lat[(size_t)c * E + e];
            if (d <= 0) continue;
            for (auto [v, n] : ereq[e]) {
                gain[(size_t)v * C + c] += (long long)n * d;
            }
        }
    }

    priority_queue<tuple<double, int, int>> pq;
    for (int v = 0; v < V; v++)
        for (int c = 0; c < C; c++) {
            long long g = gain[(size_t)v * C + c];
            if (g > 0) pq.emplace((double)g / S[v], v, c);
        }
    vector<long long>().swap(gain);

    vector<long long> used(C, 0);
    vector<vector<int>> chosen(C);

    while (!pq.empty()) {
        auto [k, v, c] = pq.top();
        pq.pop();
        (void)k;
        if (used[c] + S[v] > X) continue;

        // Recompute current marginal gain
        long long g = 0;
        for (auto [e, n] : reqs[v]) {
            int l = lat[(size_t)c * E + e];
            if (l < 0) continue;
            int cur = best[(size_t)v * E + e];
            if (cur > l) g += (long long)n * (cur - l);
        }
        if (g <= 0) continue;

        double nk = (double)g / S[v];
        if (!pq.empty()) {
            auto top = pq.top();
            if (make_tuple(nk, v, c) < top) {
                pq.emplace(nk, v, c);
                continue;
            }
        }

        // Accept placement
        used[c] += S[v];
        chosen[c].push_back(v);
        for (auto [e, n] : reqs[v]) {
            int l = lat[(size_t)c * E + e];
            if (l < 0) continue;
            int &cur = best[(size_t)v * E + e];
            if (l < cur) cur = l;
        }
    }

    int cnt = 0;
    for (int c = 0; c < C; c++)
        if (!chosen[c].empty()) cnt++;

    string out;
    out += to_string(cnt) + "\n";
    for (int c = 0; c < C; c++) {
        if (chosen[c].empty()) continue;
        out += to_string(c);
        for (int v : chosen[c]) {
            out += ' ';
            out += to_string(v);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
