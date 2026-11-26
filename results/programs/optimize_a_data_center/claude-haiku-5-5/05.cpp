#include <bits/stdc++.h>
using namespace std;

int R, S, U, P, M;
vector<int> Z, C;

struct Result {
    long long score = -1;
    vector<int> ar, as, ap;
};

Result runGreedy(const vector<int>& order,
                 const vector<vector<pair<int,int>>>& initRuns) {
    vector<vector<pair<int,int>>> runs = initRuns;
    vector<int> maxlen(R, 0);
    for (int r = 0; r < R; r++) {
        for (auto& pr : runs[r]) maxlen[r] = max(maxlen[r], pr.second);
    }

    vector<long long> load((size_t)P * R, 0);
    vector<long long> tot(P, 0);
    set<pair<long long,int>> pq;
    for (int p = 0; p < P; p++) pq.insert({0LL, p});

    Result res;
    res.ar.assign(M, -1);
    res.as.assign(M, -1);
    res.ap.assign(M, -1);

    for (int idx : order) {
        int z = Z[idx];
        int c = C[idx];
        int attempts = 0;
        bool placed = false;
        for (auto it = pq.begin(); it != pq.end() && attempts < 40 && !placed; ++it, ++attempts) {
            int p = it->second;
            int bestR = -1;
            for (int r = 0; r < R; r++) {
                if (maxlen[r] < z) continue;
                if (bestR < 0 || load[(size_t)p * R + r] < load[(size_t)p * R + bestR] ||
                    (load[(size_t)p * R + r] == load[(size_t)p * R + bestR] && maxlen[r] > maxlen[bestR]))
                    bestR = r;
            }
            if (bestR < 0) continue;
            // first fit within bestR
            auto& rv = runs[bestR];
            int pos = -1;
            for (size_t k = 0; k < rv.size(); k++) {
                if (rv[k].second >= z) {
                    pos = rv[k].first;
                    rv[k].first += z;
                    rv[k].second -= z;
                    if (rv[k].second == 0) rv.erase(rv.begin() + k);
                    break;
                }
            }
            if (pos < 0) continue; // should not happen
            maxlen[bestR] = 0;
            for (auto& pr : rv) maxlen[bestR] = max(maxlen[bestR], pr.second);

            load[(size_t)p * R + bestR] += c;
            pq.erase({tot[p], p});
            tot[p] += c;
            pq.insert({tot[p], p});

            res.ar[idx] = bestR;
            res.as[idx] = pos;
            res.ap[idx] = p;
            placed = true;
        }
    }

    long long score = LLONG_MAX;
    for (int p = 0; p < P; p++) {
        long long mx = 0;
        for (int r = 0; r < R; r++) mx = max(mx, load[(size_t)p * R + r]);
        long long gc = tot[p] - mx;
        score = min(score, gc);
    }
    res.score = score;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> R >> S >> U >> P >> M;
    vector<vector<char>> blocked(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blocked[r][s] = 1;
    }
    Z.resize(M);
    C.resize(M);
    for (int i = 0; i < M; i++) cin >> Z[i] >> C[i];

    vector<vector<pair<int,int>>> initRuns(R);
    for (int r = 0; r < R; r++) {
        int s = 0;
        while (s < S) {
            if (blocked[r][s]) { s++; continue; }
            int st = s;
            while (s < S && !blocked[r][s]) s++;
            initRuns[r].push_back({st, s - st});
        }
    }

    // Ordering 1: by capacity density, descending
    vector<int> order1(M);
    iota(order1.begin(), order1.end(), 0);
    stable_sort(order1.begin(), order1.end(), [&](int a, int b) {
        long long l = (long long)C[a] * Z[b];
        long long rr = (long long)C[b] * Z[a];
        if (l != rr) return l > rr;
        return C[a] > C[b];
    });

    // Ordering 2: by raw capacity, descending
    vector<int> order2(M);
    iota(order2.begin(), order2.end(), 0);
    stable_sort(order2.begin(), order2.end(), [&](int a, int b) {
        if (C[a] != C[b]) return C[a] > C[b];
        return Z[a] < Z[b];
    });

    Result best = runGreedy(order1, initRuns);
    Result alt = runGreedy(order2, initRuns);
    if (alt.score > best.score) best = alt;

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
    cout << out;
    return 0;
}
