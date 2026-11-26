#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startTime = chrono::steady_clock::now();

    int C, T, S;
    cin >> C >> T >> S;

    vector<string> name(C);
    vector<long long> c(C), r(C);
    vector<vector<int>> dep(C);
    unordered_map<string, int> id;
    id.reserve(C * 2 + 16);

    for (int i = 0; i < C; i++) {
        cin >> name[i] >> c[i] >> r[i];
        int n;
        cin >> n;
        dep[i].resize(n);
        for (int j = 0; j < n; j++) {
            string s;
            cin >> s;
            dep[i][j] = id[s];
        }
        id[name[i]] = i;
    }

    vector<int> tf(T), td(T), tg(T);
    for (int k = 0; k < T; k++) {
        string s;
        cin >> s >> td[k] >> tg[k];
        tf[k] = id[s];
    }

    vector<int> torder(T);
    for (int k = 0; k < T; k++) torder[k] = k;
    sort(torder.begin(), torder.end(), [&](int a, int b) {
        if (td[a] != td[b]) return td[a] < td[b];
        return tg[a] > tg[b];
    });

    vector<char> sched(C, 0);
    vector<int> srv(C, -1);
    vector<long long> fin(C, 0);
    vector<long long> freeT(S, 0);

    struct Step { int f, s; long long prevFree; };
    vector<Step> steps;

    vector<int> vis(C, 0);
    int stamp = 0;

    auto scheduleFile = [&](int f) {
        int bestS = 0;
        long long bestFin = LLONG_MAX;
        for (int s = 0; s < S; s++) {
            long long start = freeT[s];
            for (int u : dep[f]) {
                long long a = (srv[u] == s) ? fin[u] : fin[u] + r[u];
                if (a > start) start = a;
            }
            long long fe = start + c[f];
            if (fe < bestFin) {
                bestFin = fe;
                bestS = s;
            }
        }
        steps.push_back({f, bestS, freeT[bestS]});
        freeT[bestS] = bestFin;
        sched[f] = 1;
        srv[f] = bestS;
        fin[f] = bestFin;
    };

    vector<int> order;
    vector<pair<int, int>> stk;

    for (int idx = 0; idx < T; idx++) {
        int k = torder[idx];
        int t = tf[k];
        if (sched[t]) continue;

        double elapsed = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
        if (elapsed > 8.5) break;

        size_t save = steps.size();

        // Gather unscheduled dependency closure of t in topological order.
        stamp++;
        order.clear();
        stk.clear();
        stk.push_back({t, 0});
        vis[t] = stamp;
        while (!stk.empty()) {
            int v = stk.back().first;
            int kk = stk.back().second;
            if (kk < (int)dep[v].size()) {
                stk.back().second++;
                int u = dep[v][kk];
                if (!sched[u] && vis[u] != stamp) {
                    vis[u] = stamp;
                    stk.push_back({u, 0});
                }
            } else {
                order.push_back(v);
                stk.pop_back();
            }
        }

        for (int f : order) scheduleFile(f);

        if (fin[t] <= td[k]) {
            // keep
        } else {
            while (steps.size() > save) {
                Step st = steps.back();
                steps.pop_back();
                sched[st.f] = 0;
                srv[st.f] = -1;
                freeT[st.s] = st.prevFree;
            }
        }
    }

    if (steps.empty()) {
        for (int i = 0; i < C; i++) {
            if (dep[i].empty()) {
                steps.push_back({i, 0, freeT[0]});
                break;
            }
        }
    }

    string out;
    out.reserve(steps.size() * 16 + 16);
    out += to_string(steps.size());
    out += '\n';
    for (const Step &st : steps) {
        out += name[st.f];
        out += ' ';
        out += to_string(st.s);
        out += '\n';
    }
    cout << out;
    return 0;
}
