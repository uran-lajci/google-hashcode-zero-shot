#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long R, C, F, N, B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<long long> A(N), Bc(N), X(N), Y(N), S(N), Fi(N);
    for (int i = 0; i < N; ++i) {
        cin >> A[i] >> Bc[i] >> X[i] >> Y[i] >> S[i] >> Fi[i];
    }

    vector<long long> dist(N);
    for (int i = 0; i < N; ++i) {
        dist[i] = llabs(X[i] - A[i]) + llabs(Y[i] - Bc[i]);
    }

    vector<bool> used(N, false);
    vector<vector<int>> assigned(F);

    // Vehicle state: (free time, vehicle id); position stored separately.
    vector<long long> px(F, 0), py(F, 0), pt(F, 0);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int v = 0; v < F; ++v) pq.push({0LL, v});

    while (!pq.empty()) {
        auto [t, v] = pq.top();
        pq.pop();

        int best = -1;
        double bestVal = -1.0;
        long long bestFinish = 0;

        for (int i = 0; i < N; ++i) {
            if (used[i]) continue;
            long long toStart = llabs(px[v] - A[i]) + llabs(py[v] - Bc[i]);
            long long start = max(t + toStart, S[i]);
            long long finish = start + dist[i];
            if (finish > Fi[i]) continue;
            if (finish > T) continue;
            double pts = (double)dist[i] + (start == S[i] ? (double)B : 0.0);
            double val = pts / (double)(finish - t);
            if (val > bestVal) {
                bestVal = val;
                best = i;
                bestFinish = finish;
            }
        }

        if (best < 0) continue; // this vehicle has no further feasible ride

        used[best] = true;
        assigned[v].push_back(best);
        px[v] = X[best];
        py[v] = Y[best];
        pt[v] = bestFinish;
        pq.push({bestFinish, v});
    }

    string out;
    for (int v = 0; v < F; ++v) {
        out += to_string(assigned[v].size());
        for (int r : assigned[v]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
