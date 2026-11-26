#include <bits/stdc++.h>
using namespace std;

struct Ride {
    int a, b, x, y, s, f;
    int d;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, F, N, B;
    long long T;
    cin >> R >> C >> F >> N >> B >> T;
    vector<Ride> rides(N);
    for (int i = 0; i < N; ++i) {
        cin >> rides[i].a >> rides[i].b >> rides[i].x >> rides[i].y >> rides[i].s >> rides[i].f;
        rides[i].d = abs(rides[i].x - rides[i].a) + abs(rides[i].y - rides[i].b);
    }

    vector<vector<int>> assigned(F);
    vector<bool> used(N, false);

    // Vehicle state: (free time, vehicle id). Position and time tracked separately.
    vector<int> px(F, 0), py(F, 0);
    vector<long long> ft(F, 0);

    using Item = pair<long long, int>; // (time, vehicle)
    priority_queue<Item, vector<Item>, greater<Item>> pq;
    for (int v = 0; v < F; ++v) pq.push({0, v});

    while (!pq.empty()) {
        auto [t, v] = pq.top();
        pq.pop();
        int cx = px[v], cy = py[v];

        long long bestKey = LLONG_MIN;
        int best = -1;
        long long bestStart = 0, bestFinish = 0;

        for (int i = 0; i < N; ++i) {
            if (used[i]) continue;
            const Ride &r = rides[i];
            long long arrive = t + abs(cx - r.a) + abs(cy - r.b);
            long long st = max(arrive, (long long)r.s);
            long long fin = st + r.d;
            if (fin > r.f) continue;
            if (fin > T) continue;
            long long gain = r.d + (st == r.s ? B : 0);
            long long key = gain - (st - t);
            if (key > bestKey) {
                bestKey = key;
                best = i;
                bestStart = st;
                bestFinish = fin;
            }
        }

        if (best < 0) continue; // vehicle retires

        used[best] = true;
        assigned[v].push_back(best);
        px[v] = rides[best].x;
        py[v] = rides[best].y;
        ft[v] = bestFinish;
        (void)bestStart;
        pq.push({bestFinish, v});
    }

    string out;
    out.reserve(1 << 20);
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
