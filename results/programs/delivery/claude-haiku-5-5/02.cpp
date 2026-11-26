#include <bits/stdc++.h>
using namespace std;

static long long ceilDist(long long r1, long long c1, long long r2, long long c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    long long s = dr * dr + dc * dc;
    long long n = (long long)sqrtl((long double)s);
    while (n * n < s) n++;
    while (n > 0 && (n - 1) * (n - 1) >= s) n--;
    return n;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, L;
    cin >> R >> Cc >> D >> T >> L;

    int P;
    cin >> P;
    vector<int> wt(P);
    for (int i = 0; i < P; i++) cin >> wt[i];

    int W;
    cin >> W;
    vector<int> wr(W), wc(W);
    vector<vector<int>> stock(W, vector<int>(P, 0));
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) cin >> stock[w][p];
    }

    int C;
    cin >> C;
    vector<int> or_(C), oc_(C);
    // needs[o] = list of (product, count)
    vector<vector<pair<int, int>>> needs(C);
    for (int o = 0; o < C; o++) {
        cin >> or_[o] >> oc_[o];
        int Li;
        cin >> Li;
        map<int, int> mp;
        for (int i = 0; i < Li; i++) {
            int p;
            cin >> p;
            mp[p]++;
        }
        for (auto& kv : mp) needs[o].push_back(kv);
    }

    // per-product list of warehouses with positive stock
    vector<vector<int>> wlist(P);
    for (int p = 0; p < P; p++)
        for (int w = 0; w < W; w++)
            if (stock[w][p] > 0) wlist[p].push_back(w);

    // pending entries: (order, index into needs[order])
    vector<pair<int, int>> pending;
    for (int o = 0; o < C; o++)
        for (int i = 0; i < (int)needs[o].size(); i++)
            if (needs[o][i].second > 0) pending.push_back({o, i});

    vector<int> dr(D), dc(D);
    vector<long long> dtime(D, 0);
    vector<bool> alive(D, true);
    for (int d = 0; d < D; d++) {
        dr[d] = wr[0];
        dc[d] = wc[0];
    }

    vector<string> lines;
    lines.reserve(1 << 16);

    while (true) {
        int d = -1;
        for (int i = 0; i < D; i++) {
            if (!alive[i]) continue;
            if (d == -1 || dtime[i] < dtime[d]) d = i;
        }
        if (d == -1) break;

        bool found = false;
        for (size_t idx = 0; idx < pending.size() && !found; idx++) {
            int o = pending[idx].first;
            int pi = pending[idx].second;
            int p = needs[o][pi].first;
            int rem = needs[o][pi].second;

            int bestW = -1;
            long long bestDur = LLONG_MAX;
            for (int w : wlist[p]) {
                long long d1 = ceilDist(dr[d], dc[d], wr[w], wc[w]);
                long long d2 = ceilDist(wr[w], wc[w], or_[o], oc_[o]);
                long long dur = d1 + 1 + d2 + 1;
                if (dtime[d] + dur <= T && dur < bestDur) {
                    bestDur = dur;
                    bestW = w;
                }
            }
            if (bestW == -1) continue;

            int w = bestW;
            int maxk = L / wt[p];
            int k = min(stock[w][p], min(rem, maxk));
            if (k <= 0) continue;

            lines.push_back(to_string(d) + " L " + to_string(w) + " " + to_string(p) + " " + to_string(k));
            lines.push_back(to_string(d) + " D " + to_string(o) + " " + to_string(p) + " " + to_string(k));

            stock[w][p] -= k;
            if (stock[w][p] == 0) {
                auto& wl = wlist[p];
                wl.erase(find(wl.begin(), wl.end(), w));
            }
            needs[o][pi].second -= k;
            dtime[d] += bestDur;
            dr[d] = or_[o];
            dc[d] = oc_[o];

            if (needs[o][pi].second == 0) {
                pending[idx] = pending.back();
                pending.pop_back();
            }
            found = true;
        }
        if (!found) alive[d] = false;
    }

    ostringstream out;
    out << lines.size() << "\n";
    for (const auto& s : lines) out << s << "\n";
    cout << out.str();
    return 0;
}
