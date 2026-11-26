#include <bits/stdc++.h>
using namespace std;

static int W, H, R, M, T, L, N;
static vector<char> isMount, occ;
static vector<int> stampA, parA, distA, qv;
static int stampCur = 0;
static long long workCount = 0;
static const long long WORK_LIMIT = 250000000LL;
static bool workExceeded = false;
static const int MAXK = 40;

struct Arm {
    int mount;
    vector<int> body;
    string ins;
};
static vector<Arm> arms;

struct LogEnt {
    int arm;
    bool push;
    int cell;
};
static vector<LogEnt> logv;

static inline int manh(int a, int b) {
    return abs(a % W - b % W) + abs(a / W - b / W);
}

static char dirChar(int from, int to) {
    int fx = from % W, fy = from / W, tx = to % W, ty = to / W;
    if (tx == fx + 1) return 'R';
    if (tx == fx - 1) return 'L';
    if (ty == fy + 1) return 'U';
    return 'D';
}

static void moveTo(int a, int n) {
    Arm &A = arms[a];
    int cur = A.body.back();
    A.ins.push_back(dirChar(cur, n));
    if (A.body.size() >= 2 && A.body[A.body.size() - 2] == n) {
        A.body.pop_back();
        occ[cur] = 0;
        logv.push_back({a, false, cur});
    } else {
        A.body.push_back(n);
        occ[n] = 1;
        logv.push_back({a, true, n});
    }
}

// BFS from src to target avoiding occupied and mount cells, depth <= maxD.
static int bfs(int src, int target, int maxD, vector<int> &path) {
    path.clear();
    if (src == target) return 0;
    if (maxD <= 0) return -1;
    stampCur++;
    qv.clear();
    qv.push_back(src);
    stampA[src] = stampCur;
    distA[src] = 0;
    size_t head = 0;
    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};
    while (head < qv.size()) {
        int u = qv[head++];
        if (++workCount > WORK_LIMIT) {
            workExceeded = true;
            return -1;
        }
        int ux = u % W, uy = u / W;
        for (int d = 0; d < 4; d++) {
            int nx = ux + dx[d], ny = uy + dy[d];
            if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
            int v = ny * W + nx;
            if (stampA[v] == stampCur || occ[v] || isMount[v]) continue;
            stampA[v] = stampCur;
            distA[v] = distA[u] + 1;
            parA[v] = u;
            if (v == target) {
                int cur = v;
                while (cur != src) {
                    path.push_back(cur);
                    cur = parA[cur];
                }
                reverse(path.begin(), path.end());
                return distA[v];
            }
            if (distA[v] < maxD) qv.push_back(v);
        }
    }
    return -1;
}

// Move the gripper of arm a to cell g (retracting and/or extending).
static bool moveSeg(int a, int g) {
    Arm &A = arms[a];
    int len = A.body.size();
    int rem = L - (int)A.ins.size();
    if (rem <= 0) return false;
    int best = INT_MAX, bestK = -1;
    vector<int> bestPath, p;
    for (int k = 0; k < len && k <= MAXK && k <= rem; k++) {
        int j = len - 1 - k;
        int src = A.body[j];
        for (int i = j + 1; i < len; i++) occ[A.body[i]] = 0;
        int maxD = (best == INT_MAX) ? rem - k : min(rem - k, best - k - 1);
        int d = -1;
        if (maxD >= 0) d = bfs(src, g, maxD, p);
        for (int i = j + 1; i < len; i++) occ[A.body[i]] = 1;
        if (workExceeded) return false;
        if (d >= 0 && k + d < best) {
            best = k + d;
            bestK = k;
            bestPath = p;
        }
    }
    if (bestK < 0) return false;
    for (int i = 0; i < bestK; i++) {
        moveTo(a, arms[a].body[arms[a].body.size() - 2]);
    }
    for (int c : bestPath) moveTo(a, c);
    return true;
}

static bool attempt(int a, const vector<int> &pts, int now) {
    Arm &A = arms[a];
    while ((int)A.ins.size() < now) A.ins.push_back('W');
    size_t saved = A.ins.size();
    logv.clear();
    bool ok = true;
    for (int p : pts) {
        if (arms[a].body.back() == p) continue;
        if (!moveSeg(a, p)) {
            ok = false;
            break;
        }
    }
    if (workExceeded) ok = false;
    if (!ok) {
        for (int i = (int)logv.size() - 1; i >= 0; i--) {
            const LogEnt &e = logv[i];
            if (e.push) {
                arms[e.arm].body.pop_back();
                occ[e.cell] = 0;
            } else {
                arms[e.arm].body.push_back(e.cell);
                occ[e.cell] = 1;
            }
        }
        A.ins.resize(saved);
        return false;
    }
    return true;
}

