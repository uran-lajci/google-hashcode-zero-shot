#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// ceil of Euclidean distance between two cells (exact integer arithmetic)
ll cdist(int r1, int c1, int r2, int c2) {
    ll dr = r1 - r2, dc = c1 - c2;
    ll s = dr * dr + dc * dc;
    ll r = (ll)sqrt((double)s);
    while (r * r < s) r++;
    while (r > 0 && (r - 1) * (r - 1) >= s) r--;
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, cap;
    cin >> R >> Cc >> D >> T >> cap;
    int P;
    cin >> P;
    vector<int> weight(P);
    for (int i = 0; i < P; i++) cin >> weight[i];

    int W;
    cin >> W;
    vector<int> wr(W), wc(W);
    vector<int> stock((size_t)W * P, 0);
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) cin >> stock[(size_t)w * P + p];
    }
    // warehouses holding each product type (lazily cleaned)
    vector<vector<int>> whs(P);
    for (int w = 0; w < W; w++)
        for (int p = 0; p < P; p++)
            if (stock[(size_t)w * P + p] > 0) whs[p].push_back(w);

    int C;
    cin >> C;
    vector<int> orr(C), occ(C);
    vector<vector<pair<int, int>>> need(C);
    vector<ll> remTotal(C, 0);
    for (int o = 0; o < C; o++) {
        cin >> orr[o] >> occ[o];
        int L;
        cin >> L;
        vector<int> items(L);
        for (int i = 0; i < L; i++) cin >> items[i];
        sort(items.begin(), items.end());
        for (int i = 0; i < L;) {
            int j = i;
            while (j < L && items[j] == items[i]) j++;
            need[o].push_back({items[i], j - i});
            remTotal[o] += j - i;
            i = j;
        }
    }

    // drone state
    vector<ll> dtime(D, 0);
    vector<int> dr(D, wr.empty() ? 0 : wr[0]), dc(D, wc.empty() ? 0 : wc[0]);
    vector<vector<string>> cmds(D);

    auto cleanWhs = [&](int p) {
        auto& L = whs[p];
        for (size_t i = 0; i < L.size();) {
            if (stock[(size_t)L[i] * P + p] == 0) {
                L[i] = L.back();
                L.pop_back();
            } else {
                i++;
            }
        }
    };

    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int d = 0; d < D; d++) pq.push({0, d});

    int ptr = 0;
    while (!pq.empty()) {
        while (ptr < C && remTotal[ptr] == 0) ptr++;
        if (ptr >= C) break;
        int o = ptr;

        auto [t, d] = pq.top();
        pq.pop();
        int pr = dr[d], pc = dc[d];

        // choose the product type to fetch first
        int pType = -1;
        for (auto& [type, cnt] : need[o]) {
            if (cnt <= 0) continue;
            cleanWhs(type);
            if (!whs[type].empty()) { pType = type; break; }
        }
        if (pType < 0) continue; // should not happen

        // choose best warehouse for pType
        int bestW = -1;
        ll bestCost = LLONG_MAX;
        for (int w : whs[pType]) {
            ll cost = cdist(pr, pc, wr[w], wc[w]) + cdist(wr[w], wc[w], orr[o], occ[o]);
            if (cost < bestCost) { bestCost = cost; bestW = w; }
        }
        int w = bestW;

        // plan loads at warehouse w for order o
        vector<pair<int, int>> loads; // (type, count)
        {
            int rem = cap;
            // pType first
            vector<int> order;
            order.push_back(pType);
            for (auto& [type, cnt] : need[o])
                if (type != pType) order.push_back(type);
            for (int type : order) {
                int cnt = 0;
                for (auto& [tt, cc] : need[o]) if (tt == type) { cnt = cc; break; }
                if (cnt <= 0) continue;
                int st = stock[(size_t)w * P + type];
                if (st <= 0) continue;
                int fit = rem / weight[type];
                int k = min(cnt, min(st, fit));
                if (k > 0) {
                    loads.push_back({type, k});
                    rem -= k * weight[type];
                }
            }
        }
        if (loads.empty()) continue;

        ll d1 = cdist(pr, pc, wr[w], wc[w]);
        ll d2 = cdist(wr[w], wc[w], orr[o], occ[o]);
        ll total = d1 + d2 + 2LL * (ll)loads.size();
        if (t + total > T) {
            // this drone cannot complete this trip before the deadline; retire it
            continue;
        }

        // commit
        string wstr = to_string(w), ostr = to_string(o), dstr = to_string(d);
        for (auto& [type, k] : loads) {
            cmds[d].push_back(dstr + " L " + wstr + " " + to_string(type) + " " + to_string(k));
            stock[(size_t)w * P + type] -= k;
        }
        for (auto& [type, k] : loads) {
            cmds[d].push_back(dstr + " D " + ostr + " " + to_string(type) + " " + to_string(k));
            for (auto& [tt, cc] : need[o]) {
                if (tt == type) { cc -= k; break; }
            }
            remTotal[o] -= k;
        }
        dr[d] = orr[o];
        dc[d] = occ[o];
        dtime[d] = t + total;
        pq.push({dtime[d], d});
    }

    size_t Q = 0;
    for (int d = 0; d < D; d++) Q += cmds[d].size();
    string out;
    out.reserve(Q * 20 + 16);
    out += to_string(Q);
    out += '\n';
    for (int d = 0; d < D; d++)
        for (auto& s : cmds[d]) {
            out += s;
            out += '\n';
        }
    cout << out;
    return 0;
}
