#include <bits/stdc++.h>
using namespace std;

int H, W, D, B;
struct Proj { char t; int h, w, v; vector<pair<int,int>> occ, dil; int area; int tidx; };
vector<Proj> P;
vector<int> cand;
int U;
typedef chrono::steady_clock Clock;
Clock::time_point T0;

struct Result { vector<array<int,3>> pl; long long score = 0; int Gh, Gw; bool wr, wc; double est = -1; };

Result run(int Gh, int Gw, bool wr, bool wc, double alpha, Clock::time_point deadline) {
    Result res; res.Gh = Gh; res.Gw = Gw; res.wr = wr; res.wc = wc;
    int OFF = D + 2;
    vector<int> ri(Gh + D + 120), ci(Gw + D + 120);
    for (int i = 0; i < (int)ri.size(); i++) { int x = i - OFF; ri[i] = wr ? ((x % Gh) + Gh) % Gh : ((x >= 0 && x < Gh) ? x : -1); }
    for (int i = 0; i < (int)ci.size(); i++) { int x = i - OFF; ci[i] = wc ? ((x % Gw) + Gw) % Gw : ((x >= 0 && x < Gw) ? x : -1); }
    vector<int> grid((size_t)Gh * Gw, -1);
    vector<int> bcap, btype, seen; vector<uint64_t> bmask;
    long long score = 0; int stamp = 0;
    for (int r = 0; r < Gh; r++) {
        for (int c = 0; c < Gw; c++) {
            if ((c & 15) == 0 && Clock::now() > deadline) goto done;
            while (true) {
                int best = -1; double bv = -1;
                for (int p : cand) {
                    const Proj& pr = P[p];
                    if (!wr && r + pr.h > Gh) continue;
                    if (!wc && c + pr.w > Gw) continue;
                    if (pr.t == 'R') { double ub = pr.v * (U + alpha) / pr.area; if (ub <= bv) continue; }
                    bool ok = true;
                    for (auto& o : pr.occ) { int cell = ri[r + o.first + OFF] * Gw + ci[c + o.second + OFF]; if (grid[cell] != -1) { ok = false; break; } }
                    if (!ok) continue;
                    double val;
                    if (pr.t == 'R') {
                        uint64_t m = 0;
                        for (auto& o : pr.dil) {
                            int a = ri[r + o.first + OFF], b = ci[c + o.second + OFF];
                            if (a < 0 || b < 0) continue;
                            int id = grid[a * Gw + b];
                            if (id >= 0 && btype[id] >= 0) m |= 1ull << btype[id];
                        }
                        val = pr.v * (__builtin_popcountll(m) + alpha) / pr.area;
                    } else {
                        stamp++; long long g = 0; uint64_t bit = 1ull << pr.tidx;
                        for (auto& o : pr.dil) {
                            int a = ri[r + o.first + OFF], b = ci[c + o.second + OFF];
                            if (a < 0 || b < 0) continue;
                            int id = grid[a * Gw + b];
                            if (id >= 0 && btype[id] < 0 && seen[id] != stamp) { seen[id] = stamp; if (!(bmask[id] & bit)) g += bcap[id]; }
                        }
                        val = (double)g / pr.area;
                    }
                    if (val > bv) { bv = val; best = p; }
                }
                if (best < 0) break;
                const Proj& pr = P[best];
                int id = bcap.size();
                if (pr.t == 'R') {
                    uint64_t m = 0;
                    for (auto& o : pr.dil) {
                        int a = ri[r + o.first + OFF], b = ci[c + o.second + OFF];
                        if (a < 0 || b < 0) continue;
                        int j = grid[a * Gw + b];
                        if (j >= 0 && btype[j] >= 0) m |= 1ull << btype[j];
                    }
                    score += (long long)pr.v * __builtin_popcountll(m);
                    bcap.push_back(pr.v); btype.push_back(-1); bmask.push_back(m); seen.push_back(0);
                } else {
                    stamp++; uint64_t bit = 1ull << pr.tidx;
                    for (auto& o : pr.dil) {
                        int a = ri[r + o.first + OFF], b = ci[c + o.second + OFF];
                        if (a < 0 || b < 0) continue;
                        int j = grid[a * Gw + b];
                        if (j >= 0 && btype[j] < 0 && seen[j] != stamp) { seen[j] = stamp; if (!(bmask[j] & bit)) { bmask[j] |= bit; score += bcap[j]; } }
                    }
                    bcap.push_back(0); btype.push_back(pr.tidx); bmask.push_back(0); seen.push_back(0);
                }
                for (auto& o : pr.occ) grid[ri[r + o.first + OFF] * Gw + ci[c + o.second + OFF]] = id;
                res.pl.push_back({best, r, c});
            }
        }
    }
done:
    res.score = score;
    res.est = score * (wr ? (double)H / Gh : 1.0) * (wc ? (double)W / Gw : 1.0);
    return res;
}

