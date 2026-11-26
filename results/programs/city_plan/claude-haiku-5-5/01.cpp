#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64;

struct Proj {
    int t;   // 0 = residential, 1 = utility
    int hp, wp, val;
    vector<pair<int,int>> cells;
};

struct Pl { int b, r, c; };

int H, W, D, B;
vector<Proj> P;
vector<uint8_t> occ;
vector<array<u64,16>> cov;

static bool fits(const Proj& p, int r, int c) {
    if (r < 0 || c < 0 || r + p.hp > H || c + p.wp > W) return false;
    for (auto& [dr, dc] : p.cells)
        if (occ[(r + dr) * W + (c + dc)]) return false;
    return true;
}

static void occupy(const Proj& p, int r, int c) {
    for (auto& [dr, dc] : p.cells) occ[(r + dr) * W + (c + dc)] = 1;
}

static void updateCov(const Proj& p, int r, int c) {
    int r0 = max(0, r - D), r1 = min(H, r + p.hp + D);
    int c0 = max(0, c - D), c1 = min(W, c + p.wp + D);
    int bh = r1 - r0, bw = c1 - c0;
    const int INF = 1000000000;
    vector<int> d((size_t)bh * bw, INF);
    for (auto& [dr, dc] : p.cells)
        d[(size_t)(r + dr - r0) * bw + (c + dc - c0)] = 0;
    for (int i = 0; i < bh; i++)
        for (int j = 0; j < bw; j++) {
            int idx = i * bw + j;
            if (i > 0) d[idx] = min(d[idx], d[idx - bw] + 1);
            if (j > 0) d[idx] = min(d[idx], d[idx - 1] + 1);
        }
    for (int i = bh - 1; i >= 0; i--)
        for (int j = bw - 1; j >= 0; j--) {
            int idx = i * bw + j;
            if (i < bh - 1) d[idx] = min(d[idx], d[idx + bw] + 1);
            if (j < bw - 1) d[idx] = min(d[idx], d[idx + 1] + 1);
        }
    int t = p.val;
    int word = t / 64; u64 bit = 1ULL << (t % 64);
    for (int i = 0; i < bh; i++)
        for (int j = 0; j < bw; j++)
            if (d[i * bw + j] <= D)
                cov[(size_t)(r0 + i) * W + (c0 + j)][word] |= bit;
}

static long long evalRes(const Proj& p, int r, int c) {
    u64 acc[16] = {0};
    for (auto& [dr, dc] : p.cells) {
        const auto& cv = cov[(size_t)(r + dr) * W + (c + dc)];
        for (int k = 0; k < 16; k++) acc[k] |= cv[k];
    }
    long long cnt = 0;
    for (int k = 0; k < 16; k++) cnt += __builtin_popcountll(acc[k]);
    return cnt * (long long)p.val;
}

