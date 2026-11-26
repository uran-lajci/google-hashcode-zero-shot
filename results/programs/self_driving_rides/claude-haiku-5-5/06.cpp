#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long R, C, F, N, B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<long long> a(N), b(N), x(N), y(N), s(N), f(N), dist(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i] >> b[i] >> x[i] >> y[i] >> s[i] >> f[i];
        dist[i] = llabs(a[i] - x[i]) + llabs(b[i] - y[i]);
    }

    vector<vector<int>> plan(F);
    vector<long long> px(F, 0), py(F, 0), pt(F, 0);
    vector<char> used(N, 0);
    vector<char> alive(F, 1);

    // min-heap by (time, vehicle id)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
    for (int v = 0; v < F; ++v) pq.push({0LL, v});

    while (!pq.empty()) {
        auto [t, v] = pq.top();
        pq.pop();
        long long cx = px[v], cy = py[v];

        int best = -1;
        double bestRatio = -1.0;
        long long bestFinish = 0;

        for (int r = 0; r < N; ++r) {
            if (used[r]) continue;
            long long d = llabs(cx - a[r]) + llabs(cy - b[r]);
            long long arrive = t + d;
            long long start = max(arrive, s[r]);
            long long finish = start + dist[r];
            if (finish > f[r]) continue;
            long long bonus = (arrive <= s[r]) ? B : 0;
            double value = (double)(dist[r] + bonus);
            double denom = (double)(finish - t);
            double ratio = value / denom;
            if (best == -1 || ratio > bestRatio ||
                (ratio == bestRatio && finish < bestFinish)) {
                best = r;
                bestRatio = ratio;
                bestFinish = finish;
            }
        }

        if (best == -1) {
            alive[v] = 0;
            continue;
        }

        used[best] = 1;
        plan[v].push_back(best);
        px[v] = x[best];
        py[v] = y[best];
        pt[v] = bestFinish;
        pq.push({bestFinish, v});
    }

    string out;
    out.reserve(1 << 20);
    for (int v = 0; v < F; ++v) {
        out += to_string(plan[v].size());
        for (int r : plan[v]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
