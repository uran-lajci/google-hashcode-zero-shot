#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long L;
    int G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    map<string, int> sid;
    vector<int> sbin(S);
    vector<long long> R(B, 0);
    for (int i = 0; i < S; i++) {
        string nm;
        int b;
        cin >> nm >> b;
        sid[nm] = i;
        sbin[i] = b;
        R[b]++;
    }

    struct Feat {
        string name;
        long long D, U;
        vector<int> bins;
        long long work;
    };
    vector<Feat> fs(F);
    for (int i = 0; i < F; i++) {
        string nm;
        int M;
        long long D, U;
        cin >> nm >> M >> D >> U;
        vector<int> bins;
        for (int j = 0; j < M; j++) {
            string sn;
            cin >> sn;
            bins.push_back(sbin[sid[sn]]);
        }
        sort(bins.begin(), bins.end());
        bins.erase(unique(bins.begin(), bins.end()), bins.end());
        long long work = 0;
        for (int b : bins) work += D + R[b];
        fs[i] = {nm, D, U, bins, work};
    }

    // Priority: value per unit of work, descending.
    vector<int> order(F);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        // U_a / work_a > U_b / work_b
        long long lhs = fs[a].U * fs[b].work;
        long long rhs = fs[b].U * fs[a].work;
        if (lhs != rhs) return lhs > rhs;
        return a < b;
    });

    // Work items: (feature, binary) pairs in priority order.
    vector<pair<int, int>> items;
    for (int f : order)
        for (int b : fs[f].bins)
            items.push_back({f, b});

    // Engineers: min-heap by (free time, index).
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int e = 0; e < G; e++) pq.push({0LL, e});

    // Per-binary end times of ongoing feature tasks.
    vector<priority_queue<long long, vector<long long>, greater<long long>>> ends(B);

    vector<vector<string>> tasks(G);
    size_t head = 0;

    while (!pq.empty() && head < items.size()) {
        auto [t, e] = pq.top();
        pq.pop();
        if (t >= L) break;

        while (head < items.size()) {
            auto [f, b] = items[head];
            head++;
            auto &q = ends[b];
            while (!q.empty() && q.top() <= t) q.pop();
            long long cost = fs[f].D + R[b] + (long long)q.size();
            if (t + cost >= L) continue; // cannot complete in time; drop
            q.push(t + cost);
            tasks[e].push_back("impl " + fs[f].name + " " + to_string(b));
            pq.push({t + cost, e});
            break;
        }
    }

    int E = 0;
    for (int e = 0; e < G; e++)
        if (!tasks[e].empty()) E++;

    string out;
    out += to_string(E) + "\n";
    for (int e = 0; e < G; e++) {
        if (tasks[e].empty()) continue;
        out += to_string(tasks[e].size()) + "\n";
        for (const auto &s : tasks[e]) {
            out += s;
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
