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
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return neg ? -x : x;
}

int main() {
    auto startTime = chrono::steady_clock::now();
    int R = readInt(), C = readInt(), A = readInt();
    int L = readInt(), V = readInt(), B = readInt(), T = readInt();
    int rs = readInt(), cs = readInt();
    vector<int> tr(L), tc(L);
    for (int i = 0; i < L; i++) { tr[i] = readInt(); tc[i] = readInt(); }

    int N = R * C;
    vector<signed char> WR((size_t)A * N), WC((size_t)A * N);
    for (int a = 0; a < A; a++) {
        for (int idx = 0; idx < N; idx++) {
            WR[(size_t)a * N + idx] = (signed char)readInt();
            WC[(size_t)a * N + idx] = (signed char)readInt();
        }
    }

    // For each balloon cell, list of targets it would cover.
    vector<vector<int>> lst(N);
    vector<int> stamp(N, -1);
    for (int ti = 0; ti < L; ti++) {
        int u = tr[ti], v = tc[ti];
        for (int r = max(0, u - V); r <= min(R - 1, u + V); r++) {
            int dr2 = (r - u) * (r - u);
            for (int d = -V; d <= V; d++) {
                int x = ((d % C) + C) % C;
                int cd = min(x, C - x);
                if (dr2 + cd * cd <= V * V) {
                    int c = ((v + d) % C + C) % C;
                    int idx = r * C + c;
                    if (stamp[idx] != ti) {
                        stamp[idx] = ti;
                        lst[idx].push_back(ti);
                    }
                }
            }
        }
    }

    const double LAM = 0.5, GAM = 0.8;
    const double TIME_LIMIT = 8.0;

    vector<char> launched(B, 0), alive(B, 1);
    vector<int> alt(B, 0), pr(B, rs), pc(B, cs);
    vector<int> covT(L, -1);

    auto mv = [&](int a0, int r, int c, int &nr, int &nc) -> bool {
        size_t idx = (size_t)a0 * N + (size_t)r * C + c;
        nr = r + WR[idx];
        if (nr < 0 || nr >= R) return false;
        nc = ((c + WC[idx]) % C + C) % C;
        return true;
    };

    auto gain = [&](int r, int c, int turn) -> int {
        int g = 0;
        for (int t : lst[r * C + c]) if (covT[t] != turn) g++;
        return g;
    };

    auto look = [&](int a1, int r, int c, int H) -> double {
        double s = 0, pw = 1;
        int cr = r, cc = c;
        for (int k = 1; k <= H; k++) {
            int nr, nc;
            if (!mv(a1 - 1, cr, cc, nr, nc)) break;
            pw *= GAM;
            s += pw * (double)lst[nr * C + nc].size();
            cr = nr; cc = nc;
        }
        return s;
    };

    string out;
    out.reserve((size_t)T * B * 3 + 16);
    bool timeUp = false;

    for (int turn = 0; turn < T; turn++) {
        vector<int> act(B, 0);
        if (!timeUp) {
            double el = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
            if (el > TIME_LIMIT) timeUp = true;
        }
        if (!timeUp) {
            int H = min(10, T - 1 - turn);
            for (int i = 0; i < B; i++) {
                if (!launched[i]) {
                    int nr, nc;
                    bool ok = mv(0, rs, cs, nr, nc);
                    double value = 0;
                    if (ok) value = gain(nr, nc, turn) + LAM * look(1, nr, nc, H);
                    if (ok && value > 0) {
                        launched[i] = 1;
                        alt[i] = 1;
                        pr[i] = nr; pc[i] = nc;
                        act[i] = 1;
                        for (int t : lst[nr * C + nc]) covT[t] = turn;
                    } else if (!ok && value <= 0) {
                        // not launching: value 0 means no benefit
                    }
                } else if (alive[i]) {
                    double best = -1e18;
                    int bestD = 0;
                    int bestR = pr[i], bestC = pc[i];
                    bool bestOk = false;
                    const int order[3] = {0, -1, 1};
                    for (int oi = 0; oi < 3; oi++) {
                        int d = order[oi];
                        int na = alt[i] + d;
                        if (na < 1 || na > A) continue;
                        int nr, nc;
                        bool ok = mv(na - 1, pr[i], pc[i], nr, nc);
                        double val;
                        if (ok) val = gain(nr, nc, turn) + LAM * look(na, nr, nc, H);
                        else val = -1.0;
                        if (val > best + 1e-12) {
                            best = val;
                            bestD = d;
                            bestR = nr; bestC = nc;
                            bestOk = ok;
                        }
                    }
                    alt[i] += bestD;
                    act[i] = bestD;
                    if (bestOk) {
                        pr[i] = bestR; pc[i] = bestC;
                        for (int t : lst[bestR * C + bestC]) covT[t] = turn;
                    } else {
                        alive[i] = 0;
                    }
                }
            }
        }
        for (int i = 0; i < B; i++) {
            if (i) out.push_back(' ');
            if (act[i] == 1) out.push_back('1');
            else if (act[i] == -1) out.push_back('-'), out.push_back('1');
            else out.push_back('0');
        }
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
