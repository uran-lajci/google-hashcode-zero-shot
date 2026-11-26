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

    // Unassigned rides as a list (swap-remove on assignment).
    vector<int> unassigned(N);
    for (int i = 0; i < N; ++i) unassigned[i] = i;

    // Per-vehicle state.
    vector<long long> curT(F, 0), posR(F, 0), posC(F, 0);
    vector<vector<int>> plan(F);

    // Min-heap of (time, vehicle id).
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int v = 0; v < F; ++v) pq.push({0, v});

    while (!pq.empty() && !unassigned.empty()) {
        auto [t, v] = pq.top();
        pq.pop();

        int bestK = -1;
        double bestRatio = -1.0;
        long long bestFin = 0;

        for (int k = 0; k < (int)unassigned.size(); ++k) {
            int i = unassigned[k];
            long long arrive = t + llabs(posR[v] - A[i]) + llabs(posC[v] - Bc[i]);
            long long st = max(arrive, S[i]);
            long long fin = st + dist[i];
            if (fin > Fi[i]) continue; // cannot finish on time
            long long pts = dist[i] + (arrive <= S[i] ? B : 0);
            double ratio = (double)pts / (double)(fin - t);
            if (bestK < 0 || ratio > bestRatio ||
                (ratio == bestRatio && fin < bestFin) ||
                (ratio == bestRatio && fin == bestFin && i < unassigned[bestK])) {
                bestK = k;
                bestRatio = ratio;
                bestFin = fin;
            }
        }

        if (bestK < 0) {
            // This vehicle can no longer take any ride; retire it.
            continue;
        }

        int i = unassigned[bestK];
        unassigned[bestK] = unassigned.back();
        unassigned.pop_back();

        plan[v].push_back(i);
        curT[v] = bestFin;
        posR[v] = X[i];
        posC[v] = Y[i];
        if (bestFin < T) pq.push({bestFin, v});
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
