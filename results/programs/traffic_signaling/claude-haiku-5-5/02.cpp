#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> Eend, Lt;
vector<string> names;
vector<vector<int>> paths;
vector<vector<int>> inc;   // candidate incoming streets sorted by first arrival
vector<int> wgt;
vector<long long> firstA;

struct Sched {
    vector<int> st;
    vector<int> dur;
    vector<long long> start;
    long long C = 0;
};
vector<Sched> sc;
vector<double> lev;

// simulation buffers
vector<int> qh, qt, qn, cur, wc;
vector<vector<int>> arrB;
vector<int> active;

void build(int i) {
    Sched &s = sc[i];
    s.st.clear(); s.dur.clear(); s.start.clear(); s.C = 0;
    if (lev[i] <= 0) return;
    for (int st : inc[i]) {
        long long d = llround((double)wgt[st] * lev[i]);
        if (d < 1) d = 1;
        if (d > D) d = D;
        s.st.push_back(st);
        s.dur.push_back((int)d);
        s.start.push_back(s.C);
        s.C += d;
    }
}

long long simulate() {
    fill(qh.begin(), qh.end(), -1);
    fill(qt.begin(), qt.end(), -1);
    fill(wc.begin(), wc.end(), 0);
    fill(cur.begin(), cur.end(), 0);
    for (auto &v : arrB) v.clear();
    active.clear();
    long long score = 0;

    auto enq = [&](int s, int c) {
        qn[c] = -1;
        int i = Eend[s];
        if (qh[s] < 0) { qh[s] = c; qt[s] = c; }
        else { qn[qt[s]] = c; qt[s] = c; }
        if (wc[i]++ == 0) active.push_back(i);
    };

    for (int c = 0; c < V; c++) enq(paths[c][0], c);

    for (int t = 0; t <= D; t++) {
        for (int c : arrB[t]) enq(paths[c][cur[c]], c);
        for (int i : active) {
            Sched &s = sc[i];
            if (s.C == 0) continue;
            long long p = t % s.C;
            int idx = int(upper_bound(s.start.begin(), s.start.end(), p) - s.start.begin()) - 1;
            int g = s.st[idx];
            if (qh[g] < 0) continue;
            int c = qh[g];
            qh[g] = qn[c];
            if (qh[g] < 0) qt[g] = -1;
            wc[i]--;
            int k = cur[c];
            int ns = paths[c][k + 1];
            long long arr = (long long)t + Lt[ns];
            if (k + 1 == (int)paths[c].size() - 1) {
                if (arr <= D) score += F + (D - arr);
            } else {
                cur[c] = k + 1;
                if (arr <= D) arrB[arr].push_back(c);
            }
        }
        // filter active
        size_t w = 0;
        for (size_t j = 0; j < active.size(); j++) {
            if (wc[active[j]] > 0) active[w++] = active[j];
        }
        active.resize(w);
    }
    return score;
}

int main() {
    auto startTime = chrono::steady_clock::now();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> D >> I >> S >> V >> F;
    Eend.assign(S, 0);
    Lt.assign(S, 0);
    names.resize(S);
    unordered_map<string, int> id;
    id.reserve(S * 2);
    vector<int> Bst(S);
    for (int s = 0; s < S; s++) {
        int b, e, l;
        string nm;
        cin >> b >> e >> nm >> l;
        Bst[s] = b;
        Eend[s] = e;
        Lt[s] = l;
        names[s] = nm;
        id[nm] = s;
    }
    paths.assign(V, {});
    for (int c = 0; c < V; c++) {
        int p;
        cin >> p;
        paths[c].resize(p);
        for (int k = 0; k < p; k++) {
            string nm;
            cin >> nm;
            paths[c][k] = id[nm];
        }
    }

    // free-flow demand estimate
    wgt.assign(S, 0);
    firstA.assign(S, LLONG_MAX);
    for (int c = 0; c < V; c++) {
        int P = paths[c].size();
        vector<long long> A(P, 0);
        for (int k = 1; k < P; k++) A[k] = A[k - 1] + Lt[paths[c][k]];
        if (A[P - 1] > D) continue;
        for (int k = 0; k + 1 < P; k++) {
            int s = paths[c][k];
            wgt[s]++;
            firstA[s] = min(firstA[s], A[k]);
        }
    }
    inc.assign(I, {});
    for (int s = 0; s < S; s++) {
        if (wgt[s] > 0) inc[Eend[s]].push_back(s);
    }
    for (int i = 0; i < I; i++) {
        sort(inc[i].begin(), inc[i].end(), [&](int a, int b) {
            if (firstA[a] != firstA[b]) return firstA[a] < firstA[b];
            return a < b;
        });
    }

    sc.assign(I, Sched());
    lev.assign(I, 0.0);
    qh.assign(S, -1);
    qt.assign(S, -1);
    qn.assign(V, -1);
    cur.assign(V, 0);
    wc.assign(I, 0);
    arrB.assign(D + 1, {});

    vector<double> cand = {0.0, 0.1, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0};
    vector<int> cands;
    for (int i = 0; i < I; i++) if (!inc[i].empty()) cands.push_back(i);

    // global initialization
    long long bestScore = -1;
    double bestC = 0.0;
    for (double c : cand) {
        if (c <= 0) continue;
        for (int i : cands) lev[i] = c;
        for (int i : cands) build(i);
        long long sc0 = simulate();
        if (sc0 > bestScore) { bestScore = sc0; bestC = c; }
    }
    if (bestScore < 0) bestScore = 0;
    for (int i : cands) lev[i] = bestC;
    for (int i : cands) build(i);
    long long curScore = (bestC > 0) ? simulate() : 0;
    if (bestC <= 0) {
        for (int i : cands) lev[i] = 0;
        for (int i : cands) build(i);
    }

    // local search
    uint64_t rng = 88172645463325252ULL;
    auto nextRand = [&]() {
        rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
        return rng;
    };
    if (!cands.empty()) {
        while (true) {
            auto now = chrono::steady_clock::now();
            double el = chrono::duration<double>(now - startTime).count();
            if (el > 6.5) break;
            int i = cands[nextRand() % cands.size()];
            double nc = cand[nextRand() % cand.size()];
            if (nc == lev[i]) continue;
            double old = lev[i];
            lev[i] = nc;
            build(i);
            long long ns = simulate();
            if (ns >= curScore) {
                curScore = ns;
            } else {
                lev[i] = old;
                build(i);
            }
        }
    }

    // output
    int A = 0;
    for (int i = 0; i < I; i++) if (!sc[i].st.empty()) A++;
    string out;
    out += to_string(A) + "\n";
    for (int i = 0; i < I; i++) {
        if (sc[i].st.empty()) continue;
        out += to_string(i) + "\n";
        out += to_string(sc[i].st.size()) + "\n";
        for (size_t j = 0; j < sc[i].st.size(); j++) {
            out += names[sc[i].st[j]];
            out += " ";
            out += to_string(sc[i].dur[j]);
            out += "\n";
        }
    }
    cout << out;
    return 0;
}
