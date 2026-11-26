#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<long long> S;
vector<int> Nj, Tj, Mj;
vector<vector<int>> sortedBooks;
vector<char> scanned;

// Returns estimated value of library j if signed up starting at day t.
// If out != nullptr, appends the selected unscanned books (in order) to it.
// Returns -1 if no shipping days remain, 0 if no unscanned books.
double evalLib(int j, long long t, vector<int>* out) {
    long long rem = (long long)D - t - Tj[j];
    if (rem <= 0) return -1.0;
    long long k = min((long long)Mj[j] * rem, (long long)sortedBooks[j].size());
    long long sum = 0, cnt = 0;
    for (int b : sortedBooks[j]) {
        if (cnt >= k) break;
        if (scanned[b]) continue;
        sum += S[b];
        cnt++;
        if (out) out->push_back(b);
    }
    if (cnt == 0) return 0.0;
    return (double)sum / (double)Tj[j];
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> B >> L >> D;
    S.assign(B, 0);
    for (int i = 0; i < B; i++) cin >> S[i];

    Nj.assign(L, 0);
    Tj.assign(L, 0);
    Mj.assign(L, 0);
    sortedBooks.assign(L, {});
    for (int j = 0; j < L; j++) {
        cin >> Nj[j] >> Tj[j] >> Mj[j];
        sortedBooks[j].resize(Nj[j]);
        for (int i = 0; i < Nj[j]; i++) cin >> sortedBooks[j][i];
        sort(sortedBooks[j].begin(), sortedBooks[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    scanned.assign(B, 0);

    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        double v = evalLib(j, 0, nullptr);
        if (v > 0) pq.push({v, j});
    }

    long long t = 0;
    vector<pair<int, vector<int>>> result;

    while (!pq.empty() && t < D) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;
        double nv = evalLib(j, t, nullptr);
        if (nv <= 0) continue;
        if (!pq.empty() && nv < pq.top().first) {
            pq.push({nv, j});
            continue;
        }
        // Select library j.
        vector<int> books;
        evalLib(j, t, &books);
        for (int b : books) scanned[b] = 1;
        result.push_back({j, books});
        t += Tj[j];
    }

    // Output.
    string out;
    out.reserve(1 << 20);
    out += to_string(result.size());
    out += '\n';
    for (auto& r : result) {
        out += to_string(r.first);
        out += ' ';
        out += to_string(r.second.size());
        out += '\n';
        for (size_t i = 0; i < r.second.size(); i++) {
            if (i) out += ' ';
            out += to_string(r.second[i]);
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
