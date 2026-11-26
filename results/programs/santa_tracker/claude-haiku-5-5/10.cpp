#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 LL;

static ll T_, D_;
static int W_, G_;
static vector<ll> Lw, Aw;
struct Gift { string name; ll s, w, c, r; };
static vector<Gift> gifts;

static ll amaxFor(ll w) {
    if (w <= 0) return 0;
    for (int i = 0; i < W_; i++) if (w <= Lw[i]) return Aw[i];
    return 0;
}
static bool within(ll x1, ll y1, ll x2, ll y2, ll D) {
    ll dx = x1 - x2, dy = y1 - y2;
    LL d2 = (LL)dx * dx + (LL)dy * dy;
    return d2 <= (LL)D * D;
}
static double distD(ll x1, ll y1, ll x2, ll y2) {
    return hypot((double)(x1 - x2), (double)(y1 - y2));
}

// Kinematic state. dir: 0 up, 1 down, 2 right, 3 left
struct Kin {
    ll t = 0, px = 0, py = 0, vx = 0, vy = 0, carrots = 0, gw = 0;
    bool acc(int dir, ll amt) {
        if (amt <= 0 || carrots < 1) return false;
        if (amt > amaxFor(gw + carrots)) return false;
        switch (dir) {
            case 0: vy += amt; break;
            case 1: vy -= amt; break;
            case 2: vx += amt; break;
            default: vx -= amt; break;
        }
        carrots--;
        return true;
    }
    bool fly(ll limit) {
        if (t + 1 > limit) return false;
        px += vx; py += vy; t++;
        return true;
    }
};

static const char* accName[4] = {"AccUp", "AccDown", "AccRight", "AccLeft"};

static bool doAccOut(Kin& k, vector<string>& out, int dir, ll amt) {
    if (!k.acc(dir, amt)) return false;
    out.push_back(string(accName[dir]) + " " + to_string(amt));
    return true;
}
static bool doFlyOut(Kin& k, vector<string>& out, ll limit) {
    if (!k.fly(limit)) return false;
    out.push_back("Float 1");
    return true;
}

struct Sim {
    Kin k;
    vector<char> ld, dl;
    ll score = 0;
};

static bool doLoadCarrots(Sim& S, vector<string>& out, ll N) {
    if (N <= 0 || !within(S.k.px, S.k.py, 0, 0, D_)) return false;
    S.k.carrots += N;
    out.push_back("LoadCarrots " + to_string(N));
    return true;
}
static bool doLoadGift(Sim& S, vector<string>& out, int g) {
    if (S.ld[g] || S.dl[g] || !within(S.k.px, S.k.py, 0, 0, D_)) return false;
    S.ld[g] = 1;
    S.k.gw += gifts[g].w;
    out.push_back("LoadGift " + gifts[g].name);
    return true;
}
static bool doDeliver(Sim& S, vector<string>& out, int g) {
    if (!S.ld[g] || S.dl[g] || !within(S.k.px, S.k.py, gifts[g].c, gifts[g].r, D_)) return false;
    S.dl[g] = 1;
    S.k.gw -= gifts[g].w;
    S.score += gifts[g].s;
    out.push_back("DeliverGift " + gifts[g].name);
    return true;
}

struct Act { int dir; ll amt; };  // dir -1 = float only

// Depth-limited search for a short acceleration sequence that ends within D of (qx,qy).
static bool dfsHit(const Kin& K, ll qx, ll qy, int depth, ll limit, vector<Act>& seq) {
    ll A = K.carrots >= 1 ? amaxFor(K.gw + K.carrots) : 0;
    Act c[12];
    int nc = 0;
    c[nc++] = {-1, 0};
    if (A > 0) {
        ll ex = (qx - K.px) - K.vx, ey = (qy - K.py) - K.vy;
        if (ex != 0) c[nc++] = {ex > 0 ? 2 : 3, min(llabs(ex), A)};
        if (ey != 0) c[nc++] = {ey > 0 ? 0 : 1, min(llabs(ey), A)};
        for (int dir = 0; dir < 4; dir++) {
            c[nc++] = {dir, 1};
            c[nc++] = {dir, A};
        }
    }
    for (int i = 0; i < nc; i++) {
        Kin N = K;
        if (c[i].dir >= 0 && !N.acc(c[i].dir, c[i].amt)) continue;
        if (!N.fly(limit)) continue;
        seq.push_back(c[i]);
        if (within(N.px, N.py, qx, qy, D_)) return true;
        if (depth > 1 && dfsHit(N, qx, qy, depth - 1, limit, seq)) return true;
        seq.pop_back();
    }
    return false;
}

