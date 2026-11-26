#include <bits/stdc++.h>
using namespace std;

int H, W, D, B;

struct Proj {
    int type;               // 0 residential, 1 utility
    int h, w, val, area;
    int offR, offC;         // representative occupied cell (nearest to center)
    int tid;                // utility type index
    vector<pair<int,int>> cells;
};

vector<uint8_t> occ;

bool fits(const Proj& p, int r, int c) {
    if (r < 0 || c < 0 || r + p.h > H || c + p.w > W) return false;
    for (const auto& [dr, dc] : p.cells)
        if (occ[(size_t)(r + dr) * W + (c + dc)]) return false;
    return true;
}

void place(const Proj& p, int r, int c) {
    for (const auto& [dr, dc] : p.cells)
        occ[(size_t)(r + dr) * W + (c + dc)] = 1;
}

int main() {
    auto t0 = chrono::steady_clock::now();
    auto el = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - t0).count();
    };

    cin >> H >> W >> D >> B;
    vector<Proj> P(B);
    map<int, int> utilIdx;

    for (int i = 0; i < B; i++) {
        string tp;
        cin >> tp;
        int hp, wp, v;
        cin >> hp >> wp >> v;
        Proj& p = P[i];
        p.type = (tp == "R") ? 0 : 1;
        p.h = hp;
        p.w = wp;
        p.val = v;
        vector<string> g(hp);
        for (auto& s : g) cin >> s;
        for (int r = 0; r < hp; r++)
            for (int c = 0; c < wp; c++)
                if (g[r][c] == '#') p.cells.push_back({r, c});
        p.area = (int)p.cells.size();
        int best = INT_MAX;
        for (auto [r, c] : p.cells) {
            int d = abs(2 * r - (hp - 1)) + abs(2 * c - (wp - 1));
            if (d < best) { best = d; p.offR = r; p.offC = c; }
        }
        p.tid = -1;
        if (p.type == 1) {
            if (!utilIdx.count(v)) {
                int id = (int)utilIdx.size();
                utilIdx[v] = id;
            }
            p.tid = utilIdx[v];
        }
    }

    int T = (int)utilIdx.size();
    vector<vector<int>> projsOfType(T);
    vector<int> resList;
    for (int i = 0; i < B; i++) {
        if (P[i].type == 1) projsOfType[P[i].tid].push_back(i);
        else resList.push_back(i);
    }
    for (auto& lst : projsOfType)
        sort(lst.begin(), lst.end(), [&](int a, int b) { return P[a].area < P[b].area; });
    sort(resList.begin(), resList.end(), [&](int a, int b) {
        return (double)P[a].val / P[a].area > (double)P[b].val / P[b].area;
    });

    occ.assign((size_t)H * W, 0);
    vector<array<int, 3>> placed;

    // Phase 1: utilities
    long long used = 0;
    long long budget = (long long)(0.4 * H * W);
    int s = max(2, D);
    vector<pair<int, int>> L;
    for (int r = 0; r < H; r += s)
        for (int c = 0; c < W; c += s)
            L.push_back({r, c});

    vector<size_t> ptr(T, 0), pi(T, 0);
    vector<int> fails(T, 0);
    vector<char> dead(T, 0);
    bool progress = true;
    while (progress && used < budget && el() < 3.0 && T > 0) {
        progress = false;
        for (int k = 0; k < T && used < budget; k++) {
            if (dead[k]) continue;
            auto& lst = projsOfType[k];
            int pid = lst[pi[k]];
            const Proj& p = P[pid];
            bool ok = false;
            size_t n = L.size();
            for (size_t t = 0; t < n; t++) {
                size_t idx = (ptr[k] + t) % n;
                int r = L[idx].first, c = L[idx].second;
                if (fits(p, r, c)) {
                    place(p, r, c);
                    placed.push_back({pid, r, c});
                    used += p.area;
                    ptr[k] = (idx + 1) % n;
                    ok = true;
                    break;
                }
            }
            pi[k] = (pi[k] + 1) % lst.size();
            if (ok) {
                progress = true;
                fails[k] = 0;
            } else {
                fails[k]++;
                if (fails[k] >= (int)lst.size()) dead[k] = 1;
            }
        }
    }

    // Coverage: number of distinct utility types within distance D of each cell
    vector<vector<int>> cellsByType(T);
    for (auto& pl : placed) {
        const Proj& p = P[pl[0]];
        if (p.type != 1) continue;
        for (auto [dr, dc] : p.cells)
            cellsByType[p.tid].push_back((pl[1] + dr) * W + (pl[2] + dc));
    }
    vector<uint16_t> cover((size_t)H * W, 0);
    vector<int> last((size_t)H * W, -1);
    for (int k = 0; k < T; k++) {
        for (int idx : cellsByType[k]) {
            int y = idx / W, x = idx % W;
            for (int dr = -D; dr <= D; dr++) {
                int rr = y + dr;
                if (rr < 0 || rr >= H) continue;
                int rem = D - abs(dr);
                for (int dc = -rem; dc <= rem; dc++) {
                    int cc = x + dc;
                    if (cc < 0 || cc >= W) continue;
                    int id = rr * W + cc;
                    if (last[id] != k) {
                        last[id] = k;
                        cover[id]++;
                    }
                }
            }
        }
    }

    // Phase 2: residentials, in decreasing coverage order
    vector<int> order;
    order.reserve((size_t)H * W);
    for (int i = 0; i < H * W; i++)
        if (cover[i] > 0) order.push_back(i);
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (cover[a] != cover[b]) return cover[a] > cover[b];
        return a < b;
    });

    for (size_t oi = 0; oi < order.size(); oi++) {
        if ((oi & 255) == 0 && el() > 8.5) break;
        int idx = order[oi];
        int y = idx / W, x = idx % W;
        for (int pid : resList) {
            const Proj& p = P[pid];
            int r = y - p.offR, c = x - p.offC;
            if (fits(p, r, c)) {
                place(p, r, c);
                placed.push_back({pid, r, c});
                break;
            }
        }
    }

    // Output
    string out;
    out += to_string(placed.size()) + "\n";
    for (auto& pl : placed) {
        out += to_string(pl[0]) + " " + to_string(pl[1]) + " " + to_string(pl[2]) + "\n";
    }
    fputs(out.c_str(), stdout);
    return 0;
}
