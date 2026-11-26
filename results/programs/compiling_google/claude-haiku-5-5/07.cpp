#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll INF = (ll)4e18;

int C, T, S;
vector<string> names;
vector<ll> cT, rT, dl, gp;
vector<vector<int>> deps;
vector<char> needed;
vector<ll> pr;

struct Result {
    vector<ll> fin, start;
    vector<int> place;
    ll score;
};

Result schedule(const vector<int>& order) {
    Result R;
    R.fin.assign(C, 0);
    R.start.assign(C, 0);
    R.place.assign(C, -1);
    vector<ll> freeT(S, 0);
    vector<ll> own(S, 0);
    for (int f : order) {
        // reset own
        fill(own.begin(), own.end(), 0);
        ll best1 = 0;
        int p1 = -1;
        for (int d : deps[f]) {
            int pd = R.place[d];
            own[pd] = max(own[pd], R.fin[d]);
            ll A = R.fin[d] + rT[d];
            if (A > best1 || p1 == -1) {
                if (A > best1) { best1 = A; p1 = pd; }
            }
        }
        ll bestExcl = 0;
        for (int d : deps[f]) {
            int pd = R.place[d];
            if (pd != p1) {
                bestExcl = max(bestExcl, R.fin[d] + rT[d]);
            }
        }
        ll bestFin = INF;
        int bestS = 0;
        ll bestStart = 0;
        for (int s = 0; s < S; s++) {
            ll ready = own[s];
            ll other = (s == p1) ? bestExcl : best1;
            ready = max(ready, other);
            ll st = max(freeT[s], ready);
            ll fn = st + cT[f];
            if (fn < bestFin) {
                bestFin = fn;
                bestS = s;
                bestStart = st;
            }
        }
        R.place[f] = bestS;
        R.fin[f] = bestFin;
        R.start[f] = bestStart;
        freeT[bestS] = bestFin;
    }
    R.score = 0;
    for (int i = 0; i < C; i++) {
        if (dl[i] >= 0 && needed[i] && R.fin[i] <= dl[i]) {
            R.score += dl[i] - R.fin[i] + gp[i];
        }
    }
    return R;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> C >> T >> S;
    names.resize(C);
    cT.resize(C);
    rT.resize(C);
    deps.resize(C);
    dl.assign(C, -1);
    gp.assign(C, 0);
    unordered_map<string, int> idx;
    idx.reserve(C * 2 + 10);
    for (int i = 0; i < C; i++) {
        cin >> names[i] >> cT[i] >> rT[i];
        idx[names[i]] = i;
        int n;
        cin >> n;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string dn;
            cin >> dn;
            deps[i][k] = idx[dn];
        }
    }
    for (int t = 0; t < T; t++) {
        string tn;
        ll d, g;
        cin >> tn >> d >> g;
        int id = idx[tn];
        dl[id] = d;
        gp[id] = g;
    }

    // needed: ancestors of targets (including targets)
    needed.assign(C, 0);
    vector<int> st;
    for (int i = 0; i < C; i++) {
        if (dl[i] >= 0) {
            if (!needed[i]) {
                needed[i] = 1;
                st.push_back(i);
            }
        }
    }
    while (!st.empty()) {
        int x = st.back();
        st.pop_back();
        for (int d : deps[x]) {
            if (!needed[d]) {
                needed[d] = 1;
                st.push_back(d);
            }
        }
    }

    // priority: min deadline of targets depending on file (propagated backward)
    pr.assign(C, INF);
    for (int i = 0; i < C; i++) {
        if (dl[i] >= 0) pr[i] = min(pr[i], dl[i]);
    }
    for (int i = C - 1; i >= 0; i--) {
        if (!needed[i]) continue;
        for (int d : deps[i]) {
            pr[d] = min(pr[d], pr[i]);
        }
    }

    // order 1: index order (topological since deps come earlier)
    vector<int> order1;
    for (int i = 0; i < C; i++) if (needed[i]) order1.push_back(i);

    // order 2: Kahn with priority by deadline
    vector<vector<int>> children(C);
    vector<int> indeg(C, 0);
    for (int i = 0; i < C; i++) {
        if (!needed[i]) continue;
        indeg[i] = deps[i].size();
        for (int d : deps[i]) children[d].push_back(i);
    }
    vector<int> order2;
    {
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
        for (int i = 0; i < C; i++) {
            if (needed[i] && indeg[i] == 0) pq.push({pr[i], i});
        }
        while (!pq.empty()) {
            auto [p, x] = pq.top();
            pq.pop();
            order2.push_back(x);
            for (int ch : children[x]) {
                indeg[ch]--;
                if (indeg[ch] == 0) pq.push({pr[ch], ch});
            }
        }
    }

    Result R1 = schedule(order1);
    Result R2 = schedule(order2);
    Result& best = (R2.score > R1.score) ? R2 : R1;

    // collect steps, sort by start time (stable with index)
    vector<int> steps;
    for (int i = 0; i < C; i++) if (needed[i]) steps.push_back(i);
    sort(steps.begin(), steps.end(), [&](int a, int b) {
        if (best.start[a] != best.start[b]) return best.start[a] < best.start[b];
        return a < b;
    });

    string out;
    out.reserve(steps.size() * 16 + 16);
    out += to_string(steps.size());
    out += '\n';
    for (int f : steps) {
        out += names[f];
        out += ' ';
        out += to_string(best.place[f]);
        out += '\n';
    }
    cout << out;
    return 0;
}
