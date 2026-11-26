#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static ll cd(ll ax, ll ay, ll bx, ll by) {
    ll dx = ax - bx, dy = ay - by, d2 = dx * dx + dy * dy;
    ll s = (ll)sqrtl((long double)d2);
    while (s * s < d2) s++;
    while (s > 0 && (s - 1) * (s - 1) >= d2) s--;
    return s;
}
int main() {
    int R, Cc, D, T, cap;
    scanf("%d %d %d %d %d", &R, &Cc, &D, &T, &cap);
    int P;
    scanf("%d", &P);
    vector<int> wt(P);
    for (auto &x : wt) scanf("%d", &x);
    int W;
    scanf("%d", &W);
    vector<int> wr(W), wc(W);
    vector<vector<int>> stock(W, vector<int>(P));
    for (int i = 0; i < W; i++) {
        scanf("%d %d", &wr[i], &wc[i]);
        for (int j = 0; j < P; j++) scanf("%d", &stock[i][j]);
    }
    int C;
    scanf("%d", &C);
    vector<int> orr(C), occ(C);
    vector<vector<int>> items(C);
    vector<ll> tw(C, 0);
    for (int i = 0; i < C; i++) {
        int L;
        scanf("%d %d", &orr[i], &occ[i]);
        scanf("%d", &L);
        items[i].resize(L);
        for (auto &x : items[i]) {
            scanf("%d", &x);
            tw[i] += wt[x];
        }
    }
    vector<int> ord(C);
    iota(ord.begin(), ord.end(), 0);
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) { return tw[a] < tw[b]; });
    vector<ll> dr(D, wr[0]), dc(D, wc[0]), dt(D, 0);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int i = 0; i < D; i++) pq.push({0, i});
    string out;
    int Q = 0;
    vector<int> cnt(P, 0);
    for (int oi : ord) {
        if (pq.empty()) break;
        vector<int> types;
        for (int p : items[oi]) {
            if (cnt[p]++ == 0) types.push_back(p);
        }
        map<int, vector<pair<int, int>>> perW;  // w -> (p,k)
        for (int p : types) {
            int n = cnt[p];
            cnt[p] = 0;
            while (n > 0) {
                int best = -1;
                ll bd = LLONG_MAX;
                for (int w = 0; w < W; w++) {
                    if (stock[w][p] <= 0) continue;
                    ll dx = wr[w] - orr[oi], dy = wc[w] - occ[oi];
                    ll d = dx * dx + dy * dy;
                    if (d < bd) { bd = d; best = w; }
                }
                if (best < 0) break;
                int k = min(n, stock[best][p]);
                stock[best][p] -= k;
                n -= k;
                perW[best].push_back({p, k});
            }
        }
        for (auto &pw : perW) {
            int w = pw.first;
            auto &ent = pw.second;
            sort(ent.begin(), ent.end(), [&](const pair<int, int> &a, const pair<int, int> &b) {
                return wt[a.first] > wt[b.first];
            });
            while (true) {
                int load = 0;
                vector<pair<int, int>> trip;
                for (auto &e : ent) {
                    if (e.second <= 0) continue;
                    int m = min(e.second, (cap - load) / wt[e.first]);
                    if (m > 0) {
                        trip.push_back({e.first, m});
                        e.second -= m;
                        load += m * wt[e.first];
                    }
                }
                if (trip.empty()) break;
                if (pq.empty()) continue;
                auto top = pq.top();
                pq.pop();
                int d = top.second;
                ll t = dt[d];
                ll d1 = cd(dr[d], dc[d], wr[w], wc[w]);
                ll d2 = cd(wr[w], wc[w], orr[oi], occ[oi]);
                ll total = t + d1 + (ll)trip.size() + d2 + (ll)trip.size();
                if (total > T) continue;  // retire drone
                for (auto &x : trip) {
                    out += to_string(d) + " L " + to_string(w) + " " + to_string(x.first) + " " + to_string(x.second) + "\n";
                    out += "";
                    Q++;
                }
                for (auto &x : trip) {
                    out += to_string(d) + " D " + to_string(oi) + " " + to_string(x.first) + " " + to_string(x.second) + "\n";
                    Q++;
                }
                dt[d] = total;
                dr[d] = orr[oi];
                dc[d] = occ[oi];
                pq.push({total, d});
            }
        }
    }
    printf("%d\n%s", Q, out.c_str());
}
