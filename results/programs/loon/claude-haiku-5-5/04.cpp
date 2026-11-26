#include <bits/stdc++.h>
using namespace std;

// ---------- fast input ----------
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

// ---------- problem globals ----------
int R, C, A, L, V, B, T, startR, startC;
const int D = 4;               // lookahead depth (including current step)
const double MU = 0.5;         // potential weight (distance to coverage zone)
const double LOSS = 30.0;      // penalty for losing a balloon
const double DISC = 0.7;       // discount for future steps

vector<int8_t> WDR, WDC;       // wind vectors, index ((alt-1)*R + r)*C + c
vector<vector<int>> cellT;     // targets covering each cell
vector<int> distv;             // distance to nearest covered cell
vector<int> covTurn;           // last turn a target was counted

struct St {
    bool launched;
    bool alive;
    int alt;   // 0 if not launched, else 1..A
    int r, c;
};

static inline int wcount(int cell) { return (int)cellT[cell].size(); }

static inline St nextState(const St& s, int a) {
    St o = s;
    if (!s.launched) {
        if (a != 1) return s;          // stays on ground at start
        o.launched = true;
        o.alt = 1;
    } else {
        o.alt = s.alt + a;
    }
    int idx = ((o.alt - 1) * R + s.r) * C + s.c;
    int nr = s.r + WDR[idx];
    int nc = ((s.c + WDC[idx]) % C + C) % C;
    if (nr < 0 || nr >= R) {
        o.alive = false;
    } else {
        o.r = nr;
        o.c = nc;
    }
    return o;
}

static inline int getOpts(const St& s, int* out) {
    int n = 0;
    if (!s.launched) {
        out[n++] = 0;
        out[n++] = 1;
    } else {
        int cand[3] = {0, 1, -1};
        for (int k = 0; k < 3; k++) {
            int na = s.alt + cand[k];
            if (na >= 1 && na <= A) out[n++] = cand[k];
        }
    }
    return n;
}

double dfs(const St& s, int rem, double w) {
    if (rem == 0 || !s.alive) return 0.0;
    double best = -1e18;
    int opts[3];
    int no = getOpts(s, opts);
    for (int k = 0; k < no; k++) {
        St n = nextState(s, opts[k]);
        double v;
        if (!n.alive) {
            v = -LOSS * w;
        } else {
            int cell = n.r * C + n.c;
            double g = n.launched ? wcount(cell) : 0.0;
            v = w * (g - MU * distv[cell]);
            v += dfs(n, rem - 1, w * DISC);
        }
        if (v > best) best = v;
    }
    return best;
}

int main() {
    R = readInt(); C = readInt(); A = readInt();
    L = readInt(); V = readInt(); B = readInt(); T = readInt();
    startR = readInt(); startC = readInt();

    vector<int> tr(L), tc(L);
    for (int i = 0; i < L; i++) {
        tr[i] = readInt();
        tc[i] = readInt();
    }

    size_t total = (size_t)A * R * C;
    WDR.assign(total, 0);
    WDC.assign(total, 0);
    for (size_t i = 0; i < total; i++) {
        WDR[i] = (int8_t)readInt();
        WDC[i] = (int8_t)readInt();
    }

    // Build coverage lists
    cellT.assign((size_t)R * C, vector<int>());
    vector<int> seen(C, -1);
    for (int i = 0; i < L; i++) {
        int ri = tr[i], ci = tc[i];
        vector<pair<int, int>> cols;
        for (int dc = -V; dc <= V; dc++) {
            int col = ((ci + dc) % C + C) % C;
            if (seen[col] != i) {
                seen[col] = i;
                int d = abs(col - ci);
                int dd = min(d, C - d);
                cols.push_back({col, dd});
            }
        }
        for (int dr = -V; dr <= V; dr++) {
            int r = ri + dr;
            if (r < 0 || r >= R) continue;
            for (auto& p : cols) {
                if (dr * dr + p.second * p.second <= V * V) {
                    cellT[(size_t)r * C + p.first].push_back(i);
                }
            }
        }
    }

    // Distance to nearest covered cell (BFS with horizontal wrap)
    int N = R * C;
    distv.assign(N, -1);
    vector<int> q;
    q.reserve(N);
    for (int i = 0; i < N; i++) {
        if (!cellT[i].empty()) {
            distv[i] = 0;
            q.push_back(i);
        }
    }
    for (size_t h = 0; h < q.size(); h++) {
        int cur = q[h];
        int r = cur / C, c = cur % C;
        int nb[4][2] = {{r - 1, c}, {r + 1, c}, {r, (c + 1) % C}, {r, (c - 1 + C) % C}};
        for (int k = 0; k < 4; k++) {
            int nr = nb[k][0], nc = nb[k][1];
            if (nr < 0 || nr >= R) continue;
            int id = nr * C + nc;
            if (distv[id] < 0) {
                distv[id] = distv[cur] + 1;
                q.push_back(id);
            }
        }
    }
    for (int i = 0; i < N; i++) if (distv[i] < 0) distv[i] = 0;

    covTurn.assign(L, 0);

    vector<St> bal(B);
    for (int b = 0; b < B; b++) {
        bal[b].launched = false;
        bal[b].alive = true;
        bal[b].alt = 0;
        bal[b].r = startR;
        bal[b].c = startC;
    }

    string out;
    out.reserve((size_t)T * B * 3);
    long long score = 0;

    for (int turn = 1; turn <= T; turn++) {
        for (int b = 0; b < B; b++) {
            St s = bal[b];
            if (!s.alive) {
                out += '0';
                if (b + 1 < B) out += ' ';
                continue;
            }
            int opts[3];
            int no = getOpts(s, opts);
            double best = -1e18;
            int bestA = 0;
            St bestN = s;
            for (int k = 0; k < no; k++) {
                St n = nextState(s, opts[k]);
                double v;
                if (!n.alive) {
                    v = -LOSS;
                } else {
                    int cell = n.r * C + n.c;
                    double g0 = 0;
                    if (n.launched) {
                        for (int t : cellT[cell]) {
                            if (covTurn[t] != turn) g0 += 1.0;
                        }
                    }
                    v = g0 - MU * distv[cell];
                    v += dfs(n, D - 1, DISC);
                }
                if (v > best) {
                    best = v;
                    bestA = opts[k];
                    bestN = n;
                }
            }
            bal[b] = bestN;
            // apply coverage
            if (bestN.alive && bestN.launched) {
                int cell = bestN.r * C + bestN.c;
                for (int t : cellT[cell]) {
                    if (covTurn[t] != turn) {
                        covTurn[t] = turn;
                        score++;
                    }
                }
            }
            if (bestA == -1) out += "-1";
            else if (bestA == 1) out += '1';
            else out += '0';
            if (b + 1 < B) out += ' ';
        }
        out += '\n';
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
