#include <bits/stdc++.h>
using namespace std;

struct Entry {
    long long key;   // c * V + v
    int p;           // request-pair index
    int lc;          // latency from cache to endpoint
};

struct Group {
    int c, v;
    int start, end; // range in entries
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E, R, C;
    long long X;
    cin >> V >> E >> R >> C >> X;

    vector<int> S(V);
    for (int i = 0; i < V; i++) cin >> S[i];

    vector<int> LD(E);
    vector<vector<pair<int,int>>> conn(E); // (cache, Lc)
    for (int e = 0; e < E; e++) {
        int K;
        cin >> LD[e] >> K;
        conn[e].resize(K);
        for (int k = 0; k < K; k++) {
            int c, lc;
            cin >> c >> lc;
            conn[e][k] = {c, lc};
        }
    }

    // Read requests, aggregate by (video, endpoint)
    struct Req { long long key; int v, e; long long n; };
    vector<Req> reqs(R);
    for (int i = 0; i < R; i++) {
        int v, e;
        long long n;
        cin >> v >> e >> n;
        reqs[i] = {(long long)v * E + e, v, e, n};
    }
    sort(reqs.begin(), reqs.end(), [](const Req& a, const Req& b) { return a.key < b.key; });

    vector<int> pv, pe;
    vector<long long> pn;
    for (size_t i = 0; i < reqs.size();) {
        size_t j = i;
        long long total = 0;
        while (j < reqs.size() && reqs[j].key == reqs[i].key) {
            total += reqs[j].n;
            j++;
        }
        pv.push_back(reqs[i].v);
        pe.push_back(reqs[i].e);
        pn.push_back(total);
        i = j;
    }
    int P = pv.size();
    reqs.clear();
    reqs.shrink_to_fit();

    vector<long long> best(P);
    for (int p = 0; p < P; p++) best[p] = LD[pe[p]];

    // Build entries: for each request pair, each connected cache with lower latency
    vector<Entry> entries;
    for (int p = 0; p < P; p++) {
        int e = pe[p], v = pv[p];
        for (auto [c, lc] : conn[e]) {
            if (lc < LD[e]) {
                entries.push_back({(long long)c * V + v, p, lc});
            }
        }
    }
    sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.key < b.key;
    });

    vector<Group> groups;
    for (size_t i = 0; i < entries.size();) {
        size_t j = i;
        while (j < entries.size() && entries[j].key == entries[i].key) j++;
        int c = (int)(entries[i].key / V);
        int v = (int)(entries[i].key % V);
        groups.push_back({c, v, (int)i, (int)j});
        i = j;
    }

    auto gainOf = [&](const Group& g) -> long long {
        long long gain = 0;
        for (int i = g.start; i < g.end; i++) {
            const Entry& en = entries[i];
            long long d = best[en.p] - en.lc;
            if (d > 0) gain += pn[en.p] * d;
        }
        return gain;
    };

    priority_queue<pair<double,int>> pq;
    for (int g = 0; g < (int)groups.size(); g++) {
        long long gain = gainOf(groups[g]);
        if (gain > 0) {
            pq.push({(double)gain / S[groups[g].v], g});
        }
    }

    vector<long long> used(C, 0);
    vector<vector<int>> chosen(C);

    while (!pq.empty()) {
        auto [key, g] = pq.top();
        pq.pop();
        const Group& gr = groups[g];
        if (used[gr.c] + S[gr.v] > X) continue;
        long long gain = gainOf(gr);
        if (gain <= 0) continue;
        double nk = (double)gain / S[gr.v];
        if (!pq.empty() && nk < pq.top().first) {
            pq.push({nk, g});
            continue;
        }
        // Accept
        used[gr.c] += S[gr.v];
        chosen[gr.c].push_back(gr.v);
        for (int i = gr.start; i < gr.end; i++) {
            const Entry& en = entries[i];
            if (en.lc < best[en.p]) best[en.p] = en.lc;
        }
    }

    int N = 0;
    for (int c = 0; c < C; c++) if (!chosen[c].empty()) N++;

    string out;
    out += to_string(N) + "\n";
    for (int c = 0; c < C; c++) {
        if (chosen[c].empty()) continue;
        out += to_string(c);
        for (int v : chosen[c]) {
            out += ' ';
            out += to_string(v);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
