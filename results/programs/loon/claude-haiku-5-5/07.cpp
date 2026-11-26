#include <bits/stdc++.h>
using namespace std;

static char buf[1 << 25];
size_t bufLen = 0, bufPos = 0;

inline int readChar() {
    if (bufPos == bufLen) {
        bufLen = fread(buf, 1, sizeof(buf), stdin);
        bufPos = 0;
        if (bufLen == 0) return -1;
    }
    return buf[bufPos++];
}

inline int readInt() {
    int ch = readChar();
    while (ch != '-' && (ch < '0' || ch > '9')) {
        if (ch == -1) return 0;
        ch = readChar();
    }
    bool neg = false;
    if (ch == '-') { neg = true; ch = readChar(); }
    int x = 0;
    while (ch >= '0' && ch <= '9') {
        x = x * 10 + (ch - '0');
        ch = readChar();
    }
    return neg ? -x : x;
}

int R, C, A, L, V, B, T, RS, CS;
vector<int8_t> W;   // index: (((alt-1)*R + r)*C + c)*2 -> dr, dc
vector<int> cov;    // static coverage count per cell
const int D = 4;
const double G = 0.9;

inline int wIdx(int a, int r, int c) {
    return ((a - 1) * R + r) * C + c;
}

double f(int a, int r, int c, int depth) {
    if (depth == 0) return 0.0;
    double best = 0.0;
    for (int d = -1; d <= 1; d++) {
        int na = a + d;
        if (na < 1 || na > A) continue;
        int idx = wIdx(na, r, c) * 2;
        int nr = r + W[idx];
        if (nr < 0 || nr >= R) continue;
        int nc = ((c + W[idx + 1]) % C + C) % C;
        double v = cov[nr * C + nc] + G * f(na, nr, nc, depth - 1);
        if (v > best) best = v;
    }
    return best;
}

int main() {
    R = readInt(); C = readInt(); A = readInt();
    L = readInt(); V = readInt(); B = readInt(); T = readInt();
    RS = readInt(); CS = readInt();

    vector<int> tr(L), tc(L);
    for (int i = 0; i < L; i++) {
        tr[i] = readInt();
        tc[i] = readInt();
    }

    // Read wind
    W.assign((size_t)A * R * C * 2, 0);
    for (int a = 1; a <= A; a++) {
        for (int r = 0; r < R; r++) {
            for (int c = 0; c < C; c++) {
                size_t idx = (size_t)wIdx(a, r, c) * 2;
                W[idx] = (int8_t)readInt();
                W[idx + 1] = (int8_t)readInt();
            }
        }
    }

    // Static coverage
    cov.assign((size_t)R * C, 0);
    int V2 = V * V;
    for (int i = 0; i < L; i++) {
        int u = tr[i], v = tc[i];
        for (int dr = -V; dr <= V; dr++) {
            int r = u + dr;
            if (r < 0 || r >= R) continue;
            int w = (int)floor(sqrt((double)(V2 - dr * dr)) + 1e-9);
            while (w > 0 && w * w > V2 - dr * dr) w--;
            while ((w + 1) * (w + 1) <= V2 - dr * dr) w++;
            if (2 * w + 1 >= C) {
                for (int c = 0; c < C; c++) cov[r * C + c]++;
            } else {
                for (int dc = -w; dc <= w; dc++) {
                    int c = ((v + dc) % C + C) % C;
                    cov[r * C + c]++;
                }
            }
        }
    }

    // Simulation state
    vector<int> alt(B, 0), pr(B, 0), pc(B, 0);
    vector<int> state(B, 0); // 0 unlaunched, 1 active, 2 lost
    vector<int> occ((size_t)R * C, 0);
    vector<int> touched;

    string out;
    out.reserve((size_t)T * B * 3 + 16);

    for (int t = 0; t < T; t++) {
        for (int idx : touched) occ[idx] = 0;
        touched.clear();

        for (int b = 0; b < B; b++) {
            int delta = 0;
            if (state[b] == 0) {
                int idx = wIdx(1, RS, CS) * 2;
                int nr = RS + W[idx];
                if (nr >= 0 && nr < R) {
                    int nc = ((CS + W[idx + 1]) % C + C) % C;
                    int p = nr * C + nc;
                    double val = cov[p] / (1.0 + occ[p]) + G * f(1, nr, nc, D - 1);
                    if (val > 0) {
                        delta = 1;
                        state[b] = 1;
                        alt[b] = 1;
                        pr[b] = nr;
                        pc[b] = nc;
                        if (occ[p] == 0) touched.push_back(p);
                        occ[p]++;
                    }
                } else {
                    // launching would immediately lose it; do not launch
                }
            } else if (state[b] == 1) {
                double bestVal = -1e18;
                int bestD = 0;
                int bestNr = -1, bestNc = -1, bestNa = alt[b];
                bool bestLost = false;
                int order[3] = {0, 1, -1};
                for (int k = 0; k < 3; k++) {
                    int d = order[k];
                    int na = alt[b] + d;
                    if (na < 1 || na > A) continue;
                    int idx = wIdx(na, pr[b], pc[b]) * 2;
                    int nr = pr[b] + W[idx];
                    if (nr < 0 || nr >= R) {
                        // lost option, value 0 (only chosen if nothing better)
                        if (0.0 > bestVal) {
                            bestVal = 0.0; bestD = d; bestLost = true;
                            bestNa = na;
                        }
                        continue;
                    }
                    int nc = ((pc[b] + W[idx + 1]) % C + C) % C;
                    int p = nr * C + nc;
                    double val = cov[p] / (1.0 + occ[p]) + G * f(na, nr, nc, D - 1);
                    if (val > bestVal) {
                        bestVal = val; bestD = d;
                        bestNr = nr; bestNc = nc; bestNa = na;
                        bestLost = false;
                    }
                }
                delta = bestD;
                alt[b] = bestNa;
                if (bestLost) {
                    state[b] = 2;
                } else {
                    pr[b] = bestNr;
                    pc[b] = bestNc;
                    int p = bestNr * C + bestNc;
                    if (occ[p] == 0) touched.push_back(p);
                    occ[p]++;
                }
            } else {
                delta = 0;
            }

            if (b) out.push_back(' ');
            if (delta == 1) out += "1";
            else if (delta == -1) out += "-1";
            else out += "0";
        }
        out.push_back('\n');
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
