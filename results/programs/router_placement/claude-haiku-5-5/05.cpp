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
    for (int i = 0; i < H; i++) cin >> g[i];

    const int n = H * W;
    const int INF = INT_MAX / 2;
    const int S = W + 1;

    // 2D prefix sums for walls and targets
    vector<int> wps((H + 1) * S, 0), tps((H + 1) * S, 0);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            int w = (g[r][c] == '#');
            int t = (g[r][c] == '.');
            wps[(r + 1) * S + c + 1] = wps[r * S + c + 1] + wps[(r + 1) * S + c] - wps[r * S + c] + w;
            tps[(r + 1) * S + c + 1] = tps[r * S + c + 1] + tps[(r + 1) * S + c] - tps[r * S + c] + t;
        }
    }
    auto rc = [&](const vector<int>& ps, int r1, int c1, int r2, int c2) -> int {
        return ps[(r2 + 1) * S + (c2 + 1)] - ps[r1 * S + (c2 + 1)] - ps[(r2 + 1) * S + c1] + ps[r1 * S + c1];
    };

    vector<char> covered(n, 0), isR(n, 0);

    // Count uncovered targets a router at (a,b) would newly cover
    auto gainOf = [&](int a, int b) -> int {
        int cnt = 0;
        int r1 = max(0, a - R), r2 = min(H - 1, a + R);
        int c1 = max(0, b - R), c2 = min(W - 1, b + R);
        for (int x = r1; x <= r2; x++)
            for (int y = c1; y <= c2; y++) {
                int id = x * W + y;
                if (g[x][y] != '.' || covered[id]) continue;
                if (rc(wps, min(a, x), min(b, y), max(a, x), max(b, y)) == 0) cnt++;
            }
        return cnt;
    };
    auto applyCover = [&](int a, int b) {
        int r1 = max(0, a - R), r2 = min(H - 1, a + R);
        int c1 = max(0, b - R), c2 = min(W - 1, b + R);
        for (int x = r1; x <= r2; x++)
            for (int y = c1; y <= c2; y++) {
                int id = x * W + y;
                if (g[x][y] != '.' || covered[id]) continue;
                if (rc(wps, min(a, x), min(b, y), max(a, x), max(b, y)) == 0) covered[id] = 1;
            }
    };

    // Distance (number of new backbone cells needed) from current backbone
    vector<int> dist(n, INF);
    int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    {
        vector<int> q;
        q.reserve(n);
        int s = br * W + bc;
        dist[s] = 0;
        q.push_back(s);
        for (size_t h = 0; h < q.size(); h++) {
            int cur = q[h];
            int r = cur / W, c = cur % W;
            for (int k = 0; k < 8; k++) {
                int nr = r + dr8[k], nc = c + dc8[k];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int nb = nr * W + nc;
                if (dist[nb] > dist[cur] + 1) {
                    dist[nb] = dist[cur] + 1;
                    q.push_back(nb);
                }
            }
        }
    }

    // Initial candidate heap with upper-bound keys
    priority_queue<pair<double, int>> pq;
    for (int idx = 0; idx < n; idx++) {
        int a = idx / W, b = idx % W;
        if (g[a][b] == '#') continue;
        int r1 = max(0, a - R), r2 = min(H - 1, a + R);
        int c1 = max(0, b - R), c2 = min(W - 1, b + R);
        int ub = rc(tps, r1, c1, r2, c2);
        if (ub > 0) pq.push({ub / (double)Pr, idx});
    }

    long long remaining = B;
    vector<int> outB;     // backbone cells added, in valid connection order
    vector<int> routers;

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int idx = top.second;
        if (isR[idx]) continue;
        int a = idx / W, b = idx % W;
        if (g[a][b] == '#') continue;
        int d = dist[idx];
        long long cost = (long long)d * Pb + Pr;
        if (cost > remaining) continue;
        int gain = gainOf(a, b);
        if (gain == 0) continue;
        double ratio = (double)gain / (double)cost;
        if (!pq.empty() && ratio < pq.top().first) {
            pq.push({ratio, idx});
            continue;
        }

        // Build path from backbone to idx
        vector<int> path;
        int cur = idx;
        while (dist[cur] > 0) {
            path.push_back(cur);
            int cr = cur / W, cc = cur % W;
            int nxt = -1;
            for (int k = 0; k < 8; k++) {
                int nr = cr + dr8[k], nc = cc + dc8[k];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int nb = nr * W + nc;
                if (dist[nb] == dist[cur] - 1) { nxt = nb; break; }
            }
            cur = nxt;
        }
        // Commit path cells as backbone (reverse so that each cell follows its neighbor)
        vector<int> sources;
        for (int i = (int)path.size() - 1; i >= 0; i--) {
            int p = path[i];
            dist[p] = 0;
            outB.push_back(p);
            sources.push_back(p);
        }
        // Propagate distance decreases from new backbone cells
        {
            vector<int> q = sources;
            for (size_t h = 0; h < q.size(); h++) {
                int c0 = q[h];
                int r = c0 / W, c = c0 % W;
                for (int k = 0; k < 8; k++) {
                    int nr = r + dr8[k], nc = c + dc8[k];
                    if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                    int nb = nr * W + nc;
                    if (dist[nb] > dist[c0] + 1) {
                        dist[nb] = dist[c0] + 1;
                        q.push_back(nb);
                    }
                }
            }
        }

        // Place router
        isR[idx] = 1;
        routers.push_back(idx);
        remaining -= cost;
        applyCover(a, b);
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(outB.size()) + "\n";
    for (int id : outB) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    out += to_string(routers.size()) + "\n";
    for (int id : routers) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    cout << out;
    return 0;
}
