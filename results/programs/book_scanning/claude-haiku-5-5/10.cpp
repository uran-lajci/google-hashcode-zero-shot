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

    vector<long long> N(L), T(L), M(L);
    vector<vector<int>> lib(L);
    for (int j = 0; j < L; j++) {
        cin >> N[j] >> T[j] >> M[j];
        lib[j].resize(N[j]);
        for (int k = 0; k < N[j]; k++) cin >> lib[j][k];
        // sort by score descending, ties by id ascending
        sort(lib[j].begin(), lib[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    vector<char> scanned(B, 0);
    vector<size_t> ptr(L, 0);

    // Evaluate library j if signup starts at day t.
    // Returns ratio value (score / signup time); if out != nullptr, fills chosen books.
    auto evalLib = [&](int j, long long t, vector<int>* out) -> double {
        long long rem = D - t - T[j];
        if (rem <= 0) return 0.0;
        long long k = min(N[j], rem * M[j]);
        if (k <= 0) return 0.0;
        // advance past scanned prefix
        while (ptr[j] < lib[j].size() && scanned[lib[j][ptr[j]]]) ptr[j]++;
        long long sum = 0;
        long long cnt = 0;
        for (size_t i = ptr[j]; i < lib[j].size() && cnt < k; i++) {
            int b = lib[j][i];
            if (scanned[b]) continue;
            sum += S[b];
            cnt++;
            if (out) out->push_back(b);
        }
        if (cnt == 0) return 0.0;
        return (double)sum / (double)T[j];
    };

    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        double v = evalLib(j, 0, nullptr);
        if (v > 0) pq.push({v, j});
    }

    struct Sel { int lib; vector<int> books; };
    vector<Sel> sel;
    long long t = 0;

    while (!pq.empty() && t < D) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;
        vector<int> tmp;
        double nv = evalLib(j, t, &tmp);
        if (nv <= 0) continue;
        if (pq.empty() || nv >= pq.top().first) {
            for (int b : tmp) scanned[b] = 1;
            sel.push_back({j, tmp});
            t += T[j];
        } else {
            pq.push({nv, j});
        }
    }

    string out;
    out.reserve(1 << 20);
    out += to_string(sel.size());
    out += '\n';
    for (auto &s : sel) {
        out += to_string(s.lib);
        out += ' ';
        out += to_string(s.books.size());
        out += '\n';
        for (size_t i = 0; i < s.books.size(); i++) {
            if (i) out += ' ';
            out += to_string(s.books[i]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
