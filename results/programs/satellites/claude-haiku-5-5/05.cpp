#include <bits/stdc++.h>
using namespace std;

const int HALF_LAT = 324000;
const int HALF_LON = 648000;
const int FULL = 1296000;
const int G = 4000;      // grid cell size (divides FULL and 2*HALF_LAT)
const int NLAT = 163;    // cells in latitude
const int NLON = 324;    // cells in longitude (wrapping)

int T, S;
vector<int> SD, SW;
vector<vector<int>> PH, LM;       // per satellite positions per turn
vector<vector<int>> CS, CD;       // per satellite CSR grid index
vector<map<int, pair<int, int>>> sched;  // satellite -> turn -> (dphi, dlam)

int wrapLon(long long x) {
    x %= FULL;
    if (x < -HALF_LON) x += FULL;
    else if (x > 647999) x -= FULL;
    return (int)x;
}

int diffLon(long long a) {
    long long x = ((a % FULL) + FULL) % FULL;
    if (x >= HALF_LON) x -= FULL;
    return (int)x;
}

int keyOf(int phi, int lam) {
    int la = (phi + HALF_LAT) / G;
    if (la < 0) la = 0;
    if (la > NLAT - 1) la = NLAT - 1;
    int lo = (lam + HALF_LON) / G;
    if (lo < 0) lo = 0;
    if (lo > NLON - 1) lo = NLON - 1;
    return la * NLON + lo;
}

bool moveOk(int s, int t, int dp, int dl, int w) {
    auto &M = sched[s];
    auto it = M.lower_bound(t);
    if (it != M.end()) {
        long long dt = it->first - t;
        if (llabs((long long)dp - it->second.first) > (long long)w * dt) return false;
        if (llabs((long long)dl - it->second.second) > (long long)w * dt) return false;
    }
    if (it != M.begin()) {
        auto p = prev(it);
        long long dt = t - p->first;
        if (llabs((long long)dp - p->second.first) > (long long)w * dt) return false;
        if (llabs((long long)dl - p->second.second) > (long long)w * dt) return false;
    } else {
        long long dt = t;
        if (llabs((long long)dp) > (long long)w * dt) return false;
        if (llabs((long long)dl) > (long long)w * dt) return false;
    }
    return true;
}

