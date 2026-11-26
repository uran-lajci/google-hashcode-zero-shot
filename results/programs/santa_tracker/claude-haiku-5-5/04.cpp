#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 i128;

int T_, D_, W_, G_;
vector<ll> Lw, Aw;
ll LW;
vector<string> gname;
vector<ll> gs, gwt, gc, gr;

ll amaxW(ll wt) {
    if (wt <= 0) return 0;
    for (size_t i = 0; i < Lw.size(); i++)
        if (wt <= Lw[i]) return Aw[i];
    return 0;
}

ll isq(ll x) {
    if (x <= 0) return 0;
    ll r = (ll)sqrtl((long double)x);
    while (r > 0 && r * r > x) r--;
    while ((r + 1) * (r + 1) <= x) r++;
    return r;
}

bool inR(ll x1, ll y1, ll x2, ll y2) {
    i128 dx = (i128)x1 - x2, dy = (i128)y1 - y2;
    return dx * dx + dy * dy <= (i128)D_ * D_;
}

struct Res {
    ll score;
    vector<string> acts;
};

Res simulate(ll K, const vector<int>& base) {
    vector<int> pool = base;
    vector<char> st(G_, 0);
    ll t = 0, px = 0, py = 0, vx = 0, vy = 0, k = 0, gw = 0, score = 0;
    vector<int> carried;
    vector<string> acts;
    ll R = min<ll>(K, 10);
    ll failGw = -1;
    long long failPool = -1;

    auto deliverIn = [&]() {
        for (size_t j = 0; j < carried.size();) {
            int i = carried[j];
            if (inR(px, py, gc[i], gr[i])) {
                acts.push_back("DeliverGift " + gname[i]);
                gw -= gwt[i];
                score += gs[i];
                st[i] = 2;
                carried[j] = carried.back();
                carried.pop_back();
            } else {
                j++;
            }
        }
    };

    while (true) {
        deliverIn();
        if (inR(px, py, 0, 0)) {
            if (!(gw == failGw && (long long)pool.size() == failPool)) {
                vector<int> np;
                np.reserve(pool.size());
                bool any = false;
                for (int i : pool) {
                    if (gw + gwt[i] + R <= LW) {
                        acts.push_back("LoadGift " + gname[i]);
                        gw += gwt[i];
                        carried.push_back(i);
                        st[i] = 1;
                        any = true;
                    } else {
                        np.push_back(i);
                    }
                }
                pool.swap(np);
                if (!any) {
                    failGw = gw;
                    failPool = (long long)pool.size();
                } else {
                    failGw = -1;
                    failPool = -1;
                }
            }
            deliverIn();
            ll want = min(K, LW - gw) - k;
            if (want > 0) {
                acts.push_back("LoadCarrots " + to_string(want));
                k += want;
            }
        }
        if (t >= T_) break;

        ll qx = 0, qy = 0;
        if (!carried.empty()) {
            double best = 1e300;
            for (int i : carried) {
                double dx = (double)(gc[i] - px), dy = (double)(gr[i] - py);
                double d = dx * dx + dy * dy;
                if (d < best) {
                    best = d;
                    qx = gc[i];
                    qy = gr[i];
                }
            }
        } else {
            if (pool.empty()) break;
            if (inR(px, py, 0, 0) && failGw == 0 && failPool == (long long)pool.size()) break;
            qx = 0;
            qy = 0;
        }

        ll A = (k > 0) ? amaxW(gw + k) : 0;
        if (A > 0) {
            ll dx = qx - px, dy = qy - py;
            ll sx = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
            ll sy = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
            ll vdx = sx * isq(2 * A * llabs(dx));
            ll vdy = sy * isq(2 * A * llabs(dy));
            ll ex = vdx - vx, ey = vdy - vy;
            if (ex != 0 || ey != 0) {
                if (llabs(ex) >= llabs(ey)) {
                    ll amt = min(A, llabs(ex));
                    int sg = ex > 0 ? 1 : -1;
                    acts.push_back(string(sg > 0 ? "AccRight " : "AccLeft ") + to_string(amt));
                    vx += sg * amt;
                } else {
                    ll amt = min(A, llabs(ey));
                    int sg = ey > 0 ? 1 : -1;
                    acts.push_back(string(sg > 0 ? "AccUp " : "AccDown ") + to_string(amt));
                    vy += sg * amt;
                }
                k--;
            }
        }
        acts.push_back("Float 1");
        px += vx;
        py += vy;
        t++;
    }
    return {score, acts};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> T_ >> D_ >> W_ >> G_;
    Lw.resize(W_);
    Aw.resize(W_);
    for (int i = 0; i < W_; i++) cin >> Lw[i] >> Aw[i];
    LW = Lw.back();
    gname.resize(G_);
    gs.resize(G_);
    gwt.resize(G_);
    gc.resize(G_);
    gr.resize(G_);
    for (int i = 0; i < G_; i++) {
        cin >> gname[i] >> gs[i] >> gwt[i] >> gc[i] >> gr[i];
    }

    vector<vector<int>> orders;
    {
        vector<int> idx(G_);
        iota(idx.begin(), idx.end(), 0);
        vector<int> o0 = idx;
        stable_sort(o0.begin(), o0.end(), [&](int a, int b) { return gs[a] > gs[b]; });
        orders.push_back(o0);

        vector<int> o1 = idx;
        stable_sort(o1.begin(), o1.end(), [&](int a, int b) {
            return (double)gs[a] / gwt[a] > (double)gs[b] / gwt[b];
        });
        orders.push_back(o1);

        vector<int> o2 = idx;
        stable_sort(o2.begin(), o2.end(), [&](int a, int b) {
            double da = (double)gc[a] * gc[a] + (double)gr[a] * gr[a];
            double db = (double)gc[b] * gc[b] + (double)gr[b] * gr[b];
            if (da != db) return da < db;
            return gs[a] > gs[b];
        });
        orders.push_back(o2);
    }

    vector<ll> Ks = {3, 10, 30, 100, 300, 1000, 3000, 10000};

    Res best{-1, {}};
    for (ll K : Ks) {
        for (const auto& ord : orders) {
            Res r = simulate(K, ord);
            if (r.score > best.score) best = move(r);
        }
    }

    string out;
    out.reserve(best.acts.size() * 24 + 16);
    out += to_string(best.acts.size());
    out += '\n';
    for (const auto& s : best.acts) {
        out += s;
        out += '\n';
    }
    cout << out;
    return 0;
}
