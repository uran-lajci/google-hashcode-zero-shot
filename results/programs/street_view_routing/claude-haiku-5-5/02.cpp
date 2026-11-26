#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto startTime = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    int N, M;
    long long T;
    int C, S;
    cin >> N >> M >> T >> C >> S;

    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<int> EA(M), EB(M), ED(M);
    vector<long long> EC(M), EL(M);
    // adjacency: arcs from node
    struct Arc { int to; long long cost; int edge; };
    vector<vector<Arc>> adj(N);
    for (int j = 0; j < M; j++) {
        cin >> EA[j] >> EB[j] >> ED[j] >> EC[j] >> EL[j];
        adj[EA[j]].push_back({EB[j], EC[j], j});
        if (ED[j] == 2) {
            adj[EB[j]].push_back({EA[j], EC[j], j});
        }
    }

    vector<char> visited(M, 0);
    const long long INF = LLONG_MAX / 4;
    vector<long long> dist(N);
    vector<int> par(N), parEdge(N);

    vector<vector<int>> itineraries(C);
    bool timeUp = false;

    for (int car = 0; car < C; car++) {
        itineraries[car].push_back(S);
        int cur = S;
        long long rem = T;

        while (!timeUp) {
            if (elapsed() > 8.5) { timeUp = true; break; }

            fill(dist.begin(), dist.end(), INF);
            fill(par.begin(), par.end(), -1);
            fill(parEdge.begin(), parEdge.end(), -1);

            dist[cur] = 0;
            priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                           greater<pair<long long, int>>> pq;
            pq.push({0, cur});

            double bestRatio = -1.0;
            int bestV = -1, bestE = -1, bestW = -1;
            long long bestD = 0, bestC = 0;

            while (!pq.empty()) {
                auto [d, v] = pq.top();
                pq.pop();
                if (d != dist[v]) continue;
                if (d > rem) break;
                for (const Arc& a : adj[v]) {
                    long long nd = d + a.cost;
                    if (nd > rem) continue;
                    if (!visited[a.edge]) {
                        double ratio = (double)EL[a.edge] / (double)nd;
                        if (ratio > bestRatio) {
                            bestRatio = ratio;
                            bestV = v;
                            bestE = a.edge;
                            bestW = a.to;
                            bestD = d;
                            bestC = a.cost;
                        }
                    }
                    if (nd < dist[a.to]) {
                        dist[a.to] = nd;
                        par[a.to] = v;
                        parEdge[a.to] = a.edge;
                        pq.push({nd, a.to});
                    }
                }
            }

            if (bestV < 0) break;

            // Reconstruct path from cur to bestV.
            vector<int> pathNodes;
            vector<int> pathEdges;
            int x = bestV;
            while (x != cur) {
                pathNodes.push_back(x);
                pathEdges.push_back(parEdge[x]);
                x = par[x];
            }
            reverse(pathNodes.begin(), pathNodes.end());
            reverse(pathEdges.begin(), pathEdges.end());

            for (size_t k = 0; k < pathNodes.size(); k++) {
                itineraries[car].push_back(pathNodes[k]);
                visited[pathEdges[k]] = 1;
            }

            // Traverse the chosen street.
            visited[bestE] = 1;
            itineraries[car].push_back(bestW);
            cur = bestW;
            rem -= bestD + bestC;
        }
        if (timeUp) {
            // Remaining cars stay at S.
        }
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int car = 0; car < C; car++) {
        out += to_string(itineraries[car].size());
        out += '\n';
        for (int v : itineraries[car]) {
            out += to_string(v);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
