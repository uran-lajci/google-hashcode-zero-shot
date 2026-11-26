#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const ll FULL = 1296000LL;
static const ll HALF = 648000LL;

ll wrapL(ll x) {
    x %= FULL;
    if (x < 0) x += FULL;
    if (x >= HALF) x -= FULL;
    return x;
}
ll fdiv(ll a, ll b) { // b > 0
    ll q = a / b;
    if (a % b != 0 && a < 0) q--;
    return q;
}
ll cdiv(ll a, ll b) { // b > 0
    return -fdiv(-a, b);
}

struct Seg { ll ts, p0, l0, v; };
struct Img { ll dp, dl, lp, lg; };
struct Sat {
    ll w, d;
    vector<Seg> segs;
    map<int, Img> sched;
};

int main() {
    auto startTime = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    ll T;
    cin >> T;
    int S;
    cin >> S;
    vector<Sat> sats(S);
    for (int s = 0; s < S; s++) {
        ll phi, lam, v, w, d;
        cin >> phi >> lam >> v >> w >> d;
        sats[s].w = w;
        sats[s].d = d;
        auto& segs = sats[s].segs;
        segs.push_back({0, phi, lam, v});
        for (ll t = 0; t + 1 < T; t++) {
            ll x = phi + v;
            ll nphi, nlam, nv;
            bool nw = false;
            if (x > 324000) {
                nphi = 648000 - x;
                nlam = wrapL(lam - 15 + 648000);
                nv = -v;
                nw = true;
            } else if (x < -324000) {
                nphi = -648000 - x;
                nlam = wrapL(lam - 15 + 648000);
                nv = -v;
                nw = true;
            } else {
                nphi = x;
                nlam = wrapL(lam - 15);
                nv = v;
            }
            if (nw) segs.push_back({t + 1, nphi, nlam, nv});
            phi = nphi;
            lam = nlam;
            v = nv;
        }
    }

    int C;
    cin >> C;
    vector<ll> V(C);
    vector<vector<pair<ll, ll>>> locs(C), wins(C);
    for (int c = 0; c < C; c++) {
        ll L, R;
        cin >> V[c] >> L >> R;
        locs[c].resize(L);
        for (auto& p : locs[c]) cin >> p.first >> p.second;
        wins[c].resize(R);
        for (auto& p : wins[c]) cin >> p.first >> p.second;
    }

    vector<int> order;
    for (int c = 0; c < C; c++) if (V[c] > 0 && !locs[c].empty()) order.push_back(c);
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return (double)V[a] / locs[a].size() > (double)V[b] / locs[b].size();
    });

    map<pair<ll, ll>, map<int, int>> cover;

    auto feasible = [&](Sat& st, int t, ll dp, ll dl) -> bool {
        if (st.sched.count(t)) return false;
        ll w = st.w;
        auto it = st.sched.lower_bound(t);
        if (it != st.sched.end()) {
            ll dt = it->first - t;
            if (llabs(dp - it->second.dp) > w * dt || llabs(dl - it->second.dl) > w * dt) return false;
        }
        ll tp = 0, pp = 0, pl = 0;
        if (it != st.sched.begin()) {
            auto pit = prev(it);
            tp = pit->first;
            pp = pit->second.dp;
            pl = pit->second.dl;
        }
        ll dt = t - tp;
        if (llabs(dp - pp) > w * dt || llabs(dl - pl) > w * dt) return false;
        return true;
    };

    auto findSlot = [&](ll lp, ll lg, const vector<pair<ll, ll>>& W,
                        int& fs, int& ft, ll& fdp, ll& fdl) -> bool {
        for (auto [a, b] : W) {
            for (int s = 0; s < S; s++) {
                Sat& st = sats[s];
                ll d = st.d;
                auto& sg = st.segs;
                // last segment with ts <= a
                int lo_i = 0, hi_i = (int)sg.size() - 1;
                while (lo_i < hi_i) {
                    int mid = (lo_i + hi_i + 1) / 2;
                    if (sg[mid].ts <= a) lo_i = mid;
                    else hi_i = mid - 1;
                }
                for (int j = lo_i; j < (int)sg.size() && sg[j].ts <= b; j++) {
                    ll ts = sg[j].ts;
                    ll te = (j + 1 < (int)sg.size()) ? sg[j + 1].ts : T;
                    ll u0 = max(a, ts) - ts;
                    ll u1 = min(b, te - 1) - ts;
                    if (u0 > u1) continue;
                    ll p0 = sg[j].p0, l0 = sg[j].l0, v = sg[j].v;
                    ll A = llabs(v);
                    ll sgn = v > 0 ? 1 : -1;
                    ll m = sgn * (lp - p0);
                    ll latlo = cdiv(m - d, A), lathi = fdiv(m + d, A);
                    ll loU = max(u0, latlo), hiU = min(u1, lathi);
                    if (loU > hiU) continue;
                    ll c = lg - l0;
                    for (ll k = -3; k <= 3; k++) {
                        ll K = FULL * k;
                        ll ulo = cdiv(-d + K - c, 15);
                        ll uhi = fdiv(d + K - c, 15);
                        ll L2 = max(loU, ulo), H2 = min(hiU, uhi);
                        for (ll u = L2; u <= H2; u++) {
                            ll t = ts + u;
                            ll phS = p0 + v * u;
                            ll lmS = wrapL(l0 - 15 * u);
                            ll dp = lp - phS;
                            ll dl = wrapL(lg - lmS);
                            if (llabs(dp) > d || llabs(dl) > d) continue;
                            if (feasible(st, (int)t, dp, dl)) {
                                fs = s;
                                ft = (int)t;
                                fdp = dp;
                                fdl = dl;
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    };

    for (int c : order) {
        if (elapsed() > 8.0) break;
        bool ok = true;
        struct Added { int s, t; ll lp, lg; };
        vector<Added> added;
        const auto& W = wins[c];
        for (auto [lp, lg] : locs[c]) {
            if (elapsed() > 8.5) { ok = false; break; }
            // covered check
            bool covered = false;
            auto itc = cover.find({lp, lg});
            if (itc != cover.end()) {
                for (auto [a, b] : W) {
                    auto it = itc->second.lower_bound((int)a);
                    if (it != itc->second.end() && it->first <= b) { covered = true; break; }
                }
            }
            if (covered) continue;
            int fs = -1, ft = -1;
            ll fdp = 0, fdl = 0;
            if (!findSlot(lp, lg, W, fs, ft, fdp, fdl)) { ok = false; break; }
            sats[fs].sched[ft] = Img{fdp, fdl, lp, lg};
            cover[{lp, lg}][ft] = fs;
            added.push_back({fs, ft, lp, lg});
        }
        if (!ok) {
            for (auto& ad : added) {
                sats[ad.s].sched.erase(ad.t);
                auto itc = cover.find({ad.lp, ad.lg});
                if (itc != cover.end()) itc->second.erase(ad.t);
            }
        }
    }

    // output
    size_t total = 0;
    for (auto& st : sats) total += st.sched.size();
    string out;
    out.reserve(total * 32 + 16);
    out += to_string(total);
    out += '\n';
    for (int s = 0; s < S; s++) {
        for (auto& kv : sats[s].sched) {
            out += to_string(kv.second.lp);
            out += ' ';
            out += to_string(kv.second.lg);
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
