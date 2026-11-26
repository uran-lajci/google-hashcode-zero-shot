#include <bits/stdc++.h>
using namespace std;

struct Coll { int V, L; vector<pair<int,int>> ranges; int off; };
int T, S, C;
vector<int> sLat0, sLon0, sV0, sW, sD;
vector<Coll> colls;
vector<int> locLat, locLon;
vector<vector<pair<int,int>>> locEnt; // (collection, entry index)
const int G = 10000;
const int GR = 66, GC = 131;
vector<int> cellStart, cellItems;

static inline void stepSat(int &lat, int &lon, int &v) {
    int nl = lat + v; int nlon = lon - 15;
    if (nl > 324000) { nl = 648000 - nl; nlon -= 648000; v = -v; }
    else if (nl < -324000) { nl = -648000 - nl; nlon -= 648000; v = -v; }
    while (nlon < -648000) nlon += 1296000;
    while (nlon > 647999) nlon -= 1296000;
    lat = nl; lon = nlon;
}
static inline bool inRange(const Coll &c, int t) {
    const auto &r = c.ranges;
    int lo = 0, hi = (int)r.size() - 1, idx = -1;
    while (lo <= hi) { int m = (lo + hi) / 2; if (r[m].first <= t) { idx = m; lo = m + 1; } else hi = m - 1; }
    return idx >= 0 && t <= r[idx].second;
}

struct Pic { int lat, lon, t, s; };

