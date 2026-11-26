#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int B, L, D;
    cin >> B >> L >> D;
    vector<long long> S(B);
    for (int i = 0; i < B; i++) cin >> S[i];

    vector<int> N(L), T(L), M(L);
    vector<vector<int>> books(L);
    for (int j = 0; j < L; j++) {
        cin >> N[j] >> T[j] >> M[j];
        books[j].resize(N[j]);
        for (int i = 0; i < N[j]; i++) cin >> books[j][i];
        // sort by score descending, ties by id for determinism
        sort(books[j].begin(), books[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    vector<char> scanned(B, 0);
    vector<vector<int>> chosen(L);
    vector<int> order;

    // Compute the value of library j if its signup starts at day 'start'.
    // If collect is true, also mark and store the chosen books.
    auto evaluate = [&](int j, long long start, bool collect) -> long long {
        long long finish = start + T[j];
        if (finish >= D) return 0;
        long long days = D - finish;
        long long k = min<long long>((long long)M[j] * days, (long long)N[j]);
        long long sum = 0;
        long long cnt = 0;
        for (int b : books[j]) {
            if (cnt >= k) break;
            if (scanned[b]) continue;
            sum += S[b];
            cnt++;
            if (collect) {
                chosen[j].push_back(b);
            }
        }
        if (collect) {
            for (int b : chosen[j]) scanned[b] = 1;
        }
        return sum;
    };

    // Max-heap of (key, library) where key = estimated value / signup time.
    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        long long v = evaluate(j, 0, false);
        if (v > 0) pq.push({(double)v / T[j], j});
    }

    long long t = 0;  // day on which the next signup can start
    vector<char> used(L, 0);

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;
        if (used[j]) continue;
        if (t + T[j] >= D) continue;

        long long v = evaluate(j, t, false);
        if (v <= 0) continue;
        double nk = (double)v / T[j];

        if (pq.empty() || nk >= pq.top().first) {
            // select library j
            evaluate(j, t, true);
            used[j] = 1;
            order.push_back(j);
            t += T[j];
        } else {
            pq.push({nk, j});
        }
    }

    // Output submission
    string out;
    out.reserve(1 << 20);
    out += to_string(order.size());
    out += '\n';
    for (int j : order) {
        out += to_string(j);
        out += ' ';
        out += to_string(chosen[j].size());
        out += '\n';
        for (size_t i = 0; i < chosen[j].size(); i++) {
            if (i) out += ' ';
            out += to_string(chosen[j][i]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
