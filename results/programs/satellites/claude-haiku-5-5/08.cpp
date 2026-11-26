#include <bits/stdc++.h>
using namespace std;

struct Pt { int dp, dl, la, lo; };

static int T, S;
static vector<vector<int>> LAT, LON;          // per satellite, per turn
static vector<vector<vector<int>>> BIN;       // per satellite, per lat bin: sorted turns
static vector<int> SW, SD;                    // w and d per satellite
static int B;                                 // lat bin width
static vector<map<int, Pt>> TL;               // per satellite timeline of photos (key 0 = sentinel)
static map<long long, vector<pair<int,int>>> LOCP; // location key -> (turn, sat)

static long long wrapLon(long long x) {
    return ((x + 648000) % 1296000 + 1296000) % 1296000 - 648000;
}

static long long locKey(int la, int lo) {
    return (long long)(la + 400000) * 4000000LL + (lo + 700000);
}

static bool feasible(int s, int t, int dp, int dl) {
    if (t == 0) return false;
    auto &tl = TL[s];
    auto it = tl.lower_bound(t);
    if (it != tl.end() && it->first == t) return false;
    auto pv = prev(it);
    long long w = SW[s];
    long long dt = t - pv->first;
    if (llabs((long long)dp - pv->second.dp) > w * dt) return false;
    if (llabs((long long)dl - pv->second.dl) > w * dt) return false;
    if (it != tl.end()) {
        long long dt2 = it->first - t;
        if (llabs((long long)it->second.dp - dp) > w * dt2) return false;
        if (llabs((long long)it->second.dl - dl) > w * dt2) return false;
    }
    return true;
}

static bool findSlot(int la, int lo, int a, int b, int &fs, int &ft) {
    for (int s = 0; s < S; s++) {
        int d = SD[s];
        int latLo = max(la - d, -324000);
        int latHi = min(la + d, 324000);
        int b0 = (latLo + 324000) / B;
        int b1 = (latHi + 324000) / B;
        for (int bi = b0; bi <= b1; bi++) {
            const vector<int> &vec = BIN[s][bi];
            auto it = lower_bound(vec.begin(), vec.end(), a);
            for (; it != vec.end() && *it <= b; ++it) {
                int t = *it;
                int dp = la - LAT[s][t];
                if (abs(dp) > d) continue;
                long long dl = wrapLon((long long)lo - LON[s][t]);
                if (llabs(dl) > d) continue;
                if (feasible(s, t, dp, (int)dl)) {
                    fs = s; ft = t;
                    return true;
                }
            }
        }
    }
    return false;
}

struct Col {
    int V;
    vector<pair<int,int>> locs;
    vector<pair<int,int>> ranges;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> T >> S;
    vector<long long> ph(S), lh(S), vel(S);
    SW.assign(S, 0);
    SD.assign(S, 0);
    int maxD = 0;
    for (int s = 0; s < S; s++) {
        cin >> ph[s] >> lh[s] >> vel[s] >> SW[s] >> SD[s];
        maxD = max(maxD, SD[s]);
    }
    B = max(4000, 2 * maxD + 1);
    int nb = 648000 / B + 1;

    LAT.assign(S, vector<int>(T));
    LON.assign(S, vector<int>(T));
    BIN.assign(S, vector<vector<int>>(nb));
    TL.assign(S, map<int, Pt>());

    for (int s = 0; s < S; s++) {
        long long p = ph[s], l = lh[s], v = vel[s];
        for (int t = 0; t < T; t++) {
            LAT[s][t] = (int)p;
            LON[s][t] = (int)l;
            BIN[s][(p + 324000) / B].push_back(t);
            if (t + 1 < T) {
                long long np = p + v;
                long long nl = l - 15;
                if (np > 324000) { np = 648000 - np; v = -v; }
                else if (np < -324000) { np = -648000 - np; v = -v; }
                p = np;
                l = wrapLon(nl);
            }
        }
        TL[s][0] = Pt{0, 0, 0, 0};
    }

    int C;
    cin >> C;
    vector<Col> cols(C);
    for (int c = 0; c < C; c++) {
        int L, R;
        cin >> cols[c].V >> L >> R;
        cols[c].locs.resize(L);
        for (int i = 0; i < L; i++) cin >> cols[c].locs[i].first >> cols[c].locs[i].second;
        cols[c].ranges.resize(R);
        for (int i = 0; i < R; i++) {
            cin >> cols[c].ranges[i].first >> cols[c].ranges[i].second;
            cols[c].ranges[i].second = min(cols[c].ranges[i].second, T - 1);
        }
    }

    vector<int> order(C);
    for (int i = 0; i < C; i++) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int x, int y) {
        long long lx = cols[x].locs.size(), ly = cols[y].locs.size();
        // density V/L descending
        return (long long)cols[x].V * ly > (long long)cols[y].V * lx;
    });

    long long total = 0;
    for (int ci : order) {
        const Col &col = cols[ci];
        if (col.V == 0) continue;
        bool ok = true;
        vector<tuple<int,int,int,int>> ins; // s, t, la, lo

        for (auto &loc : col.locs) {
            int la = loc.first, lo = loc.second;
            long long key = locKey(la, lo);
            bool covered = false;
            auto itL = LOCP.find(key);
            if (itL != LOCP.end()) {
                for (auto &pr : itL->second) {
                    for (auto &rg : col.ranges) {
                        if (pr.first >= rg.first && pr.first <= rg.second) { covered = true; break; }
                    }
                    if (covered) break;
                }
            }
            if (covered) continue;

            bool found = false;
            int fs = -1, ft = -1;
            for (auto &rg : col.ranges) {
                if (rg.first > rg.second) continue;
                if (findSlot(la, lo, rg.first, rg.second, fs, ft)) { found = true; break; }
            }
            if (!found) { ok = false; break; }

            int dp = la - LAT[fs][ft];
            int dl = (int)wrapLon((long long)lo - LON[fs][ft]);
            TL[fs][ft] = Pt{dp, dl, la, lo};
            LOCP[key].push_back({ft, fs});
            ins.emplace_back(fs, ft, la, lo);
        }

        if (ok) {
            total += col.V;
        } else {
            for (int i = (int)ins.size() - 1; i >= 0; i--) {
                auto [s, t, la, lo] = ins[i];
                TL[s].erase(t);
                auto &vec = LOCP[locKey(la, lo)];
                for (size_t k = 0; k < vec.size(); k++) {
                    if (vec[k].first == t && vec[k].second == s) {
                        vec.erase(vec.begin() + k);
                        break;
                    }
                }
            }
        }
    }

    long long P = 0;
    for (int s = 0; s < S; s++) P += (long long)TL[s].size() - 1;

    string out;
    out.reserve((size_t)P * 40 + 16);
    out += to_string(P);
    out += '\n';
    for (int s = 0; s < S; s++) {
        for (auto &kv : TL[s]) {
            if (kv.first == 0) continue;
            out += to_string(kv.second.la);
            out += ' ';
            out += to_string(kv.second.lo);
            out += ' ';
            out += to_string(kv.first);
            out += ' ';
            out += to_string(s);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
