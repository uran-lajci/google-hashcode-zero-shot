#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S;
vector<int> Tj, Mj;
vector<vector<int>> books;

struct Result {
    long long score = -1;
    vector<int> order;
    vector<vector<int>> taken;
};

chrono::steady_clock::time_point st;
double elapsed() {
    return chrono::duration<double>(chrono::steady_clock::now() - st).count();
}

bool runGreedy(double alpha, Result& res, double timeLimit, bool force) {
    vector<char> tk(B, 0);
    vector<double> denom(L);
    for (int j = 0; j < L; j++) denom[j] = pow((double)Tj[j], alpha);
    long long t = 0;
    int cnt = 0;
    auto gainOf = [&](int j, long long tt) -> long long {
        long long cap = (long long)(D - tt - Tj[j]) * Mj[j];
        if (cap <= 0) return 0;
        long long g = 0, c = 0;
        for (int b : books[j]) {
            if (c >= cap) break;
            if (tk[b]) continue;
            g += S[b];
            c++;
        }
        return g;
    };
    priority_queue<tuple<double, int, int>> pq;
    for (int j = 0; j < L; j++) {
        long long g = gainOf(j, 0);
        if (g > 0) pq.push({g / denom[j], -j, 0});
    }
    res.score = 0;
    res.order.clear();
    res.taken.clear();
    long long ops = 0;
    while (!pq.empty()) {
        if (!force && ((++ops) & 255) == 0 && elapsed() > timeLimit) return false;
        auto [v, nj, stamp] = pq.top();
        pq.pop();
        int j = -nj;
        if (t + Tj[j] >= D) continue;
        if (stamp == cnt) {
            long long cap = (long long)(D - t - Tj[j]) * Mj[j];
            vector<int> tb;
            long long c = 0;
            for (int b : books[j]) {
                if (c >= cap) break;
                if (tk[b]) continue;
                tk[b] = 1;
                tb.push_back(b);
                res.score += S[b];
                c++;
            }
            if (tb.empty()) continue;
            res.order.push_back(j);
            res.taken.push_back(move(tb));
            t += Tj[j];
            cnt++;
            if (t >= D) break;
        } else {
            long long g = gainOf(j, t);
            if (g > 0) pq.push({g / denom[j], -j, cnt});
        }
    }
    return true;
}

static char buf[1 << 25];
int bufLen = 0, bufPos = 0;
inline int readInt() {
    int c = getchar_unlocked();
    while (c != '-' && (c < '0' || c > '9')) c = getchar_unlocked();
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = getchar_unlocked(); }
    return x;
}

int main() {
    st = chrono::steady_clock::now();
    B = readInt(); L = readInt(); D = readInt();
    S.resize(B);
    for (int i = 0; i < B; i++) S[i] = readInt();
    Tj.resize(L); Mj.resize(L); books.resize(L);
    for (int j = 0; j < L; j++) {
        int n = readInt();
        Tj[j] = readInt();
        Mj[j] = readInt();
        books[j].resize(n);
        for (int k = 0; k < n; k++) books[j][k] = readInt();
        sort(books[j].begin(), books[j].end(), [&](int a, int b) {
            if (S[a] != S[b]) return S[a] > S[b];
            return a < b;
        });
    }
    vector<double> alphas = {1.0, 0.9, 1.1, 0.8, 1.2, 0.7, 1.3, 0.6, 1.5, 0.5, 0.95, 1.05, 0.85, 1.15, 2.0, 0.3};
    Result best;
    bool first = true;
    for (double a : alphas) {
        if (!first && elapsed() > 7.5) break;
        Result r;
        bool ok = runGreedy(a, r, 8.5, first);
        first = false;
        if (ok && r.score > best.score) best = move(r);
    }
    string out;
    out.reserve(1 << 22);
    out += to_string(best.order.size());
    out += '\n';
    for (size_t i = 0; i < best.order.size(); i++) {
        out += to_string(best.order[i]);
        out += ' ';
        out += to_string(best.taken[i].size());
        out += '\n';
        for (size_t k = 0; k < best.taken[i].size(); k++) {
            if (k) out += ' ';
            out += to_string(best.taken[i][k]);
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
