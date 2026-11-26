#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S;
vector<long long> Nn, Tt, Mm;
vector<vector<int>> lib;   // books of each library, sorted by score desc
vector<char> scanned;
vector<int> ptrs;          // skip pointer: prefix of scanned books

// Value of library j when signup starts at day t: sum of top-k unscanned
// scores divided by signup time. Optionally collects the chosen books.
double evalLib(int j, long long t, vector<int>* out) {
    long long rem = (long long)D - t - Tt[j];
    if (rem <= 0) return 0.0;
    long long kmax = min(Mm[j] * rem, Nn[j]);
    const vector<int>& bk = lib[j];
    int n = (int)bk.size();
    while (ptrs[j] < n && scanned[bk[ptrs[j]]]) ptrs[j]++;
    double sum = 0;
    long long cnt = 0;
    for (int idx = ptrs[j]; idx < n && cnt < kmax; idx++) {
        int b = bk[idx];
        if (!scanned[b]) {
            sum += S[b];
            cnt++;
            if (out) out->push_back(b);
        }
    }
    if (cnt == 0) return 0.0;
    return sum / (double)Tt[j];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> B >> L >> D;
    S.assign(B, 0);
    for (int i = 0; i < B; i++) cin >> S[i];

    Nn.assign(L, 0);
    Tt.assign(L, 0);
    Mm.assign(L, 0);
    lib.assign(L, {});
    for (int j = 0; j < L; j++) {
        cin >> Nn[j] >> Tt[j] >> Mm[j];
        lib[j].resize((size_t)Nn[j]);
        for (int k = 0; k < Nn[j]; k++) cin >> lib[j][k];
        sort(lib[j].begin(), lib[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    scanned.assign(B, 0);
    ptrs.assign(L, 0);

    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        double v = evalLib(j, 0, nullptr);
        if (v > 0) pq.push({v, j});
    }

    long long t = 0;
    vector<pair<int, vector<int>>> chosen;

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;
        if ((long long)D - t - Tt[j] <= 0) continue;

        double v = evalLib(j, t, nullptr);
        if (v <= 0) continue;

        if (pq.empty() || v >= pq.top().first) {
            vector<int> books;
            evalLib(j, t, &books);
            for (int b : books) scanned[b] = 1;
            t += Tt[j];
            chosen.push_back({j, books});
        } else {
            pq.push({v, j});
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
