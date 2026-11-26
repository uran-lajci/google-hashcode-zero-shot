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
    int R = readInt(), C = readInt(), A = readInt();
    int L = readInt(), V = readInt(), B = readInt(), T = readInt();
    int rs = readInt(), cs = readInt();
    vector<int> tr(L), tc(L);
    for (int i = 0; i < L; i++) {
        tr[i] = readInt();
        tc[i] = readInt();
    }

    size_t total = (size_t)A * R * C;
    vector<int8_t> wr(total), wc(total);
    for (int a = 0; a < A; a++)
        for (int r = 0; r < R; r++)
            for (int c = 0; c < C; c++) {
                size_t idx = ((size_t)a * R + r) * C + c;
                wr[idx] = (int8_t)readInt();
                wc[idx] = (int8_t)readInt();
            }

    // Cover structure: for each cell, list of targets whose coverage disk includes it.
    int NC = R * C;
    vector<int> stamp(C, -1);
    auto enumerate = [&](int i, auto&& fn) {
        for (int du = -V; du <= V; du++) {
            int u = tr[i] + du;
            if (u < 0 || u >= R) continue;
            for (int dv = -V; dv <= V; dv++) {
                if (du * du + dv * dv > V * V) continue;
                int v = ((tc[i] + dv) % C + C) % C;
                if (stamp[v] == i) continue;
                stamp[v] = i;
                fn(u * C + v);
            }
        }
    };
    vector<uint32_t> start(NC + 1, 0);
    fill(stamp.begin(), stamp.end(), -1);
    for (int i = 0; i < L; i++) {
        enumerate(i, [&](int cell) { start[cell + 1]++; });
    }
    for (int k = 0; k < NC; k++) start[k + 1] += start[k];
    vector<uint16_t> lst(start[NC]);
    {
        vector<uint32_t> pos(start.begin(), start.end() - 1);
        fill(stamp.begin(), stamp.end(), -1);
        for (int i = 0; i < L; i++) {
            enumerate(i, [&](int cell) { lst[pos[cell]++] = (uint16_t)i; });
        }
    }

    const int H = 6;
    vector<char> claimed((size_t)T * L, 0);
    vector<char> actions((size_t)T * B, 0);
    long long score = 0;

    auto evalPath = [&](int t, int alt, int r, int c) -> int {
        int tot = 0;
        int rr = r, cc = c;
        for (int s = t; s < T && s < t + H; s++) {
            size_t idx = ((size_t)(alt - 1) * R + rr) * C + cc;
            int nr = rr + wr[idx];
            if (nr < 0 || nr >= R) break;
            int nc = ((cc + wc[idx]) % C + C) % C;
            rr = nr;
            cc = nc;
            int cell = rr * C + cc;
            for (uint32_t k = start[cell]; k < start[cell + 1]; k++) {
                int i = lst[k];
                if (!claimed[(size_t)s * L + i]) tot++;
            }
        }
        return tot;
    };

    vector<pair<int, int>> gains;
    for (int b = 0; b < B; b++) {
        bool launched = false, lost = false;
        int alt = 0, r = rs, c = cs;
        gains.clear();
        for (int t = 0; t < T; t++) {
            int act = 0;
            if (!lost && launched) {
                int bestv = -1, bestd = 0;
                const int ds[3] = {0, 1, -1};
                for (int d : ds) {
                    int na = alt + d;
                    if (na < 1 || na > A) continue;
                    int v = evalPath(t, na, r, c);
                    if (v > bestv) { bestv = v; bestd = d; }
                }
                act = bestd;
                alt += act;
            } else if (!lost && !launched) {
                if (evalPath(t, 1, rs, cs) > 0) {
                    act = 1;
                    launched = true;
                    alt = 1;
                    r = rs;
                    c = cs;
                }
            }
            actions[(size_t)t * B + b] = (char)act;
            if (launched && !lost) {
                size_t idx = ((size_t)(alt - 1) * R + r) * C + c;
                int nr = r + wr[idx];
                if (nr < 0 || nr >= R) {
                    lost = true;
                } else {
                    int nc = ((c + wc[idx]) % C + C) % C;
                    r = nr;
                    c = nc;
                    int cell = r * C + c;
                    for (uint32_t k = start[cell]; k < start[cell + 1]; k++) {
                        int i = lst[k];
                        if (!claimed[(size_t)t * L + i]) gains.push_back({t, i});
                    }
                }
            }
        }
        for (auto& g : gains) {
            claimed[(size_t)g.first * L + g.second] = 1;
            score++;
        }
    }

    string out;
    out.reserve((size_t)T * B * 3 + T);
    for (int t = 0; t < T; t++) {
        for (int b = 0; b < B; b++) {
            int v = actions[(size_t)t * B + b];
            if (b) out.push_back(' ');
            if (v == -1) out += "-1";
            else out.push_back(v == 1 ? '1' : '0');
        }
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
