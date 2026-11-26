#include <bits/stdc++.h>
using namespace std;

static const int INF = INT_MAX;
int W, H, R, M, T, L;
vector<int> lastUse, owner_, xs, ys, stampv, gv, parv;
int gen_ = 0;
long long budget = 70000000LL;

struct Task { long long score; vector<int> pts; long long internal; };
vector<Task> tasks;
vector<char> assignedT;

struct Arm {
    int mount; vector<int> path; int clock = 0; string moves; vector<int> done;
    vector<char> fail; bool finished = false;
};
vector<Arm> arms;
struct LogE { int cell, lu, ow; };
vector<LogE> logv;

inline int manh(int a, int b) { return abs(xs[a] - xs[b]) + abs(ys[a] - ys[b]); }
inline char dirc(int u, int v) {
    if (xs[v] > xs[u]) return 'R';
    if (xs[v] < xs[u]) return 'L';
    if (ys[v] > ys[u]) return 'U';
    return 'D';
}
inline void setCell(int c, int lu, int ow) {
    logv.push_back({c, lastUse[c], owner_[c]});
    lastUse[c] = lu; owner_[c] = ow;
}

struct Node { int f, g, c; };
struct Cmp { bool operator()(const Node& a, const Node& b) const {
    if (a.f != b.f) return a.f > b.f; return a.g < b.g; } };

bool planTo(int ai, int target) {
    Arm& A = arms[ai];
    if (A.path.back() == target) return true;
    if (lastUse[target] == INF && owner_[target] != ai) return false;
    gen_++;
    priority_queue<Node, vector<Node>, Cmp> pq;
    int len = A.path.size();
    for (int j = 0; j < len; j++) {
        int tm = A.clock + (len - 1 - j);
        if (tm > L) continue;
        int c = A.path[j];
        stampv[c] = gen_; gv[c] = tm; parv[c] = -(j + 2);
        pq.push({tm + manh(c, target), tm, c});
        budget--;
    }
    bool found = false; long long localPops = 0;
    static const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    while (!pq.empty()) {
        Node n = pq.top(); pq.pop();
        budget--; localPops++;
        if (budget < 0 || localPops > 1500000) break;
        if (n.g != gv[n.c]) continue;
        if (n.c == target) { found = true; break; }
        int x = xs[n.c], y = ys[n.c];
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
            int v = ny * W + nx;
            int lu = lastUse[v];
            if (lu == INF) continue;
            int nt = max(n.g + 1, lu + 1);
            if (nt > L) continue;
            if (stampv[v] != gen_ || nt < gv[v]) {
                stampv[v] = gen_; gv[v] = nt; parv[v] = n.c;
                pq.push({nt + manh(v, target), nt, v});
            }
        }
    }
    if (!found) return false;
    vector<int> route;
    int c = target;
    while (parv[c] >= 0) { route.push_back(c); c = parv[c]; }
    int j = -parv[c] - 2;
    reverse(route.begin(), route.end());
    while ((int)A.path.size() - 1 > j) {
        int x = A.path.back();
        setCell(x, A.clock, -1);
        A.moves.push_back(dirc(x, A.path[A.path.size() - 2]));
        A.clock++;
        A.path.pop_back();
    }
    for (int v : route) {
        while (A.clock + 1 <= lastUse[v]) { A.moves.push_back('W'); A.clock++; }
        setCell(v, INF, ai);
        A.moves.push_back(dirc(A.path.back(), v));
        A.clock++;
        A.path.push_back(v);
    }
    return A.clock <= L;
}

bool planTask(int ai, int ti) {
    Arm& A = arms[ai];
    size_t logSz = logv.size();
    vector<int> pathSave = A.path; int clockSave = A.clock; size_t mvSave = A.moves.size();
    bool ok = true;
    for (int p : tasks[ti].pts) {
        if (!planTo(ai, p)) { ok = false; break; }
    }
    if (!ok) {
        while (logv.size() > logSz) {
            LogE e = logv.back(); logv.pop_back();
            lastUse[e.cell] = e.lu; owner_[e.cell] = e.ow;
        }
        A.path = pathSave; A.clock = clockSave; A.moves.resize(mvSave);
        return false;
    }
    logv.clear();
    return true;
}

void retractAll(int ai) {
    Arm& A = arms[ai];
    while (A.path.size() > 1 && A.clock < L) {
        int x = A.path.back();
        lastUse[x] = A.clock; owner_[x] = -1;
        A.moves.push_back(dirc(x, A.path[A.path.size() - 2]));
        A.clock++;
        A.path.pop_back();
    }
}

