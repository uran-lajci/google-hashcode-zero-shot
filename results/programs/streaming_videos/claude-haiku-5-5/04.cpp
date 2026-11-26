#include <bits/stdc++.h>
using namespace std;

struct Entry {
    double key;
    long long gain;
    int v, c;
    bool operator<(const Entry& o) const {
        if (key != o.key) return key < o.key;
        if (gain != o.gain) return gain < o.gain;
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
    for (int i = 0; i < V; i++) cin >> S[i];

    vector<int> LD(E);
    vector<vector<pair<int,int>>> conn(E);          // (cache, latency)
    vector<int> lat((size_t)E * C, -1);             // lat[e*C + c]
    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        conn[e].resize(K);
        for (int k = 0; k < K; k++) {
            int c, Lc;
            cin >> c >> Lc;
            conn[e][k] = {c, Lc};
            lat[(size_t)e * C + c] = Lc;
        }
    }

    // requests per video: (endpoint, count), merged
    vector<vector<pair<int,int>>> rq(V);
    for (int r = 0; r < R; r++) {
        int v, e, n;
        cin >> v >> e >> n;
        rq[v].push_back({e, n});
    }
    for (int v = 0; v < V; v++) {
        auto& a = rq[v];
        sort(a.begin(), a.end());
        vector<pair<int,int>> m;
        for (auto& p : a) {
            if (!m.empty() && m.back().first == p.first) m.back().second += p.second;
            else m.push_back(p);
        }
        a.swap(m);
    }

    // initial marginal gains: everything served from data center
    vector<long long> g((size_t)V * C, 0);
    for (int v = 0; v < V; v++) {
        for (auto& [e, n] : rq[v]) {
            for (auto& [c, Lc] : conn[e]) {
                g[(size_t)v * C + c] += (long long)n * (LD[e] - Lc);
            }
        }
    }

    vector<Entry> init;
    for (int v = 0; v < V; v++) {
        for (int c = 0; c < C; c++) {
            long long gv = g[(size_t)v * C + c];
            if (gv > 0) {
                init.push_back({(double)gv / S[v], gv, v, c});
            }
        }
    }
    vector<long long>().swap(g);
    priority_queue<Entry> pq(less<Entry>(), move(init));

    // current best latency per (video, endpoint)
    vector<int> best((size_t)V * E);
    for (int v = 0; v < V; v++)
        for (int e = 0; e < E; e++)
            best[(size_t)v * E + e] = LD[e];

    vector<long long> used(C, 0);
    vector<vector<int>> chosen(C);

    auto recompute = [&](int v, int c) -> long long {
        long long gain = 0;
        for (auto& [e, n] : rq[v]) {
            int L = lat[(size_t)e * C + c];
            if (L < 0) continue;
            int cur = best[(size_t)v * E + e];
            if (cur > L) gain += (long long)n * (cur - L);
        }
        return gain;
    };

    while (!pq.empty()) {
        Entry top = pq.top();
        pq.pop();
        int v = top.v, c = top.c;
        if (used[c] + S[v] > X) continue;
        long long gn = recompute(v, c);
        if (gn <= 0) continue;
        if (gn != top.gain) {
            pq.push({(double)gn / S[v], gn, v, c});
            continue;
        }
        // accept
        used[c] += S[v];
        chosen[c].push_back(v);
        for (auto& [e, n] : rq[v]) {
            int L = lat[(size_t)e * C + c];
            if (L < 0) continue;
            int& cur = best[(size_t)v * E + e];
            if (cur > L) cur = L;
        }
    }

    int N = 0;
    for (int c = 0; c < C; c++) if (!chosen[c].empty()) N++;

    string out;
    out += to_string(N) + "\n";
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