int main() {
    auto T0 = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - T0).count();
    };

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> D >> B;
    P.resize(B);
    for (int i = 0; i < B; i++) {
        char tp; int hp, wp, val;
        cin >> tp >> hp >> wp >> val;
        Proj& p = P[i];
        p.t = (tp == 'U') ? 1 : 0;
        p.hp = hp; p.wp = wp; p.val = val;
        for (int a = 0; a < hp; a++) {
            string s; cin >> s;
            for (int b = 0; b < wp; b++)
                if (s[b] == '#') p.cells.push_back({a, b});
        }
    }

    vector<int> resIdx, utIdx;
    for (int i = 0; i < B; i++) (P[i].t == 0 ? resIdx : utIdx).push_back(i);

    occ.assign((size_t)H * W, 0);
    cov.assign((size_t)H * W, array<u64,16>{});
    vector<Pl> out;
    long long usedCells = 0;

    // Phase 1: utilities on a lattice.
    {
        const double frac = 0.3;
        long long limitCells = (long long)(frac * H * W);
        int st = max(1, D);
        int U = utIdx.size();
        int ptr = 0;
        bool stop = false;
        for (int r = 0; r < H && !stop; r += st) {
            for (int c = 0; c < W; c += st) {
                if (usedCells >= limitCells || elapsed() > 2.5) { stop = true; break; }
                int attempts = min(U, 8);
                for (int a = 0; a < attempts; a++) {
                    int id = utIdx[(ptr + a) % U];
                    if (fits(P[id], r, c)) {
                        occupy(P[id], r, c);
                        updateCov(P[id], r, c);
                        out.push_back({id, r, c});
                        usedCells += P[id].cells.size();
                        ptr = (ptr + a + 1) % U;
                        break;
                    }
                }
            }
        }
    }

    // Prefix sums of cells having any utility coverage.
    vector<int> pre((size_t)(H + 1) * (W + 1), 0);
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++) {
            bool any = false;
            const auto& cv = cov[(size_t)i * W + j];
            for (int k = 0; k < 16 && !any; k++) if (cv[k]) any = true;
            pre[(size_t)(i + 1) * (W + 1) + (j + 1)] =
                (any ? 1 : 0) + pre[(size_t)i * (W + 1) + (j + 1)]
                + pre[(size_t)(i + 1) * (W + 1) + j] - pre[(size_t)i * (W + 1) + j];
        }
    auto rectSum = [&](int r, int c, int h, int w) {
        return pre[(size_t)(r + h) * (W + 1) + (c + w)]
             - pre[(size_t)r * (W + 1) + (c + w)]
             - pre[(size_t)(r + h) * (W + 1) + c]
             + pre[(size_t)r * (W + 1) + c];
    };

    // Phase 2: residentials by immediate gain.
    if (!resIdx.empty()) {
        int R = resIdx.size();
        int K = min(R, 6);
        bool stop = false;
        for (int r = 0; r < H && !stop; r++) {
            if (elapsed() > 6.5) break;
            for (int c = 0; c < W; c++) {
                if ((c & 63) == 0 && elapsed() > 6.5) { stop = true; break; }
                long long bestGain = 0;
                int bestP = -1;
                int start = (r * 31 + c) % R;
                for (int k = 0; k < K; k++) {
                    int id = resIdx[(start + k) % R];
                    const Proj& p = P[id];
                    if (r + p.hp > H || c + p.wp > W) continue;
                    if (rectSum(r, c, p.hp, p.wp) == 0) continue;
                    if (!fits(p, r, c)) continue;
                    long long g = evalRes(p, r, c);
                    if (g > bestGain) { bestGain = g; bestP = id; }
                }
                if (bestP >= 0) {
                    occupy(P[bestP], r, c);
                    out.push_back({bestP, r, c});
                }
            }
        }
    }

    // Phase 3: fill leftover space with utilities (never lowers the score).
    if (!utIdx.empty()) {
        int U = utIdx.size();
        int ptr = 0;
        bool stop = false;
        for (int r = 0; r < H && !stop; r++) {
            if (elapsed() > 8.0) break;
            for (int c = 0; c < W; c++) {
                if ((c & 63) == 0 && elapsed() > 8.0) { stop = true; break; }
                if (occ[(size_t)r * W + c]) continue;
                int attempts = min(U, 4);
                for (int a = 0; a < attempts; a++) {
                    int id = utIdx[(ptr + a) % U];
                    if (fits(P[id], r, c)) {
                        occupy(P[id], r, c);
                        out.push_back({id, r, c});
                        ptr = (ptr + a + 1) % U;
                        break;
                    }
                }
            }
        }
    }

    // Output.
    string s;
    s.reserve(out.size() * 16 + 16);
    s += to_string(out.size());
    s += '\n';
    for (auto& pl : out) {
        s += to_string(pl.b); s += ' ';
        s += to_string(pl.r); s += ' ';
        s += to_string(pl.c); s += '\n';
    }
    fwrite(s.data(), 1, s.size(), stdout);
    return 0;
}