int main() {
    scanf("%d %d %d %d %d %d", &W, &H, &R, &M, &T, &L);
    int N = W * H;
    lastUse.assign(N, -1); owner_.assign(N, -1); xs.resize(N); ys.resize(N);
    stampv.assign(N, 0); gv.assign(N, 0); parv.assign(N, 0);
    for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) { xs[y * W + x] = x; ys[y * W + x] = y; }
    vector<int> mounts(M);
    for (int i = 0; i < M; i++) { int x, y; scanf("%d %d", &x, &y); mounts[i] = y * W + x; lastUse[mounts[i]] = INF; }
    tasks.resize(T);
    for (int t = 0; t < T; t++) {
        int s, p; scanf("%d %d", &s, &p);
        tasks[t].score = s; tasks[t].pts.resize(p);
        for (int k = 0; k < p; k++) { int x, y; scanf("%d %d", &x, &y); tasks[t].pts[k] = y * W + x; }
        long long in = 0;
        for (int k = 1; k < p; k++) in += manh(tasks[t].pts[k - 1], tasks[t].pts[k]);
        tasks[t].internal = in;
    }
    // mount selection
    vector<double> cur(T, 0.0);
    vector<char> chosen(M, 0);
    vector<int> sel;
    for (int r = 0; r < R && r < M; r++) {
        double best = -1; int bm = -1;
        for (int m = 0; m < M; m++) {
            if (chosen[m]) continue;
            double g = 0;
            int mc = mounts[m];
            for (int t = 0; t < T; t++) {
                double f = (double)tasks[t].score / (manh(mc, tasks[t].pts[0]) + tasks[t].internal + 1.0);
                if (f > cur[t]) g += f - cur[t];
            }
            if (g > best) { best = g; bm = m; }
        }
        if (bm < 0) break;
        chosen[bm] = 1; sel.push_back(bm);
        int mc = mounts[bm];
        for (int t = 0; t < T; t++) {
            double f = (double)tasks[t].score / (manh(mc, tasks[t].pts[0]) + tasks[t].internal + 1.0);
            cur[t] = max(cur[t], f);
        }
    }
    int NA = sel.size();
    arms.resize(NA);
    for (int i = 0; i < NA; i++) {
        arms[i].mount = mounts[sel[i]];
        arms[i].path = {arms[i].mount};
        owner_[arms[i].mount] = i;
        arms[i].fail.assign(T, 0);
    }
    assignedT.assign(T, 0);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> evq;
    for (int i = 0; i < NA; i++) evq.push({0, i});
    while (!evq.empty()) {
        auto [ck, ai] = evq.top(); evq.pop();
        Arm& A = arms[ai];
        if (budget < 0) { continue; }
        bool success = false;
        for (int round = 0; round < 2 && !success; round++) {
            for (int attempt = 0; attempt < 3 && !success; attempt++) {
                if (budget < 0) break;
                int gp = A.path.back();
                double bestV = -1; int bt = -1;
                for (int t = 0; t < T; t++) {
                    if (assignedT[t] || A.fail[t]) continue;
                    int f0 = tasks[t].pts[0];
                    if (lastUse[f0] == INF && owner_[f0] != ai) continue;
                    long long est = manh(gp, f0) + tasks[t].internal;
                    if (A.clock + est > L) continue;
                    double v = (double)tasks[t].score / (est + 1.0);
                    if (v > bestV) { bestV = v; bt = t; }
                }
                if (bt < 0) break;
                if (planTask(ai, bt)) {
                    assignedT[bt] = 1; A.done.push_back(bt); success = true;
                    fill(A.fail.begin(), A.fail.end(), 0);
                } else A.fail[bt] = 1;
            }
            if (!success) {
                if (A.path.size() <= 1) break;
                retractAll(ai);
                fill(A.fail.begin(), A.fail.end(), 0);
            }
        }
        if (success) evq.push({A.clock, ai});
        else { retractAll(ai); A.finished = true; }
    }
    vector<int> outArms;
    for (int i = 0; i < NA; i++) if (!arms[i].done.empty()) outArms.push_back(i);
    string out;
    out += to_string(outArms.size()) + "\n";
    for (int i : outArms) {
        Arm& A = arms[i];
        out += to_string(xs[A.mount]) + " " + to_string(ys[A.mount]) + " " +
               to_string(A.done.size()) + " " + to_string(A.moves.size()) + "\n";
        for (size_t k = 0; k < A.done.size(); k++) { if (k) out += ' '; out += to_string(A.done[k]); }
        out += "\n";
        for (size_t k = 0; k < A.moves.size(); k++) { if (k) out += ' '; out += A.moves[k]; }
        out += "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
