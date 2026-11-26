#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 i128;

struct Gift {
    string name;
    ll s, w, c, r;
    bool loaded = false, delivered = false;
};

int main() {
    ll T, D;
    int W, G;
    cin >> T >> D >> W >> G;
    vector<ll> Lv(W), Av(W);
    for (int i = 0; i < W; i++) cin >> Lv[i] >> Av[i];
    vector<Gift> gs(G);
    for (auto& g : gs) cin >> g.name >> g.s >> g.w >> g.c >> g.r;
    ll Lmax = Lv[W - 1];

    auto amaxOf = [&](ll wt) -> ll {
        if (wt <= 0) return 0;
        for (int i = 0; i < W; i++)
            if (wt <= Lv[i]) return Av[i];
        return 0;
    };

    // Estimated number of acceleration seconds to travel a leg (dx,dy) from rest to rest.
    auto legSteps = [&](ll dx, ll dy, double a) -> double {
        double dist = sqrt((double)dx * dx + (double)dy * dy);
        if (dist <= (double)D) return 0;
        double deff = dist - (double)D;
        double ex = fabs((double)dx) * deff / dist;
        double ey = fabs((double)dy) * deff / dist;
        double s = 0;
        if (ex > 0.5) s += 2 * ceil(sqrt(ex / a));
        if (ey > 0.5) s += 2 * ceil(sqrt(ey / a));
        return s;
    };
    auto totalSteps = [&](const vector<pair<ll, ll>>& chain, double a) -> double {
        double s = 0;
        ll px = 0, py = 0;
        for (auto& p : chain) {
            s += legSteps(p.first - px, p.second - py, a);
            px = p.first;
            py = p.second;
        }
        return s;
    };

    vector<string> acts;
    ll t = 0, px = 0, py = 0, vx = 0, vy = 0, carrots = 0, giftW = 0;
    vector<int> onboard;
    int mode = 0;  // 0 = loading (heading to origin), 1 = delivering
    bool done = false;

    auto within = [&](ll x, ll y, ll cx, ll cy) -> bool {
        i128 dx = x - cx, dy = y - cy;
        return dx * dx + dy * dy <= (i128)D * D;
    };

    auto doDeliver = [&]() {
        vector<int> keep;
        for (int idx : onboard) {
            Gift& g = gs[idx];
            if (within(px, py, g.c, g.r)) {
                acts.push_back("DeliverGift " + g.name);
                giftW -= g.w;
                g.delivered = true;
            } else {
                keep.push_back(idx);
            }
        }
        onboard.swap(keep);
    };

    auto evalK = [&](const vector<pair<ll, ll>>& chain, ll gwt, ll& Kout) -> bool {
        ll remaining = T - t;
        ll K = carrots;
        for (int it = 0; it < 6; it++) {
            ll a = amaxOf(K + gwt);
            if (a <= 0) return false;
            double need = totalSteps(chain, (double)a);
            ll Knew = max(carrots, (ll)ceil(need * 1.25) + 4);
            if (Knew > 1000000) return false;
            if (Knew == K) break;
            K = Knew;
        }
        ll a = amaxOf(K + gwt);
        if (a <= 0) return false;
        double need = totalSteps(chain, (double)a);
        if (need > (double)remaining) return false;
        Kout = K;
        return true;
    };

    auto loadRoutine = [&]() {
        ll remaining = T - t;
        vector<pair<double, int>> cand;
        for (int i = 0; i < G; i++) {
            Gift& g = gs[i];
            if (g.loaded || g.delivered || g.w > Lmax) continue;
            double a = (double)amaxOf(carrots + g.w);
            if (a <= 0) continue;
            if (legSteps(g.c, g.r, a) > (double)remaining) continue;
            double d0 = sqrt((double)g.c * g.c + (double)g.r * g.r);
            cand.push_back({(double)g.s / ((double)g.w + sqrt(d0) + 1.0), i});
        }
        size_t k = min<size_t>(150, cand.size());
        partial_sort(cand.begin(), cand.begin() + k, cand.end(),
                     [](const pair<double, int>& a, const pair<double, int>& b) {
                         return a.first > b.first;
                     });
        cand.resize(k);

        vector<pair<ll, ll>> chain;
        vector<int> chosen;
        ll gw = 0, bestK = carrots;
        bool any = false;
        for (auto& pr : cand) {
            if (chosen.size() >= 40) break;
            int i = pr.second;
            Gift& g = gs[i];
            chain.push_back({g.c, g.r});
            ll ngw = gw + g.w;
            ll K;
            if (evalK(chain, ngw, K)) {
                chosen.push_back(i);
                gw = ngw;
                bestK = K;
                any = true;
            } else {
                chain.pop_back();
            }
        }
        if (!any) {
            done = true;
            return;
        }
        ll addC = bestK - carrots;
        if (addC > 0) {
            acts.push_back("LoadCarrots " + to_string(addC));
            carrots = bestK;
        }
        for (int i : chosen) {
            acts.push_back("LoadGift " + gs[i].name);
            gs[i].loaded = true;
            giftW += gs[i].w;
            onboard.push_back(i);
        }
        mode = 1;
    };

    while (true) {
        doDeliver();
        if (mode == 1 && onboard.empty()) mode = 0;
        if (t >= T) break;

        if (mode == 0 && !done && within(px, py, 0, 0)) {
            loadRoutine();
            if (mode == 1) doDeliver();
        }

        if (!done && !(mode == 1 && onboard.empty())) {
            double tx = 0, ty = 0;
            if (mode == 1) {
                double best = 1e300;
                for (int idx : onboard) {
                    double dx = (double)gs[idx].c - (double)px;
                    double dy = (double)gs[idx].r - (double)py;
                    double d = dx * dx + dy * dy;
                    if (d < best) {
                        best = d;
                        tx = (double)gs[idx].c;
                        ty = (double)gs[idx].r;
                    }
                }
            }
            ll amaxNow = amaxOf(carrots + giftW);
            if (carrots > 0 && amaxNow > 0) {
                double dxT = tx - (double)px, dyT = ty - (double)py;
                double dist = sqrt(dxT * dxT + dyT * dyT);
                double a = (double)amaxNow;
                double deff = max(0.0, dist - (double)D);
                double vm = sqrt(2 * a * deff);
                double vdx = 0, vdy = 0;
                if (dist > 0) {
                    vdx = dxT / dist * vm;
                    vdy = dyT / dist * vm;
                }
                double ddx = vdx - (double)vx, ddy = vdy - (double)vy;
                bool xFirst = fabs(ddx) >= fabs(ddy);
                for (int k = 0; k < 2; k++) {
                    bool isX = (k == 0) == xFirst;
                    double dv = isX ? ddx : ddy;
                    ll amt = llround(dv);
                    if (amt > amaxNow) amt = amaxNow;
                    if (amt < -amaxNow) amt = -amaxNow;
                    if (amt != 0) {
                        if (isX) {
                            if (amt > 0) acts.push_back("AccRight " + to_string(amt));
                            else acts.push_back("AccLeft " + to_string(-amt));
                            vx += amt;
                        } else {
                            if (amt > 0) acts.push_back("AccUp " + to_string(amt));
                            else acts.push_back("AccDown " + to_string(-amt));
                            vy += amt;
                        }
                        carrots--;
                        break;
                    }
                }
            }
        }

        acts.push_back("Float 1");
        px += vx;
        py += vy;
        t++;
    }

    string out;
    out.reserve(acts.size() * 16 + 16);
    out += to_string(acts.size());
    out += '\n';
    for (auto& s : acts) {
        out += s;
        out += '\n';
    }
    fputs(out.c_str(), stdout);
    return 0;
}
