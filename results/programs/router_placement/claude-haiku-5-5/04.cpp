#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int H, W, R;
    cin >> H >> W >> R;
    long long Pb, Pr, B;
    cin >> Pb >> Pr >> B;
    int br, bc;
    cin >> br >> bc;
    vector<string> g(H);
    for (int i = 0; i < H; ++i) cin >> g[i];

    const int N = H * W;
    const int INF = 1000000000;
    vector<char> wall(N, 0), target(N, 0);
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c) {
            char ch = g[r][c];
            int id = r * W + c;
            if (ch == '#') wall[id] = 1;
            else if (ch == '.') target[id] = 1;
        }

    // 2D prefix sums of walls, 1-based
    vector<int> P((H + 1) * (W + 1), 0);
    auto PI = [&](int r, int c) -> int& { return P[r * (W + 1) + c]; };
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c)
            PI(r + 1, c + 1) = PI(r, c + 1) + PI(r + 1, c) - PI(r, c) + (wall[r * W + c] ? 1 : 0);

    auto rectWalls = [&](int r1, int r2, int c1, int c2) -> int {
        return PI(r2 + 1, c2 + 1) - PI(r1, c2 + 1) - PI(r2 + 1, c1) + PI(r1, c1);
    };

    vector<char> covered(N, 0), placed(N, 0);
    vector<int> dist(N, INF), par(N, -1);

    auto relax = [&](vector<int> q) {
        size_t head = 0;
        while (head < q.size()) {
            int cur = q[head++];
            int cr = cur / W, cc = cur % W;
            int nd = dist[cur] + 1;
            for (int dr = -1; dr <= 1; ++dr)
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = cr + dr, nc = cc + dc;
                    if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                    int n = nr * W + nc;
                    if (nd < dist[n]) {
                        dist[n] = nd;
                        par[n] = cur;
                        q.push_back(n);
                    }
                }
        }
    };

    int start = br * W + bc;
    dist[start] = 0;
    relax(vector<int>{start});

    auto gainOf = [&](int id) -> int {
        int a = id / W, b = id % W;
        int r0 = max(0, a - R), r1 = min(H - 1, a + R);
        int c0 = max(0, b - R), c1 = min(W - 1, b + R);
        int cnt = 0;
        for (int x = r0; x <= r1; ++x)
            for (int y = c0; y <= c1; ++y) {
                int t = x * W + y;
                if (!target[t] || covered[t]) continue;
                if (rectWalls(min(a, x), max(a, x), min(b, y), max(b, y)) == 0) ++cnt;
            }
        return cnt;
    };

    auto cover = [&](int id) {
        int a = id / W, b = id % W;
        int r0 = max(0, a - R), r1 = min(H - 1, a + R);
        int c0 = max(0, b - R), c1 = min(W - 1, b + R);
        for (int x = r0; x <= r1; ++x)
            for (int y = c0; y <= c1; ++y) {
                int t = x * W + y;
                if (!target[t] || covered[t]) continue;
                if (rectWalls(min(a, x), max(a, x), min(b, y), max(b, y)) == 0) covered[t] = 1;
            }
    };

    priority_queue<pair<double, int>> pq;
    for (int id = 0; id < N; ++id) {
        if (wall[id]) continue;
        long long cost = (long long)dist[id] * Pb + Pr;
        if (cost > B) continue;
        int gn = gainOf(id);
        if (gn > 0) pq.push({gn * 1000.0 / cost, id});
    }

    long long spent = 0;
    vector<int> backbone;   // added cells in order (excluding start)
    vector<int> routers;

    while (!pq.empty()) {
        auto [key, id] = pq.top();
        pq.pop();
        if (placed[id]) continue;
        long long d = dist[id];
        long long cost = d * Pb + Pr;
        if (spent + cost > B) continue;
        int gn = gainOf(id);
        if (gn == 0) continue;
        double nk = gn * 1000.0 / cost;
        if (!pq.empty() && nk < pq.top().first) {
            pq.push({nk, id});
            continue;
        }
        // apply placement
        vector<int> path;
        int cur = id;
        while (dist[cur] > 0) {
            path.push_back(cur);
            cur = par[cur];
        }
        for (int i = (int)path.size() - 1; i >= 0; --i) backbone.push_back(path[i]);
        for (int c : path) dist[c] = 0;
        spent += cost;
        placed[id] = 1;
        routers.push_back(id);
        cover(id);
        if (!path.empty()) relax(path);
    }

    // output
    string out;
    out.reserve(1 << 22);
    out += to_string(backbone.size()) + "\n";
    for (int id : backbone) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    out += to_string(routers.size()) + "\n";
    for (int id : routers) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
