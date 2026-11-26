#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> Bs, Es, Ls;
vector<string> names;
vector<vector<int>> paths;

// Per-street schedule info
struct StreetSched {
    bool has = false;
    int off = 0, dur = 0, cyc = 1;
};

vector<StreetSched> applySchedule(const vector<vector<pair<int,int>>>& sched) {
    vector<StreetSched> ss(S);
    for (int i = 0; i < I; i++) {
        if (sched[i].empty()) continue;
        int total = 0;
        for (auto& pr : sched[i]) total += pr.second;
        int off = 0;
        for (auto& pr : sched[i]) {
            StreetSched& x = ss[pr.first];
            x.has = true;
            x.off = off;
            x.dur = pr.second;
            x.cyc = total;
            off += pr.second;
        }
    }
    return ss;
}

long long simulate(const vector<StreetSched>& ss) {
    int V_ = paths.size();
    vector<long long> last(S, -1);
    // event: (time, car, pos)
    priority_queue<tuple<long long,int,int>, vector<tuple<long long,int,int>>,
                   greater<tuple<long long,int,int>>> pq;
    for (int c = 0; c < V_; c++) pq.emplace(0LL, c, 0);
    long long score = 0;
    while (!pq.empty()) {
        auto [t, c, p] = pq.top();
        pq.pop();
        const vector<int>& path = paths[c];
        int s = path[p];
        if (p == (int)path.size() - 1) {
            if (t <= D) score += F + (D - t);
            continue;
        }
        const StreetSched& x = ss[s];
        if (!x.has) continue;
        long long tc = max(t, last[s] + 1);
        long long r = tc % x.cyc;
        long long k = tc - r;
        long long cross;
        if (r >= x.off && r < x.off + x.dur) cross = tc;
        else if (r < x.off) cross = k + x.off;
        else cross = k + x.cyc + x.off;
        last[s] = cross;
        if (cross >= D) continue;
        int ns = path[p + 1];
        long long ta = cross + Ls[ns];
        if (ta > D) continue;
        pq.emplace(ta, c, p + 1);
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> D >> I >> S >> V >> F;
    Bs.resize(S); Es.resize(S); Ls.resize(S); names.resize(S);
    unordered_map<string,int> id;
    id.reserve(S * 2);
    for (int s = 0; s < S; s++) {
        cin >> Bs[s] >> Es[s] >> names[s] >> Ls[s];
        id[names[s]] = s;
    }
    paths.resize(V);
    for (int c = 0; c < V; c++) {
        int P;
        cin >> P;
        paths[c].resize(P);
        for (int j = 0; j < P; j++) {
            string nm;
            cin >> nm;
            paths[c][j] = id[nm];
        }
    }

    // Count usage and free-flow first arrival per street (street as incoming)
    vector<long long> cnt(S, 0), firstE(S, LLONG_MAX);
    for (int c = 0; c < V; c++) {
        const auto& path = paths[c];
        long long cur = 0;
        for (size_t p = 0; p + 1 < path.size(); p++) {
            int s = path[p];
            cnt[s]++;
            firstE[s] = min(firstE[s], cur);
            cur += Ls[path[p + 1]];
        }
    }

    vector<vector<int>> incoming(I);
    for (int s = 0; s < S; s++)
        if (cnt[s] > 0) incoming[Es[s]].push_back(s);

    // Candidate generation
    auto build = [&](int mode, int m) {
        vector<vector<pair<int,int>>> sched(I);
        for (int i = 0; i < I; i++) {
            vector<int> lst = incoming[i];
            if (lst.empty()) continue;
            sort(lst.begin(), lst.end(), [&](int a, int b) {
                if (mode == 0) {
                    if (firstE[a] != firstE[b]) return firstE[a] < firstE[b];
                } else {
                    if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
                }
                return a < b;
            });
            for (int s : lst) {
                long long d = cnt[s] * m;
                d = max(1LL, min((long long)D, d));
                sched[i].push_back({s, (int)d});
            }
        }
        return sched;
    };

    long long bestScore = -1;
    vector<vector<pair<int,int>>> bestSched(I);
    for (int mode = 0; mode < 2; mode++) {
        for (int m : {1, 2, 4}) {
            auto sched = build(mode, m);
            long long sc = simulate(applySchedule(sched));
            if (sc > bestScore) {
                bestScore = sc;
                bestSched = sched;
            }
        }
    }

    // Output
    vector<int> used;
    for (int i = 0; i < I; i++)
        if (!bestSched[i].empty()) used.push_back(i);
    string out;
    out += to_string(used.size()) + "\n";
    for (int i : used) {
        out += to_string(i) + "\n";
        out += to_string(bestSched[i].size()) + "\n";
        for (auto& pr : bestSched[i]) {
            out += names[pr.first] + " " + to_string(pr.second) + "\n";
        }
    }
    cout << out;
    return 0;
}
