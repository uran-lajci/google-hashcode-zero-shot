#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<string> G;
vector<int> P;      // 2D prefix sums of walls, (H+1)x(W+1)
vector<char> covered;
vector<int> dist_;
const int INF = INT_MAX / 2;

inline int rectWalls(int r1, int c1, int r2, int c2) {
    int W1 = W + 1;
    return P[(r2 + 1) * W1 + (c2 + 1)] - P[r1 * W1 + (c2 + 1)]
         - P[(r2 + 1) * W1 + c1] + P[r1 * W1 + c1];
}

inline bool visible(int a, int b, int x, int y) {
    return rectWalls(min(a, x), min(b, y), max(a, x), max(b, y)) == 0;
}

long long gainOf(int p) {
    int a = p / W, b = p % W;
    long long cnt = 0;
    for (int x = max(0, a - R); x <= min(H - 1, a + R); ++x)
        for (int y = max(0, b - R); y <= min(W - 1, b + R); ++y) {
            if (G[x][y] != '.') continue;
            int id = x * W + y;
            if (covered[id]) continue;
            if (visible(a, b, x, y)) ++cnt;
        }
    return cnt;
}

void markCovered(int p) {
    int a = p / W, b = p % W;
    for (int x = max(0, a - R); x <= min(H - 1, a + R); ++x)
        for (int y = max(0, b - R); y <= min(W - 1, b + R); ++y) {
            if (G[x][y] != '.') continue;
            int id = x * W + y;
            if (covered[id]) continue;
            if (visible(a, b, x, y)) covered[id] = 1;
        }
}

static const int DR[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
static const int DC[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> R;
    cin >> Pb >> Pr >> B;
    int br, bc;
    cin >> br >> bc;
    G.resize(H);
    for (int i = 0; i < H; ++i) cin >> G[i];

    P.assign((H + 1) * (W + 1), 0);
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c) {
            int w = (G[r][c] == '#') ? 1 : 0;
            P[(r + 1) * (W + 1) + (c + 1)] = P[r * (W + 1) + (c + 1)]
                + P[(r + 1) * (W + 1) + c] - P[r * (W + 1) + c] + w;
        }

    int N = H * W;
    covered.assign(N, 0);
    dist_.assign(N, INF);

    // Initial BFS from the backbone cell (unit cost per non-backbone cell)
    int init = br * W + bc;
    dist_[init] = 0;
    {
        vector<int> q;
        q.reserve(N);
        q.push_back(init);
        size_t head = 0;
        while (head < q.size()) {
            int c = q[head++];
            int r = c / W, cc = c % W;
            for (int d = 0; d < 8; ++d) {
                int nr = r + DR[d], nc = cc + DC[d];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int n = nr * W + nc;
                if (dist_[n] > dist_[c] + 1) {
                    dist_[n] = dist_[c] + 1;
                    q.push_back(n);
                }
            }
        }
    }

    vector<int> bbOrder;   // backbone cells added (excluding initial), valid order
    vector<int> routers;
    long long rem = B;

    // Initial candidate heap
    priority_queue<pair<double, int>> pq;
    for (int p = 0; p < N; ++p) {
        int r = p / W, c = p % W;
        if (G[r][c] == '#') continue;
        if (dist_[p] >= INF) continue;
        long long cost = (long long)dist_[p] * Pb + Pr;
        if (cost > rem) continue;
        long long g = gainOf(p);
        if (g <= 0) continue;
        pq.push({(double)g / (double)cost, p});
    }

    while (!pq.empty()) {
        auto [key, p] = pq.top();
        pq.pop();
        if (dist_[p] >= INF) continue;
        long long cost = (long long)dist_[p] * Pb + Pr;
        if (cost > rem) continue;
        long long g = gainOf(p);
        if (g <= 0) continue;
        double nk = (double)g / (double)cost;
        if (!pq.empty() && nk < pq.top().first) {
            pq.push({nk, p});
            continue;
        }

        // Accept: build path from p back to backbone
        vector<int> path;
        int cur = p;
        while (dist_[cur] > 0) {
            path.push_back(cur);
            int r = cur / W, c = cur % W;
            int nxt = -1;
            for (int d = 0; d < 8; ++d) {
                int nr = r + DR[d], nc = c + DC[d];
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int n = nr * W + nc;
                if (dist_[n] == dist_[cur] - 1) { nxt = n; break; }
            }
            cur = nxt;
        }
        for (int i = (int)path.size() - 1; i >= 0; --i) {
            dist_[path[i]] = 0;
            bbOrder.push_back(path[i]);
        }
        // Propagate decreased distances
        {
            vector<int> q(path.begin(), path.end());
            size_t head = 0;
            while (head < q.size()) {
                int c = q[head++];
                int r = c / W, cc = c % W;
                for (int d = 0; d < 8; ++d) {
                    int nr = r + DR[d], nc = cc + DC[d];
                    if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                    int n = nr * W + nc;
                    if (dist_[n] > dist_[c] + 1) {
                        dist_[n] = dist_[c] + 1;
                        q.push_back(n);
                    }
                }
            }
        }

        rem -= cost;
        routers.push_back(p);
        markCovered(p);
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(bbOrder.size()) + "\n";
    for (int id : bbOrder) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    out += to_string(routers.size()) + "\n";
    for (int id : routers) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    cout << out;
    return 0;
}
