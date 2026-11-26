#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long R, C, F, N, B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<long long> a(N), b(N), x(N), y(N), s(N), f(N), d(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i] >> b[i] >> x[i] >> y[i] >> s[i] >> f[i];
        d[i] = llabs(x[i] - a[i]) + llabs(y[i] - b[i]);
    }

    // Vehicle state
    vector<long long> pr(F, 0), pc(F, 0), tf(F, 0);
    vector<char> active(F, 1);
    vector<vector<int>> plan(F);

    // Candidate rides (unassigned), kept in a vector with swap-remove
    vector<int> cand(N);
    for (int i = 0; i < N; ++i) cand[i] = i;

    while (true) {
        // Pick the active vehicle that is free earliest (ties: lower index)
        int v = -1;
        for (int i = 0; i < F; ++i) {
            if (!active[i]) continue;
            if (v == -1 || tf[i] < tf[v]) v = i;
        }
        if (v == -1 || cand.empty()) break;

        long long t = tf[v];
        double bestVal = -1.0;
        int bestIdx = -1;          // index into cand
        long long bestEnd = 0;

        for (int k = 0; k < (int)cand.size(); ++k) {
            int r = cand[k];
            long long ds = llabs(pr[v] - a[r]) + llabs(pc[v] - b[r]);
            long long ts = max(t + ds, s[r]);
            long long end = ts + d[r];
            if (end > f[r]) continue;
            long long pts = d[r] + (ts == s[r] ? B : 0);
            double val = (double)pts / (double)(end - t);
            if (val > bestVal) {
                bestVal = val;
                bestIdx = k;
                bestEnd = end;
            }
        }

        if (bestIdx == -1) {
            // No feasible ride for this vehicle any more
            active[v] = 0;
            continue;
        }

        int r = cand[bestIdx];
        plan[v].push_back(r);
        tf[v] = bestEnd;
        pr[v] = x[r];
        pc[v] = y[r];

        cand[bestIdx] = cand.back();
        cand.pop_back();
    }

    string out;
    out.reserve(1 << 20);
    for (int i = 0; i < F; ++i) {
        out += to_string(plan[i].size());
        for (int r : plan[i]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
