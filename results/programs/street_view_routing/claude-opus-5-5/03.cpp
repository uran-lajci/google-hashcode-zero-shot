#include <bits/stdc++.h>
using namespace std;

struct Arc { int to; long long c; long long len; int id; };

int N, M, C, S;
long long T;
vector<vector<Arc>> out;
vector<char> vis;
long long work = 0;
int maxDepth = 8;

double bestRatio; long long bestGain; int bestFirst; long long carTime;

void dfs(int u, int depth, long long tu, long long gain, int first) {
    for (int k = 0; k < (int)out[u].size(); k++) {
        const Arc &a = out[u][k];
        long long nt = tu + a.c;
        if (carTime + nt > T) continue;
        work++;
        int f = (first < 0) ? k : first;
        bool newE = !vis[a.id];
        long long g = gain + (newE ? a.len : 0);
        if (newE) vis[a.id] = 1;
        if (g > 0) {
            double r = (nt > 0) ? (double)g / (double)nt : 1e18 + (double)g;
            if (r > bestRatio + 1e-12 || (fabs(r - bestRatio) <= 1e-12 && g > bestGain)) {
                bestRatio = r; bestGain = g; bestFirst = f;
            }
        }
        if (depth > 1) dfs(a.to, depth - 1, nt, g, f);
        if (newE) vis[a.id] = 0;
    }
}

int main() {
    if (scanf("%d %d %lld %d %d", &N, &M, &T, &C, &S) != 5) return 0;
    for (int i = 0; i < N; i++) { double a, b; scanf("%lf %lf", &a, &b); }
    out.assign(N, {});
    vis.assign(M, 0);
    for (int j = 0; j < M; j++) {
        int a, b, d; long long c, l;
        scanf("%d %d %d %lld %lld", &a, &b, &d, &c, &l);
        out[a].push_back({b, c, l, j});
        if (d == 2) out[b].push_back({a, c, l, j});
    }
    const size_t MAXV = 1000000;
    vector<vector<int>> path(C, vector<int>{S});
    vector<long long> tm(C, 0);
    vector<int> pos(C, S);
    vector<char> done(C, 0);

    vector<long long> dist(N, LLONG_MAX);
    vector<int> parNode(N, -1), parArc(N, -1);
    vector<int> touched;

    const long long BUDGET = 600000000LL;
    while (true) {
        int ci = -1;
        for (int i = 0; i < C; i++) if (!done[i] && (ci < 0 || tm[i] < tm[ci])) ci = i;
        if (ci < 0) break;
        if (path[ci].size() >= MAXV) { done[ci] = 1; continue; }
        if (work > BUDGET && maxDepth > 1) { maxDepth--; work = BUDGET / 2; }
        int u = pos[ci];
        carTime = tm[ci];
        bestRatio = -1; bestGain = 0; bestFirst = -1;
        dfs(u, maxDepth, 0, 0, -1);
        if (bestFirst >= 0) {
            const Arc &a = out[u][bestFirst];
            vis[a.id] = 1;
            tm[ci] += a.c;
            pos[ci] = a.to;
            path[ci].push_back(a.to);
            continue;
        }
        // Dijkstra to nearest node with unvisited out-edge that fits
        for (int x : touched) { dist[x] = LLONG_MAX; parNode[x] = -1; parArc[x] = -1; }
        touched.clear();
        priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<>> pq;
        dist[u] = 0; touched.push_back(u); pq.push({0, u});
        int target = -1;
        while (!pq.empty()) {
            auto [d, x] = pq.top(); pq.pop();
            if (d != dist[x]) continue;
            if (tm[ci] + d > T) break;
            bool ok = false;
            if (x != u) {
                for (auto &a : out[x]) if (!vis[a.id] && tm[ci] + d + a.c <= T) { ok = true; break; }
            }
            if (ok) { target = x; break; }
            for (int k = 0; k < (int)out[x].size(); k++) {
                const Arc &a = out[x][k];
                long long nd = d + a.c;
                if (tm[ci] + nd > T) continue;
                if (nd < dist[a.to]) {
                    if (dist[a.to] == LLONG_MAX) touched.push_back(a.to);
                    dist[a.to] = nd; parNode[a.to] = x; parArc[a.to] = k;
                    pq.push({nd, a.to});
                }
            }
        }
        if (target < 0) { done[ci] = 1; continue; }
        vector<pair<int,int>> seq;
        int x = target;
        while (x != u) { seq.push_back({parNode[x], parArc[x]}); x = parNode[x]; }
        reverse(seq.begin(), seq.end());
        for (auto &pr : seq) {
            if (path[ci].size() >= MAXV) break;
            const Arc &a = out[pr.first][pr.second];
            if (tm[ci] + a.c > T) break;
            vis[a.id] = 1;
            tm[ci] += a.c;
            pos[ci] = a.to;
            path[ci].push_back(a.to);
        }
    }

    string buf;
    buf.reserve(1 << 20);
    buf += to_string(C) + "\n";
    for (int i = 0; i < C; i++) {
        buf += to_string(path[i].size()) + "\n";
        for (int v : path[i]) { buf += to_string(v); buf += '\n'; }
    }
    fwrite(buf.data(), 1, buf.size(), stdout);
    return 0;
}
