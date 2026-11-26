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
    id.reserve(C * 2 + 10);

    for (int i = 0; i < C; i++) {
        cin >> name[i] >> c[i] >> r[i];
        int n;
        cin >> n;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string s;
            cin >> s;
            deps[i][k] = id[s];
        }
        id[name[i]] = i;
    }

    const ll INF = LLONG_MAX / 4;
    vector<ll> dl(C, -1);
    vector<char> isT(C, 0);
    for (int t = 0; t < T; t++) {
        string s;
        ll d, g;
        cin >> s >> d >> g;
        int i = id[s];
        isT[i] = 1;
        dl[i] = d;
    }

    // Needed files: ancestors of targets (including targets).
    vector<char> need(C, 0);
    for (int i = 0; i < C; i++) need[i] = isT[i];
    for (int i = C - 1; i >= 0; i--) {
        if (!need[i]) continue;
        for (int p : deps[i]) need[p] = 1;
    }

    // Latest finish time to still meet some target deadline through descendants.
    vector<ll> lf(C, INF);
    for (int i = 0; i < C; i++) if (isT[i]) lf[i] = dl[i];
    for (int i = C - 1; i >= 0; i--) {
        if (!need[i]) continue;
        for (int p : deps[i]) lf[p] = min(lf[p], lf[i] - c[i]);
    }

    // Build children lists and indegrees among needed files.
    vector<vector<int>> children(C);
    vector<int> indeg(C, 0);
    for (int i = 0; i < C; i++) {
        if (!need[i]) continue;
        for (int p : deps[i]) {
            children[p].push_back(i);
            indeg[i]++;
        }
    }

    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int i = 0; i < C; i++) {
        if (need[i] && indeg[i] == 0) pq.push({lf[i], i});
    }

    vector<ll> avail((size_t)C * S, INF);
    vector<ll> freeAt(S, 0);
    vector<ll> tmp(S);
    vector<pair<int, int>> steps;
    steps.reserve(1024);

    while (!pq.empty()) {
        int f = pq.top().second;
        pq.pop();

        for (int s = 0; s < S; s++) tmp[s] = freeAt[s];
        for (int p : deps[f]) {
            const ll* av = &avail[(size_t)p * S];
            for (int s = 0; s < S; s++) tmp[s] = max(tmp[s], av[s]);
        }

        int best = 0;
        for (int s = 1; s < S; s++) {
            if (tmp[s] < tmp[best]) best = s;
        }
        ll fin = tmp[best] + c[f];
        freeAt[best] = fin;

        ll* af = &avail[(size_t)f * S];
        for (int s = 0; s < S; s++) {
            ll v = fin + (s == best ? 0 : r[f]);
            if (v < af[s]) af[s] = v;
        }

        steps.push_back({f, best});

        for (int j : children[f]) {
            if (--indeg[j] == 0) pq.push({lf[j], j});
        }
    }

    string out;
    out.reserve(steps.size() * 16 + 16);
    out += to_string(steps.size());
    out += '\n';
    for (auto& st : steps) {
        out += name[st.first];
        out += ' ';
        out += to_string(st.second);
        out += '\n';
    }
    cout << out;
    return 0;
}
