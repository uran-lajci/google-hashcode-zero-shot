#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int C, T, S;
vector<string> names;
vector<ll> cc, rr;
vector<vector<int>> deps;
vector<ll> dl, gp;          // deadline / goal points, valid for targets
vector<int> isTarget;
vector<int> targets;

const ll WORK_LIMIT = 1500000000LL;

struct Result {
    ll score = 0;
    vector<pair<int,int>> steps;
};

Result runSchedule(const vector<int>& order) {
    vector<ll> freeT(S, 0);
    vector<int> srv(C, -1);
    vector<ll> fin(C, 0);
    vector<pair<int,int>> steps;
    vector<ll> oldFree;
    ll score = 0;
    ll work = 0;
    vector<int> stamp(C, 0);
    int cur = 0;
    vector<ll> loc(S, 0);

    auto placeFile = [&](int f) -> pair<int,ll> {
        // returns (server, finish)
        const auto& ds = deps[f];
        ll best1 = 0; int srv1 = -1;
        for (int d : ds) {
            ll v = fin[d] + rr[d];
            if (v > best1) { best1 = v; srv1 = srv[d]; }
        }
        ll best2 = 0;
        for (int d : ds) {
            if (srv[d] != srv1) best2 = max(best2, fin[d] + rr[d]);
        }
        fill(loc.begin(), loc.end(), 0LL);
        for (int d : ds) loc[srv[d]] = max(loc[srv[d]], fin[d]);
        int bestS = 0; ll bestF = LLONG_MAX;
        for (int s = 0; s < S; s++) {
            ll avail = max(loc[s], (s == srv1) ? best2 : best1);
            ll start = max(freeT[s], avail);
            ll f2 = start + cc[f];
            if (f2 < bestF) { bestF = f2; bestS = s; }
        }
        work += S + 2LL * (ll)ds.size();
        return {bestS, bestF};
    };

    auto commit = [&](int f, int s, ll finish) {
        oldFree.push_back(freeT[s]);
        freeT[s] = finish;
        srv[f] = s;
        fin[f] = finish;
        steps.push_back({f, s});
    };

    auto rollbackTo = [&](size_t pos) {
        while (steps.size() > pos) {
            auto [f, s] = steps.back();
            freeT[s] = oldFree.back();
            oldFree.pop_back();
            srv[f] = -1;
            steps.pop_back();
        }
    };

    for (int t : order) {
        if (srv[t] >= 0) {
            if (fin[t] <= dl[t]) score += dl[t] - fin[t] + gp[t];
            continue;
        }
        if (work > WORK_LIMIT) break;

        // collect unscheduled ancestors (including t) in postorder
        cur++;
        vector<int> ord;
        vector<pair<int,int>> st;
        stamp[t] = cur;
        st.push_back({t, 0});
        while (!st.empty()) {
            int v = st.back().first;
            int &idx = st.back().second;
            if (idx < (int)deps[v].size()) {
                int d = deps[v][idx];
                idx++;
                if (srv[d] < 0 && stamp[d] != cur) {
                    stamp[d] = cur;
                    st.push_back({d, 0});
                }
            } else {
                ord.push_back(v);
                st.pop_back();
            }
        }
        work += (ll)ord.size() * 4;

        size_t startPos = steps.size();
        for (int f : ord) {
            auto [s, fi] = placeFile(f);
            commit(f, s, fi);
        }
        if (fin[t] <= dl[t]) {
            score += dl[t] - fin[t] + gp[t];
        } else {
            rollbackTo(startPos);
        }
    }

    Result res;
    res.score = score;
    res.steps = steps;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> C >> T >> S;
    names.resize(C);
    cc.resize(C);
    rr.resize(C);
    deps.assign(C, {});
    dl.assign(C, 0);
    gp.assign(C, 0);
    isTarget.assign(C, 0);
    unordered_map<string,int> id;
    id.reserve(C * 2 + 1);
    for (int i = 0; i < C; i++) {
        string nm; ll c, r; int n;
        cin >> nm >> c >> r >> n;
        names[i] = nm;
        cc[i] = c;
        rr[i] = r;
        id[nm] = i;
        deps[i].resize(n);
        for (int k = 0; k < n; k++) {
            string dn;
            cin >> dn;
            deps[i][k] = id[dn];
        }
    }
    for (int k = 0; k < T; k++) {
        string nm; ll d, g;
        cin >> nm >> d >> g;
        int i = id[nm];
        isTarget[i] = 1;
        dl[i] = d;
        gp[i] = g;
        targets.push_back(i);
    }

    vector<int> ordA = targets;
    sort(ordA.begin(), ordA.end(), [&](int a, int b) {
        if (dl[a] != dl[b]) return dl[a] < dl[b];
        if (gp[a] != gp[b]) return gp[a] > gp[b];
        return a < b;
    });
    vector<int> ordB = targets;
    sort(ordB.begin(), ordB.end(), [&](int a, int b) {
        if (gp[a] != gp[b]) return gp[a] > gp[b];
        if (dl[a] != dl[b]) return dl[a] < dl[b];
        return a < b;
    });

    Result best = runSchedule(ordA);
    Result other = runSchedule(ordB);
    if (other.score > best.score) best = other;

    string out;
    if (best.steps.empty()) {
        // Must output at least one step; file 0 has no dependencies.
        out += "1\n" + names[0] + " 0\n";
    } else {
        out += to_string(best.steps.size()) + "\n";
        for (auto& [f, s] : best.steps) {
            out += names[f];
            out += ' ';
            out += to_string(s);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
