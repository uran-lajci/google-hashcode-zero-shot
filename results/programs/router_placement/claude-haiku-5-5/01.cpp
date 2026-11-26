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

    vector<string> grid(H);
    for (int i = 0; i < H; i++) cin >> grid[i];

    int N = H * W;
    vector<char> wall(N, 0), tgt(N, 0);
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++) {
            char ch = grid[r][c];
            if (ch == '#') wall[r * W + c] = 1;
            else if (ch == '.') tgt[r * W + c] = 1;
        }

    // 2D prefix sums of walls
    int PW = W + 1;
    vector<int> P((H + 1) * (W + 1), 0);
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++)
            P[(r + 1) * PW + (c + 1)] = P[r * PW + (c + 1)] + P[(r + 1) * PW + c]
                                        - P[r * PW + c] + wall[r * W + c];

    auto rectWall = [&](int r1, int c1, int r2, int c2) -> bool {
        int s = P[(r2 + 1) * PW + (c2 + 1)] - P[r1 * PW + (c2 + 1)]
              - P[(r2 + 1) * PW + c1] + P[r1 * PW + c1];
        return s > 0;
    };

    vector<char> covered(N, 0), conn(N, 0), isRouter(N, 0);
    vector<int> dist(N, -1);

    int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    auto gainOf = [&](int a, int b) -> int {
        int cnt = 0;
        int x0 = max(0, a - R), x1 = min(H - 1, a + R);
        int y0 = max(0, b - R), y1 = min(W - 1, b + R);
        for (int x = x0; x <= x1; x++)
            for (int y = y0; y <= y1; y++) {
                int id = x * W + y;
                if (tgt[id] && !covered[id] &&
                    !rectWall(min(a, x), min(b, y), max(a, x), max(b, y)))
                    cnt++;
            }
        return cnt;
    };

    auto applyCover = [&](int a, int b) {
        int x0 = max(0, a - R), x1 = min(H - 1, a + R);
        int y0 = max(0, b - R), y1 = min(W - 1, b + R);
        for (int x = x0; x <= x1; x++)
            for (int y = y0; y <= y1; y++) {
                int id = x * W + y;
                if (tgt[id] && !covered[id] &&
                    !rectWall(min(a, x), min(b, y), max(a, x), max(b, y)))
                    covered[id] = 1;
            }
    };

    auto recomputeDist = [&]() {
        fill(dist.begin(), dist.end(), -1);
        vector<int> q;
        q.reserve(N);
        for (int i = 0; i < N; i++)
            if (conn[i]) { dist[i] = 0; q.push_back(i); }
        size_t qi = 0;
        while (qi < q.size()) {
            int u = q[qi++];
            int ur = u / W, uc = u % W;
            for (int d = 0; d < 8; d++) {
                int nr = ur + dr8[d], nc = uc + dc8[d];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int v = nr * W + nc;
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q.push_back(v);
                }
            }
        }
    };

    vector<int> par(N, -1), stamp(N, 0);
    int curStamp = 0;
    // Shortest path of unconnected cells from c to the backbone, ordered from the
    // cell adjacent to the backbone outward to c.
    auto findPath = [&](int c) -> vector<int> {
        curStamp++;
        vector<int> q;
        q.push_back(c);
        stamp[c] = curStamp;
        par[c] = -1;
        size_t qi = 0;
        while (qi < q.size()) {
            int u = q[qi++];
            int ur = u / W, uc = u % W;
            bool found = false;
            for (int d = 0; d < 8 && !found; d++) {
                int nr = ur + dr8[d], nc = uc + dc8[d];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                if (conn[nr * W + nc]) found = true;
            }
            if (found) {
                vector<int> path;
                for (int x = u; x != -1; x = par[x]) path.push_back(x);
                return path;
            }
            for (int d = 0; d < 8; d++) {
                int nr = ur + dr8[d], nc = uc + dc8[d];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int v = nr * W + nc;
                if (stamp[v] == curStamp) continue;
                stamp[v] = curStamp;
                par[v] = u;
                q.push_back(v);
            }
        }
        return {};
    };

    int start = br * W + bc;
    conn[start] = 1;
    recomputeDist();

    // Initial candidate queue
    priority_queue<pair<double, int>> pq;
    for (int id = 0; id < N; id++) {
        if (wall[id]) continue;
        if (dist[id] < 0) continue;
        int g = gainOf(id / W, id % W);
        if (g <= 0) continue;
        double key = (double)g / (double)(Pr + Pb * dist[id]);
        pq.push({key, id});
    }

    vector<pair<int, int>> outCells;
    vector<pair<int, int>> routers;
    long long spent = 0;
    int accepts = 0;

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int c = top.second;
        if (isRouter[c] || wall[c]) continue;
        int a = c / W, b = c % W;
        int g = gainOf(a, b);
        if (g <= 0) continue;
        if (dist[c] < 0) continue;
        double ratio = (double)g / (double)(Pr + Pb * dist[c]);
        if (!pq.empty() && ratio < pq.top().first) {
            pq.push({ratio, c});
            continue;
        }

        vector<int> path;
        if (!conn[c]) {
            path = findPath(c);
            if (path.empty()) continue;
        }
        long long cost = (long long)path.size() * Pb + Pr;
        if (spent + cost > B) continue;

        spent += cost;
        for (int x : path) {
            conn[x] = 1;
            outCells.push_back({x / W, x % W});
        }
        isRouter[c] = 1;
        routers.push_back({a, b});
        applyCover(a, b);

        accepts++;
        if (accepts % 32 == 0) recomputeDist();
    }

    // Output
    string out;
    out.reserve(outCells.size() * 12 + routers.size() * 12 + 32);
    out += to_string(outCells.size()) + "\n";
    for (auto& p : outCells) {
        out += to_string(p.first);
        out += ' ';
        out += to_string(p.second);
        out += '\n';
    }
    out += to_string(routers.size()) + "\n";
    for (auto& p : routers) {
        out += to_string(p.first);
        out += ' ';
        out += to_string(p.second);
        out += '\n';
    }
    cout << out;
    return 0;
}
