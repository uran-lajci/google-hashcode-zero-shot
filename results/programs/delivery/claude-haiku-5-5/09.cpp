#include <bits/stdc++.h>
using namespace std;

static long long sqDist(int r1, int c1, int r2, int c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    return dr * dr + dc * dc;
}

// ceil of Euclidean distance (number of turns for a flight)
static long long ceilSqrt(long long x) {
    long long s = (long long)sqrtl((long double)x);
    while (s * s > x) s--;
    while (s * s < x) s++;
    return s;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, ML;
    cin >> R >> Cc >> D >> T >> ML;

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
    vector<int> orr(C), occ(C);
    vector<map<int, int>> needs(C);
    vector<vector<int>> byProd(P);
    for (int o = 0; o < C; o++) {
        cin >> orr[o] >> occ[o];
        int L;
        cin >> L;
        for (int i = 0; i < L; i++) {
            int p;
            cin >> p;
            needs[o][p]++;
        }
        for (auto &kv : needs[o]) byProd[kv.first].push_back(o);
    }

    // warehouse lists per product (warehouses with stock > 0)
    vector<vector<int>> whList(P);
    for (int p = 0; p < P; p++)
        for (int w = 0; w < W; w++)
            if (stock[w][p] > 0) whList[p].push_back(w);

    vector<long long> dtime(D, 0);
    vector<int> dpr(D, W > 0 ? wr[0] : 0), dpc(D, W > 0 ? wc[0] : 0);
    vector<bool> active(D, true);

    vector<string> out;
    out.reserve(1 << 16);

    auto ceilDist = [&](int r1, int c1, int r2, int c2) -> long long {
        return ceilSqrt(sqDist(r1, c1, r2, c2));
    };

    int ptr = 0;
    while (true) {
        int best = -1;
        for (int d = 0; d < D; d++) {
            if (!active[d] || dtime[d] >= T) continue;
            if (best < 0 || dtime[d] < dtime[best]) best = d;
        }
        if (best < 0) break;

        while (ptr < C && needs[ptr].empty()) ptr++;
        if (ptr >= C) break;
        int o = ptr;
        int p = needs[o].begin()->first;

        // choose warehouse with stock of p minimizing total travel
        vector<int> nl;
        int bw = -1;
        long long bc = LLONG_MAX;
        int pr = dpr[best], pc = dpc[best];
        for (int w : whList[p]) {
            if (stock[w][p] <= 0) continue;
            nl.push_back(w);
            long long cost = ceilDist(pr, pc, wr[w], wc[w]) + ceilDist(wr[w], wc[w], orr[o], occ[o]);
            if (cost < bc) {
                bc = cost;
                bw = w;
            }
        }
        whList[p].swap(nl);
        if (bw < 0) {
            // should not happen given guarantees
            break;
        }

        long long cap = ML / wt[p];
        if (cap <= 0) {
            active[best] = false;
            continue;
        }

        long long t1 = dtime[best] + ceilDist(pr, pc, wr[bw], wc[bw]) + 1;
        if (t1 + ceilDist(wr[bw], wc[bw], orr[o], occ[o]) + 1 > T) {
            active[best] = false;
            continue;
        }

        long long k = min<long long>(stock[bw][p], cap);
        vector<pair<int, int>> deliveries; // (order, amount)
        int cr = wr[bw], cc = wc[bw];
        long long tm = t1;
        long long rem = k;

        // first delivery to o
        {
            int amt = (int)min<long long>(needs[o][p], rem);
            long long dd = ceilDist(cr, cc, orr[o], occ[o]);
            tm += dd + 1;
            deliveries.push_back({o, amt});
            cr = orr[o];
            cc = occ[o];
            rem -= amt;
        }

        // subsequent nearest deliveries of the same product
        while (rem > 0) {
            vector<int> keep;
            int bo = -1;
            long long bsq = LLONG_MAX;
            for (int o2 : byProd[p]) {
                auto it = needs[o2].find(p);
                if (it == needs[o2].end() || it->second <= 0) continue;
                keep.push_back(o2);
                long long sq = sqDist(cr, cc, orr[o2], occ[o2]);
                if (sq < bsq) {
                    bsq = sq;
                    bo = o2;
                }
            }
            byProd[p].swap(keep);
            if (bo < 0) break;
            long long dd = ceilSqrt(bsq);
            if (tm + dd + 1 > T) break;
            int amt = (int)min<long long>(needs[bo][p], rem);
            tm += dd + 1;
            deliveries.push_back({bo, amt});
            cr = orr[bo];
            cc = occ[bo];
            rem -= amt;
        }

        long long loaded = 0;
        for (auto &dl : deliveries) loaded += dl.second;

        // emit commands
        out.push_back(to_string(best) + " L " + to_string(bw) + " " + to_string(p) + " " + to_string(loaded));
        for (auto &dl : deliveries) {
            out.push_back(to_string(best) + " D " + to_string(dl.first) + " " + to_string(p) + " " + to_string(dl.second));
        }

        // update state
        stock[bw][p] -= (int)loaded;
        for (auto &dl : deliveries) {
            auto it = needs[dl.first].find(p);
            it->second -= dl.second;
            if (it->second <= 0) needs[dl.first].erase(it);
        }

        dtime[best] = tm;
        dpr[best] = cr;
        dpc[best] = cc;
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
