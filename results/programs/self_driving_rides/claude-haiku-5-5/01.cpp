#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

struct Ride {
    int a, b, x, y;
    ll s, f;
    ll d;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, F, N;
    ll B, T;
    cin >> R >> C >> F >> N >> B >> T;

    vector<Ride> rides(N);
    for (int i = 0; i < N; ++i) {
        Ride& r = rides[i];
        cin >> r.a >> r.b >> r.x >> r.y >> r.s >> r.f;
        r.d = llabs((ll)r.a - r.x) + llabs((ll)r.b - r.y);
    }

    vector<bool> used(N, false);
    vector<vector<int>> routes(F);
    vector<int> px(F, 0), py(F, 0);

    // min-heap of (free time, vehicle)
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int v = 0; v < F; ++v) pq.push({0LL, v});

    while (!pq.empty()) {
        auto [t, v] = pq.top();
        pq.pop();

        int cx = px[v], cy = py[v];
        int best = -1;
        double bestVal = -1.0;
        ll bestFinish = 0;

        for (int i = 0; i < N; ++i) {
            if (used[i]) continue;
            const Ride& r = rides[i];
            ll dToStart = llabs((ll)cx - r.a) + llabs((ll)cy - r.b);
            ll start = max(t + dToStart, r.s);
            ll finish = start + r.d;
            if (finish > r.f || finish > T) continue;

            ll score = r.d + (start == r.s ? B : 0);
            ll cost = finish - t; // >= 1 since r.d >= 1
            double val = (double)score / (double)cost;
            if (val > bestVal) {
                bestVal = val;
                best = i;
                bestFinish = finish;
            }
        }

        if (best == -1) continue; // vehicle retires

        used[best] = true;
        routes[v].push_back(best);
        px[v] = rides[best].x;
        py[v] = rides[best].y;
        pq.push({bestFinish, v});
    }

    string out;
    out.reserve(1 << 20);
    for (int v = 0; v < F; ++v) {
        out += to_string(routes[v].size());
        for (int r : routes[v]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