bool findSlot(int lat, int lon, const vector<pair<int, int>> &rg,
              int &bs, int &bt, int &bdp, int &bdl) {
    for (auto &r : rg) {
        int a = r.first, b = r.second;
        for (int s = 0; s < S; s++) {
            int d = SD[s], w = SW[s];
            int la0 = max(0, (lat - d + HALF_LAT) / G);
            int la1 = min(NLAT - 1, (lat + d + HALF_LAT) / G);
            long long u0 = (long long)lon - d + HALF_LON + FULL;
            long long u1 = (long long)lon + d + HALF_LON + FULL;
            for (int la = la0; la <= la1; la++) {
                for (long long k = u0 / G; k <= u1 / G; k++) {
                    int lo = (int)(k % NLON);
                    int key = la * NLON + lo;
                    int beg = CS[s][key], en = CS[s][key + 1];
                    if (beg >= en) continue;
                    const int *arr = CD[s].data();
                    int idx = (int)(lower_bound(arr + beg, arr + en, a) - arr);
                    for (; idx < en; idx++) {
                        int t = arr[idx];
                        if (t > b) break;
                        int dp = lat - PH[s][t];
                        if (abs(dp) > d) continue;
                        int dl = diffLon((long long)lon - LM[s][t]);
                        if (abs(dl) > d) continue;
                        if (sched[s].count(t)) continue;
                        if (!moveOk(s, t, dp, dl, w)) continue;
                        bs = s; bt = t; bdp = dp; bdl = dl;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main() {
    auto startTime = chrono::steady_clock::now();

    if (scanf("%d", &T) != 1) return 0;
    scanf("%d", &S);
    vector<long long> P0(S), L0(S), V0(S);
    SW.resize(S); SD.resize(S);
    for (int s = 0; s < S; s++) {
        scanf("%lld %lld %lld %d %d", &P0[s], &L0[s], &V0[s], &SW[s], &SD[s]);
    }

    PH.assign(S, {});
    LM.assign(S, {});
    CS.assign(S, {});
    CD.assign(S, {});
    sched.assign(S, {});
    int NC = NLAT * NLON;
    for (int s = 0; s < S; s++) {
        PH[s].assign(T, 0);
        LM[s].assign(T, 0);
        long long p = P0[s], l = L0[s], v = V0[s];
        PH[s][0] = (int)p;
        LM[s][0] = (int)l;
        for (int t = 1; t < T; t++) {
            long long np = p + v, nl = l - 15;
            if (np > HALF_LAT) {
                np = 648000 - np;
                nl = wrapLon(nl - 648000);
                v = -v;
            } else if (np < -HALF_LAT) {
                np = -648000 - np;
                nl = wrapLon(nl - 648000);
                v = -v;
            } else {
                nl = wrapLon(nl);
            }
            p = np; l = nl;
            PH[s][t] = (int)p;
            LM[s][t] = (int)l;
        }
        // Build CSR grid index
        CS[s].assign(NC + 1, 0);
        for (int t = 0; t < T; t++) CS[s][keyOf(PH[s][t], LM[s][t]) + 1]++;
        for (int k = 0; k < NC; k++) CS[s][k + 1] += CS[s][k];
        vector<int> pos(CS[s].begin(), CS[s].end() - 1);
        CD[s].resize(T);
        for (int t = 0; t < T; t++) {
            int k = keyOf(PH[s][t], LM[s][t]);
            CD[s][pos[k]++] = t;
        }
    }

    int C;
    scanf("%d", &C);
    vector<int> CV(C);
    vector<vector<int>> CL(C);
    vector<vector<pair<int, int>>> CR(C);
    map<pair<int, int>, int> lid;
    vector<int> LLat, LLon;
    vector<vector<pair<int, int>>> LImg;
    for (int c = 0; c < C; c++) {
        int V, L, R;
        scanf("%d %d %d", &V, &L, &R);
        CV[c] = V;
        for (int i = 0; i < L; i++) {
            int la, lo;
            scanf("%d %d", &la, &lo);
            auto key = make_pair(la, lo);
            auto itp = lid.find(key);
            int id;
            if (itp == lid.end()) {
                id = (int)LLat.size();
                lid[key] = id;
                LLat.push_back(la);
                LLon.push_back(lo);
                LImg.emplace_back();
            } else {
                id = itp->second;
            }
            CL[c].push_back(id);
        }
        for (int i = 0; i < R; i++) {
            int a, b;
            scanf("%d %d", &a, &b);
            if (b > T - 1) b = T - 1;
            if (a <= b) CR[c].push_back({a, b});
        }
    }

    vector<int> order(C);
    for (int i = 0; i < C; i++) order[i] = i;
    sort(order.begin(), order.end(), [&](int x, int y) {
        double dx = (double)CV[x] / max<size_t>(1, CL[x].size());
        double dy = (double)CV[y] / max<size_t>(1, CL[y].size());
        if (dx != dy) return dx > dy;
        return x < y;
    });

    struct Out { int lat, lon, t, s; };
    vector<Out> out;
    struct Added { int s, t, id; };

    for (int c : order) {
        double el = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
        if (el > 8.0) break;
        if (CR[c].empty()) continue;

        size_t mark = out.size();
        vector<Added> added;
        bool ok = true;
        for (int id : CL[c]) {
            bool sat = false;
            for (auto &ti : LImg[id]) {
                for (auto &r : CR[c]) {
                    if (ti.first >= r.first && ti.first <= r.second) { sat = true; break; }
                }
                if (sat) break;
            }
            if (sat) continue;
            int bs, bt, bdp, bdl;
            if (!findSlot(LLat[id], LLon[id], CR[c], bs, bt, bdp, bdl)) {
                ok = false;
                break;
            }
            sched[bs][bt] = {bdp, bdl};
            LImg[id].push_back({bt, bs});
            out.push_back({LLat[id], LLon[id], bt, bs});
            added.push_back({bs, bt, id});
        }
        if (!ok) {
            for (int i = (int)added.size() - 1; i >= 0; i--) {
                sched[added[i].s].erase(added[i].t);
                LImg[added[i].id].pop_back();
            }
            out.resize(mark);
        }
    }

    string buf;
    buf += to_string(out.size()) + "\n";
    for (auto &o : out) {
        buf += to_string(o.lat) + " " + to_string(o.lon) + " " +
               to_string(o.t) + " " + to_string(o.s) + "\n";
    }
    fputs(buf.c_str(), stdout);
    return 0;
}
