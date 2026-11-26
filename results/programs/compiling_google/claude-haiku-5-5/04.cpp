#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, T, S;
    cin >> C >> T >> S;

    vector<string> name(C);
    vector<long long> ctime(C), rtime(C);
    vector<vector<int>> deps(C);
    unordered_map<string, int> idx;
    idx.reserve(C * 2 + 1);

    for (int i = 0; i < C; i++) {
        cin >> name[i] >> ctime[i] >> rtime[i];
        int n;
        cin >> n;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string dn;
            cin >> dn;
            deps[i][k] = idx[dn];
        }
        idx[name[i]] = i;
    }

    vector<int> targets(T);
    for (int t = 0; t < T; t++) {
        string tn;
        long long d, g;
        cin >> tn >> d >> g;
        targets[t] = idx[tn];
    }

    // Mark needed files: closure of targets. Deps have smaller indices.
    vector<char> need(C, 0);
    for (int t : targets) need[t] = 1;
    for (int i = C - 1; i >= 0; i--) {
        if (need[i]) {
            for (int d : deps[i]) need[d] = 1;
        }
    }

    const long long INF = (long long)1e18;
    // avail[d*S + s] = earliest time file d is usable on server s
    vector<long long> avail((size_t)C * S, INF);
    vector<long long> freeAt(S, 0);
    vector<long long> ready(S);

    vector<pair<int, int>> steps; // (file, server)
    steps.reserve((size_t)C);

    for (int f = 0; f < C; f++) {
        if (!need[f]) continue;

        fill(ready.begin(), ready.end(), 0LL);
        for (int d : deps[f]) {
            const long long* av = &avail[(size_t)d * S];
            for (int s = 0; s < S; s++) {
                if (av[s] > ready[s]) ready[s] = av[s];
            }
        }

        int best = 0;
        long long bestFin = INF;
        for (int s = 0; s < S; s++) {
            long long start = max(ready[s], freeAt[s]);
            long long fin = start + ctime[f];
            if (fin < bestFin) {
                bestFin = fin;
                best = s;
            }
        }

        freeAt[best] = bestFin;
        steps.push_back({f, best});

        size_t base = (size_t)f * S;
        for (int s = 0; s < S; s++) {
            if (s == best) avail[base + s] = bestFin;
            else avail[base + s] = bestFin + rtime[f];
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
