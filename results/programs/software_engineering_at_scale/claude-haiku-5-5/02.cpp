#include <bits/stdc++.h>
using namespace std;

struct Feat {
    string name;
    int D;
    long long U;
    vector<int> bins;
};

int L, G, S, B, F, N;
vector<int> R;
vector<Feat> fs;

// Range-add / point-query Fenwick per binary (diff-array BIT).
struct Fen {
    vector<int> a;
    void init(int n) { a.assign(n + 1, 0); }
    void add(int i, int v) {
        for (; i < (int)a.size(); i += i & -i) a[i] += v;
    }
    int sum(int i) const {
        int s = 0;
        for (; i > 0; i -= i & -i) s += a[i];
        return s;
    }
};

struct Job {
    int b, t, end, e;
};

pair<long long, vector<vector<string>>> runSchedule(const vector<int>& order) {
    set<pair<int, int>> pq;  // (free time, engineer)
    for (int e = 0; e < G; e++) pq.insert({0, e});

    vector<Fen> fen(B);
    for (int b = 0; b < B; b++) fen[b].init(L + 3);

    vector<vector<string>> tasks(G);
    long long score = 0;

    for (int f : order) {
        const Feat& ft = fs[f];
        vector<Job> jobs;
        bool ok = true;
        int maxEnd = 0;

        for (int b : ft.bins) {
            if (pq.empty()) { ok = false; break; }
            auto it = pq.begin();
            int t = it->first, e = it->second;
            pq.erase(it);
            if (t >= L) {
                pq.insert({t, e});
                ok = false;
                break;
            }
            // Engineers working on binary b at day t (committed intervals).
            int C = fen[b].sum(t + 1);
            int dur = ft.D + R[b] + C;
            int end = t + dur;
            pq.insert({end, e});
            jobs.push_back({b, t, end, e});
            maxEnd = max(maxEnd, end);
            if (end >= L) { ok = false; break; }
        }

        if (!ok || maxEnd >= L) {
            // Rollback in reverse order.
            for (int i = (int)jobs.size() - 1; i >= 0; i--) {
                pq.erase({jobs[i].end, jobs[i].e});
                pq.insert({jobs[i].t, jobs[i].e});
            }
            continue;
        }

        // Commit.
        for (const Job& j : jobs) {
            // Interval [t, end-1] is active for binary j.b.
            fen[j.b].add(j.t + 1, 1);
            fen[j.b].add(j.end - 1 + 2, -1);
            tasks[j.e].push_back("impl " + ft.name + " " + to_string(j.b));
        }
        score += ft.U * (long long)(L - maxEnd);
    }
    return {score, tasks};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> L >> G >> S >> B >> F >> N;
    R.assign(B, 0);
    unordered_map<string, int> serviceBin;
    serviceBin.reserve(S * 2 + 1);
    for (int i = 0; i < S; i++) {
        string name;
        int b;
        cin >> name >> b;
        serviceBin[name] = b;
        R[b]++;
    }

    fs.resize(F);
    for (int i = 0; i < F; i++) {
        int M;
        cin >> fs[i].name >> M >> fs[i].D >> fs[i].U;
        vector<int> bs;
        for (int k = 0; k < M; k++) {
            string s;
            cin >> s;
            auto it = serviceBin.find(s);
            if (it != serviceBin.end()) bs.push_back(it->second);
        }
        sort(bs.begin(), bs.end());
        bs.erase(unique(bs.begin(), bs.end()), bs.end());
        fs[i].bins = bs;
    }

    // Candidate orderings.
    vector<double> base(F, 0.0);
    for (int i = 0; i < F; i++) {
        double s = 0;
        for (int b : fs[i].bins) s += fs[i].D + R[b];
        base[i] = s;
    }

    vector<vector<int>> orders;
    {
        vector<int> o(F);
        iota(o.begin(), o.end(), 0);
        // Density: U / total base work.
        sort(o.begin(), o.end(), [&](int a, int b) {
            double ka = fs[a].U / max(1.0, base[a]);
            double kb = fs[b].U / max(1.0, base[b]);
            if (ka != kb) return ka > kb;
            return a < b;
        });
        orders.push_back(o);
    }
    {
        vector<int> o(F);
        iota(o.begin(), o.end(), 0);
        sort(o.begin(), o.end(), [&](int a, int b) {
            if (fs[a].U != fs[b].U) return fs[a].U > fs[b].U;
            return a < b;
        });
        orders.push_back(o);
    }
    {
        vector<int> o(F);
        iota(o.begin(), o.end(), 0);
        sort(o.begin(), o.end(), [&](int a, int b) {
            double ka = (double)fs[a].U / (fs[a].D + 1.0);
            double kb = (double)fs[b].U / (fs[b].D + 1.0);
            if (ka != kb) return ka > kb;
            return a < b;
        });
        orders.push_back(o);
    }

    long long bestScore = -1;
    vector<vector<string>> bestTasks;
    for (const auto& o : orders) {
        auto res = runSchedule(o);
        if (res.first > bestScore) {
            bestScore = res.first;
            bestTasks = move(res.second);
        }
    }

    int E = 0;
    for (int e = 0; e < G; e++)
        if (!bestTasks[e].empty()) E++;

    string out;
    out += to_string(E) + "\n";
    for (int e = 0; e < G; e++) {
        if (bestTasks[e].empty()) continue;
        out += to_string(bestTasks[e].size()) + "\n";
        for (const string& s : bestTasks[e]) {
            out += s;
            out += "\n";
        }
    }
    cout << out;
    return 0;
}
