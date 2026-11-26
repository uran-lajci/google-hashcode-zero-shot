#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S, N, T, M;
vector<vector<int>> books;   // per library, sorted by score desc
vector<int> ptrIdx;          // per library, skip prefix of scanned books
vector<char> scanned;        // per book
long long ops = 0;
const long long BUDGET = 400000000LL;

// Estimated score of library j if its signup starts at day t.
// If out != nullptr, the selected books are appended to it.
long long evalLib(int j, long long t, vector<int>* out) {
    long long finish = t + T[j];
    long long days = (long long)D - finish;
    if (days <= 0) return 0;
    long long cap = (long long)M[j] * days;
    const vector<int>& bk = books[j];
    int &p = ptrIdx[j];
    while (p < (int)bk.size() && scanned[bk[p]]) p++;
    long long sum = 0, cnt = 0;
    int i = p;
    for (; i < (int)bk.size() && cnt < cap; i++) {
        int b = bk[i];
        if (!scanned[b]) {
            sum += S[b];
            cnt++;
            if (out) out->push_back(b);
        }
    }
    ops += (i - p) + 1;
    return sum;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> B >> L >> D;
    S.resize(B);
    for (int i = 0; i < B; i++) cin >> S[i];
    N.resize(L); T.resize(L); M.resize(L);
    books.resize(L);
    for (int j = 0; j < L; j++) {
        cin >> N[j] >> T[j] >> M[j];
        books[j].resize(N[j]);
        for (int k = 0; k < N[j]; k++) cin >> books[j][k];
        sort(books[j].begin(), books[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }

    ptrIdx.assign(L, 0);
    scanned.assign(B, 0);

    // Max-heap of (estimated ratio, library id).
    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        long long s = evalLib(j, 0, nullptr);
        double v = (double)s / (double)T[j];
        if (s > 0) pq.push({v, j});
    }

    vector<pair<int, vector<int>>> result;
    long long t = 0;
    while (!pq.empty() && t < D) {
        auto top = pq.top();
        pq.pop();
        int j = top.second;
        double cur;
        if (ops < BUDGET) {
            long long s = evalLib(j, t, nullptr);
            cur = (double)s / (double)T[j];
            if (s <= 0) continue;
            if (!pq.empty() && cur < pq.top().first) {
                pq.push({cur, j});
                continue;
            }
        }
        vector<int> sel;
        long long s = evalLib(j, t, &sel);
        if (s <= 0 || sel.empty()) continue;
        for (int b : sel) scanned[b] = 1;
        result.push_back({j, sel});
        t += T[j];
    }

    // Output submission.
    string out;
    out.reserve(1 << 20);
    out += to_string(result.size());
    out += '\n';
    for (auto &r : result) {
        out += to_string(r.first);
        out += ' ';
        out += to_string(r.second.size());
        out += '\n';
        for (size_t k = 0; k < r.second.size(); k++) {
            if (k) out += ' ';
            out += to_string(r.second[k]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
