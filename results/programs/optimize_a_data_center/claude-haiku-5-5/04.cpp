#include <bits/stdc++.h>
using namespace std;

int R, S, U, P, M;
vector<vector<char>> occ;
vector<int> rowMax;

int maxRun(int r) {
    int best = 0, cur = 0;
    for (int s = 0; s < S; ++s) {
        if (!occ[r][s]) { ++cur; if (cur > best) best = cur; }
        else cur = 0;
    }
    return best;
}

// leftmost start of a free run of length z in row r, or -1
int findFit(int r, int z) {
    int cur = 0;
    for (int s = 0; s < S; ++s) {
        if (!occ[r][s]) {
            ++cur;
            if (cur == z) return s - z + 1;
        } else cur = 0;
    }
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> R >> S >> U >> P >> M;
    occ.assign(R, vector<char>(S, 0));
    for (int i = 0; i < U; ++i) {
        int r, s;
        cin >> r >> s;
        occ[r][s] = 1;
    }
    rowMax.assign(R, 0);
    for (int r = 0; r < R; ++r) rowMax[r] = maxRun(r);

    vector<int> z(M), c(M);
    for (int i = 0; i < M; ++i) cin >> z[i] >> c[i];

    vector<int> order(M);
    for (int i = 0; i < M; ++i) order[i] = i;
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (c[a] != c[b]) return c[a] > c[b];
        return a < b;
    });

    vector<long long> poolTot(P, 0);
    vector<vector<long long>> poolRow(P, vector<long long>(R, 0));
    vector<int> ansR(M, -1), ansS(M, -1), ansP(M, -1);
    int failSize = INT_MAX;

    for (int idx : order) {
        int zz = z[idx];
        if (zz >= failSize) continue;

        // pool with smallest total capacity so far
        int p = 0;
        for (int q = 1; q < P; ++q)
            if (poolTot[q] < poolTot[p]) p = q;

        // row with fitting space where this pool has least capacity
        int best = -1;
        for (int r = 0; r < R; ++r) {
            if (rowMax[r] >= zz) {
                if (best == -1 || poolRow[p][r] < poolRow[p][best]) best = r;
            }
        }
        if (best == -1) {
            failSize = min(failSize, zz);
            continue;
        }
        int st = findFit(best, zz);
        if (st < 0) { // should not happen
            failSize = min(failSize, zz);
            continue;
        }
        for (int s = st; s < st + zz; ++s) occ[best][s] = 1;
        rowMax[best] = maxRun(best);

        poolTot[p] += c[idx];
        poolRow[p][best] += c[idx];
        ansR[idx] = best;
        ansS[idx] = st;
        ansP[idx] = p;
    }

    string out;
    out.reserve((size_t)M * 12);
    for (int i = 0; i < M; ++i) {
        if (ansR[i] < 0) {
            out += "x\n";
        } else {
            out += to_string(ansR[i]);
            out += ' ';
            out += to_string(ansS[i]);
            out += ' ';
            out += to_string(ansP[i]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
