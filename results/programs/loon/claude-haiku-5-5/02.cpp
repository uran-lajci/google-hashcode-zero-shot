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
    int s = 1;
    if (c == '-') { s = -1; c = gc(); }
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return x * s;
}

int main() {
    auto startTime = chrono::steady_clock::now();

    int R = readInt(), C = readInt(), A = readInt();
    int L = readInt(), V = readInt(), B = readInt(), T = readInt();
    int rs = readInt(), cs = readInt();

    vector<int> ti(L), tj(L);
    for (int i = 0; i < L; i++) {
        ti[i] = readInt();
        tj[i] = readInt();
    }

    // wind[a][r][c] -> (dr, dc)
    vector<int8_t> W((size_t)A * R * C * 2);
    for (int a = 0; a < A; a++)
        for (int r = 0; r < R; r++)
            for (int c = 0; c < C; c++) {
                size_t idx = (((size_t)a * R + r) * C + c) * 2;
                W[idx] = (int8_t)readInt();
                W[idx + 1] = (int8_t)readInt();
            }

    // Coverage lists: for each cell, targets it covers when balloon is there.
    int N = R * C;
    vector<vector<int>> lists(N);
    vector<int> stamp(N, 0);
    for (int i = 0; i < L; i++) {
        int u = ti[i], v = tj[i];
        int stampId = i + 1;
        for (int r = max(0, u - V); r <= min(R - 1, u + V); r++) {
            int dr = r - u;
            for (int dc = -V; dc <= V; dc++) {
                int x = ((v + dc) % C + C) % C;
                int d = abs(x - v);
                int cd = min(d, C - d);
                if (dr * dr + cd * cd > V * V) continue;
                int p = r * C + x;
                if (stamp[p] == stampId) continue;
                stamp[p] = stampId;
                lists[p].push_back(i);
            }
        }
    }

    // Distance to nearest target cell (BFS over grid, cyclic columns).
    vector<int> dist(N, -1);
    {
        vector<int> q;
        q.reserve(N);
        for (int i = 0; i < L; i++) {
            int p = ti[i] * C + tj[i];
            if (dist[p] != 0) {
                dist[p] = 0;
                q.push_back(p);
            }
        }
        size_t head = 0;
        while (head < q.size()) {
            int p = q[head++];
            int r = p / C, c = p % C;
            int d = dist[p] + 1;
            if (r > 0 && dist[p - C] < 0) { dist[p - C] = d; q.push_back(p - C); }
            if (r + 1 < R && dist[p + C] < 0) { dist[p + C] = d; q.push_back(p + C); }
            int cl = (c + C - 1) % C, cr = (c + 1) % C;
            int pl = r * C + cl, pr = r * C + cr;
            if (dist[pl] < 0) { dist[pl] = d; q.push_back(pl); }
            if (dist[pr] < 0) { dist[pr] = d; q.push_back(pr); }
        }
        for (int p = 0; p < N; p++) if (dist[p] < 0) dist[p] = 100000;
    }

    vector<int> alt(B, 0), pr(B, rs), pc(B, cs);
    vector<char> lost(B, 0), launched(B, 0);
    vector<char> cov(L, 0);

    string out;
    out.reserve((size_t)T * B * 3);

    auto windAt = [&](int a /*1-based*/, int r, int c, int& dr, int& dc) {
        size_t idx = (((size_t)(a - 1) * R + r) * C + c) * 2;
        dr = W[idx];
        dc = W[idx + 1];
    };

    for (int t = 0; t < T; t++) {
        bool slow = chrono::duration<double>(chrono::steady_clock::now() - startTime).count() > 7.0;
        fill(cov.begin(), cov.end(), 0);
        for (int b = 0; b < B; b++) {
            int d = 0;
            if (lost[b]) {
                d = 0;
            } else if (!launched[b]) {
                // Launch in turn 0 (all balloons launched immediately).
                launched[b] = 1;
                d = 1;
                alt[b] = 1;
                int dr, dc;
                windAt(alt[b], pr[b], pc[b], dr, dc);
                int nr = pr[b] + dr;
                if (nr < 0 || nr >= R) {
                    lost[b] = 1;
                } else {
                    int nc = ((pc[b] + dc) % C + C) % C;
                    pr[b] = nr;
                    pc[b] = nc;
                    for (int i : lists[nr * C + nc]) cov[i] = 1;
                }
            } else {
                double best = -1e300;
                int bestd = 0, bnr = -1, bnc = -1, bna = alt[b];
                const int order[3] = {0, 1, -1};
                for (int k = 0; k < 3; k++) {
                    int dd = order[k];
                    int na = alt[b] + dd;
                    if (na < 1 || na > A) continue;
                    int dr, dc;
                    windAt(na, pr[b], pc[b], dr, dc);
                    int nr = pr[b] + dr;
                    double score;
                    int nc = -1;
                    if (nr < 0 || nr >= R) {
                        score = -1e15;
                    } else {
                        nc = ((pc[b] + dc) % C + C) % C;
                        int p = nr * C + nc;
                        long long gain = 0;
                        if (!slow) {
                            for (int i : lists[p]) if (!cov[i]) gain++;
                        }
                        score = (double)gain * 1e6 - dist[p];
                    }
                    if (score > best) {
                        best = score;
                        bestd = dd;
                        bnr = nr;
                        bnc = nc;
                        bna = na;
                    }
                }
                d = bestd;
                alt[b] = bna;
                if (bnr < 0 || bnr >= R) {
                    lost[b] = 1;
                } else {
                    pr[b] = bnr;
                    pc[b] = bnc;
                    for (int i : lists[bnr * C + bnc]) cov[i] = 1;
                }
            }
            if (b) out.push_back(' ');
            if (d == 1) out += "1";
            else if (d == -1) out += "-1";
            else out += "0";
        }
        out.push_back('\n');
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
