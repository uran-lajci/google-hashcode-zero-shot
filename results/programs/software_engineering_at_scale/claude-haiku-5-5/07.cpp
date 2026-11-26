#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll L;
    int G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    unordered_map<string, int> sid;
    vector<int> sbin(S);
    for (int i = 0; i < S; i++) {
        string name;
        int bi;
        cin >> name >> bi;
        sid[name] = i;
        sbin[i] = bi;
    }
    vector<ll> R(B, 0);
    for (int s = 0; s < S; s++) R[sbin[s]]++;

    struct Feat {
        string name;
        ll D, U;
        vector<int> bins;
    };
    vector<Feat> fs(F);
    for (int f = 0; f < F; f++) {
        int M;
        cin >> fs[f].name >> M >> fs[f].D >> fs[f].U;
        set<int> bs;
        for (int m = 0; m < M; m++) {
            string sn;
            cin >> sn;
            auto it = sid.find(sn);
            if (it != sid.end()) bs.insert(sbin[it->second]);
        }
        fs[f].bins.assign(bs.begin(), bs.end());
    }

    // Priority: value per unit of static work.
    vector<double> key(F, 0.0);
    vector<ll> minStatic(F, LLONG_MAX / 4);
    for (int f = 0; f < F; f++) {
        ll work = 0;
        for (int b : fs[f].bins) {
            work += fs[f].D + R[b];
            minStatic[f] = min(minStatic[f], fs[f].D + R[b]);
        }
        if (work <= 0) work = 1;
        key[f] = (double)fs[f].U / (double)work;
    }
    vector<int> order(F);
    for (int f = 0; f < F; f++) order[f] = f;
    sort(order.begin(), order.end(), [&](int a, int c) {
        if (key[a] != key[c]) return key[a] > key[c];
        return a < c;
    });

    // Started flags and remaining counts.
    vector<vector<char>> started(F);
    vector<int> remaining(F);
    for (int f = 0; f < F; f++) {
        started[f].assign(fs[f].bins.size(), 0);
        remaining[f] = (int)fs[f].bins.size();
    }

    // Ongoing task end times per binary, and globally not needed.
    vector<priority_queue<ll, vector<ll>, greater<ll>>> bend(B);

    // Engineer events: (free time, engineer index).
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> ev;
    for (int e = 0; e < G; e++) ev.push({0, e});

    vector<vector<string>> tasks(G);
    vector<int> alive = order;

    while (!ev.empty()) {
        auto [t, e] = ev.top();
        ev.pop();
        if (t >= L) continue;

        // Remove permanently infeasible / exhausted features.
        {
            size_t w = 0;
            for (size_t i = 0; i < alive.size(); i++) {
                int f = alive[i];
                if (remaining[f] > 0 && t + minStatic[f] <= L) {
                    alive[w++] = f;
                }
            }
            alive.resize(w);
        }

        bool done = false;
        for (size_t i = 0; i < alive.size() && !done; i++) {
            int f = alive[i];
            if (remaining[f] <= 0) continue;
            int best = -1;
            ll bestC = 0;
            ll bestDur = 0;
            for (size_t k = 0; k < fs[f].bins.size(); k++) {
                if (started[f][k]) continue;
                int b = fs[f].bins[k];
                while (!bend[b].empty() && bend[b].top() <= t) bend[b].pop();
                ll C = (ll)bend[b].size();
                ll dur = fs[f].D + R[b] + C;
                if (t + dur > L) continue;
                if (best < 0 || C < bestC) {
                    best = (int)k;
                    bestC = C;
                    bestDur = dur;
                }
            }
            if (best >= 0) {
                int b = fs[f].bins[best];
                started[f][best] = 1;
                remaining[f]--;
                ll end = t + bestDur;
                bend[b].push(end);
                tasks[e].push_back("impl " + fs[f].name + " " + to_string(b));
                ev.push({end, e});
                done = true;
            }
        }
        // If nothing feasible, this engineer stays idle.
    }

    int E = 0;
    for (int e = 0; e < G; e++)
        if (!tasks[e].empty()) E++;

    string out;
    out += to_string(E) + "\n";
    for (int e = 0; e < G; e++) {
        if (tasks[e].empty()) continue;
        out += to_string(tasks[e].size()) + "\n";
        for (auto &s : tasks[e]) {
            out += s;
            out += "\n";
        }
    }
    cout << out;
    return 0;
}
