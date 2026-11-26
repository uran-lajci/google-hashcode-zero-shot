#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long R, C, F, N, B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<long long> A(N), Bc(N), X(N), Y(N), S(N), Fi(N), D(N);
    for (int i = 0; i < N; ++i) {
        cin >> A[i] >> Bc[i] >> X[i] >> Y[i] >> S[i] >> Fi[i];
        D[i] = llabs(A[i] - X[i]) + llabs(Bc[i] - Y[i]);
    }

    struct Vehicle {
        long long time;
        long long r, c;
        vector<int> rides;
    };
    vector<Vehicle> veh(F);
    for (auto& v : veh) {
        v.time = 0;
        v.r = 0;
        v.c = 0;
    }

    // Min-heap by (time, id)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int i = 0; i < F; ++i) pq.push({0, i});

    // Remaining unassigned rides
    vector<int> remaining(N);
    for (int i = 0; i < N; ++i) remaining[i] = i;

    while (!pq.empty() && !remaining.empty()) {
        auto [t, id] = pq.top();
        pq.pop();
        Vehicle& v = veh[id];

        long long bestKey = LLONG_MIN;
        long long bestFinish = LLONG_MAX;
        int bestPos = -1;
        long long bestStart = 0;

        for (int k = 0; k < (int)remaining.size(); ++k) {
            int ri = remaining[k];
            long long d1 = llabs(v.r - A[ri]) + llabs(v.c - Bc[ri]);
            long long arrive = t + d1;
            long long start = max(arrive, S[ri]);
            long long finish = start + D[ri];
            if (finish > Fi[ri]) continue;
            long long gain = D[ri] + (start == S[ri] ? B : 0);
            long long key = gain - (start - t);
            if (key > bestKey || (key == bestKey && finish < bestFinish)) {
                bestKey = key;
                bestFinish = finish;
                bestPos = k;
                bestStart = start;
            }
        }
        (void)bestStart;

        if (bestPos < 0) {
            // No feasible ride for this vehicle now; its time only grows, so drop it.
            continue;
        }

        int ri = remaining[bestPos];
        v.rides.push_back(ri);
        v.time = bestFinish;
        v.r = X[ri];
        v.c = Y[ri];
        pq.push({v.time, id});

        remaining[bestPos] = remaining.back();
        remaining.pop_back();
    }

    string out;
    out.reserve(1 << 20);
    for (int i = 0; i < F; ++i) {
        out += to_string(veh[i].rides.size());
        for (int r : veh[i].rides) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
