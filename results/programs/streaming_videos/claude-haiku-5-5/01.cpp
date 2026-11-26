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
    vector<int> conn((size_t)E * C, -1);            // latency from endpoint e to cache c, -1 if not connected
    vector<vector<pair<int,int>>> adj(E);            // (cache, latency)
    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        for (int k = 0; k < K; k++) {
            int c, lc;
            cin >> c >> lc;
            conn[(size_t)e * C + c] = lc;
            adj[e].push_back({c, lc});
        }
    }

    vector<vector<pair<int,int>>> byVideo(V);        // (endpoint, count)
    vector<vector<pair<int,int>>> byEndpoint(E);     // (video, count)
    long long totalReq = 0;
    for (int i = 0; i < R; i++) {
        int v, e, n;
        cin >> v >> e >> n;
        byVideo[v].push_back({e, n});
        byEndpoint[e].push_back({v, n});
        totalReq += n;
    }

    // cur[v*E+e] = best current latency for video v at endpoint e
    vector<int> cur((size_t)V * E);
    for (int v = 0; v < V; v++)
        for (int e = 0; e < E; e++)
            cur[(size_t)v * E + e] = LD[e];

    // initial benefit of placing video v in cache c
    vector<long long> ben((size_t)C * V, 0);
    for (int e = 0; e < E; e++) {
        for (auto [v, n] : byEndpoint[e]) {
            for (auto [c, lc] : adj[e]) {
                ben[(size_t)c * V + v] += (long long)n * (LD[e] - lc);
            }
        }
    }

    vector<long long> used(C, 0);
    vector<char> placed((size_t)C * V, 0);
    vector<vector<int>> chosen(C);

    // max-heap on (ratio, c, v)
    priority_queue<tuple<double,int,int>> pq;
    for (int c = 0; c < C; c++)
        for (int v = 0; v < V; v++) {
            long long b = ben[(size_t)c * V + v];
            if (b > 0) pq.push({(double)b / S[v], c, v});
        }

    auto benefitOf = [&](int c, int v) -> long long {
        long long s = 0;
        for (auto [e, n] : byVideo[v]) {
            int lc = conn[(size_t)e * C + c];
            if (lc < 0) continue;
            int cu = cur[(size_t)v * E + e];
            if (cu > lc) s += (long long)n * (cu - lc);
        }
        return s;
    };

    while (!pq.empty()) {
        auto [r, c, v] = pq.top();
        pq.pop();
        if (placed[(size_t)c * V + v]) continue;
        if (used[c] + S[v] > X) continue;
        long long b = benefitOf(c, v);
        if (b <= 0) continue;
        double nr = (double)b / S[v];
        if (!pq.empty() && nr < get<0>(pq.top())) {
            pq.push({nr, c, v});
            continue;
        }
        // place video v in cache c
        placed[(size_t)c * V + v] = 1;
        used[c] += S[v];
        chosen[c].push_back(v);
        for (auto [e, n] : byVideo[v]) {
            int lc = conn[(size_t)e * C + c];
            if (lc >= 0 && lc < cur[(size_t)v * E + e])
                cur[(size_t)v * E + e] = lc;
        }
    }

    int cnt = 0;
    for (int c = 0; c < C; c++) if (!chosen[c].empty()) cnt++;

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
