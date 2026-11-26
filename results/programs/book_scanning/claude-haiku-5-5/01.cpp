#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int B, L;
    long long D;
    cin >> B >> L >> D;
    vector<long long> S(B);
    for (int i = 0; i < B; i++) cin >> S[i];

    vector<long long> T(L), M(L);
    vector<vector<int>> books(L);
    for (int j = 0; j < L; j++) {
        int n;
        cin >> n >> T[j] >> M[j];
        books[j].resize(n);
        for (int k = 0; k < n; k++) cin >> books[j][k];
        // sort books of this library by score descending
        sort(books[j].begin(), books[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    vector<char> scanned(B, 0);

    // Returns sum of scores of up to capacity unscanned books of library j
    // when signup finishes at time t (scanning days = D - t - T_j).
    // If commit is true, marks the chosen books and appends them to out.
    auto evalLib = [&](int j, long long t, bool commit, vector<int>* out) -> long long {
        long long days = D - t - T[j];
        if (days <= 0) return 0;
        long long cap = M[j] * days;
        long long sum = 0;
        long long taken = 0;
        for (int b : books[j]) {
            if (taken >= cap) break;
            if (scanned[b]) continue;
            sum += S[b];
            taken++;
            if (commit) {
                scanned[b] = 1;
                out->push_back(b);
            }
        }
        return sum;
    };

    // Max-heap of (upper bound on score per signup day, library id).
    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        long long s = evalLib(j, 0, false, nullptr);
        if (s > 0) pq.push({(double)s / (double)T[j], j});
    }

    long long t = 0;
    vector<pair<int, vector<int>>> result;

    while (!pq.empty()) {
        auto [key, j] = pq.top();
        pq.pop();
        if (D - t - T[j] <= 0) continue;
        long long s = evalLib(j, t, false, nullptr);
        if (s <= 0) continue;
        double v = (double)s / (double)T[j];
        if (!pq.empty() && v < pq.top().first) {
            pq.push({v, j});
            continue;
        }
        vector<int> out;
        evalLib(j, t, true, &out);
        if (!out.empty()) {
            result.push_back({j, out});
        }
        t += T[j];
        if (t >= D) break;
    }

    // Output submission
    string outStr;
    outStr.reserve(1 << 20);
    outStr += to_string(result.size());
    outStr += '\n';
    for (auto &p : result) {
        outStr += to_string(p.first);
        outStr += ' ';
        outStr += to_string(p.second.size());
        outStr += '\n';
        for (size_t k = 0; k < p.second.size(); k++) {
            if (k) outStr += ' ';
            outStr += to_string(p.second[k]);
        }
        outStr += '\n';
    }
    cout << outStr;
    return 0;
}
