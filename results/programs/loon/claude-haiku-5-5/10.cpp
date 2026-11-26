#include <bits/stdc++.h>
using namespace std;

static inline int gc() {
    static char b[1 << 16];
    static size_t n = 0, p = 0;
    if (p == n) {
        n = fread(b, 1, sizeof(b), stdin);
        p = 0;
        if (n == 0) return -1;
    }
    return b[p++];
}

static int readInt() {
    int c = gc();
    while (c != '-' && (c < '0' || c > '9')) {
        if (c == -1) return 0;
        c = gc();
    }
    bool neg = false;
    if (c == '-') { neg = true; c = gc(); }
    int x = 0;
    while (c >= '0' && c <= '9') {
        x = x * 10 + (c - '0');
        c = gc();
    }
    return neg ? -x : x;
}

int R, C, A, L, V, B, T;
int RC;
vector<int8_t> wdr;
vector<int16_t> wdc;

// altitude index a0 (0-based), cell index -> new cell or -1 if lost
inline int moveCell(int a0, int cell) {
    int r = cell / C, c = cell % C;
    int idx = a0 * RC + cell;
    int nr = r + wdr[idx];
    if (nr < 0 || nr >= R) return -1;
    int nc = ((c + wdc[idx]) % C + C) % C;
    return nr * C + nc;
}

int main() {
    R = readInt(); C = readInt(); A = readInt();
    L = readInt(); V = readInt(); B = readInt(); T = readInt();
    int rs = readInt(), cs = readInt();
    vector<int> tr(L), tc(L);
    for (int i = 0; i < L; i++) {
        tr[i] = readInt();
        tc[i] = readInt();
    }
    RC = R * C;
    size_t total = (size_t)A * RC;
    wdr.assign(total, 0);
    wdc.assign(total, 0);
    for (int a = 0; a < A; a++) {
        for (int r = 0; r < R; r++) {
            for (int c = 0; c < C; c++) {
                size_t idx = (size_t)a * RC + r * C + c;
                wdr[idx] = (int8_t)readInt();
                wdc[idx] = (int16_t)readInt();
            }
        }
    }

    // Coverage lists: for each cell, targets covered by a balloon there.
    vector<vector<int>> ct(RC);
    vector<int> seen(RC, -1);
    for (int t = 0; t < L; t++) {
        int r0 = tr[t], c0 = tc[t];
        for (int u = max(0, r0 - V); u <= min(R - 1, r0 + V); u++) {
            int du = r0 - u;
            if (du * du > V * V) continue;
            for (int dc = -V; dc <= V; dc++) {
                int d = ((dc % C) + C) % C;
                int cd = min(d, C - d);
                if (du * du + cd * cd > V * V) continue;
                int v = ((c0 + dc) % C + C) % C;
                int cell = u * C + v;
                if (seen[cell] != t) {
                    seen[cell] = t;
                    ct[cell].push_back(t);
                }
            }
        }
    }
    vector<int> cov(RC);
    for (int i = 0; i < RC; i++) cov[i] = (int)ct[i].size();

    // Horizon for backward DP.
    int H = min(T, 60);
    double work = 3.0 * (double)A * (double)RC;
    if (work > 0 && (double)H * work > 4e8) {
        H = max(1, (int)(4e8 / work));
    }
    H = max(1, H);

    vector<int> prevV(total, 0), curV(total, 0);
    for (int h = 0; h < H; h++) {
        for (int a = 0; a < A; a++) {
            for (int cell = 0; cell < RC; cell++) {
                int best = -1;
                for (int d = -1; d <= 1; d++) {
                    int a2 = a + d;
                    if (a2 < 0 || a2 >= A) continue;
                    int nc = moveCell(a2, cell);
                    int val = (nc < 0) ? 0 : cov[nc] + prevV[(size_t)a2 * RC + nc];
                    if (val > best) best = val;
                }
                curV[(size_t)a * RC + cell] = best;
            }
        }
        swap(prevV, curV);
    }
    // prevV now holds the potential V_H.

    // Simulation state.
    vector<int> alt(B, 0);   // 1-based altitude, 0 = not launched
    vector<int> pos(B, -1);  // cell index, -1 = lost
    int nextLaunch = 0;
    int startCell = rs * C + cs;
    int rate = max(1, (2 * B + T - 1) / T);

    string out;
    out.reserve((size_t)T * B * 3);
    vector<int> cnt(L, 0);
    vector<int> adj(B, 0);

    for (int turn = 0; turn < T; turn++) {
        fill(cnt.begin(), cnt.end(), 0);
        fill(adj.begin(), adj.end(), 0);

        // Move already-launched balloons.
        for (int i = 0; i < B; i++) {
            if (alt[i] == 0) continue;
            if (pos[i] < 0) { adj[i] = 0; continue; }
            int a = alt[i];
            int p = pos[i];
            int bestA = a;
            double bestScore = -1e18;
            int bestNc = -1;
            int order[3] = {a, a + 1, a - 1};
            for (int k = 0; k < 3; k++) {
                int a2 = order[k];
                if (a2 < 1 || a2 > A) continue;
                int nc = moveCell(a2 - 1, p);
                double score;
                if (nc < 0) {
                    score = -1e9;
                } else {
                    int newImm = 0;
                    for (int t : ct[nc]) if (cnt[t] == 0) newImm++;
                    double pot = (double)prevV[(size_t)(a2 - 1) * RC + nc] / H;
                    score = newImm + pot;
                }
                if (score > bestScore) {
                    bestScore = score;
                    bestA = a2;
                    bestNc = nc;
                }
            }
            adj[i] = bestA - a;
            alt[i] = bestA;
            pos[i] = bestNc;
            if (bestNc >= 0) {
                for (int t : ct[bestNc]) cnt[t]++;
            }
        }

        // Launch new balloons.
        for (int k = 0; k < rate && nextLaunch < B; k++) {
            int i = nextLaunch++;
            alt[i] = 1;
            adj[i] = 1;
            int nc = moveCell(0, startCell);
            pos[i] = nc;
            if (nc >= 0) {
                for (int t : ct[nc]) cnt[t]++;
            }
        }

        for (int i = 0; i < B; i++) {
            if (i) out.push_back(' ');
            if (adj[i] == 1) out.push_back('1');
            else if (adj[i] == -1) out += "-1";
            else out.push_back('0');
        }
        out.push_back('\n');
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
