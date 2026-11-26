#include <bits/stdc++.h>
using namespace std;

struct Arc { int to, id, c; };
int N, M, T, C, S;
vector<vector<Arc>> adj;
vector<long long> elen;
vector<char> vis;
long long ops = 0;
mt19937_64 rng;
double noise = 0;

double bestVal; int bestFirst;
const int DEPTH = 6;

void dfs(int u, int t, int depth, long long sumLen, long long sumCost, int first, double fac) {
    for (int k = 0; k < (int)adj[u].size(); k++) {
        const Arc& a = adj[u][k];
        if (t + a.c > T) continue;
        ops++;
        int f = first < 0 ? k : first;
        double ff = fac;
        if (first < 0 && noise > 0) ff = 1.0 + noise * (double)(rng() % 10000) / 10000.0;
        long long nl = vis[a.id] ? 0 : elen[a.id];
        long long sl = sumLen + nl, sc = sumCost + a.c;
        double val = (double)sl / (double)(sc + 1e-9) * ff;
        if (sl > 0 && val > bestVal) { bestVal = val; bestFirst = f; }
        if (depth > 1) {
            bool was = vis[a.id];
            vis[a.id] = 1;
            dfs(a.to, t + a.c, depth - 1, sl, sc, f, ff);
            vis[a.id] = was;
        }
    }
}

vector<long long> dist; vector<int> stampv; int stampc = 0;
vector<int> parNode, parArc;

struct Result { long long score; vector<vector<int>> paths; };

Result run() {
    fill(vis.begin(), vis.end(), 0);
    vector<int> tim(C, 0), pos(C, S);
    vector<char> done(C, 0);
    vector<vector<int>> paths(C, vector<int>{S});
    long long score = 0;
    auto take = [&](int car, const Arc& a) {
        if (!vis[a.id]) { vis[a.id] = 1; score += elen[a.id]; }
        tim[car] += a.c; pos[car] = a.to; paths[car].push_back(a.to);
    };
    while (true) {
        int car = -1;
        for (int i = 0; i < C; i++) if (!done[i] && (car < 0 || tim[i] < tim[car])) car = i;
        if (car < 0) break;
        int u = pos[car], t = tim[car];
        bestVal = 0; bestFirst = -1;
        dfs(u, t, DEPTH, 0, 0, -1, 1.0);
        if (bestFirst >= 0) { Arc a = adj[u][bestFirst]; take(car, a); continue; }
        // Dijkstra
        stampc++;
        priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<>> pq;
        dist[u] = 0; stampv[u] = stampc; parNode[u] = -1; pq.push({0, u});
        int target = -1;
        while (!pq.empty()) {
            auto [d, x] = pq.top(); pq.pop();
            if (d != dist[x]) continue;
            ops++;
            bool good = false;
            for (auto& a : adj[x]) if (!vis[a.id] && t + d + a.c <= T) { good = true; break; }
            if (good && x != u) { target = x; break; }
            for (int k = 0; k < (int)adj[x].size(); k++) {
                const Arc& a = adj[x][k];
                long long nd = d + a.c;
                if (t + nd > T) continue;
                if (stampv[a.to] != stampc || nd < dist[a.to]) {
                    stampv[a.to] = stampc; dist[a.to] = nd; parNode[a.to] = x; parArc[a.to] = k;
                    pq.push({nd, a.to});
                }
            }
        }
        if (target < 0) { done[car] = 1; continue; }
        vector<pair<int,int>> seq;
        for (int x = target; x != u; x = parNode[x]) seq.push_back({parNode[x], parArc[x]});
        reverse(seq.begin(), seq.end());
        for (auto& p : seq) { Arc a = adj[p.first][p.second]; take(car, a); }
    }
    return {score, paths};
}

int main() {
    if (scanf("%d %d %d %d %d", &N, &M, &T, &C, &S) != 5) return 0;
    for (int i = 0; i < N; i++) { double a, b; scanf("%lf %lf", &a, &b); }
    adj.assign(N, {}); elen.resize(M); vis.assign(M, 0);
    for (int j = 0; j < M; j++) {
        int a, b, d, c; long long l;
        scanf("%d %d %d %d %lld", &a, &b, &d, &c, &l);
        elen[j] = l;
        adj[a].push_back({b, j, c});
        if (d == 2) adj[b].push_back({a, j, c});
    }
    dist.assign(N, 0); stampv.assign(N, 0); parNode.assign(N, -1); parArc.assign(N, -1);
    rng.seed(12345);
    const long long BUDGET = 350000000LL;
    Result best = run();
    long long perRun = max(1LL, ops);
    int it = 0;
    while (ops + perRun * 13 / 10 < BUDGET && it < 200) {
        it++;
        noise = 0.05 + 0.25 * ((it % 5) / 4.0);
        long long before = ops;
        Result r = run();
        perRun = max(perRun, ops - before);
        if (r.score > best.score) best = move(r);
    }
    string out;
    out += to_string(C) + "\n";
    for (int i = 0; i < C; i++) {
        out += to_string(best.paths[i].size()) + "\n";
        for (int x : best.paths[i]) out += to_string(x) + "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
