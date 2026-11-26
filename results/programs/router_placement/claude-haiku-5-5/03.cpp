#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<string> g;
vector<int> P;          // 2D prefix sums of walls, (H+1)x(W+1)
vector<char> cov;       // covered target cells
vector<int> dist_, par_;
const int INF = INT_MAX / 4;
int dr[8] = {-1,-1,-1,0,0,1,1,1};
int dc[8] = {-1,0,1,-1,1,-1,0,1};

inline int PI(int r, int c) { return r * (W + 1) + c; }

inline int wallsRect(int r1, int c1, int r2, int c2) {
    // inclusive rectangle
    return P[PI(r2 + 1, c2 + 1)] - P[PI(r1, c2 + 1)] - P[PI(r2 + 1, c1)] + P[PI(r1, c1)];
}

// Counts (and optionally marks) newly covered targets for router at (a,b)
int applyRouter(int a, int b, bool mark) {
    int cnt = 0;
    int x0 = max(0, a - R), x1 = min(H - 1, a + R);
    int y0 = max(0, b - R), y1 = min(W - 1, b + R);
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            int id = x * W + y;
            if (g[x][y] != '.' || cov[id]) continue;
            int r1 = min(a, x), r2 = max(a, x);
            int c1 = min(b, y), c2 = max(b, y);
            if (wallsRect(r1, c1, r2, c2) != 0) continue;
            cnt++;
            if (mark) cov[id] = 1;
        }
    }
    return cnt;
}

void propagate(vector<int>& sources) {
    deque<int> q(sources.begin(), sources.end());
    while (!q.empty()) {
        int u = q.front(); q.pop_front();
        int ur = u / W, uc = u % W;
        for (int k = 0; k < 8; k++) {
            int vr = ur + dr[k], vc = uc + dc[k];
            if (vr < 0 || vr >= H || vc < 0 || vc >= W) continue;
            int v = vr * W + vc;
            if (dist_[v] > dist_[u] + 1) {
                dist_[v] = dist_[u] + 1;
                par_[v] = u;
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
    g.resize(H);
    for (int i = 0; i < H; i++) cin >> g[i];

    P.assign((H + 1) * (W + 1), 0);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            int w = (g[r][c] == '#') ? 1 : 0;
            P[PI(r + 1, c + 1)] = P[PI(r, c + 1)] + P[PI(r + 1, c)] - P[PI(r, c)] + w;
        }
    }

    cov.assign(H * W, 0);
    dist_.assign(H * W, INF);
    par_.assign(H * W, -1);

    int start = br * W + bc;
    dist_[start] = 0;
    {
        vector<int> src = {start};
        propagate(src);
    }

    long long spent = 0;
    vector<int> bbOrder;   // backbone cells in output order (excluding start)
    vector<int> routers;

    // initial candidates
    priority_queue<pair<long long, int>> pq;
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            if (g[r][c] == '#') continue;
            int id = r * W + c;
            int gain = applyRouter(r, c, false);
            if (gain <= 0) continue;
            long long cost = Pr + Pb * (long long)dist_[id];
            long long key = 1000LL * gain - cost;
            if (key > 0) pq.push({key, id});
        }
    }

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int id = top.second;
        int r = id / W, c = id % W;
        long long cost = Pr + Pb * (long long)dist_[id];
        if (cost > B - spent) continue;
        int gain = applyRouter(r, c, false);
        if (gain <= 0) continue;
        long long nk = 1000LL * gain - cost;
        if (nk <= 0) continue;
        if (!pq.empty() && nk < pq.top().first) {
            pq.push({nk, id});
            continue;
        }
        // accept: connect path to backbone
        vector<int> path;
        int cur = id;
        while (dist_[cur] > 0) {
            path.push_back(cur);
            cur = par_[cur];
        }
        // path[0] = id (closest to router), path.back() closest to backbone
        for (int i = (int)path.size() - 1; i >= 0; i--) {
            bbOrder.push_back(path[i]);
        }
        for (int v : path) {
            dist_[v] = 0;
            par_[v] = -1;
        }
        spent += cost;
        applyRouter(r, c, true);
        routers.push_back(id);
        propagate(path);
    }

    // output
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
