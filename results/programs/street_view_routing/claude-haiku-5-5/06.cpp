#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    long long c;    // time cost
    long long len;  // length (score)
    int id;         // street id
};

int main() {
    auto startClock = chrono::steady_clock::now();
    auto timeUp = [&]() {
        double el = chrono::duration<double>(chrono::steady_clock::now() - startClock).count();
        return el > 7.5;
    };

    int N, M;
    long long T;
    int C, S;
    if (!(cin >> N >> M >> T >> C >> S)) return 0;

    // Junction coordinates (unused by the algorithm)
    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<vector<Edge>> adj(N);
    vector<long long> eLen(M);
    for (int j = 0; j < M; j++) {
        int A, B, D;
        long long Cc, L;
        cin >> A >> B >> D >> Cc >> L;
        eLen[j] = L;
        adj[A].push_back({B, Cc, L, j});
        if (D == 2) adj[B].push_back({A, Cc, L, j});
    }

    vector<char> vis(M, 0);
    long long score = 0;

    // Per-car state
    vector<vector<int>> itin(C, vector<int>(1, S));
    vector<long long> elapsed(C, 0);
    vector<char> done(C, 0);
    const long long INF = LLONG_MAX / 4;
    const size_t MAXV = 1000000;

    vector<long long> dist(N);
    vector<int> par(N), pedge(N);

    while (true) {
        if (timeUp()) break;

        // Pick car with minimal elapsed time that is not done
        int car = -1;
        for (int i = 0; i < C; i++) {
            if (done[i]) continue;
            if (car == -1 || elapsed[i] < elapsed[car]) car = i;
        }
        if (car == -1) break;

        int cur = itin[car].back();
        long long R = T - elapsed[car];
        if (itin[car].size() >= MAXV) { done[car] = 1; continue; }

        // Dijkstra bounded by remaining time
        fill(dist.begin(), dist.end(), INF);
        fill(par.begin(), par.end(), -1);
        fill(pedge.begin(), pedge.end(), -1);
        priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                       greater<pair<long long, int>>> pq;
        dist[cur] = 0;
        pq.push({0, cur});
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d != dist[u]) continue;
            for (const Edge& e : adj[u]) {
                long long nd = d + e.c;
                if (nd > R) continue;
                if (nd < dist[e.to]) {
                    dist[e.to] = nd;
                    par[e.to] = u;
                    pedge[e.to] = e.id;
                    pq.push({nd, e.to});
                }
            }
        }

        // Choose best unvisited directed street to traverse next
        double bestRatio = -1.0;
        int bestV = -1, bestIdx = -1;
        for (int v = 0; v < N; v++) {
            if (dist[v] >= INF) continue;
            const auto& lst = adj[v];
            for (int k = 0; k < (int)lst.size(); k++) {
                const Edge& e = lst[k];
                if (vis[e.id]) continue;
                long long total = dist[v] + e.c;
                if (total > R) continue;
                double ratio = (double)e.len / (double)max(total, 1LL);
                if (ratio > bestRatio) {
                    bestRatio = ratio;
                    bestV = v;
                    bestIdx = k;
                }
            }
        }

        if (bestV == -1) {
            done[car] = 1;
            continue;
        }

        // Reconstruct path cur -> bestV
        vector<int> pathNodes;
        int x = bestV;
        while (x != cur) {
            pathNodes.push_back(x);
            if (!vis[pedge[x]]) {
                vis[pedge[x]] = 1;
                score += eLen[pedge[x]];
            }
            x = par[x];
        }
        reverse(pathNodes.begin(), pathNodes.end());

        const Edge& chosen = adj[bestV][bestIdx];
        if (itin[car].size() + pathNodes.size() + 1 > MAXV) {
            done[car] = 1;
            continue;
        }
        for (int v : pathNodes) itin[car].push_back(v);

        // Traverse the chosen street
        vis[chosen.id] = 1;
        score += chosen.len;
        itin[car].push_back(chosen.to);
        elapsed[car] += dist[bestV] + chosen.c;
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int i = 0; i < C; i++) {
        out += to_string(itin[i].size());
        out += '\n';
        for (int v : itin[i]) {
            out += to_string(v);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
