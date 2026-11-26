#include <bits/stdc++.h>
using namespace std;

int D, I, S, V, F;
vector<int> Bs, Es, Ls;
vector<string> names;
vector<vector<int>> paths;

struct Sched {
    vector<int> st, du, pre;
    int C = 0;
};
vector<Sched> sch;

vector<int> qh, qt, nxtc, posc, arrc, wait_, activeFlag;
vector<int> activeList;

void rebuild(Sched& s) {
    s.pre.assign(s.st.size() + 1, 0);
    for (size_t i = 0; i < s.st.size(); i++) s.pre[i + 1] = s.pre[i] + s.du[i];
    s.C = s.pre.back();
}

long long simulate() {
    qh.assign(S, -1);
    qt.assign(S, -1);
    wait_.assign(I, 0);
    activeFlag.assign(I, 0);
    activeList.clear();

    auto push = [&](int car, int s) {
        nxtc[car] = -1;
        if (qt[s] == -1) qh[s] = car;
        else nxtc[qt[s]] = car;
        qt[s] = car;
        int X = Es[s];
        wait_[X]++;
        if (!activeFlag[X]) {
            activeFlag[X] = 1;
            activeList.push_back(X);
        }
    };

    for (int v = 0; v < V; v++) {
        posc[v] = 0;
        arrc[v] = 0;
        nxtc[v] = -1;
        push(v, paths[v][0]);
    }

    long long score = 0;
    vector<int> cur;
    for (int t = 0; t <= D; t++) {
        if (activeList.empty()) continue;
        cur.clear();
        cur.swap(activeList);
        for (int X : cur) {
            if (wait_[X] > 0 && !sch[X].st.empty()) {
                const Sched& s = sch[X];
                int r = t % s.C;
                int idx = int(upper_bound(s.pre.begin(), s.pre.end(), r) - s.pre.begin()) - 1;
                int g = s.st[idx];
                int car = qh[g];
                if (car != -1 && arrc[car] <= t) {
                    qh[g] = nxtc[car];
                    if (qh[g] == -1) qt[g] = -1;
                    wait_[X]--;
                    int p = posc[car] + 1;
                    const vector<int>& path = paths[car];
                    int ns = path[p];
                    if (p == (int)path.size() - 1) {
                        int fin = t + Ls[ns];
                        if (fin <= D) score += F + (D - fin);
                    } else {
                        posc[car] = p;
                        arrc[car] = t + Ls[ns];
                        push(car, ns);
                    }
                }
            }
            if (wait_[X] > 0) activeList.push_back(X);
            else activeFlag[X] = 0;
        }
    }
    return score;
}

