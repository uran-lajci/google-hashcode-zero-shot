#include <bits/stdc++.h>
using namespace std;

static char ibuf[1 << 16];
size_t ipos = 0, ilen = 0;
inline int gc() {
    if (ipos == ilen) {
        ilen = fread(ibuf, 1, sizeof(ibuf), stdin);
        ipos = 0;
        if (ilen == 0) return -1;
    }
    return ibuf[ipos++];
}
inline int readInt() {
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

int R, C, A, L, V, B, T;
const int K = 15;
const double GAMMA = 0.9;
vector<int8_t> Wr, Wc;
vector<double> cnt, planned;
vector<int> touched;

// Value of a balloon at (r,c) with 0-based altitude index h over K steps
// at constant altitude. Cells already planned this turn are discounted.
double evalTraj(int r, int c, int h, bool mark) {
    double val = 0, g = 1;
    for (int k = 1; k <= K; k++) {
        size_t idx = ((size_t)h * R + r) * C + c;
        int nr = r + Wr[idx];
        if (nr < 0 || nr >= R) {
            if (k == 1) return -1;
            break;
        }
        int nc = ((c + Wc[idx]) % C + C) % C;
        r = nr; c = nc;
        g *= GAMMA;
        int cell = r * C + c;
        val += g * cnt[cell] / (1.0 + planned[cell]);
        if (mark) {
            planned[cell] += 1;
            touched.push_back(cell);
        }
    }
    return val;
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

    size_t total = (size_t)A * R * C;
    Wr.assign(total, 0);
    Wc.assign(total, 0);
    for (int h = 0; h < A; h++)
        for (int r = 0; r < R; r++)
            for (int c = 0; c < C; c++) {
                size_t idx = ((size_t)h * R + r) * C + c;
                Wr[idx] = (int8_t)readInt();
                Wc[idx] = (int8_t)readInt();
            }

    // cnt[cell] = number of target cells covered by a balloon at cell
    vector<int> ci((size_t)R * C, 0);
    for (int k = 0; k < L; k++) {
        int u = tr[k], v = tc[k];
        for (int c = 0; c < C; c++) {
            int d = abs(c - v);
            int cd = min(d, C - d);
            if (cd > V) continue;
            int rem = V * V - cd * cd;
            int h = (int)sqrt((double)rem);
            while (h * h > rem) h--;
            while ((h + 1) * (h + 1) <= rem) h++;
            int r0 = max(0, u - h), r1 = min(R - 1, u + h);
            for (int r = r0; r <= r1; r++) ci[(size_t)r * C + c]++;
        }
    }
    cnt.assign((size_t)R * C, 0.0);
    for (size_t i = 0; i < cnt.size(); i++) cnt[i] = ci[i];
    planned.assign((size_t)R * C, 0.0);

    vector<int> pr(B, rs), pc(B, cs), ph(B, 0);
    vector<char> launched(B, 0), lost(B, 0);

    string out;
    out.reserve((size_t)T * B * 3);
    vector<int> act(B);

    for (int t = 0; t < T; t++) {
        for (int cell : touched) planned[cell] = 0.0;
        touched.clear();

        for (int b = 0; b < B; b++) {
            if (!launched[b]) {
                launched[b] = 1;
                ph[b] = 1;
                act[b] = 1;
                evalTraj(rs, cs, 0, true);
                continue;
            }
            if (lost[b]) {
                act[b] = 0;
                continue;
            }
            int bestD = 0;
            double bestV = evalTraj(pr[b], pc[b], ph[b] - 1, false);
            for (int d : {-1, 1}) {
                int nh = ph[b] + d;
                if (nh < 1 || nh > A) continue;
                double v = evalTraj(pr[b], pc[b], nh - 1, false);
                if (v > bestV + 1e-9) {
                    bestV = v;
                    bestD = d;
                }
            }
            ph[b] += bestD;
            act[b] = bestD;
            evalTraj(pr[b], pc[b], ph[b] - 1, true);
        }

        // Apply wind moves
        for (int b = 0; b < B; b++) {
            if (!launched[b] || lost[b]) continue;
            size_t idx = ((size_t)(ph[b] - 1) * R + pr[b]) * C + pc[b];
            int nr = pr[b] + Wr[idx];
            if (nr < 0 || nr >= R) {
                lost[b] = 1;
            } else {
                int nc = ((pc[b] + Wc[idx]) % C + C) % C;
                pr[b] = nr;
                pc[b] = nc;
            }
        }

        for (int b = 0; b < B; b++) {
            if (b) out.push_back(' ');
            if (act[b] == -1) out += "-1";
            else if (act[b] == 1) out += "1";
            else out += "0";
        }
        out.push_back('\n');
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
