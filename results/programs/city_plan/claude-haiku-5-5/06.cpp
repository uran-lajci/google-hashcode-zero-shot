#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

struct Proj {
    bool res;
    int hp, wp, val;
    vector<pair<int,int>> cells;
    int area;
};

int H, W, D, B;
vector<Proj> P;
vector<int> occ, utype, rid, vis, distA, qbuf, visited, typeSt, resSt, resVal;
vector<uint64_t> resBits;
int stampCur = 0, tSt = 0;
long long total = 0;
vector<array<int,3>> pl;

inline bool hasBit(int res, int t) {
    return (resBits[(size_t)res * 16 + (t >> 6)] >> (t & 63)) & 1ULL;
}
inline void setBit(int res, int t) {
    resBits[(size_t)res * 16 + (t >> 6)] |= (1ULL << (t & 63));
}

// Multi-source BFS up to distance D (Manhattan, ignoring occupancy).
void explore(const vector<int>& src) {
    stampCur++;
    visited.clear();
    int qh = 0, qt = 0;
    for (int s : src) {
        if (vis[s] != stampCur) {
            vis[s] = stampCur;
            distA[s] = 0;
            qbuf[qt++] = s;
            visited.push_back(s);
        }
    }
    while (qh < qt) {
        int v = qbuf[qh++];
        int d = distA[v];
        if (d >= D) continue;
        int r = v / W, c = v % W;
        int nb[4];
        int k = 0;
        if (r > 0) nb[k++] = v - W;
        if (r < H - 1) nb[k++] = v + W;
        if (c > 0) nb[k++] = v - 1;
        if (c < W - 1) nb[k++] = v + 1;
        for (int i = 0; i < k; i++) {
            int u = nb[i];
            if (vis[u] != stampCur) {
                vis[u] = stampCur;
                distA[u] = d + 1;
                qbuf[qt++] = u;
                visited.push_back(u);
            }
        }
    }
}

// Computes marginal gain of placing project p with absolute cells cabs.
// For residential: gain = r * (#utility types reachable).
// For utility: gain = sum of r of residentials reachable lacking this type; also free-cell count.
void evaluate(int p, const vector<int>& cabs, long long& gain, int& freeCnt) {
    explore(cabs);
    gain = 0;
    freeCnt = 0;
    const Proj& pr = P[p];
    if (pr.res) {
        tSt++;
        for (int v : visited) {
            int t = utype[v];
            if (t >= 0 && typeSt[t] != tSt) {
                typeSt[t] = tSt;
                gain += pr.val;
            }
        }
    } else {
        int t = pr.val;
        for (int v : visited) {
            if (occ[v] == 0) freeCnt++;
            int res = rid[v];
            if (res >= 0 && resSt[res] != stampCur) {
                resSt[res] = stampCur;
                if (!hasBit(res, t)) gain += resVal[res];
            }
        }
    }
}

void applyPlacement(int p, int r0, int c0, const vector<int>& cabs) {
    const Proj& pr = P[p];
    if (pr.res) {
        int id = (int)resVal.size();
        resVal.push_back(pr.val);
        resSt.push_back(0);
        resBits.insert(resBits.end(), 16, 0ULL);
        for (int v : cabs) { occ[v] = 1; rid[v] = id; }
        explore(cabs);
        for (int v : visited) {
            int t = utype[v];
            if (t >= 0 && !hasBit(id, t)) {
                setBit(id, t);
                total += pr.val;
            }
        }
    } else {
        int t = pr.val;
        for (int v : cabs) { occ[v] = 1; utype[v] = t; }
        explore(cabs);
        for (int v : visited) {
            int res = rid[v];
            if (res >= 0 && !hasBit(res, t)) {
                setBit(res, t);
                total += resVal[res];
            }
        }
    }
    pl.push_back({p, r0, c0});
}

int main() {
    auto t0 = steady_clock::now();
    auto elapsed = [&]() { return duration<double>(steady_clock::now() - t0).count(); };

    cin >> H >> W >> D >> B;
    P.resize(B);
    for (int i = 0; i < B; i++) {
        char t;
        int hp, wp, val;
        cin >> t >> hp >> wp >> val;
        P[i].res = (t == 'R');
        P[i].hp = hp;
        P[i].wp = wp;
        P[i].val = val;
        for (int r = 0; r < hp; r++) {
            string s;
            cin >> s;
            for (int c = 0; c < wp; c++)
                if (s[c] == '#') P[i].cells.push_back({r, c});
        }
        P[i].area = (int)P[i].cells.size();
    }

    double sumR = 0;
    int cntR = 0;
    for (auto& p : P) if (p.res) { sumR += p.val; cntR++; }
    double avgR = cntR ? sumR / cntR : 1.0;
    double norm = 2.0 * D * D + 2.0 * D + 1.0;

    int N = H * W;
    occ.assign(N, 0);
    utype.assign(N, -1);
    rid.assign(N, -1);
    vis.assign(N, 0);
    distA.assign(N, 0);
    qbuf.assign(N, 0);
    typeSt.assign(1024, 0);

    mt19937 rng(20180217u);
    vector<double> ks = {0.5, 1.0, 2.0, 4.0};
    const double TL = 8.0;
    const int S = 5;
    const int MAXF = 3000;

    long long bestTotal = -1;
    vector<array<int,3>> bestPl;

    for (size_t idx = 0; idx < ks.size(); idx++) {
        double deadline = TL * (idx + 1) / ks.size();
        double alpha = ks[idx] * avgR / norm;

        fill(occ.begin(), occ.end(), 0);
        fill(utype.begin(), utype.end(), -1);
        fill(rid.begin(), rid.end(), -1);
        resSt.clear();
        resVal.clear();
        resBits.clear();
        pl.clear();
        total = 0;

        int fails = 0;
        vector<int> cabs, bcabs;
        while (true) {
            if (elapsed() > deadline) break;
            if (fails >= MAXF) break;

            double bestRatio = 0;
            int bp = -1, br = 0, bc = 0;
            int found = 0, tries = 0;
            while (found < S && tries < 60) {
                tries++;
                int p = (int)(rng() % B);
                const Proj& pr = P[p];
                int r0 = (int)(rng() % (H - pr.hp + 1));
                int c0 = (int)(rng() % (W - pr.wp + 1));
                bool ok = true;
                for (auto& pc : pr.cells) {
                    if (occ[(r0 + pc.first) * W + c0 + pc.second]) { ok = false; break; }
                }
                if (!ok) continue;
                found++;
                cabs.clear();
                for (auto& pc : pr.cells)
                    cabs.push_back((r0 + pc.first) * W + c0 + pc.second);
                long long gain;
                int freeCnt;
                evaluate(p, cabs, gain, freeCnt);
                double ratio;
                if (pr.res) ratio = (double)gain / pr.area;
                else ratio = (gain + alpha * freeCnt) / pr.area;
                if (ratio > bestRatio) {
                    bestRatio = ratio;
                    bp = p; br = r0; bc = c0;
                    bcabs = cabs;
                }
            }
            if (bp < 0) { fails++; continue; }
            fails = 0;
            applyPlacement(bp, br, bc, bcabs);
        }

        if (total > bestTotal) {
            bestTotal = total;
            bestPl = pl;
        }
    }

    string out;
    out.reserve(bestPl.size() * 16 + 16);
    out += to_string(bestPl.size());
    out += '\n';
    for (auto& a : bestPl) {
        out += to_string(a[0]);
        out += ' ';
        out += to_string(a[1]);
        out += ' ';
        out += to_string(a[2]);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
