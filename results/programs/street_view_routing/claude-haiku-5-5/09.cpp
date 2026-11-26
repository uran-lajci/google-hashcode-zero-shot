#include <bits/stdc++.h>
using namespace std;

struct Arc {
    int to;
    long long cost;
    long long len;
    int id;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    long long T;
    int C, S;
    cin >> N >> M >> T >> C >> S;

    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<vector<Arc>> adj(N);
    vector<long long> elen(M);
    for (int j = 0; j < M; j++) {
        int A, B, D;
        long long Cc, L;
        cin >> A >> B >> D >> Cc >> L;
        elen[j] = L;
        adj[A].push_back({B, Cc, L, j});
        if (D == 2) adj[B].push_back({A, Cc, L, j});
    }

    vector<char> vis(M, 0);
    const long long INF = LLONG_MAX / 4;
    vector<long long> dist(N, INF), gain(N, 0);
    vector<int> par(N, -1), parE(N, -1);

    // Per-car state
    vector<int> cur(C, S);
    vector<long long> tm(C, 0);
    vector<vector<int>> itin(C);
    vector<char> alive(C, 1);

    // Min-heap over (time, car index)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int c = 0; c < C; c++) pq.push({0, c});

    while (!pq.empty()) {
        auto [t, c] = pq.top();
        pq.pop();
        if (!alive[c]) continue;
        long long R = T - t;
        int u = cur[c];

        // Reset Dijkstra arrays
        fill(dist.begin(), dist.end(), INF);
        fill(gain.begin(), gain.end(), 0LL);
        fill(par.begin(), par.end(), -1);
        fill(parE.begin(), parE.end(), -1);

        priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                       greater<pair<long long, int>>> dq;
        dist[u] = 0;
        dq.push({0, u});
        while (!dq.empty()) {
            auto [d, x] = dq.top();
            dq.pop();
            if (d != dist[x]) continue;
            for (const Arc &e : adj[x]) {
                long long nd = d + e.cost;
                if (nd > R) continue;
                if (nd < dist[e.to]) {
                    dist[e.to] = nd;
                    gain[e.to] = gain[x] + (vis[e.id] ? 0 : e.len);
                    par[e.to] = x;
                    parE[e.to] = e.id;
                    dq.push({nd, e.to});
                }
            }
        }

        // Pick best target by gain per unit time
        int best = -1;
        long long bg = 0, bd = 1;
        for (int v = 0; v < N; v++) {
            if (v == u || dist[v] >= INF || gain[v] <= 0) continue;
            long long dv = max(dist[v], 1LL);
            if (best == -1) {
                best = v; bg = gain[v]; bd = dv;
                continue;
            }
            // compare gain[v]/dv > bg/bd
            __int128 lhs = (__int128)gain[v] * bd;
            __int128 rhs = (__int128)bg * dv;
            if (lhs > rhs) {
                best = v; bg = gain[v]; bd = dv;
            }
        }

        if (best == -1) {
            alive[c] = 0;
            continue;
        }

        // Reconstruct path
        vector<int> nodes;
        vector<int> edges;
        int x = best;
        while (x != u) {
            nodes.push_back(x);
            edges.push_back(parE[x]);
            x = par[x];
        }
        reverse(nodes.begin(), nodes.end());
        for (int id : edges) vis[id] = 1;

        long long newT = t + dist[best];
        for (int v : nodes) itin[c].push_back(v);
        cur[c] = best;
        tm[c] = newT;
        pq.push({newT, c});
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int c = 0; c < C; c++) {
        out += to_string(1 + (int)itin[c].size());
        out += '\n';
        out += to_string(S);
        out += '\n';
        for (int v : itin[c]) {
            out += to_string(v);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
