#include <bits/stdc++.h>
using namespace std;

int H, W, R;
long long Pb, Pr, B;
vector<string> g;
vector<int> nwl, nwr, tp; // next wall left/right, target prefix per row
vector<char> covered;
vector<int> distB;
int loA[64], hiA[64], rowA[64], cntA;

inline void ranges(int a, int b) {
    cntA = 0;
    for (int dir = -1; dir <= 1; dir += 2) {
        int L = max(b - R, 0), Rr = min(b + R, W - 1);
        for (int dx = (dir == -1 ? 0 : 1); dx <= R; dx++) {
            int r = a + dir * dx;
            if (r < 0 || r >= H) break;
            if (g[r][b] == '#') break;
            L = max(L, nwl[r * W + b] + 1);
            Rr = min(Rr, nwr[r * W + b] - 1);
            rowA[cntA] = r; loA[cntA] = L; hiA[cntA] = Rr; cntA++;
        }
    }
}

int initGain(int a, int b) {
    ranges(a, b);
    int s = 0;
    for (int i = 0; i < cntA; i++) {
        int r = rowA[i];
        s += tp[r * (W + 1) + hiA[i] + 1] - tp[r * (W + 1) + loA[i]];
    }
    return s;
}
int curGain(int a, int b) {
    ranges(a, b);
    int s = 0;
    for (int i = 0; i < cntA; i++) {
        int r = rowA[i];
        const char* row = g[r].data();
        int base = r * W;
        for (int c = loA[i]; c <= hiA[i]; c++)
            if (row[c] == '.' && !covered[base + c]) s++;
    }
    return s;
}

int main() {
    auto t0 = chrono::steady_clock::now();
    int br, bc;
    if (!(cin >> H >> W >> R)) return 0;
    cin >> Pb >> Pr >> B >> br >> bc;
    g.resize(H);
    for (int i = 0; i < H; i++) {
        cin >> g[i];
        if ((int)g[i].size() < W) g[i].resize(W, '-');
    }
    int N = H * W;
    nwl.assign(N, -1); nwr.assign(N, W); tp.assign(H * (W + 1), 0);
    for (int r = 0; r < H; r++) {
        int last = -1;
        for (int c = 0; c < W; c++) { if (g[r][c] == '#') last = c; nwl[r * W + c] = last; }
        last = W;
        for (int c = W - 1; c >= 0; c--) { if (g[r][c] == '#') last = c; nwr[r * W + c] = last; }
        for (int c = 0; c < W; c++) tp[r * (W + 1) + c + 1] = tp[r * (W + 1) + c] + (g[r][c] == '.');
    }
    covered.assign(N, 0);
    distB.assign(N, INT_MAX);
    vector<int> q; q.reserve(N);
    auto propagate = [&](vector<int>& qq) {
        size_t h = 0;
        while (h < qq.size()) {
            int u = qq[h++];
            int ur = u / W, uc = u % W, du = distB[u] + 1;
            for (int dr = -1; dr <= 1; dr++) {
                int nr = ur + dr; if (nr < 0 || nr >= H) continue;
                for (int dc = -1; dc <= 1; dc++) {
                    int nc = uc + dc; if (nc < 0 || nc >= W) continue;
                    int v = nr * W + nc;
                    if (distB[v] > du) { distB[v] = du; qq.push_back(v); }
                }
            }
        }
        qq.clear();
    };
    distB[br * W + bc] = 0;
    q.push_back(br * W + bc);
    propagate(q);

    priority_queue<pair<double, int>> pq;
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++) {
            if (g[r][c] == '#') continue;
            int gn = initGain(r, c);
            if (gn <= 0) continue;
            double key = gn / (double)(Pr + Pb * distB[r * W + c]);
            pq.push({key, r * W + c});
        }

    vector<int> backbone, routers;
    long long spent = 0;
    long long iter = 0;
    vector<int> path;
    while (!pq.empty()) {
        if ((++iter & 255) == 0) {
            double el = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
            if (el > 9.0) break;
        }
        auto [k, p] = pq.top(); pq.pop();
        int a = p / W, b = p % W;
        int gn = curGain(a, b);
        if (gn <= 0) continue;
        long long d = distB[p];
        long long cost = Pr + Pb * d;
        if (spent + cost > B) continue;
        if (1000LL * gn <= cost) continue;
        double nk = gn / (double)cost;
        if (!pq.empty() && nk < pq.top().first) { pq.push({nk, p}); continue; }
        // place
        path.clear();
        int cur = p;
        while (distB[cur] > 0) {
            path.push_back(cur);
            int cr = cur / W, cc = cur % W, want = distB[cur] - 1, nxt = -1;
            for (int dr = -1; dr <= 1 && nxt < 0; dr++) {
                int nr = cr + dr; if (nr < 0 || nr >= H) continue;
                for (int dc = -1; dc <= 1; dc++) {
                    int nc = cc + dc; if (nc < 0 || nc >= W) continue;
                    if (distB[nr * W + nc] == want) { nxt = nr * W + nc; break; }
                }
            }
            cur = nxt;
        }
        reverse(path.begin(), path.end());
        for (int x : path) { backbone.push_back(x); distB[x] = 0; q.push_back(x); }
        propagate(q);
        routers.push_back(p);
        spent += cost;
        ranges(a, b);
        for (int i = 0; i < cntA; i++) {
            int r = rowA[i];
            for (int c = loA[i]; c <= hiA[i]; c++)
                if (g[r][c] == '.') covered[r * W + c] = 1;
        }
    }

    string out;
    out.reserve((backbone.size() + routers.size()) * 12 + 32);
    out += to_string(backbone.size()) + "\n";
    for (int x : backbone) { out += to_string(x / W); out += ' '; out += to_string(x % W); out += '\n'; }
    out += to_string(routers.size()) + "\n";
    for (int x : routers) { out += to_string(x / W); out += ' '; out += to_string(x % W); out += '\n'; }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
