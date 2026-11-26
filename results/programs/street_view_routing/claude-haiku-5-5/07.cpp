#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Arc { int to; ll c; int id; };

int main() {
    auto startTime = chrono::steady_clock::now();
    auto elapsedSec = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    int N, M, T, C, S;
    if (!(cin >> N >> M >> T >> C >> S)) return 0;
    vector<double> lat(N), lon(N);
    for (int i = 0; i < N; i++) cin >> lat[i] >> lon[i];

    vector<int> A(M), B(M), D(M);
    vector<ll> Cost(M), Len(M);
    vector<vector<Arc>> adj(N);
    for (int j = 0; j < M; j++) {
        cin >> A[j] >> B[j] >> D[j] >> Cost[j] >> Len[j];
        adj[A[j]].push_back({B[j], Cost[j], j});
        if (D[j] == 2) adj[B[j]].push_back({A[j], Cost[j], j});
    }

    vector<char> vis(M, 0);
    vector<int> cur(C, S);
    vector<ll> tm(C, 0);
    vector<vector<int>> paths(C, vector<int>(1, S));
    const ll INF = LLONG_MAX / 4;
    const size_t MAXV = 1000000;
    const double TIME_LIMIT = 8.5;

    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int i = 0; i < C; i++) pq.push({0, i});

    vector<ll> dist(N);
    vector<int> parNode(N), parStreet(N);

    while (!pq.empty()) {
        if (elapsedSec() > TIME_LIMIT) break;
        auto [t, i] = pq.top();
        pq.pop();
        ll rem = (ll)T - t;
        if (rem <= 0) continue;

        // Dijkstra from cur[i] bounded by rem
        fill(dist.begin(), dist.end(), INF);
        int s = cur[i];
        dist[s] = 0;
        parNode[s] = -1;
        parStreet[s] = -1;
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> hq;
        hq.push({0, s});
        while (!hq.empty()) {
            auto [d, u] = hq.top();
            hq.pop();
            if (d != dist[u]) continue;
            for (const Arc &a : adj[u]) {
                ll nd = d + a.c;
                if (nd <= rem && nd < dist[a.to]) {
                    dist[a.to] = nd;
                    parNode[a.to] = u;
                    parStreet[a.to] = a.id;
                    hq.push({nd, a.to});
                }
            }
        }

        // Choose best unvisited street to traverse
        double bestVal = -1.0;
        int bu = -1, bv = -1, bid = -1;
        ll bTotal = 0;
        for (int u = 0; u < N; u++) {
            if (dist[u] >= INF) continue;
            for (const Arc &a : adj[u]) {
                if (vis[a.id]) continue;
                ll total = dist[u] + a.c;
                if (total > rem) continue;
                double val = (double)Len[a.id] / ((double)total + 1e-9);
                if (val > bestVal) {
                    bestVal = val;
                    bu = u; bv = a.to; bid = a.id; bTotal = total;
                }
            }
        }
        if (bu < 0) continue; // this car is finished

        // Build chain from s to bu
        vector<int> chainNodes;
        vector<int> chainStreets;
        for (int x = bu; x != s; x = parNode[x]) {
            chainNodes.push_back(x);
            chainStreets.push_back(parStreet[x]);
        }
        reverse(chainNodes.begin(), chainNodes.end());
        reverse(chainStreets.begin(), chainStreets.end());

        if (paths[i].size() + chainNodes.size() + 1 > MAXV) continue;

        for (int k = 0; k < (int)chainNodes.size(); k++) {
            paths[i].push_back(chainNodes[k]);
            vis[chainStreets[k]] = 1;
        }
        paths[i].push_back(bv);
        vis[bid] = 1;
        cur[i] = bv;
        tm[i] = t + bTotal;
        pq.push({tm[i], i});
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
