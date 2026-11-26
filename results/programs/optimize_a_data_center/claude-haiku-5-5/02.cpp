#include <bits/stdc++.h>
using namespace std;

struct Run { int st, len; };

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, S, U, P, M;
    cin >> R >> S >> U >> P >> M;

    vector<vector<char>> blocked(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blocked[r][s] = 1;
    }

    vector<int> z(M), c(M);
    for (int i = 0; i < M; i++) cin >> z[i] >> c[i];

    // Build free runs per row.
    vector<vector<Run>> runs(R);
    vector<int> maxRun(R, 0), freeCnt(R, 0);
    for (int r = 0; r < R; r++) {
        int s = 0;
        while (s < S) {
            if (blocked[r][s]) { s++; continue; }
            int st = s;
            while (s < S && !blocked[r][s]) s++;
            runs[r].push_back({st, s - st});
            maxRun[r] = max(maxRun[r], s - st);
            freeCnt[r] += s - st;
        }
    }

    vector<vector<long long>> load(P, vector<long long>(R, 0));
    vector<long long> poolTot(P, 0);

    vector<int> order(M);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (c[a] != c[b]) return c[a] > c[b];
        if (z[a] != z[b]) return z[a] < z[b];
        return a < b;
    });

    vector<int> outR(M, -1), outS(M, -1), outP(M, -1);

    for (int idx : order) {
        int sz = z[idx];
        // Pool with least total capacity so far.
        int p = 0;
        for (int q = 1; q < P; q++)
            if (poolTot[q] < poolTot[p]) p = q;

        // Choose row where this pool has least load and the server fits.
        int bestR = -1;
        for (int r = 0; r < R; r++) {
            if (maxRun[r] < sz) continue;
            if (bestR == -1 ||
                load[p][r] < load[p][bestR] ||
                (load[p][r] == load[p][bestR] && freeCnt[r] > freeCnt[bestR]))
                bestR = r;
        }
        if (bestR == -1) continue; // stays unallocated

        // Best fit inside the chosen row.
        int bi = -1;
        for (int k = 0; k < (int)runs[bestR].size(); k++) {
            if (runs[bestR][k].len >= sz) {
                if (bi == -1 || runs[bestR][k].len < runs[bestR][bi].len)
                    bi = k;
            }
        }
        int st = runs[bestR][bi].st;
        int len = runs[bestR][bi].len;
        if (len == sz) {
            runs[bestR].erase(runs[bestR].begin() + bi);
        } else {
            runs[bestR][bi].st += sz;
            runs[bestR][bi].len -= sz;
        }

        outR[idx] = bestR;
        outS[idx] = st;
        outP[idx] = p;

        load[p][bestR] += c[idx];
        poolTot[p] += c[idx];
        freeCnt[bestR] -= sz;
        int mx = 0;
        for (const Run& rn : runs[bestR]) mx = max(mx, rn.len);
        maxRun[bestR] = mx;
    }

    string out;
    out.reserve((size_t)M * 16);
    for (int i = 0; i < M; i++) {
        if (outR[i] < 0) {
            out += "x\n";
        } else {
            out += to_string(outR[i]);
            out += ' ';
            out += to_string(outS[i]);
            out += ' ';
            out += to_string(outP[i]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
