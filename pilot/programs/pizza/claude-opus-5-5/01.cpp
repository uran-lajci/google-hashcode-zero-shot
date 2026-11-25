#include <bits/stdc++.h>
using namespace std;

int R, C, L, H;
vector<string> g;
vector<int> pre; // (R+1)*(C+1)
vector<int> owner;
struct Sl { int r1, c1, r2, c2; bool alive; };
vector<Sl> sl;
vector<pair<int,int>> shapes; // ascending area
long long work = 0;
long long score = 0;
mt19937_64 rng(12345);

inline int P(int r, int c) { return pre[r * (C + 1) + c]; }
inline int cntT(int r1, int c1, int r2, int c2) {
    return P(r2 + 1, c2 + 1) - P(r1, c2 + 1) - P(r2 + 1, c1) + P(r1, c1);
}
inline bool ingOK(int r1, int c1, int r2, int c2) {
    int a = (r2 - r1 + 1) * (c2 - c1 + 1);
    int t = cntT(r1, c1, r2, c2);
    return t >= L && a - t >= L;
}
inline bool isFree(int r1, int c1, int r2, int c2) {
    for (int r = r1; r <= r2; r++) {
        const int* o = &owner[r * C];
        for (int c = c1; c <= c2; c++) { work++; if (o[c] >= 0) return false; }
    }
    return true;
}
int place(int r1, int c1, int r2, int c2) {
    int id = sl.size();
    sl.push_back({r1, c1, r2, c2, true});
    for (int r = r1; r <= r2; r++) for (int c = c1; c <= c2; c++) owner[r * C + c] = id;
    score += (r2 - r1 + 1) * (c2 - c1 + 1);
    return id;
}
void killS(int id) {
    Sl& s = sl[id];
    for (int r = s.r1; r <= s.r2; r++) for (int c = s.c1; c <= s.c2; c++) owner[r * C + c] = -1;
    s.alive = false;
    score -= (s.r2 - s.r1 + 1) * (s.c2 - s.c1 + 1);
}
void revive(int id) {
    Sl& s = sl[id];
    for (int r = s.r1; r <= s.r2; r++) for (int c = s.c1; c <= s.c2; c++) owner[r * C + c] = id;
    s.alive = true;
    score += (s.r2 - s.r1 + 1) * (s.c2 - s.c1 + 1);
}
void extendS(int id, int startDir) {
    bool ch = true;
    while (ch) {
        ch = false;
        for (int k = 0; k < 4; k++) {
            int d = (startDir + k) & 3;
            Sl& s = sl[id];
            int h = s.r2 - s.r1 + 1, w = s.c2 - s.c1 + 1;
            if (d == 0 || d == 1) {
                if ((h + 1) * w > H) continue;
                int nr = d == 0 ? s.r1 - 1 : s.r2 + 1;
                if (nr < 0 || nr >= R) continue;
                if (!isFree(nr, s.c1, nr, s.c2)) continue;
                for (int c = s.c1; c <= s.c2; c++) owner[nr * C + c] = id;
                if (d == 0) s.r1 = nr; else s.r2 = nr;
                score += w; ch = true;
            } else {
                if (h * (w + 1) > H) continue;
                int nc = d == 2 ? s.c1 - 1 : s.c2 + 1;
                if (nc < 0 || nc >= C) continue;
                if (!isFree(s.r1, nc, s.r2, nc)) continue;
                for (int r = s.r1; r <= s.r2; r++) owner[r * C + nc] = id;
                if (d == 2) s.c1 = nc; else s.c2 = nc;
                score += h; ch = true;
            }
        }
    }
}
// corner: bit0 -> rows go up, bit1 -> cols go left. mode 0 smallest,1 largest,2 random
int tryAnchor(int r, int c, int corner, int mode) {
    int dr = (corner & 1) ? -1 : 1, dc = (corner & 2) ? -1 : 1;
    int n = shapes.size();
    static vector<int> cand;
    cand.clear();
    for (int k = 0; k < n; k++) {
        int idx = (mode == 1) ? n - 1 - k : k;
        int h = shapes[idx].first, w = shapes[idx].second;
        int r1, r2, c1, c2;
        if (dr > 0) { r1 = r; r2 = r + h - 1; } else { r2 = r; r1 = r - h + 1; }
        if (dc > 0) { c1 = c; c2 = c + w - 1; } else { c2 = c; c1 = c - w + 1; }
        work++;
        if (r1 < 0 || c1 < 0 || r2 >= R || c2 >= C) continue;
        if (!ingOK(r1, c1, r2, c2)) continue;
        if (!isFree(r1, c1, r2, c2)) continue;
        if (mode != 2) return place(r1, c1, r2, c2);
        cand.push_back(idx);
    }
    if (cand.empty()) return -1;
    int idx = cand[rng() % cand.size()];
    int h = shapes[idx].first, w = shapes[idx].second;
    int r1, r2, c1, c2;
    if (dr > 0) { r1 = r; r2 = r + h - 1; } else { r2 = r; r1 = r - h + 1; }
    if (dc > 0) { c1 = c; c2 = c + w - 1; } else { c2 = c; c1 = c - w + 1; }
    return place(r1, c1, r2, c2);
}

