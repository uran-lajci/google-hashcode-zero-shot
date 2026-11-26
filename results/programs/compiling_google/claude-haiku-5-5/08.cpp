#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF = LLONG_MAX / 4;
const ll WORK_LIMIT = 300000000LL;

int C, T, S;
vector<string> nm;
vector<ll> cc, rr;
vector<vector<int>> dep;
vector<vector<pair<int, ll>>> cop;   // copies: (server, finish time)
vector<ll> F;                        // per-server free time
struct LogEntry { int f, s; ll oldF; };
vector<LogEntry> lg;                 // compile log (== submission steps)
ll work = 0;

ll avail(int f, int s) {
    ll best = INF;
    for (auto &p : cop[f]) {
        ll v = p.second + (p.first == s ? 0 : rr[f]);
        if (v < best) best = v;
    }
    work += cop[f].size() + 1;
    return best;
}

void undoTo(size_t mark) {
    while (lg.size() > mark) {
        LogEntry e = lg.back();
        lg.pop_back();
        F[e.s] = e.oldF;
        cop[e.f].pop_back();
    }
}

struct Fr { int f; size_t idx; ll mx; };

// Returns time when file `root` is available on server s.
// If forceRoot, root is compiled on s regardless of existing copies.
ll compileTree(int root, int s, bool forceRoot) {
    if (!forceRoot) {
        ll a = avail(root, s);
        if (a < INF) return a;
    }
    vector<Fr> st;
    st.push_back({root, 0, 0});
    ll ret = 0;
    bool haveRet = false;
    while (!st.empty()) {
        size_t top = st.size() - 1;
        if (haveRet) {
            st[top].mx = max(st[top].mx, ret);
            st[top].idx++;
            haveRet = false;
        }
        bool pushed = false;
        int f = st[top].f;
        while (st[top].idx < dep[f].size()) {
            int d = dep[f][st[top].idx];
            ll a = avail(d, s);
            work++;
            if (a < INF) {
                st[top].mx = max(st[top].mx, a);
                st[top].idx++;
            } else {
                st.push_back({d, 0, 0});
                pushed = true;
                break;
            }
        }
        if (pushed) continue;
        ll start = max(F[s], st[top].mx);
        ll fin = start + cc[f];
        lg.push_back({f, s, F[s]});
        F[s] = fin;
        cop[f].push_back({s, fin});
        work++;
        ret = fin;
        haveRet = true;
        st.pop_back();
        if (st.empty()) return fin;
    }
    return ret;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> C >> T >> S;
    nm.resize(C);
    cc.resize(C);
    rr.resize(C);
    dep.assign(C, {});
    cop.assign(C, {});
    F.assign(S, 0);
    unordered_map<string, int> id;
    id.reserve(C * 2 + 1);
    for (int i = 0; i < C; i++) {
        string name;
        cin >> name >> cc[i] >> rr[i];
        int n;
        cin >> n;
        nm[i] = name;
        id[name] = i;
        dep[i].resize(n);
        for (int j = 0; j < n; j++) {
            string dn;
            cin >> dn;
            dep[i][j] = id[dn];
        }
    }
    vector<int> tg(T);
    vector<ll> dl(T), gp(T);
    for (int i = 0; i < T; i++) {
        string name;
        cin >> name >> dl[i] >> gp[i];
        tg[i] = id[name];
    }

    vector<int> order(T);
    for (int i = 0; i < T; i++) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int a, int b) { return dl[a] < dl[b]; });

    for (int oi = 0; oi < T; oi++) {
        if (work > WORK_LIMIT) break;
        int ti = order[oi];
        int t = tg[ti];
        ll d = dl[ti], g = gp[ti];
        ll bestVal = 0;
        int bestS = -1;
        for (int s = 0; s < S; s++) {
            ll A = avail(t, s);
            ll oldScore = (A < INF && A <= d) ? d - A + g : 0;
            size_t mark = lg.size();
            ll fin = compileTree(t, s, true);
            undoTo(mark);
            if (fin < A && fin <= d) {
                ll val = (d - fin + g) - oldScore;
                if (val > bestVal) {
                    bestVal = val;
                    bestS = s;
                }
            }
            if (work > WORK_LIMIT) break;
        }
        if (bestS >= 0) {
            compileTree(t, bestS, true);
        }
    }

    if (lg.empty()) {
        // Fallback: produce a valid (zero-score) submission with one target.
        compileTree(tg[order[0]], 0, true);
    }

    string out;
    out.reserve(lg.size() * 16 + 16);
    out += to_string(lg.size());
    out += '\n';
    for (auto &e : lg) {
        out += nm[e.f];
        out += ' ';
        out += to_string(e.s);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
