#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// Smallest integer s >= Euclidean distance between two cells.
ll ceilDist(int r1, int c1, int r2, int c2) {
    ll dr = r1 - r2, dc = c1 - c2;
    ll d2 = dr * dr + dc * dc;
    ll s = (ll)sqrtl((long double)d2);
    while (s * s < d2) s++;
    while (s > 0 && (s - 1) * (s - 1) >= d2) s--;
    return s;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, M;
    cin >> R >> Cc >> D >> T >> M;

    int P;
    cin >> P;
    vector<int> wt(P);
    for (int i = 0; i < P; i++) cin >> wt[i];

    int W;
    cin >> W;
    vector<int> wr(W), wc(W);
    vector<unordered_map<int, int>> st(W);
    vector<vector<int>> whList(P);
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) {
            int x;
            cin >> x;
            if (x > 0) {
                st[w][p] = x;
                whList[p].push_back(w);
            }
        }
    }

    int C;
    cin >> C;
    vector<int> orow(C), ocol(C);
    vector<vector<pair<int, int>>> dem(C);  // (product, remaining)
    for (int o = 0; o < C; o++) {
        cin >> orow[o] >> ocol[o];
        int L;
        cin >> L;
        map<int, int> cnt;
        for (int i = 0; i < L; i++) {
            int p;
            cin >> p;
            cnt[p]++;
        }
        for (auto &kv : cnt) dem[o].push_back({kv.first, kv.second});
    }

    // Entries (order, index into dem[order]) that still have remaining demand.
    vector<pair<int, int>> ent;
    for (int o = 0; o < C; o++)
        for (int i = 0; i < (int)dem[o].size(); i++) ent.push_back({o, i});

    vector<int> lr(D), lc(D);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int d = 0; d < D; d++) {
        lr[d] = wr[0];
        lc[d] = wc[0];
        pq.push({0LL, d});
    }

    vector<string> out;
    out.reserve(1 << 16);

    while (!pq.empty() && !ent.empty()) {
        auto top = pq.top();
        pq.pop();
        ll t = top.first;
        int d = top.second;

        ll bestCost = LLONG_MAX;
        int bo = -1, bIdx = -1, bw = -1;
        int checked = 0;

        for (size_t i = 0; i < ent.size() && checked < 40;) {
            int o = ent[i].first, idx = ent[i].second;
            int p = dem[o][idx].first, rem = dem[o][idx].second;
            if (rem == 0) {
                ent[i] = ent.back();
                ent.pop_back();
                continue;
            }
            checked++;
            i++;
            auto &lst = whList[p];
            for (size_t j = 0; j < lst.size();) {
                int w = lst[j];
                auto it = st[w].find(p);
                if (it == st[w].end() || it->second == 0) {
                    lst[j] = lst.back();
                    lst.pop_back();
                    continue;
                }
                ll cost = ceilDist(lr[d], lc[d], wr[w], wc[w]) +
                          ceilDist(wr[w], wc[w], orow[o], ocol[o]);
                if (t + cost + 2 <= T && cost < bestCost) {
                    bestCost = cost;
                    bo = o;
                    bIdx = idx;
                    bw = w;
                }
                j++;
            }
        }

        if (bo < 0) continue;  // this drone cannot complete anything more

        int w = bw, o = bo;
        ll dw = ceilDist(lr[d], lc[d], wr[w], wc[w]);
        ll dwo = ceilDist(wr[w], wc[w], orow[o], ocol[o]);

        // items: (product, count, index in dem[o])
        vector<array<int, 3>> items;
        int capLeft = M;
        {
            int p0 = dem[o][bIdx].first;
            int rem0 = dem[o][bIdx].second;
            int s0 = st[w][p0];
            int n0 = min(rem0, min(s0, M / wt[p0]));
            if (n0 > 0) {
                items.push_back({p0, n0, bIdx});
                capLeft -= n0 * wt[p0];
            }
        }
        for (int idx2 = 0; idx2 < (int)dem[o].size(); idx2++) {
            if (idx2 == bIdx) continue;
            int q = dem[o][idx2].first;
            int rem = dem[o][idx2].second;
            if (rem <= 0) continue;
            auto it = st[w].find(q);
            if (it == st[w].end() || it->second <= 0) continue;
            int n = min(rem, min(it->second, capLeft / wt[q]));
            if (n > 0) {
                items.push_back({q, n, idx2});
                capLeft -= n * wt[q];
            }
        }
        if (items.empty()) continue;

        // Trim to fit the deadline.
        while (items.size() > 1 &&
               t + dw + dwo + 2LL * (ll)items.size() > T)
            items.pop_back();
        ll dur = dw + dwo + 2LL * (ll)items.size();
        if (t + dur > T) continue;

        // Emit commands and update state.
        for (auto &it : items) {
            int q = it[0], n = it[1], idx = it[2];
            st[w][q] -= n;
            dem[o][idx].second -= n;
            out.push_back(to_string(d) + " L " + to_string(w) + " " +
                          to_string(q) + " " + to_string(n));
        }
        for (auto &it : items) {
            int q = it[0], n = it[1];
            out.push_back(to_string(d) + " D " + to_string(o) + " " +
                          to_string(q) + " " + to_string(n));
        }

        t += dur;
        lr[d] = orow[o];
        lc[d] = ocol[o];
        pq.push({t, d});
    }

    string result;
    result.reserve(out.size() * 24 + 16);
    result += to_string(out.size());
    result += '\n';
    for (auto &s : out) {
        result += s;
        result += '\n';
    }
    cout << result;
    return 0;
}
