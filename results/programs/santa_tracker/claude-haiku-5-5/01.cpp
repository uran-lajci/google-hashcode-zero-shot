#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll T, D;
    int NW, G;
    cin >> T >> D >> NW >> G;
    vector<ll> L(NW), A(NW);
    for (int i = 0; i < NW; i++) cin >> L[i] >> A[i];
    vector<string> name(G);
    vector<ll> S(G), Wt(G), C(G), R(G);
    for (int i = 0; i < G; i++) cin >> name[i] >> S[i] >> Wt[i] >> C[i] >> R[i];

    const ll Lmax = L[NW - 1];
    const double amin = (double)A[NW - 1];
    auto amaxF = [&](ll w) -> ll {
        if (w <= 0 || w > Lmax) return 0;
        for (int i = 0; i < NW; i++)
            if (w <= L[i]) return A[i];
        return 0;
    };
    const double Dd = (double)D;
    const double D2 = Dd * Dd;
    auto dist = [&](int i) { return sqrt((double)C[i] * C[i] + (double)R[i] * R[i]); };

    // candidate gifts (loadable at all), ordered by priority
    vector<int> cand;
    for (int i = 0; i < G; i++)
        if (Wt[i] + 1 <= Lmax) cand.push_back(i);
    {
        vector<double> ratio(G, 0.0);
        for (int i : cand) ratio[i] = (double)S[i] / (sqrt(dist(i)) + 1.0);
        stable_sort(cand.begin(), cand.end(), [&](int a, int b) {
            if (ratio[a] != ratio[b]) return ratio[a] > ratio[b];
            return a < b;
        });
    }

    vector<char> done(G, 0), isLoaded(G, 0);
    vector<int> loaded;
    ll px = 0, py = 0, vx = 0, vy = 0, W = 0, carrots = 0;
    vector<string> out;
    ll t = 0;

    auto d2 = [&](double x1, double y1, double x2, double y2) {
        double dx = x1 - x2, dy = y1 - y2;
        return dx * dx + dy * dy;
    };

    while (true) {
        // 1. deliveries at beginning of second
        {
            vector<int> keep;
            for (int idx : loaded) {
                if (d2((double)px, (double)py, (double)C[idx], (double)R[idx]) <= D2 + 1e-9) {
                    out.push_back("DeliverGift " + name[idx]);
                    done[idx] = 1;
                    isLoaded[idx] = 0;
                    W -= Wt[idx];
                } else {
                    keep.push_back(idx);
                }
            }
            loaded = keep;
        }

        // 2. loads at origin
        if (d2((double)px, (double)py, 0.0, 0.0) <= D2 + 1e-9) {
            vector<int> newCand;
            for (int idx : cand) {
                if (done[idx] || isLoaded[idx]) continue;
                if (W + Wt[idx] + 1 <= Lmax) {
                    out.push_back("LoadGift " + name[idx]);
                    isLoaded[idx] = 1;
                    loaded.push_back(idx);
                    W += Wt[idx];
                } else {
                    newCand.push_back(idx);
                }
            }
            cand = newCand;

            // carrots top-up
            if (!loaded.empty() || !cand.empty()) {
                double Lref = 0.0;
                if (!loaded.empty()) {
                    for (int idx : loaded) Lref = max(Lref, dist(idx));
                } else {
                    Lref = dist(cand[0]);
                    for (int idx : cand) Lref = min(Lref, dist(idx));
                }
                ll K = 10 + (ll)(4.0 * ceil(sqrt(Lref / amin)));
                K = min(K, T);
                ll need = K - carrots;
                ll room = Lmax - W;
                ll N = min(need, room);
                N = min(N, 1000000LL);
                if (N > 0) {
                    out.push_back("LoadCarrots " + to_string(N));
                    carrots += N;
                    W += N;
                }
            }
        }

        if (t >= T) break;

        // 3. acceleration
        ll Acur = (carrots > 0) ? amaxF(W) : 0;
        bool hasTarget = false;
        double qx = 0, qy = 0;
        if (!loaded.empty()) {
            double best = -1;
            for (int idx : loaded) {
                double dd = d2((double)px, (double)py, (double)C[idx], (double)R[idx]);
                if (!hasTarget || dd < best) {
                    best = dd;
                    qx = (double)C[idx];
                    qy = (double)R[idx];
                    hasTarget = true;
                }
            }
        } else if (!cand.empty()) {
            hasTarget = true;
            qx = 0;
            qy = 0;
        }

        if (Acur > 0 && hasTarget) {
            double dx = qx - (double)px, dy = qy - (double)py;
            double vxd = (dx >= 0 ? 1.0 : -1.0) * sqrt(2.0 * Acur * fabs(dx));
            double vyd = (dy >= 0 ? 1.0 : -1.0) * sqrt(2.0 * Acur * fabs(dy));
            double nx = vxd - (double)vx, ny = vyd - (double)vy;
            pair<double, int> ax[2] = {{fabs(nx), 0}, {fabs(ny), 1}};
            if (ax[1].first > ax[0].first) swap(ax[0], ax[1]);
            for (int k = 0; k < 2; k++) {
                int axis = ax[k].second;
                double need = axis == 0 ? nx : ny;
                ll m = llround(fabs(need));
                if (m <= 0) continue;
                m = min(m, Acur);
                if (axis == 0) {
                    if (need > 0) { out.push_back("AccRight " + to_string(m)); vx += m; }
                    else { out.push_back("AccLeft " + to_string(m)); vx -= m; }
                } else {
                    if (need > 0) { out.push_back("AccUp " + to_string(m)); vy += m; }
                    else { out.push_back("AccDown " + to_string(m)); vy -= m; }
                }
                carrots--;
                W--;
                break;
            }
        }

        // 4. float one second
        out.push_back("Float 1");
        px += vx;
        py += vy;
        t++;
    }

    string res;
    res.reserve(out.size() * 24 + 16);
    res += to_string(out.size());
    res += '\n';
    for (const string& s : out) {
        res += s;
        res += '\n';
    }
    cout << res;
    return 0;
}
