#include <bits/stdc++.h>
using namespace std;

int H, W, R;
vector<char> g;
vector<char> covered;

template<class F>
void forCov(int a, int b, F f) {
    for (int s = 1; s >= -1; s -= 2)
        for (int t = 1; t >= -1; t -= 2) {
            int lim = R;
            for (int k = 0; k <= R; k++) {
                int x = a + s * k;
                if (x < 0 || x >= H) break;
                int j = 0;
                while (j <= lim) {
                    int y = b + t * j;
                    if (y < 0 || y >= W || g[x * W + y] == '#') break;
                    if ((k > 0 || s == 1) && (j > 0 || t == 1)) f(x * W + y);
                    j++;
                }
                lim = j - 1;
                if (lim < 0) break;
            }
        }
}

int gainOf(int c) {
    int a = c / W, b = c % W;
    int cnt = 0;
    forCov(a, b, [&](int id) { if (g[id] == '.' && !covered[id]) cnt++; });
    return cnt;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long Pb, Pr, B;
    int br, bc;
    cin >> H >> W >> R;
    cin >> Pb >> Pr >> B;
    cin >> br >> bc;
    int N = H * W;
    g.assign(N, '-');
    for (int i = 0; i < H; i++) {
        string s;
        cin >> s;
        for (int j = 0; j < W && j < (int)s.size(); j++) g[i * W + j] = s[j];
    }
    covered.assign(N, 0);
    vector<char> isRouter(N, 0), isBack(N, 0);
    vector<int> dist(N, INT_MAX), par(N, -1);
    int src = br * W + bc;
    isBack[src] = 1;
    dist[src] = 0;
    {
        vector<int> q;
        q.reserve(N);
        q.push_back(src);
        for (size_t qi = 0; qi < q.size(); qi++) {
            int u = q[qi];
            int ux = u / W, uy = u % W;
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++) {
                    if (!dx && !dy) continue;
                    int nx = ux + dx, ny = uy + dy;
                    if (nx < 0 || ny < 0 || nx >= H || ny >= W) continue;
                    int n = nx * W + ny;
                    if (dist[n] > dist[u] + 1) {
                        dist[n] = dist[u] + 1;
                        par[n] = u;
                        q.push_back(n);
                    }
                }
        }
    }
    vector<int> lastGain(N, 0);
    priority_queue<pair<double, int>> pq;
    for (int c = 0; c < N; c++) {
        if (g[c] == '#') continue;
        int gg = gainOf(c);
        lastGain[c] = gg;
        if (gg > 0) pq.push({gg / (double)(Pr + Pb * dist[c]), c});
    }
    long long remaining = B;
    vector<int> backList, routers;
    vector<int> bq;
    while (!pq.empty() && remaining >= Pr) {
        auto [k, c] = pq.top();
        pq.pop();
        if (isRouter[c]) continue;
        int gnow = gainOf(c);
        lastGain[c] = gnow;
        if (gnow == 0) continue;
        double key = gnow / (double)(Pr + Pb * dist[c]);
        if (!pq.empty() && key < pq.top().first - 1e-12) {
            pq.push({key, c});
            continue;
        }
        // build path
        vector<int> path;
        int u = c;
        while (!isBack[u]) { path.push_back(u); u = par[u]; }
        long long cost = Pr + Pb * (long long)path.size();
        if (cost > remaining) continue;
        remaining -= cost;
        reverse(path.begin(), path.end());
        bq.clear();
        for (int p : path) {
            backList.push_back(p);
            isBack[p] = 1;
            dist[p] = 0;
            bq.push_back(p);
        }
        isRouter[c] = 1;
        routers.push_back(c);
        forCov(c / W, c % W, [&](int id) { covered[id] = 1; });
        // incremental BFS
        for (size_t qi = 0; qi < bq.size(); qi++) {
            int v = bq[qi];
            int vx = v / W, vy = v % W;
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++) {
                    if (!dx && !dy) continue;
                    int nx = vx + dx, ny = vy + dy;
                    if (nx < 0 || ny < 0 || nx >= H || ny >= W) continue;
                    int n = nx * W + ny;
                    if (dist[n] > dist[v] + 1) {
                        dist[n] = dist[v] + 1;
                        par[n] = v;
                        bq.push_back(n);
                        if (g[n] != '#' && !isRouter[n] && lastGain[n] > 0)
                            pq.push({lastGain[n] / (double)(Pr + Pb * dist[n]), n});
                    }
                }
        }
        // also router cells on path with dist 0 may be candidates: push them
        for (int p : path) {
            if (g[p] != '#' && !isRouter[p] && lastGain[p] > 0)
                pq.push({lastGain[p] / (double)(Pr), p});
        }
    }
    string out;
    out.reserve((backList.size() + routers.size()) * 12 + 32);
    out += to_string(backList.size());
    out += '\n';
    for (int p : backList) {
        out += to_string(p / W);
        out += ' ';
        out += to_string(p % W);
        out += '\n';
    }
    out += to_string(routers.size());
    out += '\n';
    for (int p : routers) {
        out += to_string(p / W);
        out += ' ';
        out += to_string(p % W);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
