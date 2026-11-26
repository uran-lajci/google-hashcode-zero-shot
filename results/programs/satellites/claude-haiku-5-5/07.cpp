#include <bits/stdc++.h>
using namespace std;

const int LAT_MAX = 324000;
const int LON_OFF = 648000;
const int FULL = 1296000;
const int G = 10800;      // cell size (3 degrees), divides 648000 and 1296000
const int NR = 61;        // lat rows: 0..60
const int NC = 120;       // lon cols: 0..119

struct Photo { int pl, ll, dp, dl; };

struct Sat {
    int w, d;
    vector<int> lat, lon;
    vector<vector<int>> cells;
    map<int, Photo> ph;
};

struct Ins { int s, t; long long key; };

static int wrapLon(long long x) {
    long long r = ((x + LON_OFF) % FULL + FULL) % FULL;
    return (int)(r - LON_OFF);
}

static int floorDiv(long long a, long long b) {
    long long q = a / b;
    if ((a % b != 0) && (a < 0)) q--;
    return (int)q;
}

vector<Sat> sats;
unordered_map<long long, vector<pair<int,int>>> locMap; // key -> (turn, sat)

bool camOK(const Sat& S, int t, int dp, int dl) {
    long long W = S.w;
    auto it = S.ph.lower_bound(t);
    if (it != S.ph.begin()) {
        auto p = prev(it);
        long long dt = t - p->first;
        if (llabs((long long)dp - p->second.dp) > W * dt) return false;
        if (llabs((long long)dl - p->second.dl) > W * dt) return false;
    } else {
        if (llabs((long long)dp) > W * t) return false;
        if (llabs((long long)dl) > W * t) return false;
    }
    if (it != S.ph.end()) {
        long long dt = it->first - t;
        if (llabs((long long)it->second.dp - dp) > W * dt) return false;
        if (llabs((long long)it->second.dl - dl) > W * dt) return false;
    }
    return true;
}

bool findSlot(int pl, int ll, int a, int b, int& bs, int& bt, int& bdp, int& bdl) {
    for (int s = 0; s < (int)sats.size(); s++) {
        Sat& S = sats[s];
        int d = S.d;
        int rlo = max(-LAT_MAX, pl - d), rhi = min(LAT_MAX, pl + d);
        int ri0 = (rlo + LAT_MAX) / G, ri1 = (rhi + LAT_MAX) / G;
        int cj0 = floorDiv((long long)ll - d + LON_OFF, G);
        int cj1 = floorDiv((long long)ll + d + LON_OFF, G);
        for (int ri = ri0; ri <= ri1; ri++) {
            for (int cj = cj0; cj <= cj1; cj++) {
                int c = ((cj % NC) + NC) % NC;
                const vector<int>& v = S.cells[ri * NC + c];
                auto it = lower_bound(v.begin(), v.end(), a);
                for (; it != v.end() && *it <= b; ++it) {
                    int t = *it;
                    if (S.ph.count(t)) continue;
                    int dp = pl - S.lat[t];
                    if (abs(dp) > d) continue;
                    int dl = wrapLon((long long)ll - S.lon[t]);
                    if (abs(dl) > d) continue;
                    if (!camOK(S, t, dp, dl)) continue;
                    bs = s; bt = t; bdp = dp; bdl = dl;
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    int S;
    cin >> S;
    sats.resize(S);
    for (int s = 0; s < S; s++) {
        int phi, lam, v, w, d;
        cin >> phi >> lam >> v >> w >> d;
        Sat& st = sats[s];
        st.w = w;
        st.d = d;
        st.lat.resize(T);
        st.lon.resize(T);
        int la = phi, lo = lam, vel = v;
        for (int t = 0; t < T; t++) {
            st.lat[t] = la;
            st.lon[t] = lo;
            int nl = la + vel;
            long long nlon = (long long)lo - 15;
            if (nl > LAT_MAX) {
                nl = 2 * LAT_MAX - nl;
                nlon -= LON_OFF;
                vel = -vel;
            } else if (nl < -LAT_MAX) {
                nl = -2 * LAT_MAX - nl;
                nlon -= LON_OFF;
                vel = -vel;
            }
            la = nl;
            lo = wrapLon(nlon);
        }
        st.cells.assign(NR * NC, vector<int>());
        for (int t = 0; t < T; t++) {
            int li = (st.lat[t] + LAT_MAX) / G;
            int lj = (st.lon[t] + LON_OFF) / G;
            st.cells[li * NC + lj].push_back(t);
        }
    }

    int C;
    cin >> C;
    struct Coll {
        int V, L, R;
        vector<array<int,2>> locs;
        vector<pair<int,int>> ranges;
    };
    vector<Coll> cols(C);
    for (int c = 0; c < C; c++) {
        Coll& co = cols[c];
        cin >> co.V >> co.L >> co.R;
        co.locs.resize(co.L);
        for (int i = 0; i < co.L; i++) cin >> co.locs[i][0] >> co.locs[i][1];
        co.ranges.resize(co.R);
        for (int i = 0; i < co.R; i++) cin >> co.ranges[i].first >> co.ranges[i].second;
    }

    vector<int> order;
    for (int c = 0; c < C; c++) if (cols[c].V > 0) order.push_back(c);
    sort(order.begin(), order.end(), [&](int x, int y) {
        long long l = (long long)cols[x].V * cols[y].L;
        long long r = (long long)cols[y].V * cols[x].L;
        if (l != r) return l > r;
        if (cols[x].V != cols[y].V) return cols[x].V > cols[y].V;
        return x < y;
    });

    for (int c : order) {
        const Coll& co = cols[c];
        vector<Ins> tmp;
        bool ok = true;
        for (auto& loc : co.locs) {
            int pl = loc[0], ll = loc[1];
            long long key = (long long)(pl + 400000) * 2000000LL + (ll + 700000);
            bool covered = false;
            auto mit = locMap.find(key);
            if (mit != locMap.end()) {
                for (auto& tp : mit->second) {
                    for (auto& r : co.ranges) {
                        if (tp.first >= r.first && tp.first <= r.second) { covered = true; break; }
                    }
                    if (covered) break;
                }
            }
            if (covered) continue;

            bool found = false;
            int bs = -1, bt = -1, bdp = 0, bdl = 0;
            for (auto& r : co.ranges) {
                if (findSlot(pl, ll, r.first, r.second, bs, bt, bdp, bdl)) {
                    found = true;
                    break;
                }
            }
            if (!found) { ok = false; break; }

            sats[bs].ph[bt] = Photo{pl, ll, bdp, bdl};
            locMap[key].push_back({bt, bs});
            tmp.push_back({bs, bt, key});
        }
        if (!ok) {
            for (int i = (int)tmp.size() - 1; i >= 0; i--) {
                sats[tmp[i].s].ph.erase(tmp[i].t);
                auto& vec = locMap[tmp[i].key];
                vec.pop_back();
            }
        }
    }

    long long total = 0;
    for (auto& st : sats) total += (long long)st.ph.size();
    string out;
    out.reserve((size_t)total * 32 + 16);
    out += to_string(total);
    out += '\n';
    for (int s = 0; s < S; s++) {
        for (auto& kv : sats[s].ph) {
            out += to_string(kv.second.pl);
            out += ' ';
            out += to_string(kv.second.ll);
            out += ' ';
            out += to_string(kv.first);
            out += ' ';
            out += to_string(s);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