int main() {
    T0 = Clock::now();
    if (scanf("%d %d %d %d", &H, &W, &D, &B) != 4) return 0;
    P.resize(B);
    for (int i = 0; i < B; i++) {
        char buf[8]; scanf("%s %d %d %d", buf, &P[i].h, &P[i].w, &P[i].v); P[i].t = buf[0];
        P[i].tidx = -1;
        static char line[128];
        for (int r = 0; r < P[i].h; r++) { scanf("%s", line); for (int c = 0; c < P[i].w; c++) if (line[c] == '#') P[i].occ.push_back({r, c}); }
        P[i].area = P[i].occ.size();
    }
    // residential candidates
    vector<int> res, uti;
    for (int i = 0; i < B; i++) (P[i].t == 'R' ? res : uti).push_back(i);
    vector<int> rs = res;
    sort(rs.begin(), rs.end(), [&](int a, int b) { double da = (double)P[a].v / P[a].area, db = (double)P[b].v / P[b].area; if (da != db) return da > db; return a < b; });
    set<int> cs;
    for (int i = 0; i < (int)rs.size() && i < 8; i++) cs.insert(rs[i]);
    vector<int> rs2 = res;
    sort(rs2.begin(), rs2.end(), [&](int a, int b) { if (P[a].area != P[b].area) return P[a].area < P[b].area; if (P[a].v != P[b].v) return P[a].v > P[b].v; return a < b; });
    for (int i = 0; i < (int)rs2.size() && i < 3; i++) cs.insert(rs2[i]);
    // utility candidates
    map<int, vector<int>> byType;
    for (int u : uti) byType[P[u].v].push_back(u);
    vector<pair<int,int>> types;
    for (auto& kv : byType) {
        sort(kv.second.begin(), kv.second.end(), [&](int a, int b) { if (P[a].area != P[b].area) return P[a].area < P[b].area; return a < b; });
        types.push_back({P[kv.second[0]].area, kv.first});
    }
    sort(types.begin(), types.end());
    if (types.size() > 40) types.resize(40);
    U = types.size();
    vector<int> uc;
    for (int i = 0; i < U; i++) { int p = byType[types[i].second][0]; P[p].tidx = i; uc.push_back(p); }
    for (int i = 0; i < U && (int)uc.size() < 48; i++) {
        auto& v = byType[types[i].second];
        if (v.size() > 1) { P[v[1]].tidx = i; uc.push_back(v[1]); }
    }
    for (int p : cs) cand.push_back(p);
    for (int p : uc) cand.push_back(p);
    // dilations
    for (int p : cand) {
        Proj& pr = P[p];
        int gh = pr.h + 2 * D, gw = pr.w + 2 * D;
        vector<int> dist(gh * gw, -1);
        deque<int> q;
        for (auto& o : pr.occ) { int id = (o.first + D) * gw + (o.second + D); dist[id] = 0; q.push_back(id); }
        while (!q.empty()) {
            int id = q.front(); q.pop_front();
            int a = id / gw, b = id % gw;
            if (dist[id] >= D) continue;
            int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
            for (int k = 0; k < 4; k++) {
                int na = a + dr[k], nb = b + dc[k];
                if (na < 0 || nb < 0 || na >= gh || nb >= gw) continue;
                int nid = na * gw + nb;
                if (dist[nid] < 0) { dist[nid] = dist[id] + 1; q.push_back(nid); }
            }
        }
        for (int a = 0; a < gh; a++) for (int b = 0; b < gw; b++) { int d = dist[a * gw + b]; if (d >= 1 && d <= D) pr.dil.push_back({a - D, b - D}); }
    }
    bool wr = H > 150, wc = W > 150;
    vector<pair<int,double>> configs;
    vector<double> alphas = {1.0, 2.0, 0.3, 4.0, 8.0};
    vector<int> Ts = {100, 125, 200};
    if (!wr && !wc) Ts = {100};
    for (int T : Ts) {
        if ((wr && T > H) || (wc && T > W)) continue;
        for (double a : alphas) configs.push_back({T, a});
    }
    if (configs.empty()) for (double a : alphas) configs.push_back({100, a});
    auto deadline = T0 + chrono::milliseconds(8300);
    Result best; bool have = false;
    for (auto& cf : configs) {
        if (have && Clock::now() > deadline) break;
        int Gh = wr ? cf.first : H, Gw = wc ? cf.first : W;
        Result r = run(Gh, Gw, wr, wc, cf.second, deadline);
        if (!have || r.est > best.est) { best = move(r); have = true; }
    }
    // output
    vector<array<int,3>> out;
    int ni = best.wr ? (H + best.Gh - 1) / best.Gh : 1;
    int nj = best.wc ? (W + best.Gw - 1) / best.Gw : 1;
    for (int i = 0; i < ni; i++) for (int j = 0; j < nj; j++)
        for (auto& p : best.pl) {
            int R = i * best.Gh + p[1], C = j * best.Gw + p[2];
            if (R + P[p[0]].h <= H && C + P[p[0]].w <= W) out.push_back({p[0], R, C});
        }
    string s;
    s.reserve(out.size() * 16 + 16);
    s += to_string(out.size()); s += '\n';
    for (auto& o : out) { s += to_string(o[0]); s += ' '; s += to_string(o[1]); s += ' '; s += to_string(o[2]); s += '\n'; }
    fwrite(s.data(), 1, s.size(), stdout);
    return 0;
}
