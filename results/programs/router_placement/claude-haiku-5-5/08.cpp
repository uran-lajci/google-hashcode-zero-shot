#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<string> g;
vector<int> ps;          // 2D prefix sums of wall counts, size (H+1)*(W+1)
vector<int> dist_;       // king-move distance to backbone
vector<char> covered;
vector<int> bfsQ;

inline int PS(int r, int c) { return ps[r * (W + 1) + c]; }

// true if rectangle [r1..r2] x [c1..c2] contains a wall
inline bool wallRect(int r1, int c1, int r2, int c2) {
    int s = PS(r2 + 1, c2 + 1) - PS(r1, c2 + 1) - PS(r2 + 1, c1) + PS(r1, c1);
    return s > 0;
}

// number of uncovered targets covered by router at (a,b); if apply, mark them covered
int coverage(int a, int b, bool apply) {
    int cnt = 0;
    int x0 = max(0, a - R), x1 = min(H - 1, a + R);
    int y0 = max(0, b - R), y1 = min(W - 1, b + R);
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            int id = x * W + y;
            if (g[x][y] != '.' || covered[id]) continue;
            int r1 = min(a, x), r2 = max(a, x);
            int c1 = min(b, y), c2 = max(b, y);
            if (wallRect(r1, c1, r2, c2)) continue;
            cnt++;
            if (apply) covered[id] = 1;
        }
    }
    return cnt;
}

// add cell c to backbone and propagate decreased distances
void addCell(int c) {
    dist_[c] = 0;
    bfsQ.clear();
    bfsQ.push_back(c);
    size_t head = 0;
    while (head < bfsQ.size()) {
        int cur = bfsQ[head++];
        int cr = cur / W, cc = cur % W;
        int nd = dist_[cur] + 1;
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int nr = cr + dr, nc = cc + dc;
                if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                int nid = nr * W + nc;
                if (dist_[nid] > nd) {
                    dist_[nid] = nd;
                    bfsQ.push_back(nid);
                }
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

    ps.assign((H + 1) * (W + 1), 0);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            int w = (g[i][j] == '#') ? 1 : 0;
            ps[(i + 1) * (W + 1) + (j + 1)] = w + PS(i, j + 1) + PS(i + 1, j) - PS(i, j);
        }
    }

    int N = H * W;
    dist_.assign(N, INT_MAX / 2);
    covered.assign(N, 0);

    vector<int> backbone;   // cells added to backbone (excluding start)
    vector<int> routers;
    long long spent = 0;

    int start = br * W + bc;
    addCell(start);

    // initial candidates
    priority_queue<pair<double, int>> pq;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (g[i][j] == '#') continue;
            int id = i * W + j;
            int gn = coverage(i, j, false);
            if (gn == 0) continue;
            double cost = (double)dist_[id] * Pb + Pr;
            pq.push({(double)gn / cost, id});
        }
    }

    while (!pq.empty()) {
        auto [key, id] = pq.top();
        pq.pop();
        int a = id / W, b = id % W;
        int gn = coverage(a, b, false);
        if (gn == 0) continue;
        long long k = dist_[id];
        long long cost = k * Pb + Pr;
        if (spent + cost > B) continue;
        double newKey = (double)gn / (double)cost;
        if (!pq.empty() && newKey < pq.top().first) {
            pq.push({newKey, id});
            continue;
        }

        // build path from backbone to id
        vector<int> path;
        int cur = id;
        while (dist_[cur] > 0) {
            path.push_back(cur);
            int cr = cur / W, cc = cur % W;
            int nxt = -1;
            for (int dr = -1; dr <= 1 && nxt < 0; dr++) {
                for (int dc = -1; dc <= 1 && nxt < 0; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = cr + dr, nc = cc + dc;
                    if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                    int nid = nr * W + nc;
                    if (dist_[nid] == dist_[cur] - 1) { nxt = nid; }
                }
            }
            cur = nxt;
        }
        reverse(path.begin(), path.end());
        for (int c : path) {
            addCell(c);
            backbone.push_back(c);
        }
        spent += k * Pb;

        // place router
        routers.push_back(id);
        spent += Pr;
        coverage(a, b, true);
    }

    // output
    ostringstream out;
    out << backbone.size() << "\n";
    for (int c : backbone) out << c / W << " " << c % W << "\n";
    out << routers.size() << "\n";
    for (int c : routers) out << c / W << " " << c % W << "\n";
    cout << out.str();
    return 0;
}
