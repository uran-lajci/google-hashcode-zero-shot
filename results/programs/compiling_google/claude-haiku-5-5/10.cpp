#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, T, S;
    cin >> C >> T >> S;

    vector<string> name(C);
    vector<ll> c(C), r(C);
    vector<vector<int>> deps(C);
    unordered_map<string, int> id;
    id.reserve(C * 2 + 1);

    for (int i = 0; i < C; i++) {
        cin >> name[i] >> c[i] >> r[i];
        id[name[i]] = i;
        int n;
        cin >> n;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string s;
            cin >> s;
            deps[i][k] = id.at(s);
        }
    }

    vector<char> isT(C, 0);
    vector<ll> D(C, 0);
    for (int t = 0; t < T; t++) {
        string s;
        ll d, g;
        cin >> s >> d >> g;
        int k = id.at(s);
        isT[k] = 1;
        D[k] = d;
    }

    const ll INF = (ll)1e18;

    // needed: targets and all their ancestors (deps have smaller indices)
    vector<char> need(C, 0);
    for (int i = 0; i < C; i++)
        if (isT[i]) need[i] = 1;
    for (int i = C - 1; i >= 0; i--)
        if (need[i])
            for (int p : deps[i]) need[p] = 1;

    // latest finish time allowed for each file (same-server, no replication delay)
    vector<ll> LF(C, INF);
    for (int i = 0; i < C; i++)
        if (isT[i]) LF[i] = D[i];
    for (int i = C - 1; i >= 0; i--) {
        if (!need[i]) continue;
        for (int p : deps[i])
            LF[p] = min(LF[p], LF[i] - c[i]);
    }

    vector<vector<int>> ch(C);
    vector<int> indeg(C, 0);
    for (int i = 0; i < C; i++) {
        indeg[i] = (int)deps[i].size();
        for (int p : deps[i]) ch[p].push_back(i);
    }

    vector<ll> fin(C, 0);
    vector<int> srv(C, -1);
    vector<char> dead(C, 0);
    vector<ll> freeT(S, 0);
    vector<pair<int, int>> steps;

    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int i = 0; i < C; i++)
        if (need[i] && indeg[i] == 0)
            pq.push({LF[i] - c[i], i});

    while (!pq.empty()) {
        int f = pq.top().second;
        pq.pop();

        bool bad = false;
        for (int p : deps[f])
            if (dead[p]) { bad = true; break; }

        if (!bad) {
            ll best = INF;
            int bs = -1;
            for (int s = 0; s < S; s++) {
                ll st = freeT[s];
                for (int p : deps[f]) {
                    ll av = (srv[p] == s) ? fin[p] : fin[p] + r[p];
                    if (av > st) st = av;
                }
                ll fn = st + c[f];
                if (fn < best) {
                    best = fn;
                    bs = s;
                }
            }
            if (best > LF[f]) {
                bad = true;
            } else {
                fin[f] = best;
                srv[f] = bs;
                freeT[bs] = best;
                steps.push_back({f, bs});
            }
        }
        if (bad) dead[f] = 1;

        for (int j : ch[f]) {
            if (!need[j]) continue;
            if (--indeg[j] == 0)
                pq.push({LF[j] - c[j], j});
        }
    }

    if (steps.empty()) {
        // Fallback: a valid (zero-score) submission.
        steps.push_back({0, 0});
    }

    string out;
    out.reserve(steps.size() * 16 + 16);
    out += to_string(steps.size());
    out += '\n';
    for (auto &st : steps) {
        out += name[st.first];
        out += ' ';
        out += to_string(st.second);
        out += '\n';
    }
    cout << out;
    return 0;
}
