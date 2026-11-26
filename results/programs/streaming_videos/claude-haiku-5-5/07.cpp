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
    for (int i = 0; i < V; i++) cin >> S[i];

    vector<int> LD(E);
    vector<vector<pair<int, int>>> conn(E);  // (cache, latency)
    vector<int> connLat((size_t)E * C, -1);
    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        for (int k = 0; k < K; k++) {
            int c, lc;
            cin >> c >> lc;
            connLat[(size_t)e * C + c] = lc;
            conn[e].push_back({c, lc});
        }
    }

    // benefit[v*C + c] = total time saved if video v is cached in c (with current state)
    vector<long long> benefit((size_t)V * C, 0);
    vector<vector<pair<int, int>>> reqs(V);  // (endpoint, count)
    vector<vector<int>> cur(V);              // current best latency per request entry

    for (int r = 0; r < R; r++) {
        int v, e, n;
        cin >> v >> e >> n;
        reqs[v].push_back({e, n});
        cur[v].push_back(LD[e]);
        long long base = (long long)n;
        for (auto& pr : conn[e]) {
            benefit[(size_t)v * C + pr.first] += base * (LD[e] - pr.second);
        }
    }

    vector<long long> rem(C, X);
    vector<vector<int>> chosen(C);

    // Build initial heap
    vector<Item> init;
    init.reserve(1 << 16);
    for (int v = 0; v < V; v++) {
        for (int c = 0; c < C; c++) {
            long long b = benefit[(size_t)v * C + c];
            if (b > 0) {
                init.push_back({(double)b / S[v], v, c});
            }
        }
    }
    priority_queue<Item> pq(less<Item>(), move(init));

    auto evalBenefit = [&](int v, int c) -> long long {
        long long sum = 0;
        const auto& rv = reqs[v];
        const auto& cv = cur[v];
        for (size_t j = 0; j < rv.size(); j++) {
            int e = rv[j].first;
            int lc = connLat[(size_t)e * C + c];
            if (lc >= 0 && cv[j] > lc) {
                sum += (long long)rv[j].second * (cv[j] - lc);
            }
        }
        return sum;
    };

    while (!pq.empty()) {
        Item it = pq.top();
        pq.pop();
        int v = it.v, c = it.c;
        if (rem[c] < S[v]) continue;  // capacity only shrinks; never fits later
        long long b = evalBenefit(v, c);
        if (b <= 0) continue;
        double nk = (double)b / S[v];
        if (!pq.empty() && nk < pq.top().key) {
            pq.push({nk, v, c});
            continue;
        }
        // accept
        rem[c] -= S[v];
        chosen[c].push_back(v);
        const auto& rv = reqs[v];
        auto& cv = cur[v];
        for (size_t j = 0; j < rv.size(); j++) {
            int e = rv[j].first;
            int lc = connLat[(size_t)e * C + c];
            if (lc >= 0 && lc < cv[j]) cv[j] = lc;
        }
    }

    int N = 0;
    for (int c = 0; c < C; c++)
        if (!chosen[c].empty()) N++;

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
