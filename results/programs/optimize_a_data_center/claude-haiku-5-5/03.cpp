#include <bits/stdc++.h>
using namespace std;

int R, S, U, P, M;
vector<int> Z, C;
vector<vector<char>> base;

// Scans a row for free gaps. Returns start of the smallest gap with length >= z
// (leftmost among ties), or -1 if none. maxg receives the largest gap length.
int gapScan(const vector<char>& row, int z, int& maxg) {
    maxg = 0;
    int bestSt = -1, bestLen = INT_MAX;
    int n = (int)row.size();
    int j = 0;
    while (j < n) {
        if (row[j]) { j++; continue; }
        int st = j;
        while (j < n && !row[j]) j++;
        int len = j - st;
        maxg = max(maxg, len);
        if (len >= z && len < bestLen) {
            bestLen = len;
            bestSt = st;
        }
    }
    return bestSt;
}

struct Res {
    long long score;
    vector<int> ar, as, ap;
};

Res solveWithOrder(const vector<int>& order) {
    vector<vector<char>> occ = base;
    vector<int> maxg(R, 0);
    for (int r = 0; r < R; r++) {
        int dummy;
        gapScan(occ[r], INT_MAX, dummy);
        maxg[r] = dummy;
    }
    vector<long long> L((size_t)P * R, 0), T(P, 0), mx(P, 0), gc(P, 0);
    set<pair<long long, int>> pq;
    for (int p = 0; p < P; p++) pq.insert({0LL, p});

    Res res;
    res.ar.assign(M, -1);
    res.as.assign(M, -1);
    res.ap.assign(M, -1);

    for (int idx : order) {
        int z = Z[idx], c = C[idx];
        int p = pq.begin()->second;
        int bestR = -1;
        long long bestL = LLONG_MAX;
        for (int r = 0; r < R; r++) {
            if (maxg[r] >= z) {
                long long l = L[(size_t)p * R + r];
                if (l < bestL) {
                    bestL = l;
                    bestR = r;
                }
            }
        }
        if (bestR < 0) continue;  // no row can fit this server

        int dummy;
        int st = gapScan(occ[bestR], z, dummy);
        if (st < 0) continue;  // should not happen
        for (int k = st; k < st + z; k++) occ[bestR][k] = 1;
        {
            int m;
            gapScan(occ[bestR], INT_MAX, m);
            maxg[bestR] = m;
        }

        pq.erase({gc[p], p});
        L[(size_t)p * R + bestR] += c;
        T[p] += c;
        mx[p] = max(mx[p], L[(size_t)p * R + bestR]);
        gc[p] = T[p] - mx[p];
        pq.insert({gc[p], p});

        res.ar[idx] = bestR;
        res.as[idx] = st;
        res.ap[idx] = p;
    }
    res.score = pq.begin()->first;
    return res;
}

int main() {
    if (scanf("%d %d %d %d %d", &R, &S, &U, &P, &M) != 5) return 0;
    base.assign(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        scanf("%d %d", &r, &s);
        base[r][s] = 1;
    }
    Z.resize(M);
    C.resize(M);
    for (int i = 0; i < M; i++) {
        scanf("%d %d", &Z[i], &C[i]);
    }

    vector<vector<int>> orders;

    // Order 1: capacity descending, then size ascending, then index.
    {
        vector<int> idx(M);
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&](int a, int b) {
            if (C[a] != C[b]) return C[a] > C[b];
            if (Z[a] != Z[b]) return Z[a] < Z[b];
            return a < b;
        });
        orders.push_back(idx);
    }
    // Order 2: capacity per slot descending.
    {
        vector<int> idx(M);
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&](int a, int b) {
            long long lhs = (long long)C[a] * Z[b];
            long long rhs = (long long)C[b] * Z[a];
            if (lhs != rhs) return lhs > rhs;
            return a < b;
        });
        orders.push_back(idx);
    }
    // Order 3: size descending, then capacity descending.
    {
        vector<int> idx(M);
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&](int a, int b) {
            if (Z[a] != Z[b]) return Z[a] > Z[b];
            if (C[a] != C[b]) return C[a] > C[b];
            return a < b;
        });
        orders.push_back(idx);
    }

    Res best;
    bool have = false;
    for (auto& ord : orders) {
        Res r = solveWithOrder(ord);
        if (!have || r.score > best.score) {
            best = move(r);
            have = true;
        }
    }

    string out;
    out.reserve((size_t)M * 16);
    for (int i = 0; i < M; i++) {
        if (best.ar[i] < 0) {
            out += "x\n";
        } else {
            out += to_string(best.ar[i]);
            out += ' ';
            out += to_string(best.as[i]);
            out += ' ';
            out += to_string(best.ap[i]);
            out += '\n';
        }
    }
    fputs(out.c_str(), stdout);
    return 0;
}
