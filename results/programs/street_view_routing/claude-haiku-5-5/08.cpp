#include <bits/stdc++.h>
using namespace std;

struct Arc {
    int to, cost, len, eid;
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M, T, C, S;
    cin >> N >> M >> T >> C >> S;

    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }

    vector<vector<Arc>> adj(N);
    for (int j = 0; j < M; j++) {
        int A, B, D, Cc, L;
        cin >> A >> B >> D >> Cc >> L;
        adj[A].push_back({B, Cc, L, j});
        if (D == 2) adj[B].push_back({A, Cc, L, j});
    }

    vector<char> visited(M, 0);
    vector<int> cur(C, S), elapsed(C, 0);
    vector<char> active(C, 1);
    vector<vector<int>> itin(C, vector<int>(1, S));

    const int INF = INT_MAX / 2;
    vector<int> dist(N), par(N), pe(N);
    long long work = 0;
    const long long BUDGET = 400000000LL;

    while (true) {
        if (work > BUDGET) break;

        // pick active car with minimal elapsed time
        int c = -1;
        for (int i = 0; i < C; i++) {
            if (!active[i]) continue;
            if (c == -1 || elapsed[i] < elapsed[c]) c = i;
        }
        if (c == -1) break;

        int rem = T - elapsed[c];
        if (rem <= 0) {
            active[c] = 0;
            continue;
        }
        int src = cur[c];

        fill(dist.begin(), dist.end(), INF);
        fill(par.begin(), par.end(), -1);
        fill(pe.begin(), pe.end(), -1);
        dist[src] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, src});

        double bestScore = -1.0;
        int bestU = -1, bestK = -1, bestCost = 0;

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d != dist[u]) continue;
            if (d > rem) break;

            // evaluate candidate unvisited streets leaving u
            for (int k = 0; k < (int)adj[u].size(); k++) {
                const Arc &a = adj[u][k];
                if (visited[a.eid]) continue;
                if (a.to == par[u]) continue;
                int tot = d + a.cost;
                if (tot > rem) continue;
                double sc = (double)a.len / (double)tot;
                if (sc > bestScore) {
                    bestScore = sc;
                    bestU = u;
                    bestK = k;
                    bestCost = a.cost;
                }
            }

            // relax
            work += adj[u].size();
            for (const Arc &a : adj[u]) {
                int nd = d + a.cost;
                if (nd <= rem && nd < dist[a.to]) {
                    dist[a.to] = nd;
                    par[a.to] = u;
                    pe[a.to] = a.eid;
                    pq.push({nd, a.to});
                }
            }
            if (work > BUDGET) break;
        }

        if (bestU == -1) {
            active[c] = 0;
            continue;
        }

        // reconstruct path to bestU
        vector<int> nodes, eds;
        int v = bestU;
        while (v != src) {
            nodes.push_back(v);
            eds.push_back(pe[v]);
            v = par[v];
        }
        reverse(nodes.begin(), nodes.end());
        reverse(eds.begin(), eds.end());
        for (size_t i = 0; i < nodes.size(); i++) {
            itin[c].push_back(nodes[i]);
            visited[eds[i]] = 1;
        }

        const Arc &ta = adj[bestU][bestK];
        itin[c].push_back(ta.to);
        visited[ta.eid] = 1;
        elapsed[c] += dist[bestU] + bestCost;
        cur[c] = ta.to;
    }

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
