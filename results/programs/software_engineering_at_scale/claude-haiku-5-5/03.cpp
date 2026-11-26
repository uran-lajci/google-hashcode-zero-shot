#include <bits/stdc++.h>
using namespace std;

struct Eng {
    int busy = 0;
    int cur = 0;
    bool working = false;
    int bin = -1;
    vector<string> tasks;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int L, G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    unordered_map<string, int> svcId;
    vector<int> svcBin(S);
    vector<int> R(B, 0);
    for (int i = 0; i < S; i++) {
        string name;
        int b;
        cin >> name >> b;
        svcId[name] = i;
        svcBin[i] = b;
        R[b]++;
    }

    vector<string> fname(F);
    vector<int> D(F), U(F);
    vector<vector<int>> rem(F);
    for (int f = 0; f < F; f++) {
        int M;
        cin >> fname[f] >> M >> D[f] >> U[f];
        vector<int> bs;
        for (int k = 0; k < M; k++) {
            string sn;
            cin >> sn;
            bs.push_back(svcBin[svcId[sn]]);
        }
        sort(bs.begin(), bs.end());
        bs.erase(unique(bs.begin(), bs.end()), bs.end());
        rem[f] = bs;
    }

    // Priority: users per unit of total work.
    vector<double> key(F);
    for (int f = 0; f < F; f++) {
        double work = 0;
        for (int b : rem[f]) work += D[f] + R[b];
        key[f] = work > 0 ? (double)U[f] / work : 0.0;
    }
    vector<int> order(F);
    iota(order.begin(), order.end(), 0);
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        if (key[a] != key[b]) return key[a] > key[b];
        return a < b;
    });

    vector<Eng> eng;
    vector<int> cnt(B, 0);

    // Find a feasible (feature, binary) starting at day t.
    auto pick = [&](int t, int &pf, int &pb) -> bool {
        for (int f : order) {
            if (rem[f].empty()) continue;
            int best = -1;
            long long bestVal = LLONG_MAX;
            for (int b : rem[f]) {
                long long end = (long long)t + D[f] + R[b] + cnt[b];
                if (end <= L) {
                    long long v = R[b] + cnt[b];
                    if (v < bestVal) {
                        bestVal = v;
                        best = b;
                    }
                }
            }
            if (best != -1) {
                pf = f;
                pb = best;
                return true;
            }
        }
        return false;
    };

    auto assign = [&](int e, int t, int f, int b) {
        Eng &g = eng[e];
        if (t > g.cur) g.tasks.push_back("wait " + to_string(t - g.cur));
        int end = t + D[f] + R[b] + cnt[b];
        g.tasks.push_back("impl " + fname[f] + " " + to_string(b));
        g.cur = end;
        g.busy = end;
        g.working = true;
        g.bin = b;
        cnt[b]++;
        auto it = find(rem[f].begin(), rem[f].end(), b);
        if (it != rem[f].end()) rem[f].erase(it);
    };

    for (int t = 0; t < L; t++) {
        // Release finished tasks.
        for (auto &g : eng) {
            if (g.working && g.busy <= t) {
                g.working = false;
                cnt[g.bin]--;
                g.bin = -1;
            }
        }
        // Compact the feature order.
        {
            vector<int> nord;
            nord.reserve(order.size());
            for (int f : order)
                if (!rem[f].empty()) nord.push_back(f);
            order.swap(nord);
        }
        // Assign free existing engineers in index order.
        for (int e = 0; e < (int)eng.size(); e++) {
            if (eng[e].working || eng[e].busy > t) continue;
            int pf, pb;
            if (!pick(t, pf, pb)) break;
            assign(e, t, pf, pb);
        }
        // Create new engineers while useful.
        while ((int)eng.size() < G) {
            int pf, pb;
            if (!pick(t, pf, pb)) break;
            eng.push_back(Eng());
            int e = (int)eng.size() - 1;
            assign(e, t, pf, pb);
        }
    }

    // Output.
    ostringstream out;
    out << eng.size() << "\n";
    for (auto &g : eng) {
        out << g.tasks.size() << "\n";
        for (auto &line : g.tasks) out << line << "\n";
    }
    cout << out.str();
    return 0;
}
