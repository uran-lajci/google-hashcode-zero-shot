#include <bits/stdc++.h>
using namespace std;

int main() {
    auto startTime = chrono::steady_clock::now();
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
    vector<int> ea(M), eb(M), ed(M);
    vector<long long> ec(M), el(M);
    vector<vector<pair<int,int>>> adj(N); // (to, edge)
    for (int j = 0; j < M; j++) {
        cin >> ea[j] >> eb[j] >> ed[j] >> ec[j] >> el[j];
        adj[ea[j]].push_back({eb[j], j});
        if (ed[j] == 2) adj[eb[j]].push_back({ea[j], j});
    }

    vector<char> vis(M, 0);
    vector<vector<int>> itin(C);
    for (int i = 0; i < C; i++) itin[i].push_back(S);
    vector<long long> used(C, 0);
    vector<int> pos(C, S);

    // priority queue of cars by used time
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> pq;
    for (int i = 0; i < C; i++) pq.push({0LL, i});

    const long long INF = LLONG_MAX / 4;
    vector<long long> dist(N);
    vector<int> parNode(N), parEdge(N);

    while (!pq.empty()) {
        double el_s = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
        if (el_s > 8.0) break;

        auto [u0, car] = pq.top();
        pq.pop();
        long long remaining = T - used[car];
        if (remaining <= 0) continue;
        int src = pos[car];

        // Dijkstra bounded by remaining time
        fill(dist.begin(), dist.end(), INF);
        dist[src] = 0;
        parNode[src] = -1;
        parEdge[src] = -1;
        priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> dq;
        dq.push({0LL, src});
        while (!dq.empty()) {
            auto [d, x] = dq.top();
            dq.pop();
            if (d > dist[x]) continue;
            for (auto [y, e] : adj[x]) {
                long long nd = d + ec[e];
                if (nd <= remaining && nd < dist[y]) {
                    dist[y] = nd;
                    parNode[y] = x;
                    parEdge[y] = e;
                    dq.push({nd, y});
                }
            }
        }

        // choose best unvisited arc to traverse
        double bestScore = -1.0;
        int bestU = -1, bestV = -1, bestE = -1;
        for (int u = 0; u < N; u++) {
            if (dist[u] >= INF) continue;
            for (auto [v, e] : adj[u]) {
                if (vis[e]) continue;
                // arc direction validity: if ed==1 only ea->eb
                if (ed[e] == 1 && !(ea[e] == u && eb[e] == v)) continue;
                long long t = dist[u] + ec[e];
                if (t > remaining) continue;
                double score = (double)el[e] / (double)t;
                if (score > bestScore) {
                    bestScore = score;
                    bestU = u; bestV = v; bestE = e;
                }
            }
        }
        if (bestE < 0) continue; // this car can gain nothing more

        // reconstruct path src -> bestU
        vector<int> pathNodes; // nodes after src up to bestU, in order
        vector<int> pathEdges;
        {
            int x = bestU;
            while (x != src) {
                pathNodes.push_back(x);
                pathEdges.push_back(parEdge[x]);
                x = parNode[x];
            }
            reverse(pathNodes.begin(), pathNodes.end());
            reverse(pathEdges.begin(), pathEdges.end());
        }
        for (size_t k = 0; k < pathNodes.size(); k++) {
            itin[car].push_back(pathNodes[k]);
            vis[pathEdges[k]] = 1;
        }
        itin[car].push_back(bestV);
        vis[bestE] = 1;

        used[car] = used[car] + dist[bestU] + ec[bestE];
        pos[car] = bestV;
        if (used[car] < T) pq.push({used[car], car});
    }

    // output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int i = 0; i < C; i++) {
        out += to_string(itin[i].size());
        out += '\n';
        for (int x : itin[i]) {
            out += to_string(x);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
