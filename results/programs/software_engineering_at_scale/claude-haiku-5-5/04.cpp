#include <bits/stdc++.h>
using namespace std;

struct Feat {
    string name;
    long long D = 0;
    long long U = 0;
    vector<int> bins;
    vector<long long> dur;
    vector<char> asg;
    int rem = 0;
    double ratio = 0.0;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long L, G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    unordered_map<string, int> sid;
    vector<int> binOf(S);
    vector<long long> R(B, 0);
    for (int i = 0; i < S; ++i) {
        string nm;
        int b;
        cin >> nm >> b;
        sid[nm] = i;
        binOf[i] = b;
        R[b]++;
    }

    vector<Feat> fs(F);
    for (int f = 0; f < F; ++f) {
        string name;
        int M;
        long long D, U;
        cin >> name >> M >> D >> U;
        vector<int> bs;
        for (int m = 0; m < M; ++m) {
            string s;
            cin >> s;
            bs.push_back(binOf[sid[s]]);
        }
        sort(bs.begin(), bs.end());
        bs.erase(unique(bs.begin(), bs.end()), bs.end());
        Feat& ft = fs[f];
        ft.name = name;
        ft.D = D;
        ft.U = U;
        ft.bins = bs;
        double W = 0;
        for (int b : bs) {
            long long d = D + R[b];
            ft.dur.push_back(d);
            W += (double)d;
        }
        ft.asg.assign(bs.size(), 0);
        ft.rem = (int)bs.size();
        ft.ratio = (W > 0) ? (double)U / W : 0.0;
    }

    vector<int> order(F);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (fs[a].ratio != fs[b].ratio) return fs[a].ratio > fs[b].ratio;
        return a < b;
    });

    vector<long long> busy(B, 0);
    multiset<long long> releases;
    vector<vector<string>> tasks(G);
    vector<long long> last(G, 0);

    // min-heap of (time, engineer)
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int e = 0; e < G; ++e) pq.push({0, e});

    long long noneAt = -1;

    while (!pq.empty()) {
        auto [t, e] = pq.top();
        pq.pop();
        if (t >= L) break;

        // Remove releases that are not in the future.
        while (!releases.empty() && *releases.begin() <= t) releases.erase(releases.begin());

        int fsel = -1, ksel = -1;
        if (noneAt != t) {
            for (int fi : order) {
                Feat& ft = fs[fi];
                if (ft.rem == 0) continue;
                for (size_t k = 0; k < ft.bins.size(); ++k) {
                    if (ft.asg[k]) continue;
                    int b = ft.bins[k];
                    if (busy[b] > t) continue;
                    if (t + ft.dur[k] > L) continue;
                    fsel = fi;
                    ksel = (int)k;
                    break;
                }
                if (fsel >= 0) break;
            }
            if (fsel < 0) noneAt = t;
        }

        if (fsel >= 0) {
            Feat& ft = fs[fsel];
            int b = ft.bins[ksel];
            long long d = ft.dur[ksel];
            if (t > last[e]) {
                tasks[e].push_back("wait " + to_string(t - last[e]));
            }
            tasks[e].push_back("impl " + ft.name + " " + to_string(b));
            ft.asg[ksel] = 1;
            ft.rem--;
            busy[b] = t + d;
            releases.insert(t + d);
            last[e] = t + d;
            pq.push({t + d, e});
        } else {
            // Idle: wait until the next binary becomes free, if any.
            if (!releases.empty()) {
                pq.push({*releases.begin(), e});
            }
            // Otherwise the engineer retires.
        }
    }

    int used = 0;
    for (int e = 0; e < G; ++e)
        if (!tasks[e].empty()) ++used;

    string out;
    out.reserve(1 << 20);
    out += to_string(used) + "\n";
    for (int e = 0; e < G; ++e) {
        if (tasks[e].empty()) continue;
        out += to_string(tasks[e].size()) + "\n";
        for (const string& s : tasks[e]) {
            out += s;
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