struct Cand {
    double d;
    int a, t;
};

int main() {
    if (scanf("%d %d %d %d %d %d", &W, &H, &R, &M, &T, &L) != 6) return 0;
    N = W * H;
    isMount.assign(N, 0);
    occ.assign(N, 0);
    stampA.assign(N, 0);
    parA.assign(N, 0);
    distA.assign(N, 0);

    vector<int> mountCells(M);
    for (int i = 0; i < M; i++) {
        int x, y;
        scanf("%d %d", &x, &y);
        mountCells[i] = y * W + x;
        isMount[mountCells[i]] = 1;
    }
    vector<int> S(T);
    vector<vector<int>> pts(T);
    vector<long long> chain(T, 0);
    for (int t = 0; t < T; t++) {
        int P;
        scanf("%d %d", &S[t], &P);
        pts[t].resize(P);
        for (int i = 0; i < P; i++) {
            int x, y;
            scanf("%d %d", &x, &y);
            pts[t][i] = y * W + x;
        }
        for (int i = 0; i + 1 < P; i++) chain[t] += manh(pts[t][i], pts[t][i + 1]);
    }

    // Choose R mount points by proximity to valuable tasks' first points.
    vector<pair<double, int>> ms;
    for (int m = 0; m < M; m++) {
        int c = mountCells[m];
        double sc = 0;
        for (int t = 0; t < T; t++) sc += (double)S[t] / (1.0 + manh(c, pts[t][0]));
        ms.push_back({-sc, c});
    }
    sort(ms.begin(), ms.end());
    for (int i = 0; i < R && i < M; i++) {
        int c = ms[i].second;
        Arm A;
        A.mount = c;
        A.body = {c};
        A.ins = "";
        occ[c] = 1;
        arms.push_back(A);
    }
    int numArms = arms.size();

    int now = 0;
    vector<char> done(T, 0);
    vector<vector<int>> armTasks(numArms);

    auto cmp = [](const Cand &x, const Cand &y) {
        if (x.d != y.d) return x.d > y.d;
        if (x.a != y.a) return x.a < y.a;
        return x.t < y.t;
    };

    while (!workExceeded) {
        int rem = L - now;
        if (rem <= 0) break;
        vector<Cand> cands;
        for (int a = 0; a < numArms; a++) {
            int tip = arms[a].body.back();
            for (int t = 0; t < T; t++) {
                if (done[t]) continue;
                long long est = manh(tip, pts[t][0]) + chain[t];
                if (est > rem) continue;
                double d = (double)S[t] / (double)max<long long>(est, 1);
                cands.push_back({d, a, t});
            }
        }
        if (cands.empty()) break;
        size_t top = min<size_t>(cands.size(), 30);
        partial_sort(cands.begin(), cands.begin() + top, cands.end(), cmp);
        bool any = false;
        for (size_t i = 0; i < top; i++) {
            Cand c = cands[i];
            if (attempt(c.a, pts[c.t], now)) {
                done[c.t] = 1;
                armTasks[c.a].push_back(c.t);
                now = arms[c.a].ins.size();
                any = true;
                break;
            }
            if (workExceeded) break;
        }
        if (!any) break;
    }

    int used = 0;
    for (int a = 0; a < numArms; a++)
        if (!armTasks[a].empty()) used++;

    string out;
    out += to_string(used) + "\n";
    for (int a = 0; a < numArms; a++) {
        if (armTasks[a].empty()) continue;
        int x = arms[a].mount % W, y = arms[a].mount / W;
        out += to_string(x) + " " + to_string(y) + " " +
               to_string(armTasks[a].size()) + " " +
               to_string(arms[a].ins.size()) + "\n";
        for (size_t i = 0; i < armTasks[a].size(); i++) {
            if (i) out += " ";
            out += to_string(armTasks[a][i]);
        }
        out += "\n";
        const string &ins = arms[a].ins;
        for (size_t i = 0; i < ins.size(); i++) {
            if (i) out += " ";
            out += ins[i];
        }
        out += "\n";
    }
    fputs(out.c_str(), stdout);
    return 0;
}
