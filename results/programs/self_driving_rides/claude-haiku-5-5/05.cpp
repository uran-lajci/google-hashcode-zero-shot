#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct V {
    ll t;
    int r, c;
    int id;
    bool operator>(const V& o) const {
        if (t != o.t) return t > o.t;
        return id > o.id;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll R, C, F, N, B, T;
    cin >> R >> C >> F >> N >> B >> T;
    vector<ll> a(N), b(N), x(N), y(N), s(N), f(N), dist(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i] >> b[i] >> x[i] >> y[i] >> s[i] >> f[i];
        dist[i] = llabs(x[i] - a[i]) + llabs(y[i] - b[i]);
    }

    priority_queue<V, vector<V>, greater<V>> pq;
    for (int i = 0; i < F; ++i) pq.push(V{0, 0, 0, i});

    vector<char> used(N, 0);
    vector<vector<int>> assigned(F);

    while (!pq.empty()) {
        V v = pq.top();
        pq.pop();

        int best = -1;
        double bestVal = -1.0;
        ll bestFin = 0;

        for (int i = 0; i < N; ++i) {
            if (used[i]) continue;
            ll d0 = llabs(a[i] - v.r) + llabs(b[i] - v.c);
            ll arr = v.t + d0;
            ll st = max(arr, s[i]);
            ll fin = st + dist[i];
            if (fin > f[i]) continue;
            ll pts = dist[i] + (st == s[i] ? B : 0);
            double val = (double)pts / (double)(fin - v.t);
            if (best == -1 || val > bestVal ||
                (val == bestVal && fin < bestFin)) {
                best = i;
                bestVal = val;
                bestFin = fin;
            }
        }

        if (best == -1) continue; // this vehicle has no more feasible rides

        used[best] = 1;
        assigned[v.id].push_back(best);
        pq.push(V{bestFin, (int)x[best], (int)y[best], v.id});
    }

    string out;
    for (int i = 0; i < F; ++i) {
        out += to_string(assigned[i].size());
        for (int r : assigned[i]) {
            out += ' ';
            out += to_string(r);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
