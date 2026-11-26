#include <bits/stdc++.h>
using namespace std;

int main() {
    ios_strings_dummy:;
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long R, C, F, N, B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<long long> a(N), b(N), x(N), y(N), s(N), f(N), dist(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i] >> b[i] >> x[i] >> y[i] >> s[i] >> f[i];
        dist[i] = llabs(a[i] - x[i]) + llabs(b[i] - y[i]);
    }

    vector<int> active(N);
    for (int i = 0; i < N; ++i) active[i] = i;

    vector<long long> px(F, 0), py(F, 0);
    vector<vector<int>> assign(F);

    // min-heap of (free time, vehicle)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int v = 0; v < F; ++v) pq.push({0LL, v});

    while (!pq.empty() && !active.empty()) {
        auto [t, v] = pq.top();
        pq.pop();

        int bestK = -1;
        double bestVal = -1.0;
        long long bestFin = 0;

        for (int k = 0; k < (int)active.size(); ++k) {
            int r = active[k];
            long long d = llabs(px[v] - a[r]) + llabs(py[v] - b[r]);
            long long st = max(t + d, s[r]);
            long long fin = st + dist[r];
            if (fin > f[r]) continue;
            long long bonus = (st == s[r]) ? B : 0;
            double val = (double)(dist[r] + bonus) / (double)(fin - t);
            if (val > bestVal) {
                bestVal = val;
                bestK = k;
                bestFin = fin;
            }
        }

        if (bestK < 0) continue; // vehicle cannot do any more rides

        int r = active[bestK];
        active[bestK] = active.back();
        active.pop_back();

        assign[v].push_back(r);
        px[v] = x[r];
        py[v] = y[r];
        pq.push({bestFin, v});
    }

    string out;
    out.reserve(1 << 20);
    for (int v = 0; v < F; ++v) {
        out += to_string(assign[v].size());
        for (int r : assign[v]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
