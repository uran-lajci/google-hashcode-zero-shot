#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

ll T, D;
int NW, G;
vector<ll> Lw, Aw;
ll Lmax;

struct Gift { string name; ll s, w, c, r; };
vector<Gift> gifts;

// type: 0 AccUp,1 AccDown,2 AccLeft,3 AccRight,4 Float,5 LoadCarrots,6 LoadGift,7 Deliver
struct Act { int type; ll arg; int gi; };

struct St {
    ll t = 0, px = 0, py = 0, vx = 0, vy = 0, carrots = 0, gw = 0, score = 0;
    vector<int> onboard;
    vector<char> loaded, delivered;
};

ll work = 0;

static inline bool inR(ll x, ll y, ll qx, ll qy, ll R) {
    __int128 dx = x - qx, dy = y - qy;
    return dx * dx + dy * dy <= (__int128)R * R;
}

ll amax(const St& S) {
    if (S.carrots <= 0) return 0;
    ll w = S.carrots + S.gw;
    if (w > Lmax) return 0;
    for (size_t i = 0; i < Lw.size(); i++)
        if (w <= Lw[i]) return Aw[i];
    return 0;
}

void deliverAll(St& S, vector<Act>& out) {
    size_t j = 0;
    for (size_t i = 0; i < S.onboard.size(); i++) {
        int g = S.onboard[i];
        if (inR(S.px, S.py, gifts[g].c, gifts[g].r, D)) {
            S.score += gifts[g].s;
            S.gw -= gifts[g].w;
            S.delivered[g] = 1;
            out.push_back({7, 0, g});
        } else {
            S.onboard[j++] = g;
        }
    }
    S.onboard.resize(j);
}

bool floatOne(St& S, vector<Act>& out) {
    if (S.t >= T) return false;
    S.t++;
    S.px += S.vx;
    S.py += S.vy;
    out.push_back({4, 1, -1});
    return true;
}

bool moveTo(St& S, vector<Act>& out, ll qx, ll qy, ll R) {
    while (true) {
        deliverAll(S, out);
        if (inR(S.px, S.py, qx, qy, R)) return true;
        if (S.t >= T) return false;
        work++;
        ll a = amax(S);
        double dx = (double)(qx - S.px), dy = (double)(qy - S.py);
        double dist = sqrt(dx * dx + dy * dy);
        if (dist < 1e-9) dist = 1e-9;
        double e = dist - (double)R;
        if (e < 0.5) e = 0.5;
        if (a > 0) {
            double sp = sqrt(2.0 * (double)a * e);
            ll tvx = llround(dx / dist * sp), tvy = llround(dy / dist * sp);
            ll ddx = tvx - S.vx, ddy = tvy - S.vy;
            if (ddx != 0 || ddy != 0) {
                if (llabs(ddx) >= llabs(ddy)) {
                    ll amt = min(a, llabs(ddx));
                    if (ddx > 0) { out.push_back({3, amt, -1}); S.vx += amt; }
                    else         { out.push_back({2, amt, -1}); S.vx -= amt; }
                } else {
                    ll amt = min(a, llabs(ddy));
                    if (ddy > 0) { out.push_back({0, amt, -1}); S.vy += amt; }
                    else         { out.push_back({1, amt, -1}); S.vy -= amt; }
                }
                S.carrots--;
            }
        }
        if (!floatOne(S, out)) return false;
    }
}

bool runMission(St& S, vector<Act>& out, const vector<int>& M, ll K) {
    if (!moveTo(S, out, 0, 0, D)) return false;
    if (K > 0) {
        S.carrots += K;
        out.push_back({5, K, -1});
    }
    for (int g : M) {
        if (!S.loaded[g] && !S.delivered[g]) {
            S.loaded[g] = 1;
            S.onboard.push_back(g);
            S.gw += gifts[g].w;
            out.push_back({6, 0, g});
        }
    }
    while (true) {
        int best = -1;
        double bd = 0;
        for (int g : S.onboard) {
            double dx = (double)(gifts[g].c - S.px), dy = (double)(gifts[g].r - S.py);
            double d = dx * dx + dy * dy;
            if (best < 0 || d < bd) { best = g; bd = d; }
        }
        if (best < 0) break;
        if (!moveTo(S, out, gifts[best].c, gifts[best].r, D)) return false;
        if (!S.delivered[best]) break; // safety
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> T >> D >> NW >> G;
    Lw.resize(NW); Aw.resize(NW);
    for (int i = 0; i < NW; i++) cin >> Lw[i] >> Aw[i];
    Lmax = Lw.back();
    gifts.resize(G);
    for (int i = 0; i < G; i++) {
        cin >> gifts[i].name >> gifts[i].s >> gifts[i].w >> gifts[i].c >> gifts[i].r;
    }

    St cur;
    cur.loaded.assign(G, 0);
    cur.delivered.assign(G, 0);
    vector<Act> actions;

    const ll WORK_LIMIT = 80000000LL;
    const vector<int> sizes = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233};
    vector<double> pri(G), dist0(G);
    for (int i = 0; i < G; i++) {
        double c = (double)gifts[i].c, r = (double)gifts[i].r;
        dist0[i] = sqrt(c * c + r * r);
        pri[i] = (double)gifts[i].s / (1.0 + dist0[i]);
    }

    while (cur.t < T && work <= WORK_LIMIT) {
        vector<int> avail;
        for (int i = 0; i < G; i++)
            if (!cur.loaded[i] && !cur.delivered[i]) avail.push_back(i);
        if (avail.empty()) break;
        sort(avail.begin(), avail.end(), [&](int a, int b) {
            if (pri[a] != pri[b]) return pri[a] > pri[b];
            return a < b;
        });

        bool found = false;
        double bestRatio = 0;
        St bestSt;
        vector<Act> bestOut;

        for (int k : sizes) {
            if (k > (int)avail.size()) break;
            vector<int> M(avail.begin(), avail.begin() + k);
            double est = 0;
            for (int g : M) est += 2.0 * sqrt(dist0[g] / (double)Aw[0]) + 2.0;
            ll base = (ll)ceil(est);
            vector<ll> Ks = {base / 2 + 2, base + 2, 2 * base + 4};
            for (ll K : Ks) {
                K = max(1LL, min(1000000LL, K));
                St S = cur;
                vector<Act> out;
                runMission(S, out, M, K);
                ll gain = S.score - cur.score;
                if (gain <= 0) continue;
                double ratio = (double)gain / (double)max(1LL, S.t - cur.t);
                if (!found || ratio > bestRatio) {
                    found = true;
                    bestRatio = ratio;
                    bestSt = S;
                    bestOut = out;
                }
            }
        }
        if (!found) break;
        cur = bestSt;
        actions.insert(actions.end(), bestOut.begin(), bestOut.end());
    }

    string res;
    res.reserve(actions.size() * 24 + 16);
    res += to_string(actions.size());
    res += '\n';
    static const char* names[] = {"AccUp", "AccDown", "AccLeft", "AccRight", "Float",
                                  "LoadCarrots", "LoadGift", "DeliverGift"};
    for (const Act& a : actions) {
        res += names[a.type];
        res += ' ';
        if (a.type == 6 || a.type == 7) res += gifts[a.gi].name;
        else res += to_string(a.arg);
        res += '\n';
    }
    fwrite(res.data(), 1, res.size(), stdout);
    return 0;
}
