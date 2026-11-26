#include <bits/stdc++.h>
using namespace std;

struct Entry {
    int o, p, rem;
};

struct Drone {
    int r, c;
    long long t;
    bool active;
};

static long long ceilDist(int r1, int c1, int r2, int c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    long long s = dr * dr + dc * dc;
    long long k = (long long)sqrtl((long double)s);
    while (k * k < s) k++;
    while (k > 0 && (k - 1) * (k - 1) >= s) k--;
    return k;
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
    vector<vector<int>> stock(W, vector<int>(P, 0));
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) cin >> stock[w][p];
    }

    vector<vector<int>> wh(P);
    for (int p = 0; p < P; p++)
        for (int w = 0; w < W; w++)
            if (stock[w][p] > 0) wh[p].push_back(w);

    int C;
    cin >> C;
    vector<int> orr(C), orc(C);
    vector<Entry> entries;
    for (int o = 0; o < C; o++) {
        cin >> orr[o] >> orc[o];
        int L;
        cin >> L;
        vector<int> types(L);
        for (int i = 0; i < L; i++) cin >> types[i];
        sort(types.begin(), types.end());
        for (int i = 0; i < L;) {
            int j = i;
            while (j < L && types[j] == types[i]) j++;
            entries.push_back({o, types[i], j - i});
            i = j;
        }
    }

    vector<Drone> drones(D);
    for (int d = 0; d < D; d++) {
        drones[d] = {wr[0], wc[0], 0, true};
    }

    vector<vector<string>> cmds(D);
    const size_t K = 50;

    while (true) {
        int bd = -1;
        for (int d = 0; d < D; d++) {
            if (!drones[d].active) continue;
            if (bd == -1 || drones[d].t < drones[bd].t) bd = d;
        }
        if (bd == -1) break;

        Drone &dr = drones[bd];

        // Remove finished entries
        entries.erase(remove_if(entries.begin(), entries.end(),
                                [](const Entry &e) { return e.rem <= 0; }),
                      entries.end());

        long long bestCost = LLONG_MAX;
        int bestE = -1, bestW = -1;
        size_t lim = min(K, entries.size());
        for (size_t ei = 0; ei < lim; ei++) {
            const Entry &e = entries[ei];
            for (int w : wh[e.p]) {
                if (stock[w][e.p] <= 0) continue;
                long long d1 = ceilDist(dr.r, dr.c, wr[w], wc[w]);
                long long d2 = ceilDist(wr[w], wc[w], orr[e.o], orc[e.o]);
                long long total = d1 + 1 + d2 + 1;
                if (dr.t + total > T) continue;
                if (total < bestCost) {
                    bestCost = total;
                    bestE = (int)ei;
                    bestW = w;
                }
            }
        }

        if (bestE == -1) {
            dr.active = false;
            continue;
        }

        Entry &e = entries[bestE];
        int cap = M / wt[e.p];
        int n = min(e.rem, min(stock[bestW][e.p], cap));
        // n >= 1 because weight <= M and stock > 0 and rem > 0

        int did = bd;
        cmds[did].push_back(to_string(did) + " L " + to_string(bestW) + " " +
                            to_string(e.p) + " " + to_string(n));
        cmds[did].push_back(to_string(did) + " D " + to_string(e.o) + " " +
                            to_string(e.p) + " " + to_string(n));

        stock[bestW][e.p] -= n;
        e.rem -= n;
        dr.t += bestCost;
        dr.r = orr[e.o];
        dr.c = orc[e.o];
    }

    size_t total = 0;
    for (int d = 0; d < D; d++) total += cmds[d].size();

    string out;
    out.reserve(total * 16 + 16);
    out += to_string(total);
    out += '\n';
    for (int d = 0; d < D; d++) {
        for (const string &s : cmds[d]) {
            out += s;
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
