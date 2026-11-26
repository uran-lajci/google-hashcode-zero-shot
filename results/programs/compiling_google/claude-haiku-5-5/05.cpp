#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF = LLONG_MAX / 4;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startTime = chrono::steady_clock::now();

    int C, T, S;
    cin >> C >> T >> S;
    vector<string> name(C);
    vector<ll> cc(C), rr(C);
    vector<vector<int>> dep(C);
    unordered_map<string, int> id;
    id.reserve(C * 2 + 10);
    for (int i = 0; i < C; i++) {
        string nm;
        ll c, r;
        int n;
        cin >> nm >> c >> r >> n;
        name[i] = nm;
        id[nm] = i;
        cc[i] = c;
        rr[i] = r;
        dep[i].resize(n);
        for (int k = 0; k < n; k++) {
            string dn;
            cin >> dn;
            dep[i][k] = id[dn];
        }
    }
    vector<int> tg(T);
    vector<ll> dl(T), gp(T);
    for (int j = 0; j < T; j++) {
        string nm;
        ll d, g;
        cin >> nm >> d >> g;
        tg[j] = id[nm];
        dl[j] = d;
        gp[j] = g;
    }
    vector<int> ord(T);
    for (int j = 0; j < T; j++) ord[j] = j;
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        if (dl[a] != dl[b]) return dl[a] < dl[b];
        if (gp[a] != gp[b]) return gp[a] > gp[b];
        return a < b;
    });

    vector<ll> bestF(C, INF);
    vector<vector<pair<int, ll>>> inst(C);
    vector<ll> freeT(S, 0);
    vector<int> vis(C, 0);
    int stamp = 0;
    vector<ll> tmpFin(C, 0);
    vector<pair<int, int>> out;

    auto localFin = [&](int v, int s) -> ll {
        for (auto &p : inst[v])
            if (p.first == s) return p.second;
        return INF;
    };

    // Simulate compiling order (closure nodes) then root on server s.
    // Returns root finish time; fills tmpFin.
    auto sim = [&](const vector<int> &order, int root, int s) -> ll {
        ll fs = freeT[s];
        auto proc = [&](int u) {
            ll st = fs;
            for (int v : dep[u]) {
                ll a;
                if (bestF[v] >= INF) {
                    a = tmpFin[v];
                } else {
                    a = bestF[v] + rr[v];
                    ll lf = localFin(v, s);
                    if (lf < a) a = lf;
                }
                if (a > st) st = a;
            }
            ll fin = st + cc[u];
            tmpFin[u] = fin;
            fs = fin;
        };
        for (int u : order) proc(u);
        proc(root);
        return fs;
    };

    const size_t LIMIT = 4000;
    for (int oi = 0; oi < T; oi++) {
        double el = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
        if (el > 8.0) break;

        int idx = ord[oi];
        int t = tg[idx];
        ll d = dl[idx];
        if (bestF[t] <= d) continue;

        stamp++;
        vector<int> order;
        bool bad = false;
        vis[t] = stamp;
        vector<pair<int, int>> stk;
        stk.push_back({t, 0});
        while (!stk.empty() && !bad) {
            int u = stk.back().first;
            int k = stk.back().second;
            if (k < (int)dep[u].size()) {
                stk.back().second++;
                int v = dep[u][k];
                if (bestF[v] >= INF && vis[v] != stamp) {
                    vis[v] = stamp;
                    stk.push_back({v, 0});
                    if (order.size() + stk.size() > LIMIT) bad = true;
                }
            } else {
                if (u != t) order.push_back(u);
                stk.pop_back();
            }
        }
        if (bad) continue;

        int bestS = -1;
        ll bestFin = INF;
        for (int s = 0; s < S; s++) {
            if (localFin(t, s) < INF) continue;
            ll f = sim(order, t, s);
            if (f < bestFin) {
                bestFin = f;
                bestS = s;
            }
        }
        if (bestS < 0 || bestFin > d) continue;

        sim(order, t, bestS);
        for (int u : order) {
            ll fin = tmpFin[u];
            out.push_back({u, bestS});
            inst[u].push_back({bestS, fin});
            if (fin < bestF[u]) bestF[u] = fin;
        }
        {
            ll fin = tmpFin[t];
            out.push_back({t, bestS});
            inst[t].push_back({bestS, fin});
            if (fin < bestF[t]) bestF[t] = fin;
        }
        freeT[bestS] = bestFin;
    }

    string buf;
    buf.reserve(out.size() * 16 + 16);
    buf += to_string(out.size());
    buf += '\n';
    for (auto &p : out) {
        buf += name[p.first];
        buf += ' ';
        buf += to_string(p.second);
        buf += '\n';
    }
    fwrite(buf.data(), 1, buf.size(), stdout);
    return 0;
}
