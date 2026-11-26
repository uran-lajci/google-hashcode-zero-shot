#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int FULL = 1296000, HALF = 648000;

int wrapL(ll x) {
    x = ((x + HALF) % FULL + FULL) % FULL;
    return (int)(x - HALF);
}
ll floorDiv(ll x, ll y) { // y > 0
    return x >= 0 ? x / y : -((-x + y - 1) / y);
}
ll ceilDiv(ll x, ll y) { // y > 0
    return -floorDiv(-x, y);
}

struct Pass { int p0, p1, lat0, lon0, v; };
struct Ph { int lat, lon, dphi, dlam; };
struct Rec { int s, t; ll key; };

int T, S;
vector<int> W, D;
vector<vector<Pass>> PS;
vector<map<int, Ph>> PH;
unordered_map<ll, vector<pair<int, int>>> photosAt; // key -> (turn, sat)
ll budget = 0;
const ll LIMIT = 400000000LL;

ll locKey(int a, int b) {
    return ((ll)a + 400000LL) * 2000000LL + ((ll)b + 700000LL);
}

bool feasible(int s, int t, int dphi, int dlam) {
    auto &M = PH[s];
    if (M.count(t)) return false;
    ll w = W[s];
    auto it = M.lower_bound(t);
    if (it != M.end()) {
        ll dt = it->first - t;
        if (llabs((ll)it->second.dphi - dphi) > w * dt || llabs((ll)it->second.dlam - dlam) > w * dt)
            return false;
    }
    if (it != M.begin()) {
        auto pr = prev(it);
        ll dt = t - pr->first;
        if (llabs((ll)pr->second.dphi - dphi) > w * dt || llabs((ll)pr->second.dlam - dlam) > w * dt)
            return false;
    } else {
        if (llabs((ll)dphi) > w * (ll)t || llabs((ll)dlam) > w * (ll)t)
            return false;
    }
    return true;
}

bool covered(int a, int b, const vector<pair<int, int>> &rg) {
    auto f = photosAt.find(locKey(a, b));
    if (f == photosAt.end()) return false;
    for (auto &pr : f->second) {
        for (auto &r : rg)
            if (pr.first >= r.first && pr.first <= r.second) return true;
    }
    return false;
}

bool findPhoto(int a, int b, const vector<pair<int, int>> &rg,
               int &os, int &ot, int &odp, int &odl, int &olat, int &olon) {
    for (int s = 0; s < S; s++) {
        int d = D[s];
        auto &ps = PS[s];
        for (auto &r : rg) {
            ll ts = r.first, te = r.second;
            auto it = lower_bound(ps.begin(), ps.end(), ts,
                                  [](const Pass &p, ll x) { return (ll)p.p1 < x; });
            for (; it != ps.end() && it->p0 <= te; ++it) {
                budget++;
                const Pass &P = *it;
                ll lo = max(ts, (ll)P.p0), hi = min(te, (ll)P.p1);
                if (lo > hi) continue;
                ll klo, khi;
                ll v = P.v;
                if (v > 0) {
                    klo = ceilDiv((ll)a - d - P.lat0, v);
                    khi = floorDiv((ll)a + d - P.lat0, v);
                } else {
                    ll m = -v;
                    klo = ceilDiv((ll)P.lat0 - a - d, m);
                    khi = floorDiv((ll)P.lat0 - a + d, m);
                }
                lo = max(lo, (ll)P.p0 + klo);
                hi = min(hi, (ll)P.p0 + khi);
                for (ll t = lo; t <= hi; t++) {
                    budget++;
                    int lat = P.lat0 + P.v * (int)(t - P.p0);
                    int lon = wrapL((ll)P.lon0 - 15LL * (t - P.p0));
                    int dphi = a - lat, dlam = b - lon;
                    if (abs(dphi) > d || abs(dlam) > d) continue;
                    if (feasible(s, (int)t, dphi, dlam)) {
                        os = s; ot = (int)t; odp = dphi; odl = dlam;
                        olat = lat; olon = lon;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> T >> S;
    W.resize(S); D.resize(S); PS.resize(S); PH.resize(S);
    for (int s = 0; s < S; s++) {
        int phi, lam, v, w, d;
        cin >> phi >> lam >> v >> w >> d;
        W[s] = w; D[s] = d;
        auto &ps = PS[s];
        int cphi = phi, clam = lam, cv = v;
        int p0 = 0, lat0 = phi, lon0 = lam, vv = v;
        for (int t = 0; t < T - 1; t++) {
            ll nphi = (ll)cphi + cv;
            if (nphi >= -324000 && nphi <= 324000) {
                cphi = (int)nphi;
                clam = wrapL((ll)clam - 15);
            } else {
                bool north = nphi > 324000;
                cphi = (int)(north ? 648000 - nphi : -648000 - nphi);
                clam = wrapL((ll)clam - 15 - 648000);
                cv = -cv;
                ps.push_back({p0, t, lat0, lon0, vv});
                p0 = t + 1;
                lat0 = cphi; lon0 = clam; vv = cv;
            }
        }
        ps.push_back({p0, T - 1, lat0, lon0, vv});
    }

    int C;
    cin >> C;
    struct Coll {
        int V, L, R;
        vector<pair<int, int>> locs;
        vector<pair<int, int>> rg;
    };
    vector<Coll> cols(C);
    for (int c = 0; c < C; c++) {
        auto &co = cols[c];
        cin >> co.V >> co.L >> co.R;
        co.locs.resize(co.L);
        for (int i = 0; i < co.L; i++) cin >> co.locs[i].first >> co.locs[i].second;
        co.rg.resize(co.R);
        for (int i = 0; i < co.R; i++) cin >> co.rg[i].first >> co.rg[i].second;
    }

    vector<int> order(C);
    iota(order.begin(), order.end(), 0);
    stable_sort(order.begin(), order.end(), [&](int x, int y) {
        ll lhs = (ll)cols[x].V * cols[y].L;
        ll rhs = (ll)cols[y].V * cols[x].L;
        return lhs > rhs;
    });

    for (int idx : order) {
        if (budget > LIMIT) break;
        auto &co = cols[idx];
        if (co.V == 0) continue;
        vector<Rec> rec;
        bool ok = true;
        for (auto &loc : co.locs) {
            int a = loc.first, b = loc.second;
            if (covered(a, b, co.rg)) continue;
            int s, t, dp, dl, lt, ln;
            if (!findPhoto(a, b, co.rg, s, t, dp, dl, lt, ln)) {
                ok = false;
                break;
            }
            PH[s][t] = Ph{lt, ln, dp, dl};
            ll key = locKey(lt, ln);
            photosAt[key].push_back({t, s});
            rec.push_back({s, t, key});
        }
        if (!ok) {
            for (auto &r : rec) {
                PH[r.s].erase(r.t);
                auto &vec = photosAt[r.key];
                for (size_t i = 0; i < vec.size(); i++) {
                    if (vec[i].first == r.t && vec[i].second == r.s) {
                        vec.erase(vec.begin() + i);
                        break;
                    }
                }
            }
        }
    }

    long long total = 0;
    for (int s = 0; s < S; s++) total += (long long)PH[s].size();
    string out;
    out.reserve(total * 32 + 16);
    out += to_string(total) + "\n";
    for (int s = 0; s < S; s++) {
        for (auto &kv : PH[s]) {
            out += to_string(kv.second.lat);
            out += ' ';
            out += to_string(kv.second.lon);
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
