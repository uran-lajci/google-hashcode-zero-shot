#include <bits/stdc++.h>
using namespace std;

struct E { int v; int c; int len; int id; };
int N, M, T, C, S;
vector<vector<E>> adj;
vector<char> vis;
long long nodeCount = 0;

double bestRatio; int bestFirst; long long bestGain;
int remainT;

void dfs(int u, int depth, int tUsed, long long gained, int first) {
    for (int i = 0; i < (int)adj[u].size(); i++) {
        const E& e = adj[u][i];
        int nt = tUsed + e.c;
        if (nt > remainT) continue;
        nodeCount++;
        bool was = vis[e.id];
        long long g = gained + (was ? 0 : e.len);
        int f = (first < 0) ? i : first;
        if (g > 0) {
            double r = (double)g / (double)max(nt, 1);
            if (r > bestRatio + 1e-12) { bestRatio = r; bestFirst = f; bestGain = g; }
        }
        if (depth > 1) {
            vis[e.id] = 1;
            dfs(e.v, depth - 1, nt, g, f);
            vis[e.id] = was;
        }
    }
}

int main() {
    if (scanf("%d %d %d %d %d", &N, &M, &T, &C, &S) != 5) return 0;
    for (int i = 0; i < N; i++) { double a, b; scanf("%lf %lf", &a, &b); }
    adj.assign(N, {});
    for (int j = 0; j < M; j++) {
        int a, b, d, c, l; scanf("%d %d %d %d %d", &a, &b, &d, &c, &l);
        adj[a].push_back({b, c, l, j});
        if (d == 2) adj[b].push_back({a, c, l, j});
    }
    vis.assign(M, 0);
    vector<vector<int>> route(C, vector<int>{S});
    vector<long long> tm(C, 0);
    vector<int> pos(C, S);
    vector<char> done(C, 0);
    vector<long long> dist(N);
    vector<int> par(N), parEdge(N);
    const long long INF = LLONG_MAX / 4;

    while (true) {
        int k = -1;
        for (int i = 0; i < C; i++) if (!done[i] && (k < 0 || tm[i] < tm[k])) k = i;
        if (k < 0) break;
        int u = pos[k];
        remainT = (int)(T - tm[k]);
        int depth = 7;
        if (nodeCount > 150000000LL) depth = 5;
        if (nodeCount > 300000000LL) depth = 3;
        bestRatio = -1; bestFirst = -1; bestGain = 0;
        dfs(u, depth, 0, 0, -1);
        if (bestFirst >= 0 && bestGain > 0) {
            const E& e = adj[u][bestFirst];
            vis[e.id] = 1;
            tm[k] += e.c;
            pos[k] = e.v;
            route[k].push_back(e.v);
            continue;
        }
        // Dijkstra to nearest unvisited edge
        vector<int> touched;
        priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<>> pq;
        dist[u] = 0; par[u] = -1; touched.push_back(u);
        // initialize: use dist array lazily
        vector<char> seen; // not used
        pq.push({0, u});
        int tgtNode = -1, tgtEdge = -1;
        static vector<char> inT; if (inT.empty()) inT.assign(N, 0);
        static vector<char> closed; if (closed.empty()) closed.assign(N, 0);
        inT[u] = 1;
        while (!pq.empty()) {
            auto [d, x] = pq.top(); pq.pop();
            if (closed[x] || d != dist[x]) continue;
            closed[x] = 1;
            // check unvisited edges from x
            int bi = -1;
            for (int i = 0; i < (int)adj[x].size(); i++) {
                const E& e = adj[x][i];
                if (!vis[e.id] && d + e.c <= remainT) {
                    if (bi < 0 || (double)e.len / e.c > (double)adj[x][bi].len / adj[x][bi].c) bi = i;
                }
            }
            if (bi >= 0) { tgtNode = x; tgtEdge = bi; break; }
            for (int i = 0; i < (int)adj[x].size(); i++) {
                const E& e = adj[x][i];
                long long nd = d + e.c;
                if (nd > remainT) continue;
                if (!inT[e.v]) { inT[e.v] = 1; touched.push_back(e.v); dist[e.v] = INF; }
                if (nd < dist[e.v]) { dist[e.v] = nd; par[e.v] = x; parEdge[e.v] = i; pq.push({nd, e.v}); }
            }
        }
        if (tgtNode < 0) {
            done[k] = 1;
        } else {
            vector<int> path;
            int x = tgtNode;
            while (x != u) { path.push_back(x); x = par[x]; }
            reverse(path.begin(), path.end());
            int cur = u;
            for (int y : path) {
                const E& e = adj[par[y]][parEdge[y]];
                (void)cur;
                vis[e.id] = 1;
                tm[k] += e.c;
                route[k].push_back(y);
                cur = y;
            }
            const E& e = adj[tgtNode][tgtEdge];
            vis[e.id] = 1;
            tm[k] += e.c;
            route[k].push_back(e.v);
            pos[k] = e.v;
        }
        for (int y : touched) { inT[y] = 0; closed[y] = 0; }
    }
    string out;
    out.reserve(1 << 20);
    out += to_string(C) + "\n";
    for (int i = 0; i < C; i++) {
        out += to_string(route[i].size()) + "\n";
        for (int v : route[i]) { out += to_string(v); out += '\n'; }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
