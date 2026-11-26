#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int B, L;
    long long D;
    cin >> B >> L >> D;
    vector<long long> S(B);
    for (int i = 0; i < B; i++) cin >> S[i];

    vector<int> N(L), T(L), M(L);
    vector<vector<int>> books(L);
    for (int j = 0; j < L; j++) {
        cin >> N[j] >> T[j] >> M[j];
        books[j].resize(N[j]);
        for (int k = 0; k < N[j]; k++) cin >> books[j][k];
        sort(books[j].begin(), books[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    vector<char> scanned(B, 0);

    // Evaluate library j if its signup starts at day t.
    // Returns -1 if signup finishes at or after D, otherwise total value.
    // If out != nullptr, appends the selected books (in scan order).
    auto eval = [&](int j, long long t, vector<int>* out) -> long long {
        long long f = t + T[j];
        if (f >= D) return -1;
        long long R = D - f;
        long long k = min((long long)M[j] * R, (long long)N[j]);
        long long val = 0;
        long long cnt = 0;
        for (int id : books[j]) {
            if (cnt >= k) break;
            if (scanned[id]) continue;
            val += S[id];
            cnt++;
            if (out) out->push_back(id);
        }
        return val;
    };

    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        long long v = eval(j, 0, nullptr);
        if (v > 0) {
            pq.push({(double)v / T[j], j});
        }
    }

    vector<pair<int, vector<int>>> chosen;
    long long t = 0;
    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;
        long long v = eval(j, t, nullptr);
        if (v <= 0) continue;  // -1: too late, 0: nothing useful
        double nr = (double)v / T[j];
        if (pq.empty() || nr >= pq.top().first) {
            vector<int> sel;
            eval(j, t, &sel);
            for (int id : sel) scanned[id] = 1;
            chosen.push_back({j, sel});
            t += T[j];
        } else {
            pq.push({nr, j});
        }
    }

    string out;
    out.reserve(1 << 20);
    out += to_string(chosen.size());
    out += '\n';
    for (auto& c : chosen) {
        out += to_string(c.first);
        out += ' ';
        out += to_string(c.second.size());
        out += '\n';
        for (size_t i = 0; i < c.second.size(); i++) {
            if (i) out += ' ';
            out += to_string(c.second[i]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
