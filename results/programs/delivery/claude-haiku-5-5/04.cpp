#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// Ceiling of Euclidean distance, computed exactly with integers.
ll dist(ll r1, ll c1, ll r2, ll c2) {
    ll dr = r1 - r2, dc = c1 - c2;
    ll s = dr * dr + dc * dc;
    ll q = (ll)sqrtl((long double)s);
    while (q * q < s) q++;
    while (q > 0 && (q - 1) * (q - 1) >= s) q--;
    return q;
}

struct Order {
    int r, c;
    vector<pair<int,int>> need; // (product, count)
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, MAXL;
    cin >> R >> Cc >> D >> T >> MAXL;

    int P;
    cin >> P;
    vector<int> weight(P);
    for (int i = 0; i < P; i++) cin >> weight[i];

    int W;
    cin >> W;
    vector<int> wr(W), wc(W);
    vector<vector<pair<int,int>>> stockP(P); // product -> list of (warehouse, count)
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) {
            int cnt;
            cin >> cnt;
            if (cnt > 0) stockP[p].push_back({w, cnt});
        }
    }

    int C;
    cin >> C;
    vector<Order> ord(C);
    vector<ll> totalNeed(P, 0);
    vector<vector<int>> ordersByProduct(P);
    for (int o = 0; o < C; o++) {
        cin >> ord[o].r >> ord[o].c;
        int L;
        cin >> L;
        map<int,int> cnt;
        for (int i = 0; i < L; i++) {
            int p;
            cin >> p;
            cnt[p]++;
        }
        for (auto &kv : cnt) {
            ord[o].need.push_back({kv.first, kv.second});
            totalNeed[kv.first] += kv.second;
            ordersByProduct[kv.first].push_back(o);
        }
    }

    auto getNeed = [&](int o, int p) -> int {
        for (auto &pr : ord[o].need)
            if (pr.first == p) return pr.second;
        return 0;
    };
    auto decNeed = [&](int o, int p, int q) {
        for (auto &pr : ord[o].need)
            if (pr.first == p) { pr.second -= q; return; }
    };

    ll totalRem = 0;
    for (int p = 0; p < P; p++) totalRem += totalNeed[p];

    vector<int> active;
    for (int o = 0; o < C; o++)
        if (!ord[o].need.empty()) active.push_back(o);

    vector<ll> dpr(D, wr[0]), dpc(D, wc[0]), dtm(D, 0);
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
    for (int d = 0; d < D; d++) pq.push({0, d});

    vector<string> cmds;
    cmds.reserve(1 << 16);

    while (!pq.empty() && totalRem > 0) {
        auto [t, d] = pq.top();
        pq.pop();

        // compact active list
        {
            vector<int> na;
            na.reserve(active.size());
            for (int o : active) {
                bool any = false;
                for (auto &pr : ord[o].need) if (pr.second > 0) { any = true; break; }
                if (any) na.push_back(o);
            }
            active.swap(na);
        }
        if (active.empty()) break;

        ll pr = dpr[d], pc = dpc[d];

        // nearest order with remaining demand
        int bo = -1;
        ll bd = LLONG_MAX;
        for (int o : active) {
            ll dd = dist(pr, pc, ord[o].r, ord[o].c);
            if (dd < bd) { bd = dd; bo = o; }
        }
        if (bo < 0) continue;
        int o = bo;

        // best (product, warehouse) for this order
        ll bcost = LLONG_MAX;
        int bp = -1, bidx = -1;
        for (auto &pn : ord[o].need) {
            int p = pn.first, cnt = pn.second;
            if (cnt <= 0) continue;
            for (int idx = 0; idx < (int)stockP[p].size(); idx++) {
                int w = stockP[p][idx].first;
                ll cost = dist(pr, pc, wr[w], wc[w]) + dist(wr[w], wc[w], ord[o].r, ord[o].c);
                if (cost < bcost) {
                    bcost = cost;
                    bp = p;
                    bidx = idx;
                }
            }
        }
        if (bp < 0) continue;

        int w = stockP[bp][bidx].first;
        ll sc = stockP[bp][bidx].second;
        ll maxc = MAXL / weight[bp];
        ll k = min({sc, maxc, totalNeed[bp]});
        if (k <= 0) continue;

        ll time = t + dist(pr, pc, wr[w], wc[w]);
        ll ldone = time + 1;
        if (ldone > T) continue;

        cmds.push_back(to_string(d) + " L " + to_string(w) + " " + to_string(bp) + " " + to_string(k));
        time = ldone;
        stockP[bp][bidx].second -= (int)k;
        if (stockP[bp][bidx].second == 0) {
            stockP[bp][bidx] = stockP[bp].back();
            stockP[bp].pop_back();
        }

        ll carried = k;
        ll cr = wr[w], cc = wc[w];

        while (carried > 0) {
            int best = -1;
            ll bdd = LLONG_MAX;
            for (int o2 : ordersByProduct[bp]) {
                if (getNeed(o2, bp) <= 0) continue;
                ll dd = dist(cr, cc, ord[o2].r, ord[o2].c);
                if (dd < bdd) { bdd = dd; best = o2; }
            }
            if (best < 0) break;
            if (time + bdd + 1 > T) break;
            ll q = min(carried, (ll)getNeed(best, bp));
            cmds.push_back(to_string(d) + " D " + to_string(best) + " " + to_string(bp) + " " + to_string(q));
            time += bdd + 1;
            carried -= q;
            decNeed(best, bp, (int)q);
            totalNeed[bp] -= q;
            totalRem -= q;
            cr = ord[best].r;
            cc = ord[best].c;
        }

        dpr[d] = cr;
        dpc[d] = cc;
        dtm[d] = time;
        if (time < T) pq.push({time, d});
    }

    ostringstream out;
    out << cmds.size() << "\n";
    for (auto &s : cmds) out << s << "\n";
    cout << out.str();
    return 0;
}