struct RNG {
    unsigned long long s;
    unsigned long long next() {
        s ^= s << 13;
        s ^= s >> 7;
        s ^= s << 17;
        return s;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> D >> I >> S >> V >> F;
    Bs.resize(S);
    Es.resize(S);
    Ls.resize(S);
    names.resize(S);
    unordered_map<string, int> nameId;
    nameId.reserve(S * 2);
    for (int i = 0; i < S; i++) {
        cin >> Bs[i] >> Es[i] >> names[i] >> Ls[i];
        nameId[names[i]] = i;
    }
    paths.resize(V);
    for (int v = 0; v < V; v++) {
        int P;
        cin >> P;
        paths[v].resize(P);
        for (int k = 0; k < P; k++) {
            string nm;
            cin >> nm;
            paths[v][k] = nameId[nm];
        }
    }

    // Free-flow estimate
    vector<int> cnt(S, 0);
    const int INF = INT_MAX;
    vector<int> firstArr(S, INF);
    for (int v = 0; v < V; v++) {
        long long time = 0;
        int P = paths[v].size();
        for (int k = 0; k + 1 < P; k++) {
            int s = paths[v][k];
            cnt[s]++;
            if (time < firstArr[s]) firstArr[s] = (int)time;
            time += Ls[paths[v][k + 1]];
        }
    }
    vector<vector<int>> inc(I);
    for (int s = 0; s < S; s++)
        if (cnt[s] > 0) inc[Es[s]].push_back(s);

    sch.assign(I, Sched());

    auto setMode = [&](int mode) {
        for (int X = 0; X < I; X++) {
            if (inc[X].empty()) {
                sch[X] = Sched();
                continue;
            }
            vector<int> order = inc[X];
            if (mode == 3) {
                sort(order.begin(), order.end(), [&](int a, int b) {
                    if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
                    return a < b;
                });
            } else {
                sort(order.begin(), order.end(), [&](int a, int b) {
                    if (firstArr[a] != firstArr[b]) return firstArr[a] < firstArr[b];
                    return a < b;
                });
            }
            Sched s;
            for (int st : order) {
                int T;
                if (mode == 0 || mode == 3) T = 1;
                else if (mode == 1) T = min(D, cnt[st]);
                else T = max(1, min(D, cnt[st] / 4));
                s.st.push_back(st);
                s.du.push_back(max(1, T));
            }
            rebuild(s);
            sch[X] = s;
        }
    };

    // Allocate simulation buffers
    nxtc.assign(V, -1);
    posc.assign(V, 0);
    arrc.assign(V, 0);

    long long best = -1;
    vector<Sched> bestSch;
    for (int mode = 0; mode < 4; mode++) {
        setMode(mode);
        long long sc = simulate();
        if (sc > best) {
            best = sc;
            bestSch = sch;
        }
    }
    sch = bestSch;
    long long cur = best;

    // Local search
    auto startTime = chrono::steady_clock::now();
    const double LIMIT = 8.0;
    RNG rng{88172645463325252ULL};

    vector<int> candX;
    for (int X = 0; X < I; X++)
        if (!inc[X].empty()) candX.push_back(X);

    if (!candX.empty()) {
        while (true) {
            double el = chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
            if (el > LIMIT) break;

            int X = candX[rng.next() % candX.size()];
            Sched old = sch[X];
            Sched ns = old;
            int r = rng.next() % 100;
            if (ns.st.empty() || r < 15) {
                vector<int> avail;
                for (int s : inc[X]) {
                    bool present = false;
                    for (int q : ns.st)
                        if (q == s) { present = true; break; }
                    if (!present) avail.push_back(s);
                }
                if (!avail.empty()) {
                    int s = avail[rng.next() % avail.size()];
                    int pos = ns.st.empty() ? 0 : (int)(rng.next() % (ns.st.size() + 1));
                    ns.st.insert(ns.st.begin() + pos, s);
                    ns.du.insert(ns.du.begin() + pos, 1);
                } else {
                    continue;
                }
            } else if (r < 50) {
                int k = rng.next() % ns.st.size();
                int d = (rng.next() % 2) ? 1 : -1;
                ns.du[k] = max(1, min(D, ns.du[k] + d));
            } else if (r < 70) {
                if (ns.st.size() >= 2) {
                    int a = rng.next() % ns.st.size();
                    int b = rng.next() % ns.st.size();
                    swap(ns.st[a], ns.st[b]);
                    swap(ns.du[a], ns.du[b]);
                } else {
                    continue;
                }
            } else if (r < 85) {
                if (ns.st.size() >= 2) {
                    int k = rng.next() % ns.st.size();
                    ns.st.erase(ns.st.begin() + k);
                    ns.du.erase(ns.du.begin() + k);
                } else {
                    continue;
                }
            } else {
                int k = rng.next() % ns.st.size();
                int hi = max(1, min(D, 2 * ns.du[k]));
                ns.du[k] = 1 + (int)(rng.next() % hi);
            }
            rebuild(ns);
            sch[X] = ns;
            long long sc = simulate();
            if (sc >= cur) {
                cur = sc;
            } else {
                sch[X] = old;
            }
        }
    }

    // Output
    int A = 0;
    for (int X = 0; X < I; X++)
        if (!sch[X].st.empty()) A++;
    string out;
    out += to_string(A) + "\n";
    for (int X = 0; X < I; X++) {
        if (sch[X].st.empty()) continue;
        out += to_string(X) + "\n";
        out += to_string(sch[X].st.size()) + "\n";
        for (size_t i = 0; i < sch[X].st.size(); i++) {
            out += names[sch[X].st[i]];
            out += ' ';
            out += to_string(sch[X].du[i]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
