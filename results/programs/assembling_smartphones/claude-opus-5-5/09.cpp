#include <bits/stdc++.h>
using namespace std;

const int BIG = 100000000;
int W, H, R, M, T, L;
vector<char> isMount;
vector<vector<pair<int,int>>> blk;
vector<int> posv;
vector<int> gT, pmv, par_, dep, vis;
int stampv = 0;
long long totalExp = 0;
const long long EXP_BUDGET = 60000000LL;
chrono::steady_clock::time_point T0;

inline int manh(int a, int b) { return abs(a % W - b % W) + abs(a / W - b / W); }

inline int freeDl(int c, int t) {
    int dl = BIG;
    for (auto &p : blk[c]) {
        if (p.first <= t && t <= p.second) return -1;
        if (p.first > t && p.first < dl) dl = p.first;
    }
    return dl;
}

struct State {
    vector<int> st, dl, pm, ent;
    int s = 0;
    vector<char> moves;
    vector<array<int,3>> done;
    vector<int> tasks;
};

inline char dirOf(int a, int b) {
    int d = b - a;
    if (d == 1) return 'R';
    if (d == -1) return 'L';
    if (d == W) return 'U';
    return 'D';
}

void pushCell(State &S, int c) {
    S.moves.push_back(dirOf(S.st.back(), c));
    S.s++;
    int idx = S.st.size();
    int d = freeDl(c, S.s);
    S.st.push_back(c);
    S.dl.push_back(d);
    S.pm.push_back(min(S.pm.back(), d + idx));
    S.ent.push_back(S.s);
    posv[c] = idx;
}
void popCell(State &S) {
    int n = S.st.size();
    int top = S.st[n - 1];
    S.moves.push_back(dirOf(top, S.st[n - 2]));
    S.done.push_back({top, S.ent[n - 1], S.s});
    S.s++;
    S.st.pop_back(); S.dl.pop_back(); S.pm.pop_back(); S.ent.pop_back();
}
void waitStep(State &S) { S.moves.push_back('W'); S.s++; }

inline bool inPrefix(const State &S, int c, int len) {
    int p = posv[c];
    return p >= 0 && p < len && p < (int)S.st.size() && S.st[p] == c;
}

struct Node { int f, g, c; bool operator<(const Node &o) const { if (f != o.f) return f > o.f; return g < o.g; } };

bool astar(const State &S, int i, int t0, int q, vector<int> &path) {
    if (t0 >= L) return false;
    int n0 = i + 1;
    int start = S.st[i];
    ++stampv;
    priority_queue<Node> pq;
    vis[start] = stampv; gT[start] = t0; pmv[start] = S.pm[i]; dep[start] = 0; par_[start] = -1;
    pq.push({t0 + manh(start, q), t0, start});
    int cap = 10 * manh(start, q) + 1000;
    int pops = 0;
    while (!pq.empty()) {
        Node nd = pq.top(); pq.pop();
        if (++pops > cap) return false;
        totalExp++;
        int c = nd.c, g = gT[c];
        if (g >= L) continue;
        int x = c % W, y = c / W;
        int nb[4]; int k = 0;
        if (x + 1 < W) nb[k++] = c + 1;
        if (x > 0) nb[k++] = c - 1;
        if (y + 1 < H) nb[k++] = c + W;
        if (y > 0) nb[k++] = c - W;
        for (int j = 0; j < k; j++) {
            int v = nb[j];
            if (isMount[v] || vis[v] == stampv) continue;
            if (inPrefix(S, v, n0)) continue;
            int t = g + 1;
            int dlv = freeDl(v, t);
            if (dlv < 0) continue;
            int idx = n0 + dep[c];
            int key = dlv + idx;
            int np = min(pmv[c], key);
            if (t + idx + 1 > np) continue;
            vis[v] = stampv; gT[v] = t; pmv[v] = np; dep[v] = dep[c] + 1; par_[v] = c;
            if (v == q) {
                path.clear();
                int u = v;
                while (u != start) { path.push_back(u); u = par_[u]; }
                reverse(path.begin(), path.end());
                return true;
            }
            pq.push({t + manh(v, q), t, v});
        }
    }
    return false;
}

bool planPoint(State &S, int q) {
    int n = S.st.size();
    if (S.st[n - 1] == q) return true;
    int b1 = -1, b2 = -1, e1 = INT_MAX, e2 = INT_MAX;
    for (int i = n - 1; i >= 0; i--) {
        int e = (n - 1 - i) + manh(S.st[i], q);
        if (e < e1) { b2 = b1; e2 = e1; b1 = i; e1 = e; }
        else if (e < e2) { b2 = i; e2 = e; }
    }
    vector<int> opts;
    if (b1 >= 0) opts.push_back(b1);
    if (b2 >= 0) opts.push_back(b2);
    if (b1 != n - 1 && b2 != n - 1) opts.push_back(n - 1);
    vector<int> path;
    int waits[2] = {0, 3};
    for (int i : opts) {
        for (int wi = 0; wi < 2; wi++) {
            int w = waits[wi];
            int r = n - 1 - i;
            int t0 = S.s + r + w;
            if (t0 > L) continue;
            if (w > 0 && t0 + i + 1 > S.pm[i]) continue;
            if (S.st[i] == q) {
                if (w) continue;
                for (int k = 0; k < r; k++) popCell(S);
                return true;
            }
            if (astar(S, i, t0, q, path)) {
                for (int k = 0; k < r; k++) popCell(S);
                for (int k = 0; k < w; k++) waitStep(S);
                for (int c : path) pushCell(S, c);
                return true;
            }
        }
    }
    return false;
}

