#include <bits/stdc++.h>
using namespace std;

struct Feat {
    string name;
    int D;
    long long U;
    vector<int> bins;   // binaries still needing this feature
    double key;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int L, G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    unordered_map<string, int> binOfName;
    vector<int> binCount(B, 0);
    for (int i = 0; i < S; ++i) {
        string name;
        int bi;
        cin >> name >> bi;
        binOfName[name] = bi;
        binCount[bi]++;
    }

    vector<Feat> feats(F);
    for (int f = 0; f < F; ++f) {
        int M;
        cin >> feats[f].name >> M >> feats[f].D >> feats[f].U;
        vector<int> bs;
        for (int k = 0; k < M; ++k) {
            string sn;
            cin >> sn;
            bs.push_back(binOfName[sn]);
        }
        sort(bs.begin(), bs.end());
        bs.erase(unique(bs.begin(), bs.end()), bs.end());
        feats[f].bins = bs;
        double W = 0;
        for (int b : bs) W += feats[f].D + binCount[b];
        feats[f].key = W > 0 ? (double)feats[f].U / W : 0.0;
    }

    // Features ordered by priority (descending key, then index)
    vector<int> order(F);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (feats[a].key != feats[b].key) return feats[a].key > feats[b].key;
        return a < b;
    });
    // Remove features that need nothing (none expected, but safe)
    order.erase(remove_if(order.begin(), order.end(),
                          [&](int f) { return feats[f].bins.empty(); }),
                order.end());

    // End times of implementation tasks per binary (min-heaps).
    vector<priority_queue<int, vector<int>, greater<int>>> ends(B);
    auto countActive = [&](int b, int t) -> int {
        while (!ends[b].empty() && ends[b].top() <= t) ends[b].pop();
        return (int)ends[b].size();
    };

    // Engineers keyed by (free time, index). Processed in nondecreasing time.
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    for (int e = 0; e < G; ++e) pq.push({0, e});

    vector<vector<pair<int, int>>> plan(G); // (feature index, binary)

    while (!pq.empty()) {
        auto [t, e] = pq.top();
        pq.pop();
        if (t >= L) continue;

        int bestF = -1, bestB = -1, bestDur = 0;
        for (size_t oi = 0; oi < order.size(); ++oi) {
            int f = order[oi];
            int bDur = INT_MAX, bBin = -1;
            for (int b : feats[f].bins) {
                int c = countActive(b, t);
                int d = feats[f].D + binCount[b] + c;
                if (t + d < L && d < bDur) {
                    bDur = d;
                    bBin = b;
                }
            }
            if (bBin >= 0) {
                bestF = f;
                bestB = bBin;
                bestDur = bDur;
                break;
            }
        }
        if (bestF < 0) continue; // nothing feasible for this engineer

        // Commit the task.
        ends[bestB].push(t + bestDur);
        plan[e].push_back({bestF, bestB});
        auto &bins = feats[bestF].bins;
        bins.erase(find(bins.begin(), bins.end(), bestB));
        if (bins.empty()) {
            order.erase(find(order.begin(), order.end(), bestF));
        }
        pq.push({t + bestDur, e});
    }

    // Output
    int E = 0;
    for (int e = 0; e < G; ++e)
        if (!plan[e].empty()) ++E;

    string out;
    out += to_string(E) + "\n";
    for (int e = 0; e < G; ++e) {
        if (plan[e].empty()) continue;
        out += to_string(plan[e].size()) + "\n";
        for (auto [f, b] : plan[e]) {
            out += "impl " + feats[f].name + " " + to_string(b) + "\n";
        }
    }
    cout << out;
    return 0;
}
