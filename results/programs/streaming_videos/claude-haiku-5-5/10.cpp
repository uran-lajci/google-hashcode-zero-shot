#include <bits/stdc++.h>
using namespace std;

struct Item {
    double key;
    int v, c;
    bool operator<(const Item& o) const {
        if (key != o.key) return key < o.key;
        if (v != o.v) return v > o.v;
        return c > o.c;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E, R, C;
    long long X;
    cin >> V >> E >> R >> C >> X;

    vector<int> S(V);
    for (int i = 0; i < V; ++i) cin >> S[i];

    vector<int> LD(E);
    vector<vector<pair<int, int>>> conn(E);   // (cache, latency)
    vector<int> lat((size_t)E * C, 0);         // 0 = not connected
    for (int e = 0; e < E; ++e) {
        int K;
        cin >> LD[e] >> K;
        for (int k = 0; k < K; ++k) {
            int c, Lc;
            cin >> c >> Lc;
            conn[e].push_back({c, Lc});
            lat[(size_t)e * C + c] = Lc;
        }
    }

    vector<vector<pair<int, int>>> dem(V);     // (endpoint, count)
    for (int i = 0; i < R; ++i) {
        int v, e, n;
        cin >> v >> e >> n;
        dem[v].push_back({e, n});
    }
    for (int v = 0; v < V; ++v) {
        auto& d = dem[v];
        sort(d.begin(), d.end());
        vector<pair<int, int>> m;
        for (auto& p : d) {
            if (!m.empty() && m.back().first == p.first) m.back().second += p.second;
            else m.push_back(p);
        }
        d.swap(m);
    }

    // best[v*E+e] = current best latency for endpoint e serving video v
    vector<int> best((size_t)V * E);
    for (int v = 0; v < V; ++v)
        for (int e = 0; e < E; ++e)
            best[(size_t)v * E + e] = LD[e];

    // initial gains
    vector<long long> gain((size_t)V * C, 0);
    for (int v = 0; v < V; ++v) {
        for (auto& [e, cnt] : dem[v]) {
            for (auto& [c, Lc] : conn[e]) {
                gain[(size_t)v * C + c] += (long long)cnt * (LD[e] - Lc);
            }
        }
    }

    priority_queue<Item> pq;
    for (int v = 0; v < V; ++v) {
        for (int c = 0; c < C; ++c) {
            long long g = gain[(size_t)v * C + c];
            if (g > 0) {
                pq.push({(double)g / S[v], v, c});
            }
        }
    }

    vector<long long> rem(C, X);
    vector<vector<int>> stored(C);

    while (!pq.empty()) {
        Item t = pq.top();
        pq.pop();
        int v = t.v, c = t.c;
        if (S[v] > rem[c]) continue;

        // recompute actual gain
        long long g = 0;
        for (auto& [e, cnt] : dem[v]) {
            int l = lat[(size_t)e * C + c];
            if (l > 0) {
                int b = best[(size_t)v * E + e];
                if (b > l) g += (long long)cnt * (b - l);
            }
        }
        if (g == 0) continue;
        double key = (double)g / S[v];
        if (!pq.empty() && key < pq.top().key) {
            pq.push({key, v, c});
            continue;
        }

        // accept
        stored[c].push_back(v);
        rem[c] -= S[v];
        for (auto& [e, cnt] : dem[v]) {
            int l = lat[(size_t)e * C + c];
            if (l > 0) {
                int& b = best[(size_t)v * E + e];
                if (l < b) b = l;
            }
        }
    }

    // output
    int N = 0;
    for (int c = 0; c < C; ++c)
        if (!stored[c].empty()) ++N;

    string out;
    out += to_string(N) + "\n";
    for (int c = 0; c < C; ++c) {
        if (stored[c].empty()) continue;
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
