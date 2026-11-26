#include <bits/stdc++.h>
using namespace std;

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

    // Free segments per row: (start, length)
    vector<vector<pair<int, int>>> segs(R);
    vector<int> maxseg(R, 0), freeCnt(R, 0);
    for (int r = 0; r < R; r++) {
        int s = 0;
        while (s < S) {
            if (blocked[r][s]) { s++; continue; }
            int st = s;
            while (s < S && !blocked[r][s]) s++;
            segs[r].push_back({st, s - st});
        }
        for (auto &sg : segs[r]) {
            maxseg[r] = max(maxseg[r], sg.second);
            freeCnt[r] += sg.second;
        }
    }

    vector<int> z(M), c(M);
    for (int i = 0; i < M; i++) cin >> z[i] >> c[i];

    vector<int> order(M);
    for (int i = 0; i < M; i++) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        if (c[a] != c[b]) return c[a] > c[b];
        return a < b;
    });

    vector<long long> load((size_t)P * R, 0), total(P, 0);
    // min-heap of (total, pool)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> heap;
    for (int p = 0; p < P; p++) heap.push({0, p});

    vector<int> ar(M, -1), as(M, -1), ap(M, -1);

    for (int idx : order) {
        int zz = z[idx], cc = c[idx];
        int p = heap.top().second;

        int best = -1;
        for (int r = 0; r < R; r++) {
            if (maxseg[r] < zz) continue;
            if (best == -1) { best = r; continue; }
            long long lr = load[(size_t)p * R + r];
            long long lb = load[(size_t)p * R + best];
            if (lr < lb || (lr == lb && freeCnt[r] > freeCnt[best]))
                best = r;
        }
        if (best == -1) continue; // unallocated

        heap.pop();
        total[p] += cc;
        heap.push({total[p], p});

        // best-fit segment in row best
        auto &sv = segs[best];
        int bi = -1;
        for (int k = 0; k < (int)sv.size(); k++) {
            if (sv[k].second >= zz) {
                if (bi == -1 || sv[k].second < sv[bi].second) bi = k;
            }
        }
        int start = sv[bi].first;
        int len = sv[bi].second;
        if (len == zz) {
            sv.erase(sv.begin() + bi);
        } else {
            sv[bi] = {start + zz, len - zz};
        }
        freeCnt[best] -= zz;
        int mx = 0;
        for (auto &sg : sv) mx = max(mx, sg.second);
        maxseg[best] = mx;

        load[(size_t)p * R + best] += cc;
        ar[idx] = best;
        as[idx] = start;
        ap[idx] = p;
    }

    string out;
    out.reserve((size_t)M * 12);
    for (int i = 0; i < M; i++) {
        if (ar[i] < 0) {
            out += "x\n";
        } else {
            out += to_string(ar[i]);
            out += ' ';
            out += to_string(as[i]);
            out += ' ';
            out += to_string(ap[i]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
