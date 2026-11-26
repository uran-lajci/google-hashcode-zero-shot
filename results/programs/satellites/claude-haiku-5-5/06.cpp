#include <bits/stdc++.h>
using namespace std;

// Grid parameters for spatial index over locations
static const int G = 20000;
static const int ROWS = 33;   // (648000*2)/G -> indices 0..32
static const int COLS = 65;   // (1296000)/G -> indices 0..64

struct Pick { int phi, lam, t, s; };

// advance satellite state by one turn
static void advanceSat(int &phi, int &lam, int &v) {
    long long np = (long long)phi + v;
    int nv = v;
    bool flip = false;
    if (np > 324000) {
        np = 648000 - np;
        nv = -v;
        flip = true;
    } else if (np < -324000) {
        np = -648000 - np;
        nv = -v;
        flip = true;
    }
    long long nl = (long long)lam - 15;
    if (flip) nl += 648000;
    nl = ((nl + 648000) % 1296000 + 1296000) % 1296000 - 648000;
    phi = (int)np;
    lam = (int)nl;
    v = nv;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    int S;
    cin >> S;
    vector<int> sphi(S), slam(S), sv(S), sw(S), sd(S);
    for (int s = 0; s < S; s++) {
        cin >> sphi[s] >> slam[s] >> sv[s] >> sw[s] >> sd[s];
    }

    int C;
    cin >> C;
    vector<long long> Vc(C, 0);
    vector<int> remC(C, 0);
    vector<vector<pair<int, int>>> windows(C);

    // entries: one per (collection, location) pair
    vector<int> entC;
    vector<char> entSat;
    // unique locations
    map<pair<int, int>, int> locId;
    vector<int> locPhi, locLam, locUnsat;
    vector<vector<int>> locEntries;

    for (int c = 0; c < C; c++) {
        int V, L, R;
        cin >> V >> L >> R;
        Vc[c] = V;
        remC[c] = L;
        for (int k = 0; k < L; k++) {
            int phi, lam;
            cin >> phi >> lam;
            auto key = make_pair(phi, lam);
            auto it = locId.find(key);
            int u;
            if (it == locId.end()) {
                u = (int)locPhi.size();
                locId[key] = u;
                locPhi.push_back(phi);
                locLam.push_back(lam);
                locUnsat.push_back(0);
                locEntries.emplace_back();
            } else {
                u = it->second;
            }
            int e = (int)entC.size();
            entC.push_back(c);
            entSat.push_back(0);
            locEntries[u].push_back(e);
            locUnsat[u]++;
        }
        for (int r = 0; r < R; r++) {
            int a, b;
            cin >> a >> b;
            windows[c].push_back({a, b});
        }
    }

    long long unsatTotal = (long long)entC.size();

    // spatial grid of unique locations
    vector<vector<int>> grid(ROWS * COLS);
    for (int u = 0; u < (int)locPhi.size(); u++) {
        int r = (locPhi[u] + 324000) / G;
        int c = (locLam[u] + 648000) / G;
        if (r < 0) r = 0;
        if (r >= ROWS) r = ROWS - 1;
        if (c < 0) c = 0;
        if (c >= COLS) c = COLS - 1;
        grid[r * COLS + c].push_back(u);
    }

    // satellite state
    vector<int> cphi = sphi, clam = slam, cv = sv;
    vector<int> lastT(S, 0), lastOx(S, 0), lastOy(S, 0);

    vector<Pick> picks;

    auto isActive = [&](int c, int t) -> bool {
        for (const auto &w : windows[c]) {
            if (w.first <= t && t <= w.second) return true;
        }
        return false;
    };

    for (int t = 0; t < T && unsatTotal > 0; t++) {
        for (int s = 0; s < S; s++) {
            int phi = cphi[s], lam = clam[s];
            int d = sd[s];
            int w = sw[s];

            int bestU = -1;
            double bestScore = 0.0;
            int bestOx = 0, bestOy = 0;

            int lo = max(phi - d, -324000);
            int hi = min(phi + d, 324000);
            int r0 = (lo + 324000) / G;
            int r1 = (hi + 324000) / G;
            int llo = max(lam - d, -648000);
            int lhi = min(lam + d, 647999);
            int c0 = (llo + 648000) / G;
            int c1 = (lhi + 648000) / G;
            if (r0 < 0) r0 = 0;
            if (r1 > ROWS - 1) r1 = ROWS - 1;
            if (c0 < 0) c0 = 0;
            if (c1 > COLS - 1) c1 = COLS - 1;

            long long dt = t - lastT[s];

            for (int r = r0; r <= r1; r++) {
                for (int c = c0; c <= c1; c++) {
                    vector<int> &cell = grid[r * COLS + c];
                    size_t i = 0;
                    while (i < cell.size()) {
                        int u = cell[i];
                        if (locUnsat[u] == 0) {
                            cell[i] = cell.back();
                            cell.pop_back();
                            continue;
                        }
                        i++;
                        int ox = locPhi[u] - phi;
                        int oy = locLam[u] - lam;
                        if (ox > d || ox < -d || oy > d || oy < -d) continue;
                        long long lim = (long long)w * dt;
                        if (llabs((long long)ox - lastOx[s]) > lim) continue;
                        if (llabs((long long)oy - lastOy[s]) > lim) continue;
                        double score = 0.0;
                        for (int e : locEntries[u]) {
                            if (entSat[e]) continue;
                            int cc = entC[e];
                            if (Vc[cc] == 0) continue;
                            if (isActive(cc, t)) {
                                score += (double)Vc[cc] / (double)remC[cc];
                            }
                        }
                        if (score > bestScore) {
                            bestScore = score;
                            bestU = u;
                            bestOx = ox;
                            bestOy = oy;
                        }
                    }
                }
            }

            if (bestU >= 0) {
                int u = bestU;
                for (int e : locEntries[u]) {
                    if (entSat[e]) continue;
                    int cc = entC[e];
                    if (isActive(cc, t)) {
                        entSat[e] = 1;
                        remC[cc]--;
                        locUnsat[u]--;
                        unsatTotal--;
                    }
                }
                picks.push_back({locPhi[u], locLam[u], t, s});
                lastT[s] = t;
                lastOx[s] = bestOx;
                lastOy[s] = bestOy;
            }
        }
        for (int s = 0; s < S; s++) {
            advanceSat(cphi[s], clam[s], cv[s]);
        }
    }

    // output
    string out;
    out.reserve(picks.size() * 32 + 16);
    out += to_string(picks.size());
    out += '\n';
    for (const auto &p : picks) {
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
