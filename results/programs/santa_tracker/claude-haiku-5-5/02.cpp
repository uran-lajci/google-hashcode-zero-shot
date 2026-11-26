#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll T, D;
    int W, G;
    cin >> T >> D >> W >> G;
    vector<ll> L(W), A(W);
    for (int i = 0; i < W; i++) cin >> L[i] >> A[i];
    vector<string> name(G);
    vector<ll> S(G), Wt(G), C(G), R(G);
    for (int i = 0; i < G; i++) cin >> name[i] >> S[i] >> Wt[i] >> C[i] >> R[i];

    auto Aof = [&](ll w) -> ll {
        if (w <= 0) return 0;
        for (int i = 0; i < W; i++)
            if (w <= L[i]) return A[i];
        return 0;
    };
    ll lW = L[W - 1];
    ll Kt = min(1000LL, lW / 2);

    auto d2 = [](ll x1, ll y1, ll x2, ll y2) -> __int128 {
        __int128 dx = (__int128)x1 - x2, dy = (__int128)y1 - y2;
        return dx * dx + dy * dy;
    };
    __int128 D2 = (__int128)D * D;

    vector<int> ord(G);
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        double ka = (double)S[a] / (double)(Wt[a] + 1);
        double kb = (double)S[b] / (double)(Wt[b] + 1);
        if (ka != kb) return ka > kb;
        return d2(0, 0, C[a], R[a]) < d2(0, 0, C[b], R[b]);
    });

    vector<string> acts;
    ll pendingF = 0;
    auto flush = [&]() {
        if (pendingF > 0) {
            acts.push_back("Float " + to_string(pendingF));
            pendingF = 0;
        }
    };

    ll px = 0, py = 0, vx = 0, vy = 0, t = 0, carrots = 0, giftW = 0;
    vector<char> loaded(G, 0), delivered(G, 0);
    vector<int> onBoard;

    auto stopAx = [&](ll v, ll Ad) -> double {
        if (v == 0) return 0.0;
        ll av = llabs(v);
        if (Ad <= 0) return v > 0 ? 1e15 : -1e15;
        ll n = (av + Ad - 1) / Ad;
        ll disp = (n - 1) * av - Ad * ((n - 1) * n / 2);
        return v > 0 ? (double)disp : -(double)disp;
    };

    const int DX[4] = {0, 0, 1, -1};
    const int DY[4] = {1, -1, 0, 0};
    const char* DN[4] = {"AccUp ", "AccDown ", "AccRight ", "AccLeft "};

    while (true) {
        // Deliveries at the beginning of the second.
        {
            vector<int> keep;
            for (int g : onBoard) {
                if (d2(px, py, C[g], R[g]) <= D2) {
                    flush();
                    acts.push_back("DeliverGift " + name[g]);
                    delivered[g] = 1;
                    giftW -= Wt[g];
                } else {
                    keep.push_back(g);
                }
            }
            onBoard = keep;
        }

        // Loading at base.
        if (d2(px, py, 0, 0) <= D2) {
            if (carrots < Kt) {
                flush();
                acts.push_back("LoadCarrots " + to_string(Kt - carrots));
                carrots = Kt;
            }
            for (int g : ord) {
                if (!loaded[g] && !delivered[g] && giftW + Wt[g] + carrots <= lW) {
                    flush();
                    acts.push_back("LoadGift " + name[g]);
                    loaded[g] = 1;
                    onBoard.push_back(g);
                    giftW += Wt[g];
                }
            }
        }

        // Choose navigation goal.
        ll gx = 0, gy = 0;
        if (!onBoard.empty()) {
            int best = onBoard[0];
            double bd = 1e30;
            for (int g : onBoard) {
                double dx = (double)(C[g] - px), dy = (double)(R[g] - py);
                double d = dx * dx + dy * dy;
                if (d < bd) { bd = d; best = g; }
            }
            gx = C[best];
            gy = R[best];
        } else {
            bool can = false;
            for (int g = 0; g < G; g++) {
                if (!loaded[g] && !delivered[g] && Wt[g] + Kt <= lW) { can = true; break; }
            }
            if (!can) break;
            gx = 0;
            gy = 0;
        }

        if (carrots == 0 || t >= T) break;

        auto cost = [&](ll x, ll y, ll nvx, ll nvy, ll Ad) -> double {
            double Rx = (double)x + stopAx(nvx, Ad);
            double Ry = (double)y + stopAx(nvy, Ad);
            return hypot(Rx - (double)gx, Ry - (double)gy);
        };

        ll w = giftW + carrots;
        ll Amax = Aof(w);
        ll AdFloat = Amax;
        double bestC = cost(px + vx, py + vy, vx, vy, AdFloat);
        int bestDir = -1;
        ll bestA = 0;
        ll AdAcc = (carrots - 1 >= 1) ? Aof(w - 1) : 0;
        if (carrots >= 1) {
            for (int d = 0; d < 4; d++) {
                for (ll a = 1; a <= Amax; a++) {
                    ll nvx = vx + DX[d] * a, nvy = vy + DY[d] * a;
                    double c = cost(px + nvx, py + nvy, nvx, nvy, AdAcc);
                    if (c < bestC - 1e-9) {
                        bestC = c;
                        bestDir = d;
                        bestA = a;
                    }
                }
            }
        }

        if (bestA > 0) {
            flush();
            acts.push_back(string(DN[bestDir]) + to_string(bestA));
            carrots--;
            vx += DX[bestDir] * bestA;
            vy += DY[bestDir] * bestA;
        }
        pendingF++;
        px += vx;
        py += vy;
        t++;
    }
    flush();

    string out;
    out.reserve(acts.size() * 24 + 16);
    out += to_string(acts.size());
    out += '\n';
    for (const auto& s : acts) {
        out += s;
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
