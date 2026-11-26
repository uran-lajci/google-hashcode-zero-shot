#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> EB, EE, EL;
vector<string> names;
unordered_map<string, int> nameId;
vector<vector<int>> paths;
vector<int> crossCnt, firstArr, usedStreets;
vector<vector<int>> incoming;
vector<int> candX;

struct En { int arr, car, pos; };
vector<vector<En>> q;
vector<int> hd;
vector<char> inAct;
vector<vector<int>> st, sg, sd;
vector<int> cyc;

// Simulate schedule; returns total score. work accumulates effort.
long long simulate(const vector<vector<pair<int, int>>>& sched, long long& work) {
    for (int X : candX) {
        st[X].clear(); sg[X].clear(); sd[X].clear(); cyc[X] = 0;
        int acc = 0;
        for (auto& pr : sched[X]) {
            st[X].push_back(acc);
            sg[X].push_back(pr.first);
            sd[X].push_back(pr.second);
            acc += pr.second;
        }
        cyc[X] = acc;
    }
    for (int u : usedStreets) {
        q[u].clear();
        hd[u] = 0;
        inAct[u] = 0;
    }
    vector<int> active;
    for (int c = 0; c < V; c++) {
        int s0 = paths[c][0];
        q[s0].push_back({0, c, 0});
        if (!inAct[s0]) { inAct[s0] = 1; active.push_back(s0); }
    }
    long long score = 0;
    vector<int> nxt;
    work += (long long)D + (long long)candX.size();
    for (int t = 0; t < D; t++) {
        if (active.empty()) continue;
        nxt.clear();
        for (int u : active) {
            work++;
            int X = EE[u];
            bool isGreen = false;
            if (cyc[X] > 0) {
                int r = t % cyc[X];
                const auto& v = st[X];
                int idx = int(upper_bound(v.begin(), v.end(), r) - v.begin()) - 1;
                if (idx >= 0 && sg[X][idx] == u && r < v[idx] + sd[X][idx]) isGreen = true;
            }
            if (isGreen && hd[u] < (int)q[u].size() && q[u][hd[u]].arr <= t) {
                En e = q[u][hd[u]];
                hd[u]++;
                const auto& path = paths[e.car];
                int p = e.pos;
                int nx = path[p + 1];
                int arr = t + EL[nx];
                if (p + 2 == (int)path.size()) {
                    if (arr <= D) score += F + (D - arr);
                } else {
                    q[nx].push_back({arr, e.car, p + 1});
                    if (!inAct[nx]) { inAct[nx] = 1; nxt.push_back(nx); }
                }
            }
            if (hd[u] < (int)q[u].size()) nxt.push_back(u);
            else inAct[u] = 0;
        }
        active.swap(nxt);
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> D >> I >> S >> V >> F;
    EB.resize(S); EE.resize(S); EL.resize(S); names.resize(S);
    for (int s = 0; s < S; s++) {
        cin >> EB[s] >> EE[s] >> names[s] >> EL[s];
        nameId[names[s]] = s;
    }
    paths.resize(V);
    for (int c = 0; c < V; c++) {
        int P;
        cin >> P;
        paths[c].resize(P);
        for (int k = 0; k < P; k++) {
            string nm;
            cin >> nm;
            paths[c][k] = nameId[nm];
        }
    }

    crossCnt.assign(S, 0);
    firstArr.assign(S, INT_MAX);
    vector<char> used(S, 0);
    for (int c = 0; c < V; c++) {
        auto& p = paths[c];
        int n = p.size();
        int cum = 0;
        for (int k = 0; k < n; k++) {
            used[p[k]] = 1;
            if (k > 0) cum += EL[p[k]];
            if (k + 1 < n) {
                crossCnt[p[k]]++;
                firstArr[p[k]] = min(firstArr[p[k]], cum);
            }
        }
    }
    incoming.assign(I, {});
    for (int s = 0; s < S; s++) {
        if (used[s]) usedStreets.push_back(s);
        if (crossCnt[s] > 0) incoming[EE[s]].push_back(s);
    }
    for (int X = 0; X < I; X++)
        if (!incoming[X].empty()) candX.push_back(X);

    q.assign(S, {});
    hd.assign(S, 0);
    inAct.assign(S, 0);
    st.assign(I, {});
    sg.assign(I, {});
    sd.assign(I, {});
    cyc.assign(I, 0);

    vector<vector<pair<int, int>>> sched(I);
    for (int X : candX) {
        auto& inc = incoming[X];
        sort(inc.begin(), inc.end(), [&](int a, int b) {
            if (firstArr[a] != firstArr[b]) return firstArr[a] < firstArr[b];
            return a < b;
        });
        for (int s : inc) {
            int T = max(1, min(D, crossCnt[s]));
            sched[X].push_back({s, T});
        }
    }

    long long work = 0;
    long long cur = simulate(sched, work);

    const long long BUDGET = 400000000LL;
    mt19937 rng(12345);
    while (work < BUDGET && !candX.empty()) {
        int X = candX[rng() % candX.size()];
        vector<pair<int, int>> saved = sched[X];
        auto& v = sched[X];
        int op = rng() % 4;
        bool changed = false;
        if (op == 0 && !v.empty()) {
            int i = rng() % v.size();
            int d = (int)(rng() % 5) - 2;
            if (d == 0) d = 1;
            int nt = max(1, min(D, v[i].second + d));
            if (nt != v[i].second) { v[i].second = nt; changed = true; }
        } else if (op == 1 && v.size() >= 2) {
            int i = rng() % (v.size() - 1);
            swap(v[i], v[i + 1]);
            changed = true;
        } else if (op == 2) {
            int s = incoming[X][rng() % incoming[X].size()];
            bool present = false;
            for (auto& pr : v) if (pr.first == s) { present = true; break; }
            if (!present) {
                int pos = rng() % (v.size() + 1);
                int T = 1 + (int)(rng() % max(1, crossCnt[s]));
                T = min(T, D);
                v.insert(v.begin() + pos, {s, T});
                changed = true;
            }
        } else if (op == 3 && v.size() >= 2) {
            int i = rng() % v.size();
            v.erase(v.begin() + i);
            changed = true;
        }
        if (!changed) { v = saved; continue; }
        long long ns = simulate(sched, work);
        if (ns >= cur) {
            cur = ns;
        } else {
            sched[X] = saved;
        }
    }

    // Output
    vector<int> outX;
    for (int X : candX) if (!sched[X].empty()) outX.push_back(X);
    string out;
    out += to_string(outX.size()) + "\n";
    for (int X : outX) {
        out += to_string(X) + "\n";
        out += to_string(sched[X].size()) + "\n";
        for (auto& pr : sched[X]) {
            out += names[pr.first];
            out += " ";
            out += to_string(pr.second);
            out += "\n";
        }
    }
    cout << out;
    return 0;
}
