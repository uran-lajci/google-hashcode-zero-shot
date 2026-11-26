#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S;
vector<int> N, T, M;
vector<vector<int>> books; // sorted by score desc

struct Result {
    long long score = -1;
    vector<pair<int, vector<int>>> libs;
};

static long long evalLib(int j, long long R, const vector<char>& scanned) {
    if (R <= 0) return 0;
    long long cap = R * (long long)M[j];
    long long sum = 0, cnt = 0;
    for (int b : books[j]) {
        if (cnt >= cap) break;
        if (scanned[b]) continue;
        sum += S[b];
        cnt++;
    }
    return sum;
}

Result runGreedy(double alpha) {
    vector<char> scanned(B, 0);
    vector<double> tpow(L);
    for (int j = 0; j < L; j++) tpow[j] = pow((double)T[j], alpha);
    priority_queue<pair<double, int>> pq;
    long long cur = 0;
    for (int j = 0; j < L; j++) {
        long long R = (long long)D - cur - T[j];
        if (R <= 0) continue;
        long long g = evalLib(j, R, scanned);
        if (g <= 0) continue;
        pq.push({(double)g / tpow[j], j});
    }
    Result res;
    res.score = 0;
    while (!pq.empty()) {
        auto [v, j] = pq.top();
        pq.pop();
        long long R = (long long)D - cur - T[j];
        if (R <= 0) continue;
        long long g = evalLib(j, R, scanned);
        if (g <= 0) continue;
        double nv = (double)g / tpow[j];
        if (!pq.empty() && nv < pq.top().first) {
            pq.push({nv, j});
            continue;
        }
        // accept
        long long cap = R * (long long)M[j];
        vector<int> assigned;
        for (int b : books[j]) {
            if ((long long)assigned.size() >= cap) break;
            if (scanned[b]) continue;
            scanned[b] = 1;
            assigned.push_back(b);
            res.score += S[b];
        }
        if (assigned.empty()) continue;
        cur += T[j];
        res.libs.push_back({j, move(assigned)});
        if (cur >= D) break;
    }
    return res;
}

int main() {
    auto start = chrono::steady_clock::now();
    if (scanf("%d %d %d", &B, &L, &D) != 3) {
        printf("0\n");
        return 0;
    }
    S.resize(B);
    for (int i = 0; i < B; i++) scanf("%d", &S[i]);
    N.resize(L); T.resize(L); M.resize(L); books.resize(L);
    for (int j = 0; j < L; j++) {
        scanf("%d %d %d", &N[j], &T[j], &M[j]);
        books[j].resize(N[j]);
        for (int k = 0; k < N[j]; k++) scanf("%d", &books[j][k]);
        sort(books[j].begin(), books[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }
    vector<double> alphas = {1.0, 1.3, 0.7, 1.15, 0.85, 1.6, 0.5, 2.0, 1.05, 0.95};
    Result best;
    for (double a : alphas) {
        double el = chrono::duration<double>(chrono::steady_clock::now() - start).count();
        if (best.score >= 0 && el > 7.5) break;
        Result r = runGreedy(a);
        if (r.score > best.score) best = move(r);
    }
    // output
    string out;
    out.reserve(1 << 20);
    out += to_string(best.libs.size());
    out += '\n';
    for (auto& p : best.libs) {
        out += to_string(p.first);
        out += ' ';
        out += to_string(p.second.size());
        out += '\n';
        for (size_t i = 0; i < p.second.size(); i++) {
            if (i) out += ' ';
            out += to_string(p.second[i]);
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
