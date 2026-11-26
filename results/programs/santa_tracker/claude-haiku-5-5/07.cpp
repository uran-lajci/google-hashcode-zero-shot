#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static ll TL, RNG;
static int WW, GG;
static vector<ll> Lw, Aw;
static ll LW;
struct Gift { string name; ll s, w, c, r; };
static vector<Gift> gifts;
// types: 0 AccUp,1 AccDown,2 AccLeft,3 AccRight,4 Float,5 LoadCarrots,6 LoadGift,7 DeliverGift
struct Act { int type; ll arg; int gi; };
struct St { ll t, px, py, vx, vy, carrots, gw; };
struct Sim { St s; vector<Act> acts; vector<int> onb; ll score = 0; };

static chrono::steady_clock::time_point startT;
static double elapsedSec() {
    return chrono::duration<double>(chrono::steady_clock::now() - startT).count();
}

static ll amaxOf(const St& s) {
    if (s.carrots <= 0) return 0;
    ll w = s.carrots + s.gw;
    if (w > LW) return 0;
    for (int i = 0; i < WW; i++) if (w <= Lw[i]) return Aw[i];
    return 0;
}

static bool inRange(ll px, ll py, ll tx, ll ty) {
    ll dx = px - tx, dy = py - ty;
    if (dx > RNG || dx < -RNG || dy > RNG || dy < -RNG) return false;
    return dx * dx + dy * dy <= RNG * RNG;
}

static double brakeDisp(ll v, ll A) {
    if (A <= 0 || v == 0) return 0.0;
    ll av = v < 0 ? -v : v;
    ll n = av / A;
    double d = (double)n * (double)av - (double)A * (double)n * (double)(n + 1) / 2.0;
    return v < 0 ? -d : d;
}

static ll ceilDiv(ll a, ll b) { return (a + b - 1) / b; }

// (px,py) is position after one float second with velocity (vx,vy)
static double evalCost(ll px, ll py, ll vx, ll vy, ll A, ll carrotsAfter, ll tx, ll ty) {
    double sx = (double)px + brakeDisp(vx, A);
    double sy = (double)py + brakeDisp(vy, A);
    double dx = sx - (double)tx, dy = sy - (double)ty;
    double c = sqrt(dx * dx + dy * dy);
    if (A > 0) {
        ll need = ceilDiv(llabs(vx), A) + ceilDiv(llabs(vy), A);
        if (carrotsAfter < need) c += 1e12;
    }
    return c;
}

static void emitAct(Sim& S, int type, ll arg, int gi) { S.acts.push_back({type, arg, gi}); }

static void deliverInRange(Sim& S) {
    size_t k = 0;
    for (size_t i = 0; i < S.onb.size(); i++) {
        int g = S.onb[i];
        if (inRange(S.s.px, S.s.py, gifts[g].c, gifts[g].r)) {
            emitAct(S, 7, 0, g);
            S.s.gw -= gifts[g].w;
            S.score += gifts[g].s;
        } else {
            S.onb[k++] = g;
        }
    }
    S.onb.resize(k);
}

// returns 0 ok, 1 time out, 2 other failure
static int navigate(Sim& S, ll tx, ll ty) {
    int guard = 0;
    while (true) {
        deliverInRange(S);
        if (inRange(S.s.px, S.s.py, tx, ty)) return 0;
        if (S.s.t >= TL) return 1;
        if (++guard > 40000) return 2;
        St& s = S.s;
        ll A = amaxOf(s);
        if (A <= 0) return 2;
        ll cs[3] = {1, max(1LL, A / 2), A};
        double best = evalCost(s.px + s.vx, s.py + s.vy, s.vx, s.vy, A, s.carrots, tx, ty);
        int bt = -1; ll bu = 0;
        for (int dir = 0; dir < 4; dir++) {
            for (int k = 0; k < 3; k++) {
                ll u = cs[k];
                if (u > A) continue;
                ll nvx = s.vx, nvy = s.vy;
                if (dir == 0) nvy += u;
                else if (dir == 1) nvy -= u;
                else if (dir == 2) nvx -= u;
                else nvx += u;
                double c = evalCost(s.px + nvx, s.py + nvy, nvx, nvy, A, s.carrots - 1, tx, ty);
                if (c < best - 1e-9) { best = c; bt = dir; bu = u; }
            }
        }
        if (bt >= 0) {
            if (bt == 0) s.vy += bu;
            else if (bt == 1) s.vy -= bu;
            else if (bt == 2) s.vx -= bu;
            else s.vx += bu;
            emitAct(S, bt, bu, -1);
            s.carrots -= 1;
        }
        if (s.t + 1 > TL) return 1;
        emitAct(S, 4, 1, -1);
        s.px += s.vx;
        s.py += s.vy;
        s.t += 1;
    }
}

