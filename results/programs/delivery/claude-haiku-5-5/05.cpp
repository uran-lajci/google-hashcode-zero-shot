#include <bits/stdc++.h>
using namespace std;

struct Group { int w, p, o, cnt; };

static long long ceilDist(long long r1, long long c1, long long r2, long long c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    long long d2 = dr * dr + dc * dc;
    if (d2 == 0) return 0;
    long long n = (long long)sqrtl((long double)d2);
    while (n * n < d2) n++;
    while (n > 0 && (n - 1) * (n - 1) >= d2) n--;
    return n;
}

static int readInt() {
    int c = getchar();
    while (c != '-' && (c < '0' || c > '9')) c = getchar();
    bool neg = false;
    if (c == '-') { neg = true; c = getchar(); }
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = getchar(); }
    return neg ? -x : x;
}

int main() {
    int R = readInt(), Cc = readInt(), D = readInt(), T = readInt(), maxL = readInt();
    int P = readInt();
    vector<int> wt(P);
    for (int i = 0; i < P; i++) wt[i] = readInt();

    int W = readInt();
    vector<int> wr(W), wc(W);
    vector<vector<uint16_t>> stock(W, vector<uint16_t>(P));
    for (int w = 0; w < W; w++) {
        wr[w] = readInt();
        wc[w] = readInt();
        for (int p = 0; p < P; p++) stock[w][p] = (uint16_t)readInt();
    }
    // warehouses with stock per product
    vector<vector<int>> prodWh(P);
    for (int p = 0; p < P; p++)
        for (int w = 0; w < W; w++)
            if (stock[w][p] > 0) prodWh[p].push_back(w);

    int C = readInt();
    vector<int> orr(C), occ(C);
    vector<vector<pair<int,int>>> orderNeeds(C); // (product, count)
    for (int o = 0; o < C; o++) {
        orr[o] = readInt();
        occ[o] = readInt();
        int L = readInt();
        vector<int> items(L);
        for (int i = 0; i < L; i++) items[i] = readInt();
        sort(items.begin(), items.end());
        for (int i = 0; i < L;) {
            int j = i;
            while (j < L && items[j] == items[i]) j++;
            orderNeeds[o].push_back({items[i], j - i});
            i = j;
        }
    }

    // Step 1: assign items to nearest warehouses with stock
    vector<Group> groups;
    for (int o = 0; o < C; o++) {
        for (auto [p, n0] : orderNeeds[o]) {
            int n = n0;
            auto &lst = prodWh[p];
            while (n > 0) {
                // remove exhausted warehouses
                for (size_t i = 0; i < lst.size();) {
                    if (stock[lst[i]][p] == 0) {
                        lst[i] = lst.back();
                        lst.pop_back();
                    } else i++;
                }
                if (lst.empty()) break; // should not happen by guarantee
                int best = -1; long long bd = LLONG_MAX;
                for (int w : lst) {
                    long long d = ceilDist(orr[o], occ[o], wr[w], wc[w]);
                    if (d < bd || (d == bd && w < best)) { bd = d; best = w; }
                }
                int a = min<int>(n, stock[best][p]);
                stock[best][p] -= a;
                n -= a;
                groups.push_back({best, p, o, a});
            }
        }
    }

    // Step 2: schedule drones
    vector<long long> dt(D, 0);
    vector<int> dr(D, wr.empty() ? 0 : wr[0]), dc(D, wc.empty() ? 0 : wc[0]);
    vector<bool> active(D, true);
    vector<vector<string>> cmds(D);

    while (true) {
        int d = -1;
        for (int i = 0; i < D; i++)
            if (active[i] && (d == -1 || dt[i] < dt[d])) d = i;
        if (d == -1 || groups.empty()) break;

        int bestI = -1;
        double bestVal = 1e18;
        long long bestTotal = 0;
        int bestK = 0;
        for (int i = 0; i < (int)groups.size(); i++) {
            const Group &g = groups[i];
            int cap = maxL / wt[g.p];
            int k = min(cap, g.cnt);
            if (k <= 0) continue;
            long long d1 = ceilDist(dr[d], dc[d], wr[g.w], wc[g.w]);
            long long d2 = ceilDist(wr[g.w], wc[g.w], orr[g.o], occ[g.o]);
            long long total = dt[d] + d1 + 1 + d2 + 1;
            if (total > T) continue;
            double val = (double)(d1 + d2 + 2) / k;
            if (val < bestVal) {
                bestVal = val;
                bestI = i;
                bestTotal = total;
                bestK = k;
            }
        }
        if (bestI == -1) {
            active[d] = false;
            continue;
        }
        Group g = groups[bestI];
        cmds[d].push_back(to_string(d) + " L " + to_string(g.w) + " " + to_string(g.p) + " " + to_string(bestK));
        cmds[d].push_back(to_string(d) + " D " + to_string(g.o) + " " + to_string(g.p) + " " + to_string(bestK));
        dt[d] = bestTotal;
        dr[d] = orr[g.o];
        dc[d] = occ[g.o];
        groups[bestI].cnt -= bestK;
        if (groups[bestI].cnt == 0) {
            groups[bestI] = groups.back();
            groups.pop_back();
        }
    }

    long long Q = 0;
    for (int d = 0; d < D; d++) Q += cmds[d].size();
    string out;
    out += to_string(Q) + "\n";
    for (int d = 0; d < D; d++)
        for (auto &s : cmds[d]) { out += s; out += '\n'; }
    fputs(out.c_str(), stdout);
    return 0;
}
