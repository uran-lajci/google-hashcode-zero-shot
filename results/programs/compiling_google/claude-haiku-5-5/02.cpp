#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, T, S;
    cin >> C >> T >> S;

    vector<string> name(C);
    vector<long long> ct(C), rt(C);
    vector<vector<int>> deps(C);
    unordered_map<string, int> id;
    id.reserve(C * 2 + 1);

    for (int i = 0; i < C; i++) {
        int n;
        cin >> name[i] >> ct[i] >> rt[i] >> n;
        id[name[i]] = i;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string dn;
            cin >> dn;
            deps[i][k] = id[dn];
        }
    }

    const long long INF = LLONG_MAX / 4;
    vector<long long> eff(C, INF);
    vector<char> isTarget(C, 0);
    vector<int> targets;
    for (int t = 0; t < T; t++) {
        string tn;
        long long d, g;
        cin >> tn >> d >> g;
        int i = id[tn];
        isTarget[i] = 1;
        eff[i] = min(eff[i], d);
        targets.push_back(i);
    }

    // Needed set: ancestors of targets (including targets).
    vector<char> needed(C, 0);
    {
        vector<int> st;
        for (int t : targets) {
            if (!needed[t]) {
                needed[t] = 1;
                st.push_back(t);
            }
        }
        while (!st.empty()) {
            int u = st.back();
            st.pop_back();
            for (int p : deps[u]) {
                if (!needed[p]) {
                    needed[p] = 1;
                    st.push_back(p);
                }
            }
        }
    }

    // Effective deadlines: propagate backwards (deps have smaller index).
    for (int i = C - 1; i >= 0; i--) {
        if (!needed[i]) continue;
        if (eff[i] >= INF) continue;
        for (int p : deps[i]) {
            eff[p] = min(eff[p], eff[i] - ct[i]);
        }
    }
    // Needed nodes that have no finite eff (shouldn't happen, but be safe).
    for (int i = 0; i < C; i++) {
        if (needed[i] && eff[i] >= INF) eff[i] = INF - 1;
    }

    // Kahn's algorithm over needed nodes.
    vector<int> indeg(C, 0);
    vector<vector<int>> children(C);
    for (int i = 0; i < C; i++) {
        if (!needed[i]) continue;
        for (int p : deps[i]) {
            children[p].push_back(i);
            indeg[i]++;
        }
    }

    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    for (int i = 0; i < C; i++) {
        if (needed[i] && indeg[i] == 0) pq.push({eff[i], i});
    }

    vector<long long> freeAt(S, 0);
    vector<long long> avail((size_t)C * S, INF);
    vector<pair<int, int>> steps; // (file, server)
    vector<long long> start(S);

    while (!pq.empty()) {
        int f = pq.top().second;
        pq.pop();

        for (int s = 0; s < S; s++) start[s] = freeAt[s];
        for (int p : deps[f]) {
            const long long* a = &avail[(size_t)p * S];
            for (int s = 0; s < S; s++) {
                if (a[s] > start[s]) start[s] = a[s];
            }
        }

        int best = 0;
        long long bestF = start[0] + ct[f];
        for (int s = 1; s < S; s++) {
            long long F = start[s] + ct[f];
            if (F < bestF) {
                bestF = F;
                best = s;
            }
        }

        freeAt[best] = bestF;
        steps.push_back({f, best});

        long long* av = &avail[(size_t)f * S];
        long long rep = bestF + rt[f];
        for (int s = 0; s < S; s++) {
            if (rep < av[s]) av[s] = rep;
        }
        if (bestF < av[best]) av[best] = bestF;

        for (int ch : children[f]) {
            if (--indeg[ch] == 0) pq.push({eff[ch], ch});
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
