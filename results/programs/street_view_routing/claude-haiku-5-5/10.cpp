#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    auto startTime = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    int N, M;
    ll T;
    int C, S;
    if (!(cin >> N >> M >> T >> C >> S)) return 0;

    // Junction coordinates (ignored for the algorithm)
    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<int> SA(M), SB(M), SD(M);
    vector<ll> SC(M), SL(M);
    // adjacency: for each node, list of (to, cost, edge id)
    struct Arc { int to; ll cost; int edge; };
    vector<vector<Arc>> adj(N);
    for (int j = 0; j < M; j++) {
        int a, b, d;
        ll c, l;
        cin >> a >> b >> d >> c >> l;
        SA[j] = a; SB[j] = b; SD[j] = d; SC[j] = c; SL[j] = l;
        adj[a].push_back({b, c, j});
        if (d == 2) adj[b].push_back({a, c, j});
    }

    vector<char> visited(M, 0);
    const ll INF = LLONG_MAX / 4;

    // Per-car state
    vector<int> cur(C, S);
    vector<ll> rem(C, T);
    vector<vector<int>> path(C, vector<int>(1, S));
    vector<char> done(C, 0);

    vector<ll> dist(N);
    vector<int> parNode(N), parEdge(N);

    auto tryMove = [&](int k) -> bool {
        int u = cur[k];
        ll r = rem[k];
        if (r <= 0) return false;

        fill(dist.begin(), dist.end(), INF);
        dist[u] = 0;
        parNode[u] = -1;
        parEdge[u] = -1;
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
        pq.push({0, u});
        while (!pq.empty()) {
            auto [d, v] = pq.top();
            pq.pop();
            if (d != dist[v]) continue;
            for (const Arc& e : adj[v]) {
                ll nd = d + e.cost;
                if (nd > r) continue;
                if (nd < dist[e.to]) {
                    dist[e.to] = nd;
                    parNode[e.to] = v;
                    parEdge[e.to] = e.edge;
                    pq.push({nd, e.to});
                }
            }
        }

        // Choose best unvisited street direction
        double bestVal = -1.0;
        int bestJ = -1, bestFrom = -1, bestTo = -1;
        ll bestCost = 0;
        for (int j = 0; j < M; j++) {
            if (visited[j]) continue;
            // forward direction
            for (int dir = 0; dir < 2; dir++) {
                if (dir == 1 && SD[j] != 2) break;
                int a = dir == 0 ? SA[j] : SB[j];
                int b = dir == 0 ? SB[j] : SA[j];
                if (dist[a] >= INF) continue;
                ll total = dist[a] + SC[j];
                if (total > r) continue;
                double val = (double)SL[j] / (double)total;
                if (val > bestVal) {
                    bestVal = val;
                    bestJ = j;
                    bestFrom = a;
                    bestTo = b;
                    bestCost = total;
                }
            }
        }
        if (bestJ < 0) {
            done[k] = 1;
            return false;
        }

        // Reconstruct path from u to bestFrom
        vector<int> nodesRev;
        vector<int> edgesRev;
        int v = bestFrom;
        while (v != u) {
            nodesRev.push_back(v);
            edgesRev.push_back(parEdge[v]);
            v = parNode[v];
        }
        // Append path in forward order
        for (int i = (int)nodesRev.size() - 1; i >= 0; i--) {
            path[k].push_back(nodesRev[i]);
            visited[edgesRev[i]] = 1;
        }
        // Traverse the chosen street
        path[k].push_back(bestTo);
        visited[bestJ] = 1;
        rem[k] -= bestCost;
        cur[k] = bestTo;
        return true;
    };

    bool moved = true;
    while (moved) {
        if (elapsed() > 8.5) break;
        moved = false;
        for (int k = 0; k < C; k++) {
            if (done[k]) continue;
            if (tryMove(k)) moved = true;
            if (elapsed() > 8.5) { moved = false; break; }
        }
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int k = 0; k < C; k++) {
        out += to_string(path[k].size());
        out += '\n';
        for (int v : path[k]) {
            out += to_string(v);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
