#include <bits/stdc++.h>
using namespace std;

static long long ceilDist(int r1, int c1, int r2, int c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    long long s = dr * dr + dc * dc;
    long long n = (long long)sqrtl((long double)s);
    while (n * n < s) n++;
    while (n > 0 && (n - 1) * (n - 1) >= s) n--;
    return n;
}

struct Need {
    int c, p;
    int cnt;
};

struct Drone {
    long long t;
    int id;
    int r, c;
};

struct Cmp {
    bool operator()(const Drone& a, const Drone& b) const {
        if (a.t != b.t) return a.t > b.t;
        return a.id > b.id;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, Lmax;
    cin >> R >> Cc >> D >> T >> Lmax;

    int P;
    cin >> P;
    vector<int> wt(P);
    for (int i = 0; i < P; i++) cin >> wt[i];

    int W;
    cin >> W;
    vector<int> wr(W), wc(W);
    vector<int> stock((size_t)W * P, 0);
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) cin >> stock[(size_t)w * P + p];
    }

    // warehouses per product type (lazily compacted)
    vector<vector<int>> whFor(P);
    for (int w = 0; w < W; w++)
        for (int p = 0; p < P; p++)
            if (stock[(size_t)w * P + p] > 0) whFor[p].push_back(w);

    int C;
    cin >> C;
    vector<int> orr(C), occ(C);
    vector<Need> needs;
    vector<int> tmp(P, 0);
    for (int c = 0; c < C; c++) {
        cin >> orr[c] >> occ[c];
        int L;
        cin >> L;
        vector<int> types;
        for (int i = 0; i < L; i++) {
            int p;
            cin >> p;
            if (tmp[p] == 0) types.push_back(p);
            tmp[p]++;
        }
        sort(types.begin(), types.end());
        for (int p : types) {
            needs.push_back({c, p, tmp[p]});
            tmp[p] = 0;
        }
    }

    priority_queue<Drone, vector<Drone>, Cmp> pq;
    int w0r = W > 0 ? wr[0] : 0, w0c = W > 0 ? wc[0] : 0;
    for (int d = 0; d < D; d++) pq.push({0LL, d, w0r, w0c});

    vector<string> lines;
    size_t ptr = 0;
    long long guard = 0;
    const long long GUARD_LIMIT = 30000000LL;

    while (!pq.empty()) {
        if (++guard > GUARD_LIMIT) break;
        Drone dr = pq.top();
        pq.pop();
        while (ptr < needs.size() && needs[ptr].cnt == 0) ptr++;
        if (ptr >= needs.size()) break;

        bool found = false;
        int bestW = -1, bestIdx = -1, bestK = 0;
        long long bestEnd = 0;
        int scanned = 0;
        for (size_t idx = ptr; idx < needs.size() && scanned < 200 && !found; ++idx) {
            if (needs[idx].cnt == 0) continue;
            scanned++;
            int p = needs[idx].p;
            int c = needs[idx].c;
            // compact warehouses list and pick best
            auto& lst = whFor[p];
            size_t out = 0;
            long long bestCost = LLONG_MAX;
            int bw = -1;
            for (size_t i = 0; i < lst.size(); i++) {
                int w = lst[i];
                if (stock[(size_t)w * P + p] <= 0) continue;
                lst[out++] = w;
                long long cost = ceilDist(dr.r, dr.c, wr[w], wc[w]) +
                                 ceilDist(wr[w], wc[w], orr[c], occ[c]);
                if (cost < bestCost || (cost == bestCost && w < bw)) {
                    bestCost = cost;
                    bw = w;
                }
            }
            lst.resize(out);
            if (bw < 0) continue;
            int maxPer = Lmax / wt[p];
            if (maxPer <= 0) continue;
            int k = min(needs[idx].cnt, stock[(size_t)bw * P + p]);
            k = min(k, maxPer);
            if (k <= 0) continue;
            long long d1 = ceilDist(dr.r, dr.c, wr[bw], wc[bw]);
            long long d2 = ceilDist(wr[bw], wc[bw], orr[c], occ[c]);
            long long end = dr.t + d1 + 1 + d2 + 1;
            if (end > T) continue;
            found = true;
            bestW = bw;
            bestIdx = (int)idx;
            bestK = k;
            bestEnd = end;
        }

        if (!found) continue; // drone retires

        Need& e = needs[bestIdx];
        int p = e.p, c = e.c;
        e.cnt -= bestK;
        stock[(size_t)bestW * P + p] -= bestK;
        lines.push_back(to_string(dr.id) + " L " + to_string(bestW) + " " +
                        to_string(p) + " " + to_string(bestK));
        lines.push_back(to_string(dr.id) + " D " + to_string(c) + " " +
                        to_string(p) + " " + to_string(bestK));
        dr.t = bestEnd;
        dr.r = orr[c];
        dr.c = occ[c];
        pq.push(dr);
    }

    string out;
    out.reserve(lines.size() * 16 + 16);
    out += to_string(lines.size());
    out += '\n';
    for (auto& s : lines) {
        out += s;
        out += '\n';
    }
    cout << out;
    return 0;
}
