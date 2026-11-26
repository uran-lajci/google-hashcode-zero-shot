#include <bits/stdc++.h>
using namespace std;

struct Reader {
    static const int SZ = 1 << 16;
    char b[SZ];
    int n = 0, i = 0;
    int gc() {
        if (i == n) {
            n = (int)fread(b, 1, SZ, stdin);
            i = 0;
            if (n <= 0) return -1;
        }
        return b[i++];
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
} rd;

struct Cur { int alt, pos, score; };
struct Cand { int alt, pos, score, par; long long key; };
struct Rec { int alt, pos, par; };

int main() {
    int R = rd.readInt(), C = rd.readInt(), A = rd.readInt();
    int L = rd.readInt(), V = rd.readInt(), B = rd.readInt(), T = rd.readInt();
    int rs = rd.readInt(), cs = rd.readInt();
    vector<int> tr(L), tc(L);
    for (int l = 0; l < L; l++) { tr[l] = rd.readInt(); tc[l] = rd.readInt(); }

    int RC = R * C;
    size_t total = (size_t)A * RC;
    vector<int8_t> WR(total), WC(total);
    for (size_t k = 0; k < total; k++) {
        WR[k] = (int8_t)rd.readInt();
        WC[k] = (int8_t)rd.readInt();
    }

    int sp = rs * C + cs;

    auto moveCell = [&](int p, int alt) -> int {
        int r = p / C, c = p % C;
        size_t idx = (size_t)(alt - 1) * RC + p;
        int nr = r + WR[idx];
        int nc = ((c + WC[idx]) % C + C) % C;
        if (nr < 0 || nr >= R) return -1;
        return nr * C + nc;
    };

    // Coverage enumeration: cells covered by a balloon at target l.
    vector<char> markv(C, 0);
    auto enumCover = [&](int l, const auto& f) {
        int r0 = tr[l], c0 = tc[l];
        vector<pair<int, int>> cols; // (column, columndist)
        for (int dc = -V; dc <= V; dc++) {
            int v = ((c0 + dc) % C + C) % C;
            if (!markv[v]) {
                markv[v] = 1;
                int diff = abs(c0 - v);
                cols.push_back({v, min(diff, C - diff)});
            }
        }
        for (auto& pr : cols) markv[pr.first] = 0;
        for (int du = -V; du <= V; du++) {
            int u = r0 + du;
            if (u < 0 || u >= R) continue;
            int rem = V * V - du * du;
            for (auto& pr : cols) {
                if (pr.second * pr.second <= rem) f(u * C + pr.first);
            }
        }
    };

    vector<int> cst(RC + 1, 0);
    for (int l = 0; l < L; l++)
        enumCover(l, [&](int cell) { cst[cell + 1]++; });
    for (int i = 0; i < RC; i++) cst[i + 1] += cst[i];
    vector<int> clist(cst[RC]);
    {
        vector<int> fillp(cst.begin(), cst.begin() + RC);
        for (int l = 0; l < L; l++)
            enumCover(l, [&](int cell) { clist[fillp[cell]++] = l; });
    }

    // BFS distance from nearest target (heuristic).
    vector<int> dist(RC, -1);
    {
        deque<int> dq;
        for (int l = 0; l < L; l++) {
            int p = tr[l] * C + tc[l];
            if (dist[p] == -1) { dist[p] = 0; dq.push_back(p); }
        }
        while (!dq.empty()) {
            int p = dq.front(); dq.pop_front();
            int r = p / C, c = p % C;
            int nb[4][2] = {{r - 1, c}, {r + 1, c}, {r, (c + 1) % C}, {r, (c - 1 + C) % C}};
            for (auto& q : nb) {
                if (q[0] < 0 || q[0] >= R) continue;
                int np = q[0] * C + q[1];
                if (dist[np] == -1) { dist[np] = dist[p] + 1; dq.push_back(np); }
            }
        }
    }

    vector<char> cov((size_t)T * L, 0);
    auto gain = [&](int t, int cell) -> int {
        int g = 0;
        size_t base = (size_t)t * L;
        for (int k = cst[cell]; k < cst[cell + 1]; k++)
            if (!cov[base + clist[k]]) g++;
        return g;
    };

    const int W = 20;
    vector<vector<int>> out(T, vector<int>(B, 0));

    for (int b = 0; b < B; b++) {
        vector<Cur> cur;
        cur.push_back({0, sp, 0});
        vector<vector<Rec>> hist(T);

        for (int t = 0; t < T; t++) {
            vector<Cand> cand;
            cand.reserve(cur.size() * 3 + 2);
            for (int i = 0; i < (int)cur.size(); i++) {
                Cur s = cur[i];
                if (s.pos < 0) {
                    cand.push_back({s.alt, -1, s.score, i, 0});
                } else if (s.alt == 0) {
                    cand.push_back({0, sp, s.score, i, 0});
                    int np = moveCell(sp, 1);
                    int sc = s.score + (np >= 0 ? gain(t, np) : 0);
                    cand.push_back({1, np, sc, i, 0});
                } else {
                    for (int da = -1; da <= 1; da++) {
                        int na = s.alt + da;
                        if (na < 1 || na > A) continue;
                        int np = moveCell(s.pos, na);
                        int sc = s.score + (np >= 0 ? gain(t, np) : 0);
                        cand.push_back({na, np, sc, i, 0});
                    }
                }
            }
            for (auto& c : cand) {
                c.key = (long long)c.score * 100000LL + (c.pos < 0 ? -50000LL : -(long long)dist[c.pos]);
            }
            // Dedupe by (alt,pos), keep best key.
            sort(cand.begin(), cand.end(), [](const Cand& x, const Cand& y) {
                if (x.alt != y.alt) return x.alt < y.alt;
                if (x.pos != y.pos) return x.pos < y.pos;
                return x.key > y.key;
            });
            vector<Cand> kept;
            for (size_t k = 0; k < cand.size(); k++) {
                if (k == 0 || cand[k].alt != cand[k - 1].alt || cand[k].pos != cand[k - 1].pos)
                    kept.push_back(cand[k]);
            }
            sort(kept.begin(), kept.end(), [](const Cand& x, const Cand& y) {
                return x.key > y.key;
            });
            if ((int)kept.size() > W) kept.resize(W);

            hist[t].clear();
            cur.clear();
            for (auto& c : kept) {
                hist[t].push_back({c.alt, c.pos, c.par});
                cur.push_back({c.alt, c.pos, c.score});
            }
        }

        // Pick best final state.
        int bestIdx = 0;
        {
            long long bk = LLONG_MIN;
            for (int i = 0; i < (int)cur.size(); i++) {
                long long k = (long long)cur[i].score * 100000LL +
                              (cur[i].pos < 0 ? -50000LL : -(long long)dist[cur[i].pos]);
                if (k > bk) { bk = k; bestIdx = i; }
            }
        }

        vector<int> altSeq(T, 0), posSeq(T, -1);
        int idx = bestIdx;
        for (int t = T - 1; t >= 0; t--) {
            Rec rc = hist[t][idx];
            altSeq[t] = rc.alt;
            posSeq[t] = rc.pos;
            idx = rc.par;
        }
        for (int t = 0; t < T; t++) {
            if (posSeq[t] >= 0 && altSeq[t] >= 1) {
                size_t base = (size_t)t * L;
                int p = posSeq[t];
                for (int k = cst[p]; k < cst[p + 1]; k++) cov[base + clist[k]] = 1;
            }
        }
        int prev = 0;
        for (int t = 0; t < T; t++) {
            out[t][b] = altSeq[t] - prev;
            prev = altSeq[t];
        }
    }

    string s;
    s.reserve((size_t)T * B * 3 + T);
    for (int t = 0; t < T; t++) {
        for (int b = 0; b < B; b++) {
            if (b) s += ' ';
            s += to_string(out[t][b]);
        }
        s += '\n';
    }
    fwrite(s.data(), 1, s.size(), stdout);
    return 0;
}
