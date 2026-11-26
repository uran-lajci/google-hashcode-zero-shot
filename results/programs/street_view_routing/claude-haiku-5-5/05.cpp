#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    long long T;
    int C, S;
    cin >> N >> M >> T >> C >> S;

    // Junction coordinates are not needed for the computation.
    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<int> ea(M), eb(M), ed(M);
    vector<long long> ec(M), el(M);
    // arcs: for each node, list of (edge id, target node, cost)
    vector<vector<array<long long, 3>>> arcs(N);
    for (int j = 0; j < M; j++) {
        cin >> ea[j] >> eb[j] >> ed[j] >> ec[j] >> el[j];
        arcs[ea[j]].push_back({(long long)j, (long long)eb[j], ec[j]});
        if (ed[j] == 2) {
            arcs[eb[j]].push_back({(long long)j, (long long)ea[j], ec[j]});
        }
    }

    vector<char> visited(M, 0);
    vector<int> pos(C, S);
    vector<long long> curTime(C, 0);
    vector<vector<int>> paths(C, vector<int>(1, S));

    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> carQ;
    for (int i = 0; i < C; i++) carQ.push({0, i});

    const long long INF = LLONG_MAX / 4;
    const long long LIMIT = 600000000LL;
    long long ops = 0;

    vector<long long> dist(N);
    vector<int> parNode(N), parEdge(N);

    while (!carQ.empty() && ops < LIMIT) {
        auto [t, i] = carQ.top();
        carQ.pop();
        long long rem = T - t;
        if (rem <= 0) continue;

        int src = pos[i];
        fill(dist.begin(), dist.end(), INF);
        dist[src] = 0;
        parNode[src] = -1;
        parEdge[src] = -1;

        priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                       greater<pair<long long, int>>> pq;
        pq.push({0, src});

        double bestVal = -1.0;
        int bestX = -1, bestY = -1, bestJ = -1;
        long long bestD = 0, bestC = 0;

        while (!pq.empty()) {
            auto [d, x] = pq.top();
            pq.pop();
            ops++;
            if (d != dist[x]) continue;
            for (auto &a : arcs[x]) {
                ops++;
                int j = (int)a[0];
                int y = (int)a[1];
                long long c = a[2];
                long long nd = d + c;
                if (nd > rem) continue;
                if (!visited[j]) {
                    double v = (double)el[j] / (double)max(1LL, nd);
                    if (v > bestVal) {
                        bestVal = v;
                        bestX = x;
                        bestY = y;
                        bestJ = j;
                        bestD = d;
                        bestC = c;
                    }
                }
                if (nd < dist[y]) {
                    dist[y] = nd;
                    parNode[y] = x;
                    parEdge[y] = j;
                    pq.push({nd, y});
                }
            }
        }

        if (bestVal < 0) {
            // Nothing reachable and unvisited within remaining time: car is done.
            continue;
        }

        // Reconstruct path src -> bestX
        vector<int> nodes;
        vector<int> edges;
        int cur = bestX;
        while (cur != src) {
            nodes.push_back(cur);
            edges.push_back(parEdge[cur]);
            cur = parNode[cur];
        }
        reverse(nodes.begin(), nodes.end());
        reverse(edges.begin(), edges.end());
        for (size_t k = 0; k < nodes.size(); k++) {
            paths[i].push_back(nodes[k]);
            visited[edges[k]] = 1;
        }
        // Traverse the chosen street.
        paths[i].push_back(bestY);
        visited[bestJ] = 1;
        pos[i] = bestY;
        long long newT = t + bestD + bestC;
        curTime[i] = newT;
        carQ.push({newT, i});
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int i = 0; i < C; i++) {
        out += to_string(paths[i].size());
        out += '\n';
        for (int v : paths[i]) {
            out += to_string(v);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