static int runBatch(const St& s0, const vector<int>& batch, ll K, Sim& out) {
    Sim S;
    S.s = s0;
    int r = navigate(S, 0, 0);
    if (r) return r;
    if (K > 0) {
        emitAct(S, 5, K, -1);
        S.s.carrots += K;
    }
    for (int g : batch) {
        emitAct(S, 6, 0, g);
        S.s.gw += gifts[g].w;
        S.onb.push_back(g);
    }
    while (!S.onb.empty()) {
        int bi = -1;
        double bd = 1e300;
        for (int g : S.onb) {
            double dx = (double)gifts[g].c - (double)S.s.px;
            double dy = (double)gifts[g].r - (double)S.s.py;
            double d = dx * dx + dy * dy;
            if (d < bd) { bd = d; bi = g; }
        }
        r = navigate(S, gifts[bi].c, gifts[bi].r);
        if (r) return r;
        bool still = false;
        for (int g : S.onb) if (g == bi) still = true;
        if (still) return 2;
    }
    out = S;
    return 0;
}

static bool planBatch(const St& s0, const vector<int>& batch, Sim& res) {
    ll wsum = 0;
    for (int g : batch) wsum += gifts[g].w;
    ll cur = s0.carrots + s0.gw;
    ll Kmax = LW - cur - wsum;
    if (Kmax < 0) return false;
    ll lo = -1, hi = -1;
    Sim best;
    ll k = min<ll>(Kmax, 1);
    int tries = 0;
    while (true) {
        Sim tmp;
        int r = runBatch(s0, batch, k, tmp);
        tries++;
        if (r == 0) { hi = k; best = move(tmp); break; }
        if (r == 1) return false;
        lo = k;
        if (k >= Kmax || tries >= 20) return false;
        k = min(Kmax, k * 2);
    }
    int it = 0;
    while (hi - lo > 1 && it < 8) {
        ll mid = (lo + hi) / 2;
        it++;
        Sim tmp;
        int r = runBatch(s0, batch, mid, tmp);
        if (r == 0) { hi = mid; best = move(tmp); }
        else lo = mid;
    }
    res = move(best);
    return true;
}

int main() {
    startT = chrono::steady_clock::now();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> TL >> RNG >> WW >> GG;
    Lw.resize(WW);
    Aw.resize(WW);
    for (int i = 0; i < WW; i++) cin >> Lw[i] >> Aw[i];
    LW = Lw[WW - 1];
    gifts.resize(GG);
    for (int i = 0; i < GG; i++) {
        cin >> gifts[i].name >> gifts[i].s >> gifts[i].w >> gifts[i].c >> gifts[i].r;
    }

    St cur{0, 0, 0, 0, 0, 0, 0};
    vector<char> used(GG, 0);
    vector<Act> allActs;
    ll Amax0 = Aw[0];

    while (true) {
        if (elapsedSec() > 7.5) break;
        if (cur.t >= TL) break;
        ll curW = cur.carrots + cur.gw;
        ll cap = LW - curW - 1;
        vector<int> cand;
        for (int g = 0; g < GG; g++) {
            if (used[g]) continue;
            if (gifts[g].w > cap) continue;
            double d = sqrt((double)gifts[g].c * gifts[g].c + (double)gifts[g].r * gifts[g].r);
            double need = 2.0 * sqrt(d / (double)Amax0);
            if ((double)cur.t + need > (double)TL) continue;
            cand.push_back(g);
        }
        if (cand.empty()) break;
        sort(cand.begin(), cand.end(), [&](int a, int b) {
            if (gifts[a].s != gifts[b].s) return gifts[a].s > gifts[b].s;
            if (gifts[a].w != gifts[b].w) return gifts[a].w < gifts[b].w;
            return a < b;
        });

        bool done = false;
        Sim res;
        vector<int> chosen;
        // (a) greedy batches
        for (double frac : {1.0, 0.5}) {
            ll capf = (ll)floor((double)cap * frac);
            vector<int> batch;
            ll sw = 0;
            for (int g : cand) {
                if (sw + gifts[g].w <= capf) { batch.push_back(g); sw += gifts[g].w; }
            }
            if (batch.empty()) continue;
            if (planBatch(cur, batch, res)) { chosen = batch; done = true; break; }
            if (elapsedSec() > 7.5) break;
        }
        // (b) single gifts
        if (!done) {
            int tried = 0;
            for (int g : cand) {
                if (tried >= 10) break;
                if (elapsedSec() > 7.5) break;
                tried++;
                vector<int> batch = {g};
                if (planBatch(cur, batch, res)) { chosen = batch; done = true; break; }
            }
        }
        if (!done) break;
        for (auto& a : res.acts) allActs.push_back(a);
        for (int g : chosen) used[g] = 1;
        cur = res.s;
    }

    string out;
    out.reserve(allActs.size() * 16 + 16);
    out += to_string(allActs.size());
    out += '\n';
    for (const Act& a : allActs) {
        switch (a.type) {
            case 0: out += "AccUp " + to_string(a.arg); break;
            case 1: out += "AccDown " + to_string(a.arg); break;
            case 2: out += "AccLeft " + to_string(a.arg); break;
            case 3: out += "AccRight " + to_string(a.arg); break;
            case 4: out += "Float " + to_string(a.arg); break;
            case 5: out += "LoadCarrots " + to_string(a.arg); break;
            case 6: out += "LoadGift " + gifts[a.gi].name; break;
            case 7: out += "DeliverGift " + gifts[a.gi].name; break;
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
