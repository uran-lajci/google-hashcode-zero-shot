#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<int> PW, PT; // prefix sums of walls and targets, size (H+1)*(W+1)
vector<char> isWall, isTarget, covered, isBB, isRouter;
vector<int> dist, par;

inline int rectSum(const vector<int>& P, int r1, int c1, int r2, int c2) {
    int W1 = W + 1;
    return P[(r2 + 1) * W1 + (c2 + 1)] - P[r1 * W1 + (c2 + 1)]
         - P[(r2 + 1) * W1 + c1] + P[r1 * W1 + c1];
}

// Number of uncovered targets a router at (a,b) would newly cover.
// If mark is true, also marks them covered.
int gainOf(int a, int b, bool mark) {
    int cnt = 0;
    int xlo = max(0, a - R), xhi = min(H - 1, a + R);
    int ylo = max(0, b - R), yhi = min(W - 1, b + R);
    for (int x = xlo; x <= xhi; ++x) {
        for (int y = ylo; y <= yhi; ++y) {
            int idx = x * W + y;
            if (!isTarget[idx] || covered[idx]) continue;
            int r1 = min(a, x), r2 = max(a, x);
            int c1 = min(b, y), c2 = max(b, y);
            if (rectSum(PW, r1, c1, r2, c2) != 0) continue;
            ++cnt;
            if (mark) covered[idx] = 1;
        }
    }
    return cnt;
}

// Upper bound on gain: number of target cells in the box (ignores walls).
int boxTargets(int a, int b) {
    int xlo = max(0, a - R), xhi = min(H - 1, a + R);
    int ylo = max(0, b - R), yhi = min(W - 1, b + R);
    return rectSum(PT, xlo, ylo, xhi, yhi);
}

const int DR[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int DC[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

// Propagate distance decreases from the given new backbone cells (already set dist=0).
void propagate(const vector<int>& sources) {
    vector<int> q = sources;
    size_t head = 0;
    while (head < q.size()) {
        int u = q[head++];
        int ur = u / W, uc = u % W;
        for (int d = 0; d < 8; ++d) {
            int vr = ur + DR[d], vc = uc + DC[d];
            if (vr < 0 || vr >= H || vc < 0 || vc >= W) continue;
            int v = vr * W + vc;
            if (dist[v] > dist[u] + 1) {
                dist[v] = dist[u] + 1;
                par[v] = u;
                q.push_back(v);
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> R;
    cin >> Pb >> Pr >> B;
    int br, bc;
    cin >> br >> bc;
    vector<string> g(H);
    for (int i = 0; i < H; ++i) cin >> g[i];

    int N = H * W;
    isWall.assign(N, 0);
    isTarget.assign(N, 0);
    covered.assign(N, 0);
    isBB.assign(N, 0);
    isRouter.assign(N, 0);
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            char ch = g[i][j];
            isWall[i * W + j] = (ch == '#');
            isTarget[i * W + j] = (ch == '.');
        }

    PW.assign((H + 1) * (W + 1), 0);
    PT.assign((H + 1) * (W + 1), 0);
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            int idx = (i + 1) * (W + 1) + (j + 1);
            PW[idx] = PW[i * (W + 1) + (j + 1)] + PW[(i + 1) * (W + 1) + j] - PW[i * (W + 1) + j] + (isWall[i * W + j] ? 1 : 0);
            PT[idx] = PT[i * (W + 1) + (j + 1)] + PT[(i + 1) * (W + 1) + j] - PT[i * (W + 1) + j] + (isTarget[i * W + j] ? 1 : 0);
        }

    const int INF = INT_MAX / 4;
    dist.assign(N, INF);
    par.assign(N, -1);

    int s = br * W + bc;
    isBB[s] = 1;
    dist[s] = 0;
    par[s] = -1;
    propagate({s});

    vector<int> bbOrder;  // backbone cells excluding the initial one
    vector<pair<int,int>> routers;
    long long spent = 0;

    // Initial priority queue
    priority_queue<pair<double,int>> pq;
    for (int idx = 0; idx < N; ++idx) {
        if (isWall[idx]) continue;
        int a = idx / W, b = idx % W;
        int ub = boxTargets(a, b);
        if (ub == 0) continue;
        double denom = (double)dist[idx] * Pb + Pr;
        pq.push({ub / denom, idx});
    }

    while (!pq.empty()) {
        auto [key, p] = pq.top();
        pq.pop();
        if (isRouter[p]) continue;
        long long rem = B - spent;
        int a = p / W, b = p % W;
        double denom = (double)dist[p] * Pb + Pr;
        if (denom > rem) continue;
        int gain = gainOf(a, b, false);
        if (gain == 0) continue;
        double nk = gain / denom;
        if (!pq.empty() && nk < pq.top().first) {
            pq.push({nk, p});
            continue;
        }
        // Accept: walk the path from p to the backbone.
        vector<int> path;
        int cur = p;
        long long guard = 0;
        while (!isBB[cur] && guard++ <= N) {
            path.push_back(cur);
            cur = par[cur];
            if (cur < 0) break;
        }
        if (cur < 0) continue;
        long long cost = (long long)path.size() * Pb + Pr;
        if (cost > rem) continue;

        vector<int> newCells(path.rbegin(), path.rend());
        for (int c : newCells) {
            isBB[c] = 1;
            dist[c] = 0;
            par[c] = -1;
            bbOrder.push_back(c);
        }
        propagate(newCells);

        isRouter[p] = 1;
        routers.push_back({a, b});
        gainOf(a, b, true);
        spent += cost;
    }

    // Output
    string out;
    out.reserve(1 << 20);
    out += to_string(bbOrder.size()) + "\n";
    for (int c : bbOrder) {
        out += to_string(c / W) + " " + to_string(c % W) + "\n";
    }
    out += to_string(routers.size()) + "\n";
    for (auto& rc : routers) {
        out += to_string(rc.first) + " " + to_string(rc.second) + "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
