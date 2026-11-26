#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 LL;

struct Gift {
    string name;
    ll s, w, c, r;
    double dO;
    bool loaded = false, delivered = false;
};

int main() {
    ll T, D;
    int W, G;
    cin >> T >> D >> W >> G;
    vector<ll> lim(W), acc(W);
    for (int i = 0; i < W; i++) cin >> lim[i] >> acc[i];
    vector<Gift> gs(G);
    for (int i = 0; i < G; i++) {
        cin >> gs[i].name >> gs[i].s >> gs[i].w >> gs[i].c >> gs[i].r;
        gs[i].dO = hypot((double)gs[i].c, (double)gs[i].r);
    }

    const ll Lmax = lim[W - 1];
    const ll a1 = acc[0];

    auto maxAcc = [&](ll w) -> ll {
        if (w <= 0) return 0;
        for (int i = 0; i < W; i++)
            if (w <= lim[i]) return acc[i];
        return 0;
    };
    auto inR = [&](ll px, ll py, ll qx, ll qy) -> bool {
        LL dx = (LL)px - qx, dy = (LL)py - qy;
        return dx * dx + dy * dy <= (LL)D * D;
    };

    ll x = 0, y = 0, vx = 0, vy = 0, t = 0, carrots = 0, giftW = 0;
    vector<string> out;

    auto feasible = [&](const Gift& g) -> bool {
        double need = max(0.0, g.dO - (double)D);
        double est = 2.0 * sqrt(need / (double)a1);
        return est <= (double)(T - t);
    };

    while (true) {
        // Deliveries (free, at beginning of second)
        for (int i = 0; i < G; i++) {
            Gift& g = gs[i];
            if (g.loaded && !g.delivered && inR(x, y, g.c, g.r)) {
                g.delivered = true;
                giftW -= g.w;
                out.push_back("DeliverGift " + g.name);
            }
        }
        if (t >= T) break;

        // Loading at origin
        if (inR(x, y, 0, 0)) {
            ll ctT = min(T - t, max(1LL, Lmax / 3));
            ll CT = min(max(carrots, ctT), Lmax - giftW);
            if (CT < 0) CT = carrots;
            ll room = Lmax - giftW - CT;
            vector<int> cand;
            for (int i = 0; i < G; i++) {
                const Gift& g = gs[i];
                if (!g.loaded && !g.delivered && g.w <= room && g.w <= Lmax && feasible(g))
                    cand.push_back(i);
            }
            if (!cand.empty()) {
                sort(cand.begin(), cand.end(), [&](int a, int b) {
                    // score/weight descending, then distance ascending
                    LL lhs = (LL)gs[a].s * gs[b].w, rhs = (LL)gs[b].s * gs[a].w;
                    if (lhs != rhs) return lhs > rhs;
                    return gs[a].dO < gs[b].dO;
                });
                for (int i : cand) {
                    Gift& g = gs[i];
                    if (g.w <= room) {
                        g.loaded = true;
                        room -= g.w;
                        giftW += g.w;
                        out.push_back("LoadGift " + g.name);
                    }
                }
            }
            if (CT > carrots) {
                out.push_back("LoadCarrots " + to_string(CT - carrots));
                carrots = CT;
            }
        }

        // Choose target
        bool have = false;
        ll tx = 0, ty = 0;
        int best = -1;
        double bestScore = -1;
        for (int i = 0; i < G; i++) {
            const Gift& g = gs[i];
            if (g.loaded && !g.delivered) {
                double d = hypot((double)(g.c - x), (double)(g.r - y));
                double sc = (double)g.s / (d + 1.0);
                if (sc > bestScore) { bestScore = sc; best = i; }
            }
        }
        if (best >= 0) {
            have = true;
            tx = gs[best].c;
            ty = gs[best].r;
        } else if (!inR(x, y, 0, 0)) {
            bool any = false;
            for (int i = 0; i < G && !any; i++) {
                const Gift& g = gs[i];
                if (!g.loaded && !g.delivered && g.w <= Lmax && feasible(g)) any = true;
            }
            if (any) { have = true; tx = 0; ty = 0; }
        }

        // Movement control
        ll a = (carrots > 0) ? maxAcc(giftW + carrots) : 0;
        if (have && a > 0) {
            double ex = (double)(tx - x), ey = (double)(ty - y);
            double d = hypot(ex, ey);
            double rem = d - (double)D;
            double vxd = 0, vyd = 0;
            if (rem > 0 && d > 0) {
                double da = (double)a;
                double u = -da / 2.0 + sqrt(da * da / 4.0 + 2.0 * da * rem);
                if (u < 0) u = 0;
                vxd = u * ex / d;
                vyd = u * ey / d;
            }
            ll Vx = llround(vxd), Vy = llround(vyd);
            ll dvx = Vx - vx, dvy = Vy - vy;
            if (dvx != 0 && llabs(dvx) >= llabs(dvy)) {
                ll amt = min(a, llabs(dvx));
                if (dvx > 0) { out.push_back("AccRight " + to_string(amt)); vx += amt; }
                else { out.push_back("AccLeft " + to_string(amt)); vx -= amt; }
                carrots--;
            } else if (dvy != 0) {
                ll amt = min(a, llabs(dvy));
                if (dvy > 0) { out.push_back("AccUp " + to_string(amt)); vy += amt; }
                else { out.push_back("AccDown " + to_string(amt)); vy -= amt; }
                carrots--;
            }
        }

        out.push_back("Float 1");
        x += vx;
        y += vy;
        t++;
    }

    string res;
    res.reserve(out.size() * 16 + 16);
    res += to_string(out.size());
    res += '\n';
    for (const auto& s : out) {
        res += s;
        res += '\n';
    }
    fwrite(res.data(), 1, res.size(), stdout);
    return 0;
}
