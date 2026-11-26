#include <bits/stdc++.h>
using namespace std;

struct Arc { int to, id; long long c; };
int N, M, C, S; long long T;
vector<vector<Arc>> adj;
vector<long long> Lval;
vector<char> vis;

int DEPTH = 9; long long CAP = 30000;
long long nodesCnt;
double bestR; int bestFirst; long long remT;

void dfs(int u, int d, long long tu, long long g, int first) {
    for (int k = 0; k < (int)adj[u].size(); k++) {
        const Arc& a = adj[u][k];
        if (tu + a.c > remT) continue;
        if (nodesCnt >= CAP) return;
        nodesCnt++;
        bool nw = !vis[a.id];
        long long ng = g + (nw ? Lval[a.id] : 0);
        int f = (d == 0) ? k : first;
        if (ng > 0) {
            double r = (double)ng / (double)(tu + a.c + 1e-9);
            if (r > bestR) { bestR = r; bestFirst = f; }
        }
        if (d + 1 < DEPTH) {
            if (nw) vis[a.id] = 1;
            dfs(a.to, d + 1, tu + a.c, ng, f);
            if (nw) vis[a.id] = 0;
        }
    }
}

int main() {
    scanf("%d %d %lld %d %d", &N, &M, &T, &C, &S);
    for (int i = 0; i < N; i++) { double a, b; scanf("%lf %lf", &a, &b); }
    adj.assign(N, {});
    Lval.resize(M); vis.assign(M, 0);
    for (int j = 0; j < M; j++) {
        int a, b, d; long long c, l;
        scanf("%d %d %d %lld %lld", &a, &b, &d, &c, &l);
        Lval[j] = l;
        adj[a].push_back({b, j, c});
        if (d == 2) adj[b].push_back({a, j, c});
    }
    vector<vector<int>> path(C, vector<int>{S});
    vector<long long> tm(C, 0);
    vector<int> pos(C, S);
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<>> pq;
    for (int i = 0; i < C; i++) pq.push({0, i});
    vector<long long> dist(N, LLONG_MAX);
    vector<int> prevArcNode(N, -1), prevArcIdx(N, -1);
    vector<int> touched;
    const size_t MAXV = 1000000;
    while (!pq.empty()) {
        auto [t, ci] = pq.top(); pq.pop();
        int u = pos[ci];
        remT = T - tm[ci];
        if (path[ci].size() >= MAXV) continue;
        bestR = 0; bestFirst = -1; nodesCnt = 0;
        dfs(u, 0, 0, 0, -1);
        if (bestFirst >= 0) {
            const Arc& a = adj[u][bestFirst];
            vis[a.id] = 1;
            tm[ci] += a.c; pos[ci] = a.to; path[ci].push_back(a.to);
            pq.push({tm[ci], ci});
            continue;
        }
        // Dijkstra fallback
        for (int x : touched) { dist[x] = LLONG_MAX; prevArcNode[x] = -1; prevArcIdx[x] = -1; }
        touched.clear();
        priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<>> dq;
        dist[u] = 0; touched.push_back(u); dq.push({0, u});
        int target = -1;
        while (!dq.empty()) {
            auto [dd, x] = dq.top(); dq.pop();
            if (dd != dist[x]) continue;
            bool ok = false;
            if (x != u) {
                for (auto& a : adj[x]) if (!vis[a.id] && dd + a.c <= remT) { ok = true; break; }
            }
            if (ok) { target = x; break; }
            for (int k = 0; k < (int)adj[x].size(); k++) {
                const Arc& a = adj[x][k];
                long long nd = dd + a.c;
                if (nd > remT) continue;
                if (nd < dist[a.to]) {
                    if (dist[a.to] == LLONG_MAX) touched.push_back(a.to);
                    dist[a.to] = nd; prevArcNode[a.to] = x; prevArcIdx[a.to] = k;
                    dq.push({nd, a.to});
                }
            }
        }
        if (target < 0) continue; // car stops
        vector<int> seq;
        int x = target;
        while (x != u) { seq.push_back(x); x = prevArcNode[x]; }
        reverse(seq.begin(), seq.end());
        int cur = u;
        bool stop = false;
        for (int y : seq) {
            if (path[ci].size() >= MAXV) { stop = true; break; }
            const Arc& a = adj[prevArcNode[y]][prevArcIdx[y]];
            vis[a.id] = 1;
            tm[ci] += a.c; cur = y; path[ci].push_back(y);
        }
        pos[ci] = cur;
        if (!stop) pq.push({tm[ci], ci});
    }
    string out;
    out.reserve(1 << 22);
    out += to_string(C) + "\n";
    for (int i = 0; i < C; i++) {
        out += to_string(path[i].size()) + "\n";
        for (int v : path[i]) { out += to_string(v); out += '\n'; }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
