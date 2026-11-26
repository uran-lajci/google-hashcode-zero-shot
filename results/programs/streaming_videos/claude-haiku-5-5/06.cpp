#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E, R, C;
    long long X;
    cin >> V >> E >> R >> C >> X;

    vector<long long> S(V);
    for (int v = 0; v < V; v++) cin >> S[v];

    vector<int> LD(E);
    vector<vector<pair<int,int>>> adj(E); // (cache, latency)
    vector<int> lat((size_t)E * C, -1);   // lat[e*C + c], -1 if not connected
    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        for (int k = 0; k < K; k++) {
            int c, Lc;
            cin >> c >> Lc;
            adj[e].push_back({c, Lc});
            lat[(size_t)e * C + c] = Lc;
        }
    }

    // Aggregate requests per (video, endpoint).
    vector<long long> req((size_t)V * E, 0);
    for (int i = 0; i < R; i++) {
        int rv, re;
        long long rn;
        cin >> rv >> re >> rn;
        req[(size_t)rv * E + re] += rn;
    }

    vector<vector<pair<int,long long>>> byVideo(V); // (endpoint, count)
    for (int v = 0; v < V; v++) {
        for (int e = 0; e < E; e++) {
            long long n = req[(size_t)v * E + e];
            if (n > 0) byVideo[v].push_back({e, n});
        }
    }
    vector<long long>().swap(req);

    // cur[v*E+e]: current best latency for video v at endpoint e.
    vector<int> cur((size_t)V * E);
    for (int v = 0; v < V; v++)
        for (int e = 0; e < E; e++)
            cur[(size_t)v * E + e] = LD[e];

    // gain[v*C+c]: potential saving of placing v in c given current state.
    vector<long long> gain((size_t)V * C, 0);
    for (int v = 0; v < V; v++) {
        for (auto [e, n] : byVideo[v]) {
            for (auto [c, Lc] : adj[e]) {
                gain[(size_t)v * C + c] += n * (long long)(LD[e] - Lc);
            }
        }
    }

    // Max-heap by ratio gain/size; ties broken deterministically by (v,c).
    using Item = tuple<double,int,int>;
    priority_queue<Item> pq;
    for (int v = 0; v < V; v++) {
        for (int c = 0; c < C; c++) {
            long long g = gain[(size_t)v * C + c];
            if (g > 0) pq.push({(double)g / (double)S[v], v, c});
        }
    }

    vector<long long> used(C, 0);
    vector<vector<int>> stored(C);
    vector<char> chosen((size_t)V * C, 0);

    auto recompute = [&](int v, int c) -> long long {
        long long g = 0;
        for (auto [e, n] : byVideo[v]) {
            int l = lat[(size_t)e * C + c];
            if (l < 0) continue;
            int cu = cur[(size_t)v * E + e];
            if (cu > l) g += n * (long long)(cu - l);
        }
        return g;
    };

    while (!pq.empty()) {
        auto [key, v, c] = pq.top();
        pq.pop();
        if (chosen[(size_t)v * C + c]) continue;
        if (used[c] + S[v] > X) continue; // capacity only shrinks, discard
        long long g = recompute(v, c);
        if (g <= 0) continue;
        double nk = (double)g / (double)S[v];
        if (!pq.empty() && nk < get<0>(pq.top())) {
            pq.push({nk, v, c});
            continue;
        }
        // Accept.
        chosen[(size_t)v * C + c] = 1;
        used[c] += S[v];
        stored[c].push_back(v);
        for (auto [e, n] : byVideo[v]) {
            int l = lat[(size_t)e * C + c];
            if (l >= 0 && l < cur[(size_t)v * E + e]) cur[(size_t)v * E + e] = l;
        }
    }

    // Output.
    vector<int> nonEmpty;
    for (int c = 0; c < C; c++)
        if (!stored[c].empty()) nonEmpty.push_back(c);

    string out;
    out += to_string(nonEmpty.size()) + "\n";
    for (int c : nonEmpty) {
        out += to_string(c);
        for (int v : stored[c]) {
            out += ' ';
            out += to_string(v);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