int main() {
    auto t0 = chrono::steady_clock::now();
    if (!(cin >> R >> C >> L >> H)) return 0;
    g.resize(R);
    for (int i = 0; i < R; i++) cin >> g[i];
    pre.assign((R + 1) * (C + 1), 0);
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            pre[(r + 1) * (C + 1) + c + 1] = pre[r * (C + 1) + c + 1] + pre[(r + 1) * (C + 1) + c] - pre[r * (C + 1) + c] + (g[r][c] == 'T');
    owner.assign(R * C, -1);
    for (int h = 1; h <= R; h++)
        for (int w = 1; w <= C; w++)
            if (h * w <= H && h * w >= 2 * L) shapes.push_back({h, w});
    stable_sort(shapes.begin(), shapes.end(), [](auto& a, auto& b) { return a.first * a.second < b.first * b.second; });

    if (!shapes.empty()) {
        for (int r = 0; r < R; r++)
            for (int c = 0; c < C; c++)
                if (owner[r * C + c] < 0) tryAnchor(r, c, 0, 0);
        for (int i = 0; i < (int)sl.size(); i++) if (sl[i].alive) extendS(i, 0);

        const long long BUDGET = 1500000000LL;
        long long iters = 0;
        vector<int> removed, added, freed;
        int maxSide = 0;
        for (auto& s : shapes) maxSide = max(maxSide, max(s.first, s.second));
        while (work < BUDGET && iters < 30000000LL) {
            iters++;
            if ((iters & 255) == 0) {
                double el = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
                if (el > 9.0) break;
            }
            int r = rng() % R, c = rng() % C;
            work++;
            if (owner[r * C + c] >= 0) continue;
            int k = 1 + rng() % 3;
            int wr1 = max(0, r - k), wr2 = min(R - 1, r + k);
            int wc1 = max(0, c - k), wc2 = min(C - 1, c + k);
            removed.clear(); added.clear(); freed.clear();
            for (int y = wr1; y <= wr2; y++)
                for (int x = wc1; x <= wc2; x++) {
                    int o = owner[y * C + x];
                    work++;
                    if (o >= 0) {
                        if (find(removed.begin(), removed.end(), o) == removed.end()) removed.push_back(o);
                    } else freed.push_back(y * C + x);
                }
            long long oldScore = score;
            for (int id : removed) {
                Sl& s = sl[id];
                for (int y = s.r1; y <= s.r2; y++)
                    for (int x = s.c1; x <= s.c2; x++)
                        if (y < wr1 || y > wr2 || x < wc1 || x > wc2) freed.push_back(y * C + x);
                killS(id);
            }
            int corner = rng() & 3;
            int mode = rng() % 3;
            int dr = (corner & 1) ? -1 : 1, dc = (corner & 2) ? -1 : 1;
            sort(freed.begin(), freed.end(), [&](int a, int b) {
                int ra = a / C * dr, rb = b / C * dr;
                if (ra != rb) return ra < rb;
                return (a % C) * dc < (b % C) * dc;
            });
            for (int cell : freed) {
                if (owner[cell] >= 0) continue;
                int id = tryAnchor(cell / C, cell % C, corner, mode);
                if (id >= 0) added.push_back(id);
            }
            int sd = rng() & 3;
            for (int id : added) extendS(id, sd);
            if (score >= oldScore) {
                // accept
            } else {
                for (int id : added) killS(id);
                for (int id : removed) revive(id);
                sl.resize(sl.size() - added.size());
            }
            if (sl.size() > 4000000) {
                // compact
                vector<Sl> ns;
                for (auto& s : sl) if (s.alive) ns.push_back(s);
                sl = ns;
                fill(owner.begin(), owner.end(), -1);
                for (int i = 0; i < (int)sl.size(); i++) {
                    Sl& s = sl[i];
                    for (int y = s.r1; y <= s.r2; y++) for (int x = s.c1; x <= s.c2; x++) owner[y * C + x] = i;
                }
            }
        }
    }
    vector<Sl> out;
    for (auto& s : sl) if (s.alive) out.push_back(s);
    string res;
    res += to_string(out.size()) + "\n";
    for (auto& s : out) {
        res += to_string(s.r1) + " " + to_string(s.c1) + " " + to_string(s.r2) + " " + to_string(s.c2) + "\n";
    }
    fwrite(res.data(), 1, res.size(), stdout);
    return 0;
}
