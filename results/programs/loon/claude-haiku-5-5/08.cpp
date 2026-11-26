#include <bits/stdc++.h>
using namespace std;

struct Reader {
    static const int SZ = 1 << 16;
    char b[SZ];
    int p = 0, n = 0;
    int gc() {
        if (p == n) {
            n = (int)fread(b, 1, SZ, stdin);
            p = 0;
            if (n <= 0) return -1;
        }
        return b[p++];
    }
    int readInt() {
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
};

static int isqrtI(int v) {
    if (v < 0) return -1;
    int s = (int)sqrt((double)v);
    while (s > 0 && s * s > v) s--;
    while ((s + 1) * (s + 1) <= v) s++;
    return s;
}

int R, C, A, L, V, B, T;
vector<signed char> W;   // wind: index ((a*R + r)*C + c)*2
vector<vector<uint16_t>> cellT;
vector<char> covered;
vector<int> nearD;

inline bool mv(int r, int c, int a, int &nr, int &nc) {
    // a is 1-based altitude
    size_t idx = ((size_t)(a - 1) * R + r) * C + c;
    int dr = W[2 * idx], dc = W[2 * idx + 1];
    int tr = r + dr;
    if (tr < 0 || tr >= R) return false;
    nr = tr;
    nc = ((c + dc) % C + C) % C;
    return true;
}

inline int gainCell(int cell) {
    int g = 0;
    for (uint16_t j : cellT[cell]) if (!covered[j]) g++;
    return g;
}

int main() {
    Reader rd;
    R = rd.readInt(); C = rd.readInt(); A = rd.readInt();
    L = rd.readInt(); V = rd.readInt(); B = rd.readInt(); T = rd.readInt();
    int rs = rd.readInt(), cs = rd.readInt();
    vector<int> tr(L), tc(L);
    for (int i = 0; i < L; i++) {
        tr[i] = rd.readInt();
        tc[i] = rd.readInt();
    }
    W.assign((size_t)A * R * C * 2, 0);
    for (int a = 0; a < A; a++)
        for (int r = 0; r < R; r++)
            for (int c = 0; c < C; c++) {
                size_t idx = ((size_t)a * R + r) * C + c;
                W[2 * idx] = (signed char)rd.readInt();
                W[2 * idx + 1] = (signed char)rd.readInt();
            }

    // Coverage lists per cell
    cellT.assign((size_t)R * C, vector<uint16_t>());
    for (int j = 0; j < L; j++) {
        int ri = tr[j], ci = tc[j];
        for (int du = -V; du <= V; du++) {
            int u = ri + du;
            if (u < 0 || u >= R) continue;
            int rem = V * V - du * du;
            int h = isqrtI(rem);
            if (h < 0) continue;
            if (2 * h + 1 >= C) {
                for (int v = 0; v < C; v++) {
                    int d = abs(ci - v);
                    int cd = min(d, C - d);
                    if (cd <= h) cellT[(size_t)u * C + v].push_back((uint16_t)j);
                }
            } else {
                for (int dv = -h; dv <= h; dv++) {
                    int v = ((ci + dv) % C + C) % C;
                    cellT[(size_t)u * C + v].push_back((uint16_t)j);
                }
            }
        }
    }

    // Nearest target distance (BFS, cyclic columns)
    const int INF = 1 << 29;
    nearD.assign((size_t)R * C, INF);
    {
        vector<int> q;
        q.reserve((size_t)R * C);
        for (int j = 0; j < L; j++) {
            int id = tr[j] * C + tc[j];
            if (nearD[id] != 0) { nearD[id] = 0; q.push_back(id); }
        }
        size_t qh = 0;
        while (qh < q.size()) {
            int id = q[qh++];
            int r = id / C, c = id % C;
            int nb[4][2] = {{r - 1, c}, {r + 1, c}, {r, (c + 1) % C}, {r, (c - 1 + C) % C}};
            for (auto &x : nb) {
                if (x[0] < 0 || x[0] >= R) continue;
                int nid = x[0] * C + x[1];
                if (nearD[nid] > nearD[id] + 1) {
                    nearD[nid] = nearD[id] + 1;
                    q.push_back(nid);
                }
            }
        }
    }

    struct Ball { bool launched, lost; int alt, r, c; };
    vector<Ball> balls(B, Ball{false, false, 0, rs, cs});

    string out;
    out.reserve((size_t)T * B * 3 + 16);
    const int order[3] = {0, -1, 1};

    for (int t = 0; t < T; t++) {
        covered.assign(L, 0);
        for (int b = 0; b < B; b++) {
            Ball &bl = balls[b];
            int act = 0;
            if (bl.lost) {
                act = 0;
            } else if (!bl.launched) {
                // Launch at turn 0
                bl.launched = true;
                act = 1;
                bl.alt = 1;
                int nr, nc;
                if (!mv(bl.r, bl.c, bl.alt, nr, nc)) {
                    bl.lost = true;
                } else {
                    bl.r = nr; bl.c = nc;
                    for (uint16_t j : cellT[bl.r * C + bl.c]) covered[j] = 1;
                }
            } else {
                double bestVal = -1e18;
                int bestD = 0;
                for (int oi = 0; oi < 3; oi++) {
                    int d = order[oi];
                    int a1 = bl.alt + d;
                    if (a1 < 1 || a1 > A) continue;
                    double val;
                    int r1, c1;
                    if (!mv(bl.r, bl.c, a1, r1, c1)) {
                        val = -1.0;
                    } else {
                        double g1 = gainCell(r1 * C + c1);
                        double best2 = -1e18;
                        for (int d2 = -1; d2 <= 1; d2++) {
                            int a2 = a1 + d2;
                            if (a2 < 1 || a2 > A) continue;
                            int r2, c2;
                            double v2;
                            if (!mv(r1, c1, a2, r2, c2)) {
                                v2 = -1.0;
                            } else {
                                int id2 = r2 * C + c2;
                                v2 = 0.5 * gainCell(id2) - 1e-4 * nearD[id2];
                            }
                            if (v2 > best2) best2 = v2;
                        }
                        val = g1 + best2 - 1e-4 * nearD[r1 * C + c1];
                    }
                    if (val > bestVal) { bestVal = val; bestD = d; }
                }
                act = bestD;
                bl.alt += bestD;
                int nr, nc;
                if (!mv(bl.r, bl.c, bl.alt, nr, nc)) {
                    bl.lost = true;
                } else {
                    bl.r = nr; bl.c = nc;
                    for (uint16_t j : cellT[bl.r * C + bl.c]) covered[j] = 1;
                }
            }
            out += to_string(act);
            out += (b + 1 < B) ? ' ' : '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
