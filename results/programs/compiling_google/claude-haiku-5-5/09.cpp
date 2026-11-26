#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const ll INF = (ll)4e18;

int C, T, S;
vector<string> names;
vector<vector<int>> deps;
vector<ll> cc, rr;
vector<ll> av;        // av[f*S+s]: time f is available on server s
vector<ll> fin;       // finish time of the (first) compilation of f
vector<int> srv;      // server of the compilation of f
vector<char> sched;
vector<ll> freeT;     // free time per server
vector<pair<int,int>> seq;  // (file, server) in submission order
vector<int> mark;
int stampCnt = 0;

void commitFile(int f) {
    int bs = -1;
    ll bestF = INF;
    for (int s = 0; s < S; s++) {
        ll st = freeT[s];
        for (int d : deps[f]) st = max(st, av[(ll)d * S + s]);
        ll F = st + cc[f];
        if (F < bestF) { bestF = F; bs = s; }
    }
    srv[f] = bs;
    fin[f] = bestF;
    sched[f] = 1;
    freeT[bs] = bestF;
    for (int s = 0; s < S; s++) {
        if (s == bs) av[(ll)f * S + s] = bestF;
        else av[(ll)f * S + s] = bestF + rr[f];
    }
    seq.push_back({f, bs});
}

bool attempt(int t, ll limit) {
    // collect unscheduled ancestors (including t)
    stampCnt++;
    vector<int> list;
    vector<int> st;
    st.push_back(t);
    mark[t] = stampCnt;
    while (!st.empty()) {
        int u = st.back(); st.pop_back();
        if (sched[u]) continue;
        list.push_back(u);
        for (int d : deps[u]) {
            if (!sched[d] && mark[d] != stampCnt) {
                mark[d] = stampCnt;
                st.push_back(d);
            }
        }
    }
    sort(list.begin(), list.end());

    // save state
    size_t seqSave = seq.size();
    vector<ll> freeSave = freeT;
    vector<int> touched;
    for (int f : list) {
        commitFile(f);
        touched.push_back(f);
    }
    if (sched[t] && fin[t] <= limit) return true;

    // rollback
    for (int f : touched) {
        sched[f] = 0;
        srv[f] = -1;
        fin[f] = INF;
        for (int s = 0; s < S; s++) av[(ll)f * S + s] = INF;
    }
    seq.resize(seqSave);
    freeT = freeSave;
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> C >> T >> S;
    names.resize(C);
    deps.assign(C, {});
    cc.assign(C, 0);
    rr.assign(C, 0);
    unordered_map<string, int> idx;
    idx.reserve(C * 2 + 1);
    for (int i = 0; i < C; i++) {
        string name;
        ll c, r;
        int n;
        cin >> name >> c >> r >> n;
        names[i] = name;
        cc[i] = c;
        rr[i] = r;
        idx[name] = i;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string dn;
            cin >> dn;
            deps[i][k] = idx[dn];
        }
    }

    vector<int> tgt(T);
    vector<ll> dl(T), gp(T);
    for (int i = 0; i < T; i++) {
        string name;
        ll d, g;
        cin >> name >> d >> g;
        tgt[i] = idx[name];
        dl[i] = d;
        gp[i] = g;
    }

    av.assign((ll)C * S, INF);
    fin.assign(C, INF);
    srv.assign(C, -1);
    sched.assign(C, 0);
    freeT.assign(S, 0);
    mark.assign(C, 0);

    vector<int> order(T);
    for (int i = 0; i < T; i++) order[i] = i;
    sort(order.begin(), order.end(), [&](int a, int b) {
        if (dl[a] != dl[b]) return dl[a] < dl[b];
        if (gp[a] != gp[b]) return gp[a] > gp[b];
        return a < b;
    });

    for (int oi : order) {
        int ti = order[oi == 0 ? 0 : 0]; // placeholder to keep structure simple
        (void)ti;
    }
    for (int oi : order) {
        attempt(tgt[oi], dl[oi]);
    }

    if (seq.empty()) {
        // fallback: must output at least one step
        attempt(tgt[order[0]], INF);
    }

    string out;
    out.reserve(seq.size() * 16 + 16);
    out += to_string(seq.size());
    out += '\n';
    for (auto &p : seq) {
        out += names[p.first];
        out += ' ';
        out += to_string(p.second);
        out += '\n';
    }
    cout << out;
    return 0;
}