vector<int> tS, tLen;
vector<vector<int>> tPts;

bool planTask(State &S, int t) {
    for (int p : tPts[t]) if (!planPoint(S, p)) return false;
    return true;
}

void restorePos(const State &S) { for (int i = 0; i < (int)S.st.size(); i++) posv[S.st[i]] = i; }

bool timeUp() {
    if (totalExp > EXP_BUDGET) return true;
    double el = chrono::duration<double>(chrono::steady_clock::now() - T0).count();
    return el > 8.5;
}

int main() {
    T0 = chrono::steady_clock::now();
    if (scanf("%d %d %d %d %d %d", &W, &H, &R, &M, &T, &L) != 6) return 0;
    int N = W * H;
    isMount.assign(N, 0);
    blk.assign(N, {});
    posv.assign(N, -1);
    gT.assign(N, 0); pmv.assign(N, 0); par_.assign(N, -1); dep.assign(N, 0); vis.assign(N, 0);
    vector<int> mounts(M);
    for (int i = 0; i < M; i++) { int x, y; scanf("%d %d", &x, &y); mounts[i] = y * W + x; isMount[mounts[i]] = 1; }
    tS.resize(T); tLen.resize(T); tPts.resize(T);
    for (int t = 0; t < T; t++) {
        int s, p; scanf("%d %d", &s, &p);
        tS[t] = s;
        tPts[t].resize(p);
        for (int j = 0; j < p; j++) { int x, y; scanf("%d %d", &x, &y); tPts[t][j] = y * W + x; }
        int len = 0;
        for (int j = 1; j < p; j++) len += manh(tPts[t][j - 1], tPts[t][j]);
        tLen[t] = len;
    }
    vector<char> assigned(T, 0), mountUsed(M, 0);
    vector<pair<int, State>> result;
    for (int a = 0; a < R && !timeUp(); a++) {
        for (int attempt = 0; attempt < 4 && !timeUp(); attempt++) {
            int bestM = -1; double bestV = 0;
            for (int m = 0; m < M; m++) {
                if (mountUsed[m]) continue;
                int c = mounts[m]; int x = c % W, y = c / W;
                bool okN = false;
                if (x + 1 < W && !isMount[c + 1]) okN = true;
                if (x > 0 && !isMount[c - 1]) okN = true;
                if (y + 1 < H && !isMount[c + W]) okN = true;
                if (y > 0 && !isMount[c - W]) okN = true;
                if (!okN) continue;
                double v = 0;
                for (int t = 0; t < T; t++) {
                    if (assigned[t]) continue;
                    int d = manh(c, tPts[t][0]) + tLen[t];
                    if (d > L) continue;
                    v += (double)tS[t] / (d + 1);
                }
                if (v > bestV) { bestV = v; bestM = m; }
            }
            if (bestM < 0) break;
            mountUsed[bestM] = 1;
            int mc = mounts[bestM];
            State S;
            S.st = {mc}; S.dl = {BIG}; S.pm = {BIG}; S.ent = {0}; S.s = 0;
            posv[mc] = 0;
            while (!timeUp()) {
                int n = S.st.size();
                int top = S.st.back();
                vector<pair<double,int>> cand;
                for (int t = 0; t < T; t++) {
                    if (assigned[t]) continue;
                    int p0 = tPts[t][0];
                    int e = min(manh(top, p0), (n - 1) + manh(mc, p0)) + tLen[t];
                    if (S.s + e > L) continue;
                    cand.push_back({-(double)tS[t] / (e + 1), t});
                }
                if (cand.empty()) break;
                int K = min((int)cand.size(), 10);
                partial_sort(cand.begin(), cand.begin() + K, cand.end());
                bool have = false; State best; double bestR = -1; int bestT = -1;
                int succ = 0;
                for (int k = 0; k < K && succ < 4; k++) {
                    int t = cand[k].second;
                    State C = S;
                    bool ok = planTask(C, t);
                    restorePos(S);
                    if (ok) {
                        succ++;
                        int cost = C.s - S.s;
                        double rr = (double)tS[t] / (cost + 1);
                        if (rr > bestR) { bestR = rr; best = move(C); bestT = t; have = true; }
                    }
                    if (timeUp()) break;
                }
                if (!have) break;
                S = move(best);
                restorePos(S);
                S.tasks.push_back(bestT);
                assigned[bestT] = 1;
            }
            if (S.tasks.empty()) continue;
            // final retraction
            while (S.st.size() > 1 && S.s < L) popCell(S);
            for (auto &d : S.done) blk[d[0]].push_back({d[1] - 1, d[2] + 1});
            for (int i = 1; i < (int)S.st.size(); i++) blk[S.st[i]].push_back({S.ent[i] - 1, L + 1});
            result.push_back({mc, move(S)});
            break;
        }
    }
    string out;
    out += to_string(result.size()) + "\n";
    for (auto &pr : result) {
        int mc = pr.first; State &S = pr.second;
        out += to_string(mc % W) + " " + to_string(mc / W) + " " + to_string(S.tasks.size()) + " " + to_string(S.moves.size()) + "\n";
        for (size_t i = 0; i < S.tasks.size(); i++) { if (i) out += ' '; out += to_string(S.tasks[i]); }
        out += "\n";
        for (size_t i = 0; i < S.moves.size(); i++) { if (i) out += ' '; out += S.moves[i]; }
        out += "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
