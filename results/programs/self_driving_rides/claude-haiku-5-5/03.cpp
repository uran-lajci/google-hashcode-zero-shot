#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, F, N;
    long long B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<int> a(N), b(N), x(N), y(N);
    vector<long long> s(N), f(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i] >> b[i] >> x[i] >> y[i] >> s[i] >> f[i];
    }

    vector<char> used(N, 0);
    vector<long long> px(F, 0), py(F, 0), tm(F, 0);
    vector<vector<int>> res(F);

    // Min-heap by (free time, vehicle id)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int v = 0; v < F; ++v) pq.push({0LL, v});

    while (!pq.empty()) {
        auto [t, v] = pq.top();
        pq.pop();

        int best = -1;
        long long bestGain = 0, bestDur = 1, bestFin = 0, bestStart = 0;

        for (int i = 0; i < N; ++i) {
            if (used[i]) continue;
            long long d0 = llabs(px[v] - a[i]) + llabs(py[v] - b[i]);
            long long st = max(t + d0, s[i]);
            long long d1 = llabs((long long)a[i] - x[i]) + llabs((long long)b[i] - y[i]);
            long long fin = st + d1;
            if (fin > f[i]) continue;

            long long gain = d1 + (st == s[i] ? B : 0LL);
            long long dur = fin - t;
            if (dur < 1) dur = 1;

            bool better;
            if (best < 0) {
                better = true;
            } else {
                // compare gain/dur > bestGain/bestDur
                long long lhs = gain * bestDur;
                long long rhs = bestGain * dur;
                if (lhs != rhs) better = lhs > rhs;
                else better = fin < bestFin;
            }
            if (better) {
                best = i;
                bestGain = gain;
                bestDur = dur;
                bestFin = fin;
                bestStart = st;
            }
        }

        if (best < 0) continue; // vehicle finished, drop it

        used[best] = 1;
        res[v].push_back(best);
        px[v] = x[best];
        py[v] = y[best];
        tm[v] = bestFin;
        (void)bestStart;
        pq.push({bestFin, v});
    }

    string out;
    out.reserve(1 << 20);
    for (int v = 0; v < F; ++v) {
        out += to_string(res[v].size());
        for (int r : res[v]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
