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

    // junction coordinates (not needed for the algorithm)
    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<int> ea(M), eb(M), ed(M);
    vector<ll> ec(M), el(M);
    vector<vector<pair<int,int>>> out(N); // (edge id, neighbour)
    for (int j = 0; j < M; j++) {
        cin >> ea[j] >> eb[j] >> ed[j] >> ec[j] >> el[j];
        out[ea[j]].push_back({j, eb[j]});
        if (ed[j] == 2) out[eb[j]].push_back({j, ea[j]});
    }

    const ll INF = LLONG_MAX / 4;
    const size_t MAX_ITIN = 1000000;
    const double TIME_LIMIT = 8.0;

    vector<char> visited(M, 0);
    vector<ll> dist(N, INF);
    vector<int> pn(N, -1), pe(N, -1);
    vector<int> touched;

    vector<vector<int>> itins(C);
    bool timeUp = false;

    for (int car = 0; car < C; car++) {
        vector<int>& it = itins[car];
        it.push_back(S);
        if (timeUp) continue;

        int cur = S;
        ll rem = T;
        while (true) {
            if (elapsed() > TIME_LIMIT) { timeUp = true; break; }

            // Dijkstra from cur, bounded by rem
            for (int x : touched) { dist[x] = INF; pn[x] = -1; pe[x] = -1; }
            touched.clear();

            priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
            dist[cur] = 0;
            touched.push_back(cur);
            pq.push({0, cur});

            ll bestNum = 0, bestDen = 1; // ratio L / time
            int bestU = -1, bestE = -1, bestV = -1;
            ll bestD = 0;

            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();
                if (d != dist[u]) continue;
                if (d > rem) break;

                for (auto [e, v] : out[u]) {
                    if (visited[e]) continue;
                    ll t = d + ec[e];
                    if (t > rem) continue;
                    // compare el[e]/t > bestNum/bestDen
                    if (bestU == -1 || el[e] * bestDen > bestNum * t) {
                        bestNum = el[e];
                        bestDen = t;
                        bestU = u;
                        bestE = e;
                        bestV = v;
                        bestD = d;
                    }
                }

                for (auto [e, v] : out[u]) {
                    ll nd = d + ec[e];
                    if (nd > rem) continue;
                    if (nd < dist[v]) {
                        if (dist[v] == INF) touched.push_back(v);
                        dist[v] = nd;
                        pn[v] = u;
                        pe[v] = e;
                        pq.push({nd, v});
                    }
                }
            }

            if (bestU == -1) break;

            // Build the path cur -> bestU
            vector<int> path;
            vector<int> pathEdges;
            for (int x = bestU; x != cur; x = pn[x]) {
                path.push_back(x);
                pathEdges.push_back(pe[x]);
            }
            reverse(path.begin(), path.end());
            reverse(pathEdges.begin(), pathEdges.end());

            if (it.size() + path.size() + 1 > MAX_ITIN) break;

            for (int e : pathEdges) visited[e] = 1;
            for (int x : path) it.push_back(x);

            // traverse the chosen street
            visited[bestE] = 1;
            it.push_back(bestV);
            rem -= bestD + ec[bestE];
            cur = bestV;
        }
    }

    // Output
    string outStr;
    outStr.reserve(1 << 20);
    outStr += to_string(C);
    outStr += '\n';
    for (int car = 0; car < C; car++) {
        const vector<int>& it = itins[car];
        outStr += to_string(it.size());
        outStr += '\n';
        for (int x : it) {
            outStr += to_string(x);
            outStr += '\n';
        }
    }
    fwrite(outStr.data(), 1, outStr.size(), stdout);
    return 0;
}
