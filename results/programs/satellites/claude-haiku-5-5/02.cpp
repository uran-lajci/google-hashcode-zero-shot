#include <bits/stdc++.h>
using namespace std;

const int G = 10000;
const int ROWS = 65;   // lat rows: (phi+324000)/G in [0,64]
const int COLS = 130;  // lon cols: ((lam+648000)/G) mod 130

static long long fdiv(long long a, long long b) {
    // b > 0
    if (a >= 0) return a / b;
    return -((-a + b - 1) / b);
}
static int imod(long long x, int m) {
    x %= m;
    if (x < 0) x += m;
    return (int)x;
}
static int wrapLon(long long x) {
    long long y = (x + 648000) % 1296000;
    if (y < 0) y += 1296000;
    return (int)(y - 648000);
}
static int clampi(int x, int lo, int hi) { return x < lo ? lo : (x > hi ? hi : x); }

struct Photo { int phi, lam, t, s; };

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    int S;
    cin >> S;
    vector<int> sp(S), sl(S), sv(S), sw(S), sd(S);
    for (int i = 0; i < S; i++) {
        cin >> sp[i] >> sl[i] >> sv[i] >> sw[i] >> sd[i];
    }

    int C;
    cin >> C;
    vector<int> cV(C, 0), cRem(C, 0);
    vector<vector<pair<int, int>>> cR(C);
    vector<int> cMaxEnd(C, 0);

    map<pair<int, int>, int> locId;
    vector<int> lPhi, lLam, lMaxEnd, lUnsat;
    vector<vector<int>> lDems;
    vector<int> dColl, dLoc;
    vector<char> dSat;

    for (int c = 0; c < C; c++) {
        int V, L, R;
        cin >> V >> L >> R;
        cV[c] = V;
        vector<pair<int, int>> locs(L);
        for (int k = 0; k < L; k++) {
            int phi, lam;
            cin >> phi >> lam;
            locs[k] = {phi, lam};
        }
        for (int r = 0; r < R; r++) {
            int a, b;
            cin >> a >> b;
            cR[c].push_back({a, b});
            cMaxEnd[c] = max(cMaxEnd[c], b);
        }
        if (V == 0) continue;  // worthless collection, ignore
        cRem[c] = L;
        for (auto &pr : locs) {
            auto it = locId.find(pr);
            int id;
            if (it == locId.end()) {
                id = (int)lPhi.size();
                locId[pr] = id;
                lPhi.push_back(pr.first);
                lLam.push_back(pr.second);
                lMaxEnd.push_back(-1);
                lUnsat.push_back(0);
                lDems.emplace_back();
            } else {
                id = it->second;
            }
            int did = (int)dColl.size();
            dColl.push_back(c);
            dLoc.push_back(id);
            dSat.push_back(0);
            lDems[id].push_back(did);
            lUnsat[id]++;
            lMaxEnd[id] = max(lMaxEnd[id], cMaxEnd[c]);
        }
    }
    // Ensure cMaxEnd is final for lMaxEnd (computed per collection after reading ranges, which is already done
    // before its dems are added). Recompute to be safe:
    for (size_t id = 0; id < lPhi.size(); id++) {
        int m = -1;
        for (int did : lDems[id]) m = max(m, cMaxEnd[dColl[did]]);
        lMaxEnd[id] = m;
    }

    vector<vector<int>> grid(ROWS * COLS);
    long long activeTotal = 0;
    for (size_t id = 0; id < lPhi.size(); id++) {
        int row = clampi((int)fdiv((long long)lPhi[id] + 324000, G), 0, ROWS - 1);
        int col = imod(fdiv((long long)lLam[id] + 648000, G), COLS);
        grid[row * COLS + col].push_back((int)id);
        activeTotal++;
    }

    vector<Photo> photos;
    long long budget = 400000000LL;

    for (int s = 0; s < S && budget > 0; s++) {
        int phi = sp[s], lam = sl[s], vel = sv[s];
        int w = sw[s], d = sd[s];
        bool hasPrev = false;
        int prevT = 0, pdp = 0, pdl = 0;

        for (int t = 0; t < T; t++) {
            if (activeTotal == 0 || budget <= 0) break;

            int best = -1;
            double bestGain = 0;
            int bdp = 0, bdl = 0;

            int r0 = clampi((int)fdiv((long long)phi - d + 324000, G), 0, ROWS - 1);
            int r1 = clampi((int)fdiv((long long)phi + d + 324000, G), 0, ROWS - 1);
            long long x0 = fdiv((long long)lam - d + 648000, G);
            long long x1 = fdiv((long long)lam + d + 648000, G);

            for (int r = r0; r <= r1; r++) {
                for (long long x = x0; x <= x1; x++) {
                    int col = imod(x, COLS);
                    vector<int> &cell = grid[r * COLS + col];
                    size_t i = 0;
                    while (i < cell.size()) {
                        int id = cell[i];
                        if (lUnsat[id] == 0 || lMaxEnd[id] < t) {
                            cell[i] = cell.back();
                            cell.pop_back();
                            activeTotal--;
                            continue;
                        }
                        budget--;
                        int dp = lPhi[id] - phi;
                        if (abs(dp) > d) { i++; continue; }
                        int dl = wrapLon((long long)lLam[id] - lam);
                        if (abs(dl) > d) { i++; continue; }
                        bool ok;
                        if (!hasPrev) {
                            ok = (long long)abs(dp) <= (long long)w * t &&
                                 (long long)abs(dl) <= (long long)w * t;
                        } else {
                            long long dt = (long long)w * (t - prevT);
                            ok = (long long)abs(dp - pdp) <= dt && (long long)abs(dl - pdl) <= dt;
                        }
                        if (!ok) { i++; continue; }

                        double g = 0;
                        for (int did : lDems[id]) {
                            if (dSat[did]) continue;
                            int c = dColl[did];
                            bool in = false;
                            for (auto &rg : cR[c]) {
                                if (t >= rg.first && t <= rg.second) { in = true; break; }
                            }
                            if (in && cRem[c] > 0) g += (double)cV[c] / cRem[c];
                        }
                        if (g > bestGain) {
                            bestGain = g;
                            best = id;
                            bdp = dp;
                            bdl = dl;
                        }
                        i++;
                    }
                }
            }

            if (best >= 0 && bestGain > 0) {
                photos.push_back({lPhi[best], lLam[best], t, s});
                for (int did : lDems[best]) {
                    if (dSat[did]) continue;
                    int c = dColl[did];
                    bool in = false;
                    for (auto &rg : cR[c]) {
                        if (t >= rg.first && t <= rg.second) { in = true; break; }
                    }
                    if (in) {
                        dSat[did] = 1;
                        cRem[c]--;
                        lUnsat[best]--;
                    }
                }
                hasPrev = true;
                prevT = t;
                pdp = bdp;
                pdl = bdl;
            }

            // advance satellite state from turn t to t+1
            long long nphi = (long long)phi + vel;
            long long nlam = (long long)lam - 15;
            int nv = vel;
            if (nphi > 324000) {
                nphi = 648000 - nphi;
                nlam += 648000;
                nv = -vel;
            } else if (nphi < -324000) {
                nphi = -648000 - nphi;
                nlam += 648000;
                nv = -vel;
            }
            lam = wrapLon(nlam);
            phi = (int)nphi;
            vel = nv;
        }
    }

    string out;
    out.reserve(photos.size() * 32 + 16);
    out += to_string(photos.size());
    out += '\n';
    for (const auto &p : photos) {
        out += to_string(p.phi);
        out += ' ';
        out += to_string(p.lam);
        out += ' ';
        out += to_string(p.t);
        out += ' ';
        out += to_string(p.s);
        out += '\n';
    }
    cout << out;
    return 0;
}
