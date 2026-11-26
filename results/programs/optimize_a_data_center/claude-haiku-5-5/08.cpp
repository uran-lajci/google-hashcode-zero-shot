#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R, S, U, P, M;
int SZR, PS;
vector<vector<ll>> rt;   // per pool: max over rows of pool load in row
vector<ll> T, gcv;       // pool total, guaranteed capacity
vector<int> mt;          // min-tree over pools (stores index)
ll sumGC = 0;
vector<int> pool, pr, psl, zs, cs;

static inline ll valOf(int idx) { return idx < 0 ? LLONG_MAX : gcv[idx]; }
static inline int betterIdx(int a, int b) { return valOf(a) <= valOf(b) ? a : b; }

void updMin(int i) {
    int pos = (PS + i) >> 1;
    while (pos >= 1) {
        mt[pos] = betterIdx(mt[2 * pos], mt[2 * pos + 1]);
        pos >>= 1;
    }
}

void refreshPool(int i) {
    ll nv = T[i] - rt[i][1];
    sumGC += nv - gcv[i];
    gcv[i] = nv;
    updMin(i);
}

void addRow(int p, int r, ll d) {
    int pos = SZR + r;
    rt[p][pos] += d;
    pos >>= 1;
    while (pos >= 1) {
        rt[p][pos] = max(rt[p][2 * pos], rt[p][2 * pos + 1]);
        pos >>= 1;
    }
    T[p] += d;
    refreshPool(p);
}

void setPool(int k, int b) {
    int a = pool[k];
    if (a == b) return;
    if (a >= 0) addRow(a, pr[k], -(ll)cs[k]);
    pool[k] = b;
    if (b >= 0) addRow(b, pr[k], (ll)cs[k]);
}

double F() {
    return (double)gcv[mt[1]] + (double)sumGC / (100.0 * P);
}

int main() {
    auto startT = chrono::steady_clock::now();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> R >> S >> U >> P >> M;
    vector<vector<char>> blocked(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blocked[r][s] = 1;
    }
    zs.assign(M, 0);
    cs.assign(M, 0);
    for (int k = 0; k < M; k++) cin >> zs[k] >> cs[k];

    // free segments per row
    vector<vector<pair<int, int>>> segs(R);
    vector<int> freeCnt(R, 0), maxLen(R, 0);
    for (int r = 0; r < R; r++) {
        int s = 0;
        while (s < S) {
            if (blocked[r][s]) { s++; continue; }
            int st = s;
            while (s < S && !blocked[r][s]) s++;
            segs[r].push_back({st, s - st});
            freeCnt[r] += s - st;
            maxLen[r] = max(maxLen[r], s - st);
        }
    }

    vector<int> ord(M);
    iota(ord.begin(), ord.end(), 0);
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) {
        if (cs[a] != cs[b]) return cs[a] > cs[b];
        return zs[a] < zs[b];
    });

    pr.assign(M, -1);
    psl.assign(M, -1);
    pool.assign(M, -1);

    // placement
    for (int k : ord) {
        int z = zs[k];
        int best = -1;
        for (int r = 0; r < R; r++) {
            if (maxLen[r] >= z && (best < 0 || freeCnt[r] > freeCnt[best])) best = r;
        }
        if (best < 0) continue;
        int bj = -1;
        for (int j = 0; j < (int)segs[best].size(); j++) {
            if (segs[best][j].second >= z) {
                if (bj < 0 || segs[best][j].second < segs[best][bj].second) bj = j;
            }
        }
        auto &sg = segs[best][bj];
        pr[k] = best;
        psl[k] = sg.first;
        sg.first += z;
        sg.second -= z;
        if (sg.second == 0) segs[best].erase(segs[best].begin() + bj);
        freeCnt[best] -= z;
        int ml = 0;
        for (auto &p : segs[best]) ml = max(ml, p.second);
        maxLen[best] = ml;
    }

    // trees
    SZR = 1;
    while (SZR < R) SZR <<= 1;
    PS = 1;
    while (PS < P) PS <<= 1;
    rt.assign(P, vector<ll>(2 * SZR, 0));
    T.assign(P, 0);
    gcv.assign(P, 0);
    mt.assign(2 * PS, -1);
    for (int i = 0; i < P; i++) mt[PS + i] = i;
    for (int pos = PS - 1; pos >= 1; pos--) mt[pos] = betterIdx(mt[2 * pos], mt[2 * pos + 1]);

    // initial pool assignment: put each server into current bottleneck pool
    vector<int> placedList;
    for (int k : ord) {
        if (pr[k] < 0) continue;
        placedList.push_back(k);
        int b = mt[1];
        setPool(k, b);
    }

    ll bestMin = gcv[mt[1]];
    vector<int> bestPool = pool;

    int n = placedList.size();
    if (P > 1 && n >= 2) {
        mt19937 rng(12345);
        auto u01 = [&]() { return (rng() >> 8) * (1.0 / (1 << 24)); };
        double avgc = 0;
        for (int k : placedList) avgc += cs[k];
        avgc /= n;
        double budget = 8.0;
        double T0 = max(1.0, avgc * 0.5), Te = 0.5;
        double temp = T0;
        long long it = 0;
        while (true) {
            if ((it & 255) == 0) {
                double el = chrono::duration<double>(chrono::steady_clock::now() - startT).count();
                if (el > budget) break;
                double fr = el / budget;
                temp = T0 * pow(Te / T0, fr);
            }
            it++;
            int k = placedList[rng() % n];
            double oldf = F();
            if (rng() & 1) {
                int b = rng() % P;
                int a = pool[k];
                if (a == b) continue;
                setPool(k, b);
                double nf = F();
                double d = nf - oldf;
                if (d >= 0 || u01() < exp(d / temp)) {
                    ll m = gcv[mt[1]];
                    if (m > bestMin) { bestMin = m; bestPool = pool; }
                } else {
                    setPool(k, a);
                }
            } else {
                int k2 = placedList[rng() % n];
                int a = pool[k], b = pool[k2];
                if (a == b) continue;
                setPool(k, b);
                setPool(k2, a);
                double nf = F();
                double d = nf - oldf;
                if (d >= 0 || u01() < exp(d / temp)) {
                    ll m = gcv[mt[1]];
                    if (m > bestMin) { bestMin = m; bestPool = pool; }
                } else {
                    setPool(k2, b);
                    setPool(k, a);
                }
            }
        }
    }

    // output
    string out;
    out.reserve((size_t)M * 16);
    for (int k = 0; k < M; k++) {
        if (pr[k] < 0) {
            out += "x\n";
        } else {
            out += to_string(pr[k]);
            out += ' ';
            out += to_string(psl[k]);
            out += ' ';
            out += to_string(bestPool[k]);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
