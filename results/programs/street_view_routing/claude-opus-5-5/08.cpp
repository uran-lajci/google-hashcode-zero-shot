#include <bits/stdc++.h>
using namespace std;

struct E { int to; long long cost; long long len; int id; };
int N, M, C, S; long long T;
vector<vector<E>> adj;
vector<char> vis;
vector<int> usedTmp;
double bestRatio; int bestFirst;
chrono::steady_clock::time_point st;

void dfs(int u, int d, long long tLeft, long long g, long long c, int first) {
    for (int k = 0; k < (int)adj[u].size(); k++) {
        const E& e = adj[u][k];
        if (e.cost > tLeft) continue;
        long long ng = g + ((!vis[e.id] && !usedTmp[e.id]) ? e.len : 0);
        long long nc = c + e.cost;
        int f = first < 0 ? k : first;
        if (ng > 0) {
            double r = (double)ng / (double)max(nc, 1LL) ;
            if (nc == 0) r = 1e18;
            if (r > bestRatio) { bestRatio = r; bestFirst = f; }
        }
        if (d > 1) {
            usedTmp[e.id]++;
            dfs(e.to, d - 1, tLeft - e.cost, ng, nc, f);
            usedTmp[e.id]--;
        }
    }
}

int main() {
    st = chrono::steady_clock::now();
    if (scanf("%d %d %lld %d %d", &N, &M, &T, &C, &S) != 5) return 0;
    for (int i = 0; i < N; i++) { double a, b; scanf("%lf %lf", &a, &b); }
    adj.assign(N, {});
    for (int j = 0; j < M; j++) {
        int a, b, d; long long c, l;
        scanf("%d %d %d %lld %lld", &a, &b, &d, &c, &l);
        adj[a].push_back({b, c, l, j});
        if (d == 2) adj[b].push_back({a, c, l, j});
    }
    vis.assign(M, 0); usedTmp.assign(M, 0);
    vector<int> pos(C, S); vector<long long> tm(C, 0); vector<char> done(C, 0);
    vector<vector<int>> path(C, vector<int>{S});
    vector<long long> dist(N); vector<int> parE(N), parN(N), stamp(N, -1);
    int stampId = 0;
    while (true) {
        int ci = -1;
        for (int i = 0; i < C; i++) if (!done[i] && (ci < 0 || tm[i] < tm[ci])) ci = i;
        if (ci < 0) break;
        int u = pos[ci]; long long tLeft = T - tm[ci];
        double el = chrono::duration<double>(chrono::steady_clock::now() - st).count();
        int depth = el < 5.0 ? 8 : (el < 7.5 ? 6 : 4);
        bestRatio = -1; bestFirst = -1;
        dfs(u, depth, tLeft, 0, 0, -1);
        if (bestFirst >= 0) {
            const E& e = adj[u][bestFirst];
            vis[e.id] = 1; tm[ci] += e.cost; pos[ci] = e.to; path[ci].push_back(e.to);
            continue;
        }
        // Dijkstra to nearest unvisited edge
        stampId++;
        priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<>> pq;
        stamp[u] = stampId; dist[u] = 0; parE[u] = -1; parN[u] = -1;
        pq.push({0, u});
        int tgtNode = -1, tgtEdge = -1;
        while (!pq.empty()) {
            auto [dd, x] = pq.top(); pq.pop();
            if (dd != dist[x]) continue;
            int bk = -1; double br = -1;
            for (int k = 0; k < (int)adj[x].size(); k++) {
                const E& e = adj[x][k];
                if (!vis[e.id] && dd + e.cost <= tLeft) {
                    double r = (double)e.len / max(e.cost, 1LL);
                    if (r > br) { br = r; bk = k; }
                }
            }
            if (bk >= 0) { tgtNode = x; tgtEdge = bk; break; }
            for (auto& e : adj[x]) {
                long long nd = dd + e.cost;
                if (nd > tLeft) continue;
                if (stamp[e.to] != stampId || nd < dist[e.to]) {
                    stamp[e.to] = stampId; dist[e.to] = nd; parE[e.to] = e.id; parN[e.to] = x;
                    pq.push({nd, e.to});
                }
            }
        }
        if (tgtNode < 0) { done[ci] = 1; continue; }
        vector<int> seq;
        for (int x = tgtNode; x != u; x = parN[x]) seq.push_back(x);
        reverse(seq.begin(), seq.end());
        for (int x : seq) path[ci].push_back(x);
        tm[ci] += dist[tgtNode];
        const E& e = adj[tgtNode][tgtEdge];
        vis[e.id] = 1; tm[ci] += e.cost; pos[ci] = e.to; path[ci].push_back(e.to);
    }
    string out;
    out += to_string(C) + "\n";
    for (int i = 0; i < C; i++) {
        out += to_string(path[i].size()) + "\n";
        for (int x : path[i]) { out += to_string(x); out += '\n'; }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
