#include <bits/stdc++.h>
using namespace std;

int H, W, R;
vector<char> wall, target;

template <class F>
void forEachCovered(int a, int b, F f) {
    static int lim[32];
    for (int q = 0; q < 4; q++) {
        int sr = (q & 1) ? -1 : 1, sc = (q & 2) ? -1 : 1;
        int prev = R;
        for (int dr = 0; dr <= R; dr++) {
            int row = a + sr * dr;
            if (row < 0 || row >= H || prev < 0) { lim[dr] = -1; prev = -1; continue; }
            int l = -1;
            for (int dc = 0; dc <= prev; dc++) {
                int col = b + sc * dc;
                if (col < 0 || col >= W || wall[row * W + col]) break;
                l = dc;
            }
            lim[dr] = l; prev = l;
        }
        for (int dr = (sr < 0 ? 1 : 0); dr <= R; dr++) {
            if (lim[dr] < 0) break;
            int row = a + sr * dr;
            for (int dc = (sc < 0 ? 1 : 0); dc <= lim[dr]; dc++) {
                f(row * W + b + sc * dc);
            }
        }
    }
}

int main() {
    long long Pb, Pr, B;
    int br, bc;
    if (scanf("%d %d %d", &H, &W, &R) != 3) return 0;
    scanf("%lld %lld %lld", &Pb, &Pr, &B);
    scanf("%d %d", &br, &bc);
    int N = H * W;
    wall.assign(N, 0); target.assign(N, 0);
    {
        static char buf[2000];
        for (int r = 0; r < H; r++) {
            scanf("%s", buf);
            for (int c = 0; c < W; c++) {
                char ch = buf[c];
                if (ch == '#') wall[r * W + c] = 1;
                else if (ch == '.') target[r * W + c] = 1;
            }
        }
    }
    vector<int> dist(N);
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++)
            dist[r * W + c] = max(abs(r - br), abs(c - bc));
    vector<char> covered(N, 0), alive(N, 0);
    vector<int> gain(N, 0);
    for (int i = 0; i < N; i++) {
        if (wall[i]) continue;
        int g = 0;
        forEachCovered(i / W, i % W, [&](int j) { g += target[j]; });
        gain[i] = g;
        if (g > 0) alive[i] = 1;
    }
    auto keyOf = [&](int i) { return (double)gain[i] / (double)(Pr + Pb * dist[i]); };
    vector<pair<double, int>> heap;
    auto rebuild = [&]() {
        heap.clear();
        for (int i = 0; i < N; i++)
            if (alive[i]) heap.push_back({keyOf(i), i});
        make_heap(heap.begin(), heap.end());
    };
    rebuild();

    long long spent = 0;
    vector<int> bbList, routers;
    vector<int> bfsq;
    bfsq.reserve(N);
    int accepted = 0;
    while (!heap.empty()) {
        if (B - spent < Pr) break;
        pop_heap(heap.begin(), heap.end());
        auto [k, i] = heap.back();
        heap.pop_back();
        if (!alive[i]) continue;
        int g = 0;
        forEachCovered(i / W, i % W, [&](int j) { if (target[j] && !covered[j]) g++; });
        gain[i] = g;
        if (g == 0) { alive[i] = 0; continue; }
        long long cost = Pr + Pb * (long long)dist[i];
        if (cost > B - spent) { alive[i] = 0; continue; }
        if (1000LL * g <= cost) { alive[i] = 0; continue; }
        double v = (double)g / (double)cost;
        if (!heap.empty() && v < heap.front().first - 1e-12) {
            heap.push_back({v, i});
            push_heap(heap.begin(), heap.end());
            continue;
        }
        // accept
        vector<int> path;
        int cur = i;
        while (dist[cur] > 0) {
            path.push_back(cur);
            int r = cur / W, c = cur % W, d = dist[cur], nxt = -1;
            for (int dr = -1; dr <= 1 && nxt < 0; dr++)
                for (int dc = -1; dc <= 1; dc++) {
                    if (!dr && !dc) continue;
                    int nr = r + dr, nc = c + dc;
                    if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                    if (dist[nr * W + nc] == d - 1) { nxt = nr * W + nc; break; }
                }
            cur = nxt;
        }
        reverse(path.begin(), path.end());
        bfsq.clear();
        for (int p : path) { bbList.push_back(p); dist[p] = 0; bfsq.push_back(p); }
        for (size_t h = 0; h < bfsq.size(); h++) {
            int u = bfsq[h];
            int r = u / W, c = u % W, d = dist[u] + 1;
            for (int dr = -1; dr <= 1; dr++)
                for (int dc = -1; dc <= 1; dc++) {
                    if (!dr && !dc) continue;
                    int nr = r + dr, nc = c + dc;
                    if (nr < 0 || nr >= H || nc < 0 || nc >= W) continue;
                    int vv = nr * W + nc;
                    if (dist[vv] > d) { dist[vv] = d; bfsq.push_back(vv); }
                }
        }
        forEachCovered(i / W, i % W, [&](int j) { covered[j] = 1; });
        routers.push_back(i);
        alive[i] = 0;
        spent += Pb * (long long)path.size() + Pr;
        accepted++;
        if (accepted % 50 == 0) rebuild();
    }

    string out;
    out.reserve((bbList.size() + routers.size()) * 12 + 32);
    out += to_string(bbList.size()); out += '\n';
    for (int p : bbList) { out += to_string(p / W); out += ' '; out += to_string(p % W); out += '\n'; }
    out += to_string(routers.size()); out += '\n';
    for (int p : routers) { out += to_string(p / W); out += ' '; out += to_string(p % W); out += '\n'; }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
