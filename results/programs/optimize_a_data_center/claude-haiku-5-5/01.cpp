#include <bits/stdc++.h>
using namespace std;

int R, S, U, P, M;
vector<int> Z, C;
vector<vector<pair<int,int>>> baseSegs; // per row: (start, len) free runs
vector<int> baseRowMax, baseRowFree;

struct Result {
    long long score = -1;
    vector<int> ar, as, ap;
};

// Build allocation given pool assignment and placement order.
Result runOnce(const vector<int>& poolOf, const vector<int>& order) {
    vector<vector<pair<int,int>>> segs = baseSegs;
    vector<int> rowMax = baseRowMax;
    vector<int> rowFree = baseRowFree;
    vector<long long> load((size_t)P * R, 0);
    Result res;
    res.ar.assign(M, -1);
    res.as.assign(M, -1);
    res.ap.assign(M, -1);

    for (int idx : order) {
        int z = Z[idx];
        int p = poolOf[idx];
        int best = -1;
        for (int r = 0; r < R; r++) {
            if (rowMax[r] >= z) {
                if (best < 0) best = r;
                else {
                    long long a = load[(size_t)p * R + r];
                    long long b = load[(size_t)p * R + best];
                    if (a < b || (a == b && rowFree[r] > rowFree[best])) best = r;
                }
            }
        }
        if (best < 0) continue;
        auto& sg = segs[best];
        int start = -1;
        for (auto& seg : sg) {
            if (seg.second >= z) {
                start = seg.first;
                seg.first += z;
                seg.second -= z;
                break;
            }
        }
        if (start < 0) continue; // should not happen
        int mx = 0;
        for (auto& seg : sg) mx = max(mx, seg.second);
        rowMax[best] = mx;
        rowFree[best] -= z;
        load[(size_t)p * R + best] += C[idx];
        res.ar[idx] = best;
        res.as[idx] = start;
        res.ap[idx] = p;
    }

    long long score = LLONG_MAX;
    for (int p = 0; p < P; p++) {
        long long total = 0, mx = 0;
        for (int r = 0; r < R; r++) {
            long long v = load[(size_t)p * R + r];
            total += v;
            mx = max(mx, v);
        }
        score = min(score, total - mx);
    }
    res.score = score;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startTime = chrono::steady_clock::now();

    cin >> R >> S >> U >> P >> M;
    vector<vector<char>> blocked(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blocked[r][s] = 1;
    }
    Z.assign(M, 0);
    C.assign(M, 0);
    for (int i = 0; i < M; i++) cin >> Z[i] >> C[i];

    baseSegs.assign(R, {});
    baseRowMax.assign(R, 0);
    baseRowFree.assign(R, 0);
    for (int r = 0; r < R; r++) {
        int s = 0;
        while (s < S) {
            if (blocked[r][s]) { s++; continue; }
            int st = s;
            while (s < S && !blocked[r][s]) s++;
            baseSegs[r].push_back({st, s - st});
            baseRowMax[r] = max(baseRowMax[r], s - st);
            baseRowFree[r] += s - st;
        }
    }

    mt19937 rng(12345);
    Result best;
    best.score = -1;
    best.ar.assign(M, -1);
    best.as.assign(M, -1);
    best.ap.assign(M, -1);

    int iter = 0;
    while (true) {
        // Key for pool assignment ordering (noise for variants after the first).
        vector<double> key(M);
        for (int i = 0; i < M; i++) {
            if (iter == 0) key[i] = C[i];
            else {
                int f = 700 + (int)(rng() % 600);
                key[i] = C[i] * (f / 1000.0);
            }
        }
        vector<int> byKey(M);
        iota(byKey.begin(), byKey.end(), 0);
        stable_sort(byKey.begin(), byKey.end(), [&](int a, int b) {
            if (key[a] != key[b]) return key[a] > key[b];
            return a < b;
        });

        // LPT pool assignment: each server goes to the pool with smallest total.
        vector<int> poolOf(M, 0);
        vector<long long> poolTotal(P, 0);
        for (int idx : byKey) {
            int bp = 0;
            for (int p = 1; p < P; p++)
                if (poolTotal[p] < poolTotal[bp]) bp = p;
            poolOf[idx] = bp;
            poolTotal[bp] += C[idx];
        }

        // Placement order: larger servers first.
        vector<int> order(M);
        iota(order.begin(), order.end(), 0);
        stable_sort(order.begin(), order.end(), [&](int a, int b) {
            if (Z[a] != Z[b]) return Z[a] > Z[b];
            return key[a] > key[b];
        });

        Result cur = runOnce(poolOf, order);
        if (cur.score > best.score) best = cur;

        iter++;
        double el = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
        if (el > 6.0 || iter >= 300) break;
    }

    string out;
    out.reserve((size_t)M * 12);
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
    cout << out;
    return 0;
}
