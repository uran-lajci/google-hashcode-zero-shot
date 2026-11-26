#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static ll T, D;
static int W, G;
static vector<ll> Lr, Ar;

struct Gift { string name; ll s, w, c, r; };
static vector<Gift> gs;

// type: 0 AccUp,1 AccDown,2 AccLeft,3 AccRight,4 Float,5 LoadCarrots,6 LoadGift,7 DeliverGift
struct Act { int type; ll arg; };

struct St { ll px, py, vx, vy, t, k, gw; };

static ll maxAcc(ll w) {
    if (w <= 0) return 0;
    for (int i = 0; i < W; i++) if (w <= Lr[i]) return Ar[i];
    return 0;
}

static ll isqrtll(ll x) {
    if (x <= 0) return 0;
    ll d = (ll)sqrtl((long double)x);
    while (d > 0 && d * d > x) d--;
    while ((d + 1) * (d + 1) <= x) d++;
    return d;
}

static ll desiredV(ll e, ll A) {
    if (e == 0) return 0;
    ll ae = llabs(e);
    ll d = isqrtll(2 * A * ae);
    if (d < 1) d = 1;
    return e > 0 ? d : -d;
}

static __int128 dist2(ll x, ll y) {
    return (__int128)x * x + (__int128)y * y;
}

static bool nav(St& S, ll tx, ll ty, vector<Act>& acts, ll tlim) {
    __int128 D2 = (__int128)D * D;
    while (dist2(S.px - tx, S.py - ty) > D2) {
        if (S.t + 1 > tlim) return false;
        if (S.k <= 0) return false;
        ll Wt = S.k + S.gw;
        ll A = maxAcc(Wt);
        if (A <= 0) return false;
        ll ex = tx - S.px, ey = ty - S.py;
        ll dx = desiredV(ex, A) - S.vx;
        ll dy = desiredV(ey, A) - S.vy;
        if (dx != 0 && llabs(dx) >= llabs(dy)) {
            ll amt = min(A, llabs(dx));
            if (dx > 0) { acts.push_back({3, amt}); S.vx += amt; }
            else        { acts.push_back({2, amt}); S.vx -= amt; }
            S.k--;
        } else if (dy != 0) {
            ll amt = min(A, llabs(dy));
            if (dy > 0) { acts.push_back({0, amt}); S.vy += amt; }
            else        { acts.push_back({1, amt}); S.vy -= amt; }
            S.k--;
        }
        S.px += S.vx;
        S.py += S.vy;
        S.t += 1;
        acts.push_back({4, 1});
    }
    return true;
}

static bool trip(const St& S0, int gi, ll N, ll tlim, vector<Act>& acts, St& out) {
    acts.clear();
    St S = S0;
    if (!nav(S, 0, 0, acts, tlim)) return false;
    if (N > 0) {
        S.k += N;
        acts.push_back({5, N});
    }
    S.gw += gs[gi].w;
    acts.push_back({6, gi});
    if (!nav(S, gs[gi].c, gs[gi].r, acts, tlim)) return false;
    S.gw -= gs[gi].w;
    acts.push_back({7, gi});
    out = S;
    return true;
}

int main() {
    auto startClock = chrono::steady_clock::now();
    cin >> T >> D >> W >> G;
    Lr.resize(W);
    Ar.resize(W);
    for (int i = 0; i < W; i++) cin >> Lr[i] >> Ar[i];
    gs.resize(G);
    for (int i = 0; i < G; i++) {
        cin >> gs[i].name >> gs[i].s >> gs[i].w >> gs[i].c >> gs[i].r;
    }

    St S{0, 0, 0, 0, 0, 0, 0};
    vector<char> done(G, 0);
    vector<Act> all;
    ll a1 = Ar[0];
    if (a1 < 1) a1 = 1;
    const int K = 8;

    vector<Act> tmp, best;
    St outS, bestS;

    while (true) {
        double elapsed = chrono::duration<double>(chrono::steady_clock::now() - startClock).count();
        if (elapsed > 7.0) break;
        ll R = T - S.t;
        if (R <= 0) break;

        double px = (double)S.px, py = (double)S.py;
        double d0 = sqrt(px * px + py * py);
        vector<pair<double, int>> cand;
        for (int gi = 0; gi < G; gi++) {
            if (done[gi]) continue;
            double qx = (double)gs[gi].c, qy = (double)gs[gi].r;
            double d1 = max(0.0, d0 - (double)D);
            double d2 = max(0.0, sqrt(qx * qx + qy * qy) - (double)D);
            double lb = 2.0 * sqrt(d1 / a1) + 2.0 * sqrt(d2 / a1);
            if (lb > (double)R + 1e-9) continue;
            double key = (double)gs[gi].s / (d1 + d2 + 1.0);
            cand.push_back({key, gi});
        }
        if (cand.empty()) break;
        if ((int)cand.size() > K) {
            nth_element(cand.begin(), cand.begin() + K, cand.end(),
                        [](const pair<double, int>& a, const pair<double, int>& b) {
                            return a.first > b.first;
                        });
            cand.resize(K);
        }

        double bestRatio = -1.0;
        int bestGi = -1;
        for (auto& pr : cand) {
            int gi = pr.second;
            ll w = gs[gi].w;
            ll Aest = maxAcc(w + 1);
            if (Aest <= 0) continue;
            double qx = (double)gs[gi].c, qy = (double)gs[gi].r;
            double d1 = max(0.0, d0 - (double)D);
            double d2 = max(0.0, sqrt(qx * qx + qy * qy) - (double)D);
            double legs = 2.0 * sqrt(d1 / (double)Aest) + 2.0 * sqrt(d2 / (double)Aest);
            ll c1 = (ll)ceil(legs) + 2;
            if (c1 > 1000000) c1 = 1000000;
            ll k0 = S.k;
            vector<ll> Ns;
            ll cands[3] = {c1 - k0, c1 * 3 / 2 - k0, 2 * c1 - k0};
            for (ll nn : cands) {
                if (nn < 1) nn = 1;
                if (nn > 1000000) nn = 1000000;
                if (find(Ns.begin(), Ns.end(), nn) == Ns.end()) Ns.push_back(nn);
            }
            ll tl = min(T, S.t + 3 * (ll)legs + 200);
            for (ll N : Ns) {
                if (trip(S, gi, N, tl, tmp, outS)) {
                    ll dt = outS.t - S.t;
                    double ratio = (double)gs[gi].s / ((double)dt + 1.0);
                    if (ratio > bestRatio) {
                        bestRatio = ratio;
                        bestGi = gi;
                        best = tmp;
                        bestS = outS;
                    }
                }
            }
        }
        if (bestGi < 0) break;
        for (auto& a : best) all.push_back(a);
        S = bestS;
        done[bestGi] = 1;
    }

    string out;
    out.reserve(all.size() * 24 + 16);
    out += to_string(all.size());
    out += '\n';
    for (auto& a : all) {
        switch (a.type) {
            case 0: out += "AccUp " + to_string(a.arg); break;
            case 1: out += "AccDown " + to_string(a.arg); break;
            case 2: out += "AccLeft " + to_string(a.arg); break;
            case 3: out += "AccRight " + to_string(a.arg); break;
            case 4: out += "Float " + to_string(a.arg); break;
            case 5: out += "LoadCarrots " + to_string(a.arg); break;
            case 6: out += "LoadGift " + gs[a.arg].name; break;
            case 7: out += "DeliverGift " + gs[a.arg].name; break;
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
