#include <bits/stdc++.h>
using namespace std;

int B, L, D;
vector<int> S;
struct Lib {
    int N, T, M;
    vector<int> books; // sorted by score desc
};
vector<Lib> libs;
vector<char> taken;
long long ops = 0;
const long long OPS_LIMIT = 300000000LL;

// Estimated value per signup day if library j starts signup at day ts.
double evalLib(int j, long long ts) {
    const Lib& lb = libs[j];
    long long avail = (long long)D - ts - lb.T;
    if (avail <= 0) return 0.0;
    long long cap = avail * (long long)lb.M;
    double sum = 0;
    long long cnt = 0;
    for (int b : lb.books) {
        ops++;
        if (taken[b]) continue;
        sum += S[b];
        if (++cnt >= cap) break;
    }
    return sum / lb.T;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> B >> L >> D;
    S.resize(B);
    for (int i = 0; i < B; i++) cin >> S[i];
    libs.resize(L);
    for (int j = 0; j < L; j++) {
        cin >> libs[j].N >> libs[j].T >> libs[j].M;
        libs[j].books.resize(libs[j].N);
        for (int k = 0; k < libs[j].N; k++) cin >> libs[j].books[k];
        sort(libs[j].books.begin(), libs[j].books.end(),
             [&](int a, int b) {
                 if (S[a] != S[b]) return S[a] > S[b];
                 return a < b;
             });
    }
    taken.assign(B, 0);

    priority_queue<pair<double, int>> pq;
    for (int j = 0; j < L; j++) {
        double v = evalLib(j, 0);
        if (v > 0) pq.push({v, j});
    }

    long long t = 0;
    vector<pair<int, vector<int>>> chosen;

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        if (t >= D) break;
        int j = top.second;
        double nv = (ops < OPS_LIMIT) ? evalLib(j, t) : top.first;
        if (nv <= 0) continue;
        if (!pq.empty() && nv < pq.top().first) {
            pq.push({nv, j});
            continue;
        }
        // Select library j.
        t += libs[j].T;
        long long cap = ((long long)D - t) * (long long)libs[j].M;
        if (cap <= 0) continue;
        vector<int> list;
        for (int b : libs[j].books) {
            if ((long long)list.size() >= cap) break;
            if (taken[b]) continue;
            taken[b] = 1;
            list.push_back(b);
        }
        if (list.empty()) continue;
        chosen.push_back({j, std::move(list)});
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
