#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    ll L;
    int G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    unordered_map<string, int> sid;
    vector<int> sbin(S);
    for (int i = 0; i < S; i++) {
        string nm;
        int b;
        cin >> nm >> b;
        sid[nm] = i;
        sbin[i] = b;
    }
    vector<ll> R(B, 0);
    for (int s = 0; s < S; s++) R[sbin[s]]++;

    vector<string> fname(F);
    vector<ll> FU(F);
    vector<vector<pair<ll, int>>> fitems(F);  // (base duration, binary), sorted desc
    for (int f = 0; f < F; f++) {
        string nm;
        int M;
        ll D, U;
        cin >> nm >> M >> D >> U;
        fname[f] = nm;
        FU[f] = U;
        vector<int> bins;
        for (int m = 0; m < M; m++) {
            string sn;
            cin >> sn;
            bins.push_back(sbin[sid[sn]]);
        }
        sort(bins.begin(), bins.end());
        bins.erase(unique(bins.begin(), bins.end()), bins.end());
        for (int b : bins) fitems[f].push_back({D + R[b], b});
        sort(fitems[f].begin(), fitems[f].end(), [](const pair<ll, int>& a, const pair<ll, int>& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });
    }

    typedef vector<vector<pair<int, int>>> Tasks;  // per engineer: (feature, binary)

    auto run = [&](double p, Tasks& tasks) -> ll {
        vector<int> ptr(F, 0);
        vector<ll> rem(F, 0), fin(F, 0);
        for (int f = 0; f < F; f++)
            for (auto& it : fitems[f]) rem[f] += it.first;

        priority_queue<pair<double, int>> fh;  // max priority; ties -> smaller index
        auto pushF = [&](int f) {
            double pr = (double)FU[f] / pow((double)rem[f], p);
            fh.push({pr, -f});
        };
        for (int f = 0; f < F; f++)
            if (!fitems[f].empty()) pushF(f);

        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> eh;
        for (int e = 0; e < G; e++) eh.push({0LL, e});

        vector<priority_queue<ll, vector<ll>, greater<ll>>> ends(B);
        tasks.assign(G, {});
        ll score = 0;

        while (!eh.empty()) {
            auto [t, e] = eh.top();
            eh.pop();
            vector<int> deferred;
            bool assigned = false;
            while (!fh.empty()) {
                auto top = fh.top();
                fh.pop();
                int f = -top.second;
                ll base = fitems[f][ptr[f]].first;
                int b = fitems[f][ptr[f]].second;
                if (t + base > L) continue;  // infeasible forever: drop
                auto& pq = ends[b];
                while (!pq.empty() && pq.top() <= t) pq.pop();
                ll C = (ll)pq.size();
                ll dur = base + C;
                if (t + dur > L) {
                    deferred.push_back(f);
                    continue;
                }
                ll end = t + dur;
                pq.push(end);
                tasks[e].push_back({f, b});
                fin[f] = max(fin[f], end);
                rem[f] -= base;
                ptr[f]++;
                if (ptr[f] < (int)fitems[f].size()) {
                    pushF(f);
                } else {
                    score += FU[f] * (L - fin[f]);
                }
                eh.push({end, e});
                assigned = true;
                break;
            }
            for (int f : deferred) pushF(f);
            (void)assigned;
        }
        return score;
    };

    Tasks best;
    ll bestScore = -1;
    for (double p : {0.5, 1.0, 1.5, 2.0}) {
        Tasks cur;
        ll sc = run(p, cur);
        if (sc > bestScore) {
            bestScore = sc;
            best = cur;
        }
    }

    int E = 0;
    for (auto& v : best)
        if (!v.empty()) E++;
    string out;
    out += to_string(E) + "\n";
    for (int e = 0; e < G; e++) {
        if (best[e].empty()) continue;
        out += to_string(best[e].size()) + "\n";
        for (auto& pr : best[e]) {
            out += "impl " + fname[pr.first] + " " + to_string(pr.second) + "\n";
        }
    }
    cout << out;
    return 0;
}
