#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<string> grid;
vector<int> ps;
vector<char> covered;

inline int S(int r, int c) { return ps[r * (W + 1) + c]; }

inline int rectWalls(int r1, int c1, int r2, int c2) {
    return S(r2 + 1, c2 + 1) - S(r1, c2 + 1) - S(r2 + 1, c1) + S(r1, c1);
}

// Count uncovered targets covered by a router at (a,b)
int gainOf(int a, int b) {
    int cnt = 0;
    int x0 = max(0, a - R), x1 = min(H - 1, a + R);
    int y0 = max(0, b - R), y1 = min(W - 1, b + R);
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            if (grid[x][y] != '.') continue;
            if (covered[x * W + y]) continue;
            if (rectWalls(min(a, x), min(b, y), max(a, x), max(b, y)) != 0) continue;
            cnt++;
        }
    }
    return cnt;
}

void markCovered(int a, int b) {
    int x0 = max(0, a - R), x1 = min(H - 1, a + R);
    int y0 = max(0, b - R), y1 = min(W - 1, b + R);
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            if (grid[x][y] != '.') continue;
            if (rectWalls(min(a, x), min(b, y), max(a, x), max(b, y)) != 0) continue;
            covered[x * W + y] = 1;
        }
    }
}

int main() {
    auto startTime = chrono::steady_clock::now();
    cin >> H >> W >> R;
    cin >> Pb >> Pr >> B;
    int br, bc;
    cin >> br >> bc;
    grid.resize(H);
    for (int i = 0; i < H; i++) cin >> grid[i];

    ps.assign((H + 1) * (W + 1), 0);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            ps[(r + 1) * (W + 1) + (c + 1)] = ps[r * (W + 1) + (c + 1)] + ps[(r + 1) * (W + 1) + c]
                - ps[r * (W + 1) + c] + (grid[r][c] == '#' ? 1 : 0);
        }
    }

    int HW = H * W;
    covered.assign(HW, 0);
    vector<char> conn(HW, 0), isRouter(HW, 0);
    vector<int> g(HW, -1);
    vector<int> dist(HW), par(HW);
    vector<pair<int,int>> backbone;   // cells connected (excluding start)
    vector<pair<int,int>> routers;
    long long spent = 0;              // Pb*N + Pr*M

    int startIdx = br * W + bc;
    conn[startIdx] = 1;

    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};

    bool timeUp = false;
    while (true) {
        auto now = chrono::steady_clock::now();
        if (chrono::duration<double>(now - startTime).count() > 8.5) break;

        // Multi-source BFS from backbone
        fill(dist.begin(), dist.end(), -1);
        vector<int> q;
        q.reserve(HW);
        for (int v = 0; v < HW; v++) {
            if (conn[v]) { dist[v] = 0; q.push_back(v); }
        }
        for (size_t qi = 0; qi < q.size(); qi++) {
            int u = q[qi];
            int ur = u / W, uc = u % W;
            for (int d = 0; d < 8; d++) {
                int nr = ur + dr[d], nc = uc + dc[d];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int w = nr * W + nc;
                if (dist[w] >= 0) continue;
                dist[w] = dist[u] + 1;
                par[w] = u;
                q.push_back(w);
            }
        }

        bool done = false;
        int bestV = -1;
        while (true) {
            bestV = -1;
            long long bestG = 0, bestC = 1;
            for (int r = 0; r < H; r++) {
                for (int c = 0; c < W; c++) {
                    int v = r * W + c;
                    if (grid[r][c] == '#') continue;
                    if (isRouter[v]) continue;
                    if (dist[v] < 0) continue;
                    long long cost = Pr + Pb * dist[v];
                    if (spent + cost > B) continue;
                    if (g[v] < 0) g[v] = gainOf(r, c);
                    if (g[v] <= 0) continue;
                    if (bestV < 0 || (long long)g[v] * bestC > bestG * cost) {
                        bestV = v;
                        bestG = g[v];
                        bestC = cost;
                    }
                }
            }
            if (bestV < 0) { done = true; break; }
            int ex = gainOf(bestV / W, bestV % W);
            if (ex == (int)bestG) break;
            g[bestV] = ex;
        }
        if (done) break;

        // Place router at bestV, connect path
        int a = bestV / W, b = bestV % W;
        markCovered(a, b);
        vector<int> path;
        int cur = bestV;
        while (dist[cur] > 0) {
            path.push_back(cur);
            cur = par[cur];
        }
        reverse(path.begin(), path.end());
        for (int v : path) {
            conn[v] = 1;
            backbone.push_back({v / W, v % W});
            spent += Pb;
        }
        isRouter[bestV] = 1;
        routers.push_back({a, b});
        spent += Pr;
        g[bestV] = 0;
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(backbone.size()) + "\n";
    for (auto &p : backbone) {
        out += to_string(p.first) + " " + to_string(p.second) + "\n";
    }
    out += to_string(routers.size()) + "\n";
    for (auto &p : routers) {
        out += to_string(p.first) + " " + to_string(p.second) + "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
