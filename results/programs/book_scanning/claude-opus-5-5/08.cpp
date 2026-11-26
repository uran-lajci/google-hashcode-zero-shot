#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S;
vector<int> NT, MT;
vector<vector<int>> books;
chrono::steady_clock::time_point st;
double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-st).count(); }

struct Plan { long long score; vector<int> order; vector<vector<int>> taken; };

Plan run(double a, double timeLimit){
    vector<char> scanned(B, 0);
    long long t = 0;
    auto value = [&](int j)->long long{
        long long cap = (long long)(D - t - NT[j]) * MT[j];
        if (cap <= 0) return 0;
        long long v = 0, c = 0;
        for (int b : books[j]) {
            if (scanned[b]) continue;
            v += S[b];
            if (++c >= cap) break;
        }
        return v;
    };
    vector<double> powT(L);
    for (int j = 0; j < L; j++) powT[j] = pow((double)NT[j], a);
    priority_queue<pair<double,int>> pq;
    for (int j = 0; j < L; j++) {
        long long v = value(j);
        if (v > 0) pq.push({v / powT[j], j});
    }
    Plan p; p.score = 0;
    while (!pq.empty()) {
        int j = pq.top().second; pq.pop();
        if (t + NT[j] >= D) continue;
        long long v = value(j);
        if (v <= 0) continue;
        double key = v / powT[j];
        bool accept = pq.empty() || key >= pq.top().first - 1e-12 || elapsed() > timeLimit;
        if (!accept) { pq.push({key, j}); continue; }
        long long cap = (long long)(D - t - NT[j]) * MT[j];
        vector<int> tk;
        for (int b : books[j]) {
            if ((long long)tk.size() >= cap) break;
            if (scanned[b]) continue;
            scanned[b] = 1; tk.push_back(b); p.score += S[b];
        }
        if (tk.empty()) continue;
        // add remaining books (already scanned elsewhere) not needed
        p.order.push_back(j);
        p.taken.push_back(move(tk));
        t += NT[j];
    }
    return p;
}

int main(){
    st = chrono::steady_clock::now();
    if (scanf("%d %d %d", &B, &L, &D) != 3) { printf("0\n"); return 0; }
    S.resize(B);
    for (int i = 0; i < B; i++) scanf("%d", &S[i]);
    NT.resize(L); MT.resize(L); books.resize(L);
    for (int j = 0; j < L; j++) {
        int n; scanf("%d %d %d", &n, &NT[j], &MT[j]);
        books[j].resize(n);
        for (int k = 0; k < n; k++) scanf("%d", &books[j][k]);
        sort(books[j].begin(), books[j].end(), [&](int x, int y){
            if (S[x] != S[y]) return S[x] > S[y];
            return x < y;
        });
    }
    vector<double> exps = {1.0, 0.8, 1.2, 0.6, 1.5, 0.9, 1.1, 0.7, 1.3, 0.5, 2.0};
    Plan best; best.score = -1;
    for (size_t i = 0; i < exps.size(); i++) {
        double e0 = elapsed();
        if (i > 0 && e0 > 5.0) break;
        Plan p = run(exps[i], i == 0 ? 7.0 : 8.0);
        if (p.score > best.score) best = move(p);
        double dt = elapsed() - e0;
        if (elapsed() + dt * 1.3 > 8.0) break;
    }
    string out;
    out.reserve(1 << 20);
    out += to_string(best.order.size()) + "\n";
    for (size_t i = 0; i < best.order.size(); i++) {
        out += to_string(best.order[i]) + " " + to_string(best.taken[i].size()) + "\n";
        for (size_t k = 0; k < best.taken[i].size(); k++) {
            if (k) out += ' ';
            out += to_string(best.taken[i][k]);
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
