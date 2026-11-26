#include <bits/stdc++.h>
using namespace std;

struct Entry {
    double ratio;
    int v, c;
};

// Priority: higher ratio first; ties -> smaller v, then smaller c.
struct Cmp {
    bool operator()(const Entry& a, const Entry& b) const {
        if (a.ratio != b.ratio) return a.ratio < b.ratio;
        if (a.v != b.v) return a.v > b.v;
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
    // adjacency: for each endpoint, list of (cache, latency)
    vector<vector<pair<int,int>>> adj(E);
    // lat[e*C + c] = latency from cache c to endpoint e, 0 if not connected
    vector<int> lat((size_t)E * C, 0);
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

    // Requests grouped by video: (endpoint, count), aggregated per endpoint.
    vector<vector<pair<int,int>>> reqByVideo(V);
    for (int r = 0; r < R; r++) {
        int v, e, n;
        cin >> v >> e >> n;
        reqByVideo[v].push_back({e, n});
    }
    for (int v = 0; v < V; v++) {
        auto& vec = reqByVideo[v];
        sort(vec.begin(), vec.end());
        vector<pair<int,int>> merged;
        for (auto& p : vec) {
            if (!merged.empty() && merged.back().first == p.first)
                merged.back().second += p.second;
            else
                merged.push_back(p);
        }
        vec.swap(merged);
    }

    // cur[v][i]: current best latency for request i of video v.
    vector<vector<int>> cur(V);
    for (int v = 0; v < V; v++) {
        cur[v].resize(reqByVideo[v].size());
        for (size_t i = 0; i < reqByVideo[v].size(); i++)
            cur[v][i] = LD[reqByVideo[v][i].first];
    }

    // Initial benefit for each (video, cache) pair.
    vector<long long> ben((size_t)V * C, 0);
    for (int v = 0; v < V; v++) {
        for (auto& rq : reqByVideo[v]) {
            int e = rq.first;
            long long n = rq.second;
            for (auto& cl : adj[e]) {
                int c = cl.first, Lc = cl.second;
                long long gain = (long long)LD[e] - Lc;
                if (gain > 0)
                    ben[(size_t)v * C + c] += n * gain;
            }
        }
    }

    priority_queue<Entry, vector<Entry>, Cmp> pq;
    {
        vector<Entry> init;
        for (int v = 0; v < V; v++) {
            for (int c = 0; c < C; c++) {
                long long b = ben[(size_t)v * C + c];
                if (b > 0) {
                    init.push_back({(double)b / S[v], v, c});
                }
            }
        }
        // Build heap in linear time via the vector constructor.
        pq = priority_queue<Entry, vector<Entry>, Cmp>(Cmp(), std::move(init));
    }
    ben.clear();
    ben.shrink_to_fit();

    vector<long long> remaining(C, X);
    vector<vector<int>> placed(C);

    auto gainOf = [&](int v, int c) -> long long {
        long long g = 0;
        const auto& vec = reqByVideo[v];
        for (size_t i = 0; i < vec.size(); i++) {
            int e = vec[i].first;
            int L = lat[(size_t)e * C + c];
            if (L > 0 && L < cur[v][i]) {
                g += (long long)vec[i].second * (cur[v][i] - L);
            }
        }
        return g;
    };

    while (!pq.empty()) {
        Entry top = pq.top();
        pq.pop();
        int v = top.v, c = top.c;
        if (S[v] > remaining[c]) continue;

        long long g = gainOf(v, c);
        if (g <= 0) continue;
        Entry ne{(double)g / S[v], v, c};

        if (!pq.empty() && Cmp()(ne, pq.top())) {
            // Stale: reinsert with updated value.
            pq.push(ne);
            continue;
        }

        // Place video v in cache c.
        remaining[c] -= S[v];
        placed[c].push_back(v);
        const auto& vec = reqByVideo[v];
        for (size_t i = 0; i < vec.size(); i++) {
            int e = vec[i].first;
            int L = lat[(size_t)e * C + c];
            if (L > 0 && L < cur[v][i]) cur[v][i] = L;
        }
    }

    // Output.
    int N = 0;
    for (int c = 0; c < C; c++) if (!placed[c].empty()) N++;
    string out;
    out += to_string(N) + "\n";
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
