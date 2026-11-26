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

    vector<string> rows(H);
    for (int i = 0; i < H; i++) cin >> rows[i];

    const int N = H * W;
    vector<char> grid(N);
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            grid[i * W + j] = rows[i][j];

    // prefix sums of target cells
    vector<int> pre((H + 1) * (W + 1), 0);
    auto PI = [&](int x, int y) -> int& { return pre[x * (W + 1) + y]; };
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            PI(i + 1, j + 1) = PI(i, j + 1) + PI(i + 1, j) - PI(i, j) + (grid[i * W + j] == '.' ? 1 : 0);

    auto ubCount = [&](int a, int b) -> int {
        int x0 = max(0, a - R), x1 = min(H - 1, a + R);
        int y0 = max(0, b - R), y1 = min(W - 1, b + R);
        return PI(x1 + 1, y1 + 1) - PI(x0, y1 + 1) - PI(x1 + 1, y0) + PI(x0, y0);
    };

    vector<char> inS(N, 0), routerPlaced(N, 0), dead(N, 0), covered(N, 0);
    vector<int> dist(N, -1), par(N, -1), gainUB(N, 0), mark(N, 0);
    int stamp = 0;

    int start = br * W + bc;
    inS[start] = 1;
    long long remaining = B;

    auto bfs = [&]() {
        fill(dist.begin(), dist.end(), -1);
        vector<int> q;
        q.reserve(N);
        for (int i = 0; i < N; i++)
            if (inS[i]) {
                dist[i] = 0;
                par[i] = -1;
                q.push_back(i);
            }
        size_t head = 0;
        while (head < q.size()) {
            int cur = q[head++];
            int x = cur / W, y = cur % W;
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= H || ny < 0 || ny >= W) continue;
                    int nb = nx * W + ny;
                    if (dist[nb] == -1) {
                        dist[nb] = dist[cur] + 1;
                        par[nb] = cur;
                        q.push_back(nb);
                    }
                }
        }
    };

    vector<pair<double, int>> heap;
    auto rebuild = [&]() {
        bfs();
        heap.clear();
        heap.reserve(N);
        for (int i = 0; i < N; i++) {
            if (grid[i] == '#' || routerPlaced[i] || dead[i]) continue;
            double cost = (double)(Pr + Pb * (long long)dist[i]);
            double key = (double)gainUB[i] / cost;
            heap.push_back({key, i});
        }
        make_heap(heap.begin(), heap.end());
    };

    // initial upper bounds on gains
    for (int i = 0; i < N; i++) {
        if (grid[i] == '#') continue;
        gainUB[i] = ubCount(i / W, i % W);
        if (gainUB[i] == 0) dead[i] = 1;
    }

    // gather newly covered target cells for router at p (visibility DP per quadrant)
    bool A[11][11];
    vector<int> newCells;
    auto gather = [&](int p) {
        newCells.clear();
        stamp++;
        int a = p / W, b = p % W;
        for (int sx = -1; sx <= 1; sx += 2)
            for (int sy = -1; sy <= 1; sy += 2)
                for (int i = 0; i <= R; i++)
                    for (int j = 0; j <= R; j++) {
                        int x = a + sx * i, y = b + sy * j;
                        bool ok;
                        if (x < 0 || x >= H || y < 0 || y >= W) {
                            ok = false;
                        } else if (grid[x * W + y] == '#') {
                            ok = false;
                        } else {
                            ok = true;
                            if (i > 0 && !A[i - 1][j]) ok = false;
                            if (j > 0 && !A[i][j - 1]) ok = false;
                        }
                        A[i][j] = ok;
                        if (ok) {
                            int id = x * W + y;
                            if (grid[id] == '.' && !covered[id] && mark[id] != stamp) {
                                mark[id] = stamp;
                                newCells.push_back(id);
                            }
                        }
                    }
    };

    vector<int> bbList, routers, chain;
    long long accepts = 0, sinceRebuild = 0;

    rebuild();

    while (!heap.empty() && remaining >= Pr) {
        pop_heap(heap.begin(), heap.end());
        int p = heap.back().second;
        heap.pop_back();
        if (dead[p] || routerPlaced[p]) continue;

        gather(p);
        long long g = (long long)newCells.size();
        gainUB[p] = (int)g;
        if (g == 0) {
            dead[p] = 1;
            continue;
        }

        long long lenMax = (remaining - Pr) / Pb;
        chain.clear();
        int cur = p;
        bool ok = true;
        while (!inS[cur]) {
            if ((long long)chain.size() > lenMax) { ok = false; break; }
            chain.push_back(cur);
            cur = par[cur];
            if (cur < 0) { ok = false; break; }
        }
        if (!ok) continue;

        long long cost = Pr + Pb * (long long)chain.size();
        if (cost > remaining) continue;

        double ratio = (double)g / (double)cost;
        if (!heap.empty() && ratio < heap.front().first) {
            heap.push_back({ratio, p});
            push_heap(heap.begin(), heap.end());
            continue;
        }

        // accept: add chain from the backbone side outward, then place router
        for (int k = (int)chain.size() - 1; k >= 0; k--) {
            int c = chain[k];
            inS[c] = 1;
            bbList.push_back(c);
        }
        routerPlaced[p] = 1;
        routers.push_back(p);
        remaining -= cost;
        for (int c : newCells) covered[c] = 1;

        accepts++;
        sinceRebuild++;
        if (sinceRebuild >= max(100LL, accepts / 2)) {
            rebuild();
            sinceRebuild = 0;
        }
    }

    // output
    string out;
    out.reserve((bbList.size() + routers.size()) * 12 + 32);
    out += to_string(bbList.size());
    out += '\n';
    for (int c : bbList) {
        out += to_string(c / W);
        out += ' ';
        out += to_string(c % W);
        out += '\n';
    }
    out += to_string(routers.size());
    out += '\n';
    for (int c : routers) {
        out += to_string(c / W);
        out += ' ';
        out += to_string(c % W);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
