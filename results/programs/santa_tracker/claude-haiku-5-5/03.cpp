#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T, D, W, G;
vector<ll> Lw, Aw;
struct Gift { string name; ll s, w, c, r; };
vector<Gift> gifts;

struct Act { int type; ll arg; int gi; };
// types: 0 AccUp,1 AccDown,2 AccLeft,3 AccRight,4 Float,5 LoadCarrots,6 LoadGift,7 DeliverGift

struct St { ll t, px, py, vx, vy, carrots, gw; };

ll accelMax(ll w) {
    if (w <= 0) return 0;
    for (int i = 0; i < W; i++) if (w <= Lw[i]) return Aw[i];
    return 0;
}

bool within(ll x, ll y, ll qx, ll qy) {
    __int128 dx = x - qx, dy = y - qy;
    return dx * dx + dy * dy <= (__int128)D * D;
}

// S(u) = sum_{k>=0} max(0, u - kA): distance covered while braking from u by A per second
__int128 Sfun(ll u, ll A) {
    ll m = (u + A - 1) / A;
    return (__int128)m * u - (__int128)A * m * (m - 1) / 2;
}

ll maxU(ll dist, ll A) {
    if (dist <= 0) return 0;
    ll lo = 0, hi = dist;
    while (lo < hi) {
        ll mid = lo + (hi - lo + 1) / 2;
        if (Sfun(mid, A) <= (__int128)dist) lo = mid; else hi = mid - 1;
    }
    return lo;
}

inline ll sgnll(ll x) { return (x > 0) - (x < 0); }

// Steer until within D of (qx,qy). Returns false if time limit hit.
bool steerTo(St& s, vector<Act>& acts, ll qx, ll qy, ll limit) {
    while (true) {
        if (within(s.px, s.py, qx, qy)) return true;
        if (s.t + 1 > limit) return false;
        ll w = s.carrots + s.gw;
        ll A = s.carrots > 0 ? accelMax(w) : 0;
        if (A > 0) {
            ll dx = qx - s.px, dy = qy - s.py;
            ll ux = sgnll(dx) * maxU(llabs(dx), A);
            ll uy = sgnll(dy) * maxU(llabs(dy), A);
            ll ex = ux - s.vx, ey = uy - s.vy;
            bool useX = llabs(ex) >= llabs(ey);
            ll e = useX ? ex : ey;
            if (e != 0) {
                ll amt = min(llabs(e), A);
                int type;
                if (useX) {
                    type = e > 0 ? 3 : 2;
                    s.vx += (e > 0 ? amt : -amt);
                } else {
                    type = e > 0 ? 0 : 1;
                    s.vy += (e > 0 ? amt : -amt);
                }
                acts.push_back({type, amt, -1});
                s.carrots--;
            }
        }
        acts.push_back({4, 1, -1});
        s.px += s.vx;
        s.py += s.vy;
        s.t += 1;
    }
}

bool runTrip(St& s, vector<Act>& acts, int gi, ll K, ll limit) {
    const Gift& g = gifts[gi];
    if (!within(s.px, s.py, 0, 0)) {
        if (!steerTo(s, acts, 0, 0, limit)) return false;
    }
    if (K > 0) {
        acts.push_back({5, K, -1});
        s.carrots += K;
    }
    acts.push_back({6, 0, gi});
    s.gw += g.w;
    if (!steerTo(s, acts, g.c, g.r, limit)) return false;
    acts.push_back({7, 0, gi});
    s.gw -= g.w;
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startTime = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    cin >> T >> D >> W >> G;
    Lw.resize(W);
    Aw.resize(W);
    for (int i = 0; i < W; i++) cin >> Lw[i] >> Aw[i];
    gifts.resize(G);
    for (int i = 0; i < G; i++) {
        cin >> gifts[i].name >> gifts[i].s >> gifts[i].w >> gifts[i].c >> gifts[i].r;
    }

    ll maxW = Lw[W - 1];
    vector<double> est(G, 1e18), ratio(G, -1.0);
    for (int i = 0; i < G; i++) {
        ll A0 = accelMax(gifts[i].w + 1);
        if (A0 <= 0 || gifts[i].w + 1 > maxW) continue;
        double L = sqrt((double)gifts[i].c * gifts[i].c + (double)gifts[i].r * gifts[i].r);
        double e = 4.0 * sqrt(max(0.0, L - D) / (double)A0) + 6.0;
        est[i] = e;
        ratio[i] = gifts[i].s / e;
    }
    vector<int> order;
    for (int i = 0; i < G; i++) if (ratio[i] > 0) order.push_back(i);
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (ratio[a] != ratio[b]) return ratio[a] > ratio[b];
        return a < b;
    });

    St s{0, 0, 0, 0, 0, 0, 0};
    vector<Act> out;
    vector<char> done(G, 0);
    bool progress = true;
    const double TIME_LIMIT = 8.0;

    while (progress && elapsed() < TIME_LIMIT) {
        progress = false;
        int fails = 0;
        for (int idx : order) {
            if (done[idx]) continue;
            if (s.t >= T) break;
            if (elapsed() > TIME_LIMIT) break;
            const Gift& g = gifts[idx];
            ll cap = maxW - (s.carrots + s.gw + g.w);
            if (cap < 0) continue;
            vector<ll> cands;
            cands.push_back(0);
            for (ll k = 1; k <= cap; k *= 2) cands.push_back(k);
            if (cap > 0) cands.push_back(cap);
            sort(cands.begin(), cands.end());
            cands.erase(unique(cands.begin(), cands.end()), cands.end());

            ll limit = min((ll)T, s.t + (ll)(3.0 * est[idx]) + 200);
            bool ok = false;
            for (ll K : cands) {
                St trial = s;
                vector<Act> tacts;
                if (runTrip(trial, tacts, idx, K, limit)) {
                    out.insert(out.end(), tacts.begin(), tacts.end());
                    s = trial;
                    done[idx] = 1;
                    ok = true;
                    break;
                }
                if (elapsed() > TIME_LIMIT) break;
            }
            if (ok) {
                progress = true;
                fails = 0;
            } else {
                if (++fails > 400) break;
            }
        }
    }

    string buf;
    buf.reserve(out.size() * 24 + 16);
    buf += to_string(out.size());
    buf += '\n';
    static const char* names[] = {"AccUp", "AccDown", "AccLeft", "AccRight", "Float", "LoadCarrots", "LoadGift", "DeliverGift"};
    for (const Act& a : out) {
        buf += names[a.type];
        buf += ' ';
        if (a.type == 6 || a.type == 7) buf += gifts[a.gi].name;
        else buf += to_string(a.arg);
        buf += '\n';
    }
    cout << buf;
    return 0;
}
