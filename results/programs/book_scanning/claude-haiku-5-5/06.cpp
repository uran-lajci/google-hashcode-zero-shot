#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S;
vector<int> Tt, Mm;
vector<vector<int>> libBooks; // each sorted by score desc
vector<char> scanned;

// Computes total score obtainable from library j if signed up at time t.
// If out != nullptr, appends chosen books (unscanned, top by score).
long long evaluate(int j, long long t, vector<int>* out) {
    long long finish = t + Tt[j];
    if (finish >= D) return 0;
    long long cap = (long long)(D - finish) * Mm[j];
    long long cnt = 0, sum = 0;
    for (int b : libBooks[j]) {
        if (cnt >= cap) break;
        if (scanned[b]) continue;
        sum += S[b];
        cnt++;
        if (out) out->push_back(b);
    }
    return sum;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> B >> L >> D;
    S.assign(B, 0);
    for (int i = 0; i < B; i++) cin >> S[i];

    Tt.assign(L, 0);
    Mm.assign(L, 0);
    libBooks.assign(L, {});
    for (int j = 0; j < L; j++) {
        int n;
        cin >> n >> Tt[j] >> Mm[j];
        libBooks[j].resize(n);
        for (int k = 0; k < n; k++) cin >> libBooks[j][k];
        sort(libBooks[j].begin(), libBooks[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    scanned.assign(B, 0);

    // max-heap of (estimated ratio, library id)
    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        long long v = evaluate(j, 0, nullptr);
        if (v > 0) pq.push({(double)v / Tt[j], j});
    }

    long long t = 0;
    vector<pair<int, vector<int>>> selections;

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;

        long long v = evaluate(j, t, nullptr);
        if (v <= 0) continue;
        double nr = (double)v / Tt[j];

        if (!pq.empty() && nr < pq.top().first) {
            pq.push({nr, j});
            continue;
        }

        // select library j
        vector<int> out;
        evaluate(j, t, &out);
        for (int b : out) scanned[b] = 1;
        selections.push_back({j, out});
        t += Tt[j];
        if (t >= D) break;
    }

    string res;
    res.reserve(1 << 20);
    res += to_string(selections.size());
    res += '\n';
    for (auto& sel : selections) {
        res += to_string(sel.first);
        res += ' ';
        res += to_string(sel.second.size());
        res += '\n';
        for (size_t i = 0; i < sel.second.size(); i++) {
            if (i) res += ' ';
            res += to_string(sel.second[i]);
        }
        res += '\n';
    }
    cout << res;
    return 0;
}