// Moves the sleigh until it is within D of (qx,qy), within a time budget.
static bool goTo(Sim& S, vector<string>& out, ll qx, ll qy, ll budget) {
    ll endT = min(T_, S.k.t + budget);
    while (true) {
        Kin& K = S.k;
        if (within(K.px, K.py, qx, qy, D_)) return true;
        if (K.t >= endT) return false;

        double dist = distD(K.px, K.py, qx, qy);
        double sp = fabs((double)K.vx) + fabs((double)K.vy);
        if (dist <= D_ + 2 * sp + 2) {
            vector<Act> seq;
            int depth = (dist <= D_ + sp + 1) ? 3 : 2;
            if (dfsHit(K, qx, qy, depth, endT, seq)) {
                for (auto& a : seq) {
                    if (a.dir >= 0) doAccOut(K, out, a.dir, a.amt);
                    doFlyOut(K, out, endT);
                }
                continue;
            }
        }

        ll A = K.carrots >= 1 ? amaxFor(K.gw + K.carrots) : 0;
        if (A > 0) {
            double dx = (double)(qx - K.px), dy = (double)(qy - K.py);
            double vdx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0)) * sqrt(2.0 * A * fabs(dx));
            double vdy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0)) * sqrt(2.0 * A * fabs(dy));
            double ex = vdx - K.vx, ey = vdy - K.vy;
            if (fabs(ex) >= fabs(ey)) {
                ll amt = llround(fabs(ex));
                if (amt > A) amt = A;
                if (amt >= 1) doAccOut(K, out, ex > 0 ? 2 : 3, amt);
            } else {
                ll amt = llround(fabs(ey));
                if (amt > A) amt = A;
                if (amt >= 1) doAccOut(K, out, ey > 0 ? 0 : 1, amt);
            }
        }
        if (!doFlyOut(K, out, endT)) return false;
    }
}

static ll budgetFor(double dist) {
    return (ll)(8.0 * sqrt(dist) + 100.0);
}

// One trip: origin -> load carrots and gift -> child -> deliver -> origin.
static bool runTrip(Sim& S, vector<string>& out, int g) {
    const Gift& gf = gifts[g];
    double d = distD(gf.c, gf.r, 0, 0);
    ll K = 10;
    for (int it = 0; it < 4; it++) {
        ll aEst = amaxFor(S.k.gw + gf.w + K);
        if (aEst <= 0) aEst = 1;
        K = (ll)ceil(5.0 * sqrt(d / (double)aEst)) + 6;
    }
    ll cap = Lw[W_ - 1] - S.k.gw - gf.w;
    K = min(K, cap);
    if (K < 1) return false;
    ll need = K - S.k.carrots;

    if (!goTo(S, out, 0, 0, budgetFor(distD(S.k.px, S.k.py, 0, 0)))) return false;
    if (need > 0 && !doLoadCarrots(S, out, need)) return false;
    if (!doLoadGift(S, out, g)) return false;
    if (!goTo(S, out, gf.c, gf.r, budgetFor(distD(S.k.px, S.k.py, gf.c, gf.r)))) return false;
    if (!doDeliver(S, out, g)) return false;
    goTo(S, out, 0, 0, budgetFor(distD(S.k.px, S.k.py, 0, 0)));
    return true;
}

int main() {
    ll T, D;
    int W, G;
    cin >> T >> D >> W >> G;
    T_ = T; D_ = D; W_ = W; G_ = G;
    Lw.resize(W); Aw.resize(W);
    for (int i = 0; i < W; i++) cin >> Lw[i] >> Aw[i];
    gifts.resize(G);
    for (int i = 0; i < G; i++) {
        cin >> gifts[i].name >> gifts[i].s >> gifts[i].w >> gifts[i].c >> gifts[i].r;
    }

    Sim real;
    real.ld.assign(G, 0);
    real.dl.assign(G, 0);
    vector<string> out;

    while (real.k.t < T_) {
        vector<pair<double, int>> cand;
        for (int g = 0; g < G; g++) {
            if (real.dl[g] || real.ld[g]) continue;
            double d = distD(gifts[g].c, gifts[g].r, 0, 0);
            double key = gifts[g].s / (5.0 * sqrt(d + 1.0) + 0.05 * gifts[g].w + 5.0);
            cand.push_back({key, g});
        }
        sort(cand.begin(), cand.end(), [](const pair<double, int>& a, const pair<double, int>& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });

        double bestM = 0;
        int bestG = -1;
        size_t bestLen = 0;
        int succ = 0, att = 0;
        for (auto& pr : cand) {
            if (succ >= 5 || att >= 12) break;
            att++;
            int g = pr.second;
            Sim tr = real;
            vector<string> tmp;
            if (!runTrip(tr, tmp, g)) continue;
            if (out.size() + tmp.size() > 1000000) continue;
            succ++;
            ll used = tr.k.t - real.k.t;
            if (used < 1) used = 1;
            double m = (double)gifts[g].s / (double)used;
            if (m > bestM) {
                bestM = m;
                bestG = g;
                bestLen = tmp.size();
            }
        }
        if (bestG < 0) break;
        if (out.size() + bestLen > 1000000) break;
        if (!runTrip(real, out, bestG)) break;
    }

    string buf;
    buf += to_string(out.size());
    buf += '\n';
    for (auto& s : out) {
        buf += s;
        buf += '\n';
    }
    fwrite(buf.data(), 1, buf.size(), stdout);
    return 0;
}