int main() {
    if (scanf("%d", &T) != 1) return 0;
    scanf("%d", &S);
    sLat0.resize(S); sLon0.resize(S); sV0.resize(S); sW.resize(S); sD.resize(S);
    for (int i = 0; i < S; i++) scanf("%d %d %d %d %d", &sLat0[i], &sLon0[i], &sV0[i], &sW[i], &sD[i]);
    scanf("%d", &C);
    colls.resize(C);
    unordered_map<long long, int> mp;
    int totalEntries = 0;
    for (int c = 0; c < C; c++) {
        int V, L, R; scanf("%d %d %d", &V, &L, &R);
        colls[c].V = V; colls[c].L = L; colls[c].off = totalEntries; totalEntries += L;
        for (int i = 0; i < L; i++) {
            int a, b; scanf("%d %d", &a, &b);
            long long key = (long long)(a + 400000) * 2000000LL + (b + 700000);
            auto it = mp.find(key); int id;
            if (it == mp.end()) { id = locLat.size(); mp[key] = id; locLat.push_back(a); locLon.push_back(b); locEnt.emplace_back(); }
            else id = it->second;
            locEnt[id].push_back({c, i});
        }
        for (int i = 0; i < R; i++) { int a, b; scanf("%d %d", &a, &b); colls[c].ranges.push_back({a, b}); }
        sort(colls[c].ranges.begin(), colls[c].ranges.end());
    }
    int NL = locLat.size();
    // grid
    auto cellR = [](int lat) { int r = (lat + 324000) / G; return min(max(r, 0), GR - 1); };
    auto cellC = [](int lon) { int c = (lon + 648000) / G; return min(max(c, 0), GC - 1); };
    cellStart.assign(GR * GC + 1, 0);
    for (int i = 0; i < NL; i++) cellStart[cellR(locLat[i]) * GC + cellC(locLon[i]) + 1]++;
    for (int i = 0; i < GR * GC; i++) cellStart[i + 1] += cellStart[i];
    cellItems.resize(NL);
    { vector<int> pos(cellStart.begin(), cellStart.end() - 1);
      for (int i = 0; i < NL; i++) cellItems[pos[cellR(locLat[i]) * GC + cellC(locLon[i])]++] = i; }

    vector<char> alive(C);
    for (int c = 0; c < C; c++) alive[c] = colls[c].V > 0;
    vector<double> thr = {0.3, 0.5, 0.7, 0.85, 1.01, 1.01};
    long long bestScore = -1; vector<Pic> bestPics;
    int maxIter = 6; long long firstWork = 0;
    const int K = 16;
    for (int iter = 0; iter < maxIter; iter++) {
        long long work = 0;
        vector<char> satf(totalEntries, 0);
        vector<int> done(C, 0);
        vector<char> locUse(NL, 0);
        for (int l = 0; l < NL; l++) for (auto &e : locEnt[l]) if (alive[e.first]) { locUse[l] = 1; break; }
        vector<int> cLat(sLat0), cLon(sLon0), cV(sV0);
        vector<int> lastT(S, -1), oP(S, 0), oL(S, 0);
        vector<array<int, K>> pLat(S), pLon(S);
        vector<vector<int>> cand(S);
        vector<Pic> pics;
        for (int t = 0; t < T; t++) {
            int k = t % K;
            if (k == 0) {
                for (int s = 0; s < S; s++) {
                    int mnP = INT_MAX, mxP = INT_MIN, mnL = INT_MAX, mxL = INT_MIN;
                    for (int j = 0; j < K; j++) {
                        pLat[s][j] = cLat[s]; pLon[s][j] = cLon[s];
                        if (abs(cLat[s]) <= 317000) {
                            mnP = min(mnP, cLat[s]); mxP = max(mxP, cLat[s]);
                            mnL = min(mnL, cLon[s]); mxL = max(mxL, cLon[s]);
                        }
                        stepSat(cLat[s], cLon[s], cV[s]);
                    }
                    cand[s].clear();
                    if (mnP == INT_MAX) continue;
                    int d = sD[s];
                    int a0 = mnP - d, a1 = mxP + d, b0 = mnL - d, b1 = mxL + d;
                    int r0 = cellR(a0), r1 = cellR(a1), c0 = cellC(b0), c1 = cellC(b1);
                    for (int r = r0; r <= r1; r++) for (int c = c0; c <= c1; c++) {
                        int cid = r * GC + c; work++;
                        for (int q = cellStart[cid]; q < cellStart[cid + 1]; q++) {
                            int l = cellItems[q]; work++;
                            if (!locUse[l]) continue;
                            if (locLat[l] < a0 || locLat[l] > a1 || locLon[l] < b0 || locLon[l] > b1) continue;
                            cand[s].push_back(l);
                        }
                    }
                }
            }
            for (int s = 0; s < S; s++) {
                if (cand[s].empty()) continue;
                int lat = pLat[s][k], lon = pLon[s][k];
                if (abs(lat) > 317000) continue;
                int d = sD[s];
                long long allowed = (long long)sW[s] * (t - (lastT[s] < 0 ? 0 : lastT[s]));
                int best = -1; double bestV = 0;
                for (int l : cand[s]) {
                    work++;
                    int dp = locLat[l] - lat; if (dp > d || dp < -d) continue;
                    int dl = locLon[l] - lon; if (dl > d || dl < -d) continue;
                    if (llabs((long long)dp - oP[s]) > allowed || llabs((long long)dl - oL[s]) > allowed) continue;
                    double val = 0;
                    for (auto &e : locEnt[l]) {
                        int c = e.first;
                        if (!alive[c] || satf[colls[c].off + e.second]) continue;
                        if (!inRange(colls[c], t)) continue;
                        val += (double)colls[c].V / colls[c].L * (1.0 + 2.0 * done[c] / colls[c].L);
                    }
                    work += locEnt[l].size();
                    if (val > bestV) { bestV = val; best = l; }
                }
                if (best >= 0) {
                    for (auto &e : locEnt[best]) {
                        int c = e.first; int idx = colls[c].off + e.second;
                        if (satf[idx]) continue;
                        if (!inRange(colls[c], t)) continue;
                        satf[idx] = 1; done[c]++;
                    }
                    lastT[s] = t; oP[s] = locLat[best] - lat; oL[s] = locLon[best] - lon;
                    pics.push_back({locLat[best], locLon[best], t, s});
                }
            }
        }
        long long score = 0;
        for (int c = 0; c < C; c++) if (done[c] == colls[c].L) score += colls[c].V;
        if (score > bestScore) { bestScore = score; bestPics = pics; }
        if (iter == 0) {
            firstWork = max(1LL, work);
            long long budget = 1500000000LL;
            maxIter = (int)min<long long>(6, max<long long>(1, budget / firstWork));
        }
        bool changed = false;
        for (int c = 0; c < C; c++) {
            if (!alive[c]) continue;
            if (done[c] < colls[c].L && (double)done[c] / colls[c].L < thr[min(iter, (int)thr.size() - 1)]) { alive[c] = 0; changed = true; }
        }
        if (!changed) break;
    }
    string out;
    out.reserve(bestPics.size() * 32 + 16);
    out += to_string(bestPics.size()); out += '\n';
    for (auto &p : bestPics) {
        out += to_string(p.lat); out += ' '; out += to_string(p.lon); out += ' ';
        out += to_string(p.t); out += ' '; out += to_string(p.s); out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
