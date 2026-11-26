#include <bits/stdc++.h>
using namespace std;

static char ibuf[1 << 16];
static size_t ipos = 0, ilen = 0;
static inline int gc() {
    if (ipos == ilen) {
        ilen = fread(ibuf, 1, sizeof(ibuf), stdin);
        ipos = 0;
        if (ilen == 0) return -1;
    }
    return ibuf[ipos++];
}
static inline int readInt() {
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

int main() {
    int R = readInt(), C = readInt(), A = readInt();
    int L = readInt(), V = readInt(), B = readInt(), T = readInt();
    int rs = readInt(), cs = readInt();
    vector<int> tr(L), tc(L);
    for (int j = 0; j < L; j++) {
        tr[j] = readInt();
        tc[j] = readInt();
    }
    size_t total = (size_t)A * R * C;
    vector<int8_t> WD(total), WC(total);
    for (size_t k = 0; k < total; k++) {
        WD[k] = (int8_t)readInt();
        WC[k] = (int8_t)readInt();
    }

    // Coverage lists: for each cell, the targets whose disk covers it.
    vector<vector<int>> cover((size_t)R * C);
    for (int j = 0; j < L; j++) {
        int r = tr[j], c = tc[j];
        for (int v = 0; v < C; v++) {
            int d = abs(v - c);
            int cd = min(d, C - d);
            if (cd > V) continue;
            for (int dr = -V; dr <= V; dr++) {
                if (dr * dr + cd * cd > V * V) continue;
                int rr = r + dr;
                if (rr < 0 || rr >= R) continue;
                cover[(size_t)rr * C + v].push_back(j);
            }
        }
    }

    auto mv = [&](int r, int c, int h, int &nr, int &nc) -> bool {
        size_t idx = ((size_t)(h - 1) * R + r) * C + c;
        int a = r + WD[idx];
        if (a < 0 || a >= R) return false;
        int b = ((c + WC[idx]) % C + C) % C;
        nr = a;
        nc = b;
        return true;
    };

    const double gamma = 0.6;
    const int H = 6;

    auto gainNow = [&](int cell, int t) -> int {
        int cnt = 0;
        for (int j : cover[cell])
            if (stamp_check: true) {}
        return cnt;
    };
    (void)gainNow;

    vector<int> stampArr(L, 0);
    auto gainNow2 = [&](int cell, int t) -> double {
        int cnt = 0;
        for (int j : cover[cell])
            if (stampArr[j] != t + 1) cnt++;
        return cnt;
    };
    auto future = [&](int r, int c, int h) -> double {
        double f = 0, g = 1;
        int pr = r, pc = c;
        for (int k = 1; k <= H; k++) {
            g *= gamma;
            int nr, nc;
            if (!mv(pr, pc, h, nr, nc)) break;
            pr = nr;
            pc = nc;
            f += g * (double)cover[(size_t)pr * C + pc].size();
        }
        return f;
    };
    auto markCover = [&](int cell, int t) {
        for (int j : cover[cell]) stampArr[j] = t + 1;
    };

    vector<int> st(B, 0), pr(B, 0), pc(B, 0), ph(B, 0);
    vector<vector<int>> out(T, vector<int>(B, 0));

    for (int t = 0; t < T; t++) {
        for (int i = 0; i < B; i++) {
            if (st[i] == 2) { out[t][i] = 0; continue; }
            if (st[i] == 0) {
                int nr, nc;
                if (!mv(rs, cs, 1, nr, nc)) { out[t][i] = 0; continue; }
                int cell = nr * C + nc;
                double sc = gainNow2(cell, t) + future(nr, nc, 1);
                if (sc > 0) {
                    st[i] = 1;
                    pr[i] = nr;
                    pc[i] = nc;
                    ph[i] = 1;
                    out[t][i] = 1;
                    markCover(cell, t);
                } else {
                    out[t][i] = 0;
                }
                continue;
            }
            // launched and alive
            double best = -1e18;
            int bestd = 0, bnr = -1, bnc = -1, bnh = -1;
            const int ds[3] = {0, -1, 1};
            for (int d : ds) {
                int nh = ph[i] + d;
                if (nh < 1 || nh > A) continue;
                int nr, nc;
                if (!mv(pr[i], pc[i], nh, nr, nc)) continue;
                double sc = gainNow2(nr * C + nc, t) + future(nr, nc, nh);
                if (sc > best) {
                    best = sc;
                    bestd = d;
                    bnr = nr;
                    bnc = nc;
                    bnh = nh;
                }
            }
            if (bnr < 0) {
                // every option loses the balloon
                st[i] = 2;
                out[t][i] = 0;
                continue;
            }
            out[t][i] = bestd;
            ph[i] = bnh;
            pr[i] = bnr;
            pc[i] = bnc;
            markCover(bnr * C + bnc, t);
        }
    }

    string s;
    s.reserve((size_t)T * B * 3 + 16);
    for (int t = 0; t < T; t++) {
        for (int i = 0; i < B; i++) {
            if (i) s += ' ';
            s += to_string(out[t][i]);
        }
        s += '\n';
    }
    fwrite(s.data(), 1, s.size(), stdout);
    return 0;
}
