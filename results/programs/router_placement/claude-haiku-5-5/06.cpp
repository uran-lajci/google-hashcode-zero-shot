#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<char> g;
vector<int> P;
int WP;
vector<int> gain;
vector<char> covered, inS, used;
vector<int> dist_, par_;
const int INF = 1000000000;

inline bool covers(int a, int b, int x, int y) {
    int r1 = min(a, x), r2 = max(a, x), c1 = min(b, y), c2 = max(b, y);
    int s = P[(r2 + 1) * WP + (c2 + 1)] - P[r1 * WP + (c2 + 1)]
          - P[(r2 + 1) * WP + c1] + P[r1 * WP + c1];
    return s == 0;
}

void markTarget(int t) {
    covered[t] = 1;
    int x = t / W, y = t % W;
    for (int a = max(0, x - R); a <= min(H - 1, x + R); a++)
        for (int b = max(0, y - R); b <= min(W - 1, y + R); b++) {
            int c = a * W + b;
            if (g[c] == '#') continue;
            if (covers(a, b, x, y)) gain[c]--;
        }
}

void bfs(const vector<int>& dummy) {
    (void)dummy;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> R;
    cin >> Pb >> Pr >> B;
    int br, bc;
    cin >> br >> bc;
    g.assign((size_t)H * W, '-');
    for (int r = 0; r < H; r++) {
        string s;
        cin >> s;
        for (int c = 0; c < W; c++) g[r * W + c] = s[c];
    }

    WP = W + 1;
    P.assign((size_t)(H + 1) * WP, 0);
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++) {
            int w = (g[r * W + c] == '#') ? 1 : 0;
            P[(r + 1) * WP + (c + 1)] = P[r * WP + (c + 1)] + P[(r + 1) * WP + c] - P[r * WP + c] + w;
        }

    int NN = H * W;
    gain.assign(NN, 0);
    covered.assign(NN, 0);
    inS.assign(NN, 0);
    used.assign(NN, 0);
    dist_.assign(NN, INF);
    par_.assign(NN, -1);

    for (int a = 0; a < H; a++)
        for (int b = 0; b < W; b++) {
            int c = a * W + b;
            if (g[c] == '#') continue;
            int cnt = 0;
            for (int x = max(0, a - R); x <= min(H - 1, a + R); x++)
                for (int y = max(0, b - R); y <= min(W - 1, b + R); y++) {
                    if (g[x * W + y] == '.' && covers(a, b, x, y)) cnt++;
                }
            gain[c] = cnt;
        }

    int init = br * W + bc;
    inS[init] = 1;

    vector<int> order;
    vector<int> routers;
    long long spent = 0;

    auto bfsRun = [&]() {
        fill(dist_.begin(), dist_.end(), INF);
        deque<int> dq;
        for (int i = 0; i < NN; i++)
            if (inS[i]) { dist_[i] = 0; dq.push_back(i); }
        while (!dq.empty()) {
            int u = dq.front(); dq.pop_front();
            int ua = u / W, ub = u % W;
            for (int da = -1; da <= 1; da++)
                for (int db = -1; db <= 1; db++) {
                    if (da == 0 && db == 0) continue;
                    int va = ua + da, vb = ub + db;
                    if (va < 0 || va >= H || vb < 0 || vb >= W) continue;
                    int v = va * W + vb;
                    int w = inS[v] ? 0 : 1;
                    int nd = dist_[u] + w;
                    if (nd < dist_[v]) {
                        dist_[v] = nd;
                        par_[v] = u;
                        if (w == 0) dq.push_front(v); else dq.push_back(v);
                    }
                }
        }
    };

    vector<pair<double, int>> heap;
    auto rebuild = [&]() {
        bfsRun();
        heap.clear();
        for (int id = 0; id < NN; id++) {
            if (g[id] == '#' || used[id] || dist_[id] >= INF || gain[id] <= 0) continue;
            long long cost = (long long)dist_[id] * Pb + Pr;
            if (spent + cost > B) continue;
            heap.push_back({(double)gain[id] / (double)cost, id});
        }
        make_heap(heap.begin(), heap.end());
    };

    rebuild();
    int since = 0;
    vector<int> chain;
    while (true) {
        if (since >= 64 || heap.empty()) {
            if (heap.empty() && since == 0) break;
            rebuild();
            since = 0;
            if (heap.empty()) break;
        }
        pop_heap(heap.begin(), heap.end());
        int c = heap.back().second;
        heap.pop_back();
        if (used[c] || gain[c] <= 0) continue;
        if (dist_[c] >= INF) continue;

        chain.clear();
        int cur = c;
        while (!inS[cur]) {
            chain.push_back(cur);
            cur = par_[cur];
        }
        int k = (int)chain.size();
        long long cost = (long long)k * Pb + Pr;
        if (spent + cost > B) continue;
        double r = (double)gain[c] / (double)cost;
        if (!heap.empty() && r < heap.front().first) {
            heap.push_back({r, c});
            push_heap(heap.begin(), heap.end());
            continue;
        }

        for (int i = k - 1; i >= 0; i--) {
            inS[chain[i]] = 1;
            order.push_back(chain[i]);
        }
        spent += cost;
        used[c] = 1;
        routers.push_back(c);

        int a = c / W, b = c % W;
        for (int x = max(0, a - R); x <= min(H - 1, a + R); x++)
            for (int y = max(0, b - R); y <= min(W - 1, b + R); y++) {
                int t = x * W + y;
                if (g[t] == '.' && !covered[t] && covers(a, b, x, y))
                    markTarget(t);
            }
        since++;
    }

    string out;
    out.reserve(1 << 20);
    out += to_string(order.size()) + "\n";
    for (int id : order) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    out += to_string(routers.size()) + "\n";
    for (int id : routers) {
        out += to_string(id / W) + " " + to_string(id % W) + "\n";
    }
    cout << out;
    return 0;
}
