#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Arc { int to, e; ll c; };

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M; ll T; int C, S;
    cin >> N >> M >> T >> C >> S;
    for (int i = 0; i < N; i++) {
        double la, lo;
        cin >> la >> lo;
    }
    vector<int> A(M), B(M), D(M);
    vector<ll> Cc(M), L(M);
    vector<vector<Arc>> adj(N);
    ll Lmax = 1, cmin = LLONG_MAX;
    for (int j = 0; j < M; j++) {
        cin >> A[j] >> B[j] >> D[j] >> Cc[j] >> L[j];
        adj[A[j]].push_back({B[j], j, Cc[j]});
        if (D[j] == 2) adj[B[j]].push_back({A[j], j, Cc[j]});
        Lmax = max(Lmax, L[j]);
        cmin = min(cmin, Cc[j]);
    }
    if (M == 0) cmin = 1;
    if (cmin < 1) cmin = 1;

    const ll INF = LLONG_MAX / 4;
    vector<ll> dist(N, INF);
    vector<int> prevN(N, -1), prevE(N, -1);
    vector<char> visitedE(M, 0);

    vector<int> pos(C, S);
    vector<ll> tm(C, 0);
    vector<vector<int>> route(C, vector<int>{S});

    // cars ordered by elapsed time
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> carPQ;
    for (int c = 0; c < C; c++) carPQ.push({0, c});

    vector<int> touched;
    while (!carPQ.empty()) {
        auto [curT, car] = carPQ.top();
        carPQ.pop();
        ll R = T - tm[car];
        int src = pos[car];

        touched.clear();
        priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
        dist[src] = 0;
        touched.push_back(src);
        pq.push({0, src});

        double bestVal = -1.0;
        int bestX = -1, bestTo = -1, bestE = -1;
        ll bestD = 0, bestCost = 0;

        while (!pq.empty()) {
            auto [d, x] = pq.top();
            pq.pop();
            if (d > dist[x]) continue;
            if (bestVal > 0 && (double)Lmax / (double)(d + cmin) <= bestVal) break;
            for (const Arc& a : adj[x]) {
                ll nd = d + a.c;
                if (nd > R) continue;
                if (!visitedE[a.e]) {
                    double val = (double)L[a.e] / (double)nd;
                    if (val > bestVal) {
                        bestVal = val;
                        bestX = x;
                        bestTo = a.to;
                        bestE = a.e;
                        bestD = d;
                        bestCost = a.c;
                    }
                }
                if (nd < dist[a.to]) {
                    if (dist[a.to] == INF) touched.push_back(a.to);
                    dist[a.to] = nd;
                    prevN[a.to] = x;
                    prevE[a.to] = a.e;
                    pq.push({nd, a.to});
                }
            }
        }

        if (bestVal < 0) {
            // this car can no longer add anything; retire it
            for (int v : touched) { dist[v] = INF; prevN[v] = -1; prevE[v] = -1; }
            continue;
        }

        // reconstruct path src -> bestX
        vector<int> path; // nodes after src up to bestX
        vector<int> pathEdges;
        {
            int y = bestX;
            while (y != src) {
                path.push_back(y);
                visitedE[prevE[y]] = 1;
                y = prevN[y];
            }
            reverse(path.begin(), path.end());
        }
        for (int v : touched) { dist[v] = INF; prevN[v] = -1; prevE[v] = -1; }

        for (int v : path) route[car].push_back(v);
        route[car].push_back(bestTo);
        visitedE[bestE] = 1;
        tm[car] += bestD + bestCost;
        pos[car] = bestTo;
        if (tm[car] < T) carPQ.push({tm[car], car});
    }

    // output
    string out;
    out.reserve(1 << 20);
    out += to_string(C);
    out += '\n';
    for (int c = 0; c < C; c++) {
        out += to_string(route[c].size());
        out += '\n';
        for (int v : route[c]) {
            out += to_string(v);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
