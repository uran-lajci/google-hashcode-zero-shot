#include <bits/stdc++.h>
using namespace std;

static int W, H, R, M, T, L;
static vector<char> isMount, occ;
static vector<int> stampArr, parArr;
static int curStamp = 0;
static vector<int> qbuf;
static vector<int> body;
static vector<char> moves;
static int tStart;
static vector<long long> S;
static vector<vector<int>> pts;
static vector<int> internalD;
static vector<char> taskUsed;
static chrono::steady_clock::time_point startTime;

static double elapsedSec() {
    return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
}

static inline int manh(int a, int b) {
    int ax = a % W, ay = a / W, bx = b % W, by = b / W;
    return abs(ax - bx) + abs(ay - by);
}

static inline char dirChar(int a, int b) {
    int dx = b % W - a % W, dy = b / W - a / W;
    if (dx == 1) return 'R';
    if (dx == -1) return 'L';
    if (dy == 1) return 'U';
    return 'D';
}

// BFS from src to dst through cells that are not occupied and not mount points.
// Returns cells after src up to and including dst; empty if unreachable.
static vector<int> bfsPath(int src, int dst) {
    if (src == dst) return {};
    curStamp++;
    qbuf.clear();
    qbuf.push_back(src);
    stampArr[src] = curStamp;
    size_t head = 0;
    bool found = false;
    while (head < qbuf.size() && !found) {
        int c = qbuf[head++];
        int x = c % W, y = c / W;
        int nb[4];
        int cnt = 0;
        if (x + 1 < W) nb[cnt++] = c + 1;
        if (x > 0) nb[cnt++] = c - 1;
        if (y + 1 < H) nb[cnt++] = c + W;
        if (y > 0) nb[cnt++] = c - W;
        for (int i = 0; i < cnt; i++) {
            int n = nb[i];
            if (stampArr[n] == curStamp || occ[n] || isMount[n]) continue;
            stampArr[n] = curStamp;
            parArr[n] = c;
            qbuf.push_back(n);
            if (n == dst) { found = true; break; }
        }
    }
    if (!found) return {};
    vector<int> path;
    int c = dst;
    while (c != src) {
        path.push_back(c);
        c = parArr[c];
    }
    reverse(path.begin(), path.end());
    return path;
}

static void setBody(const vector<int>& nb) {
    for (int c : body) occ[c] = 0;
    body = nb;
    for (int c : body) occ[c] = 1;
}

static void retractTo(int j) {
    while ((int)body.size() > j + 1) {
        int c = body.back();
        body.pop_back();
        occ[c] = 0;
        moves.push_back(dirChar(c, body.back()));
    }
}

static void extendPath(const vector<int>& path) {
    for (int n : path) {
        moves.push_back(dirChar(body.back(), n));
        body.push_back(n);
        occ[n] = 1;
    }
}

// Move the gripper of the current arm to target using the cheapest valid option.
static bool moveTo(int target) {
    int tip = body.back();
    if (tip == target) return true;
    int k = (int)body.size() - 1;
    int bestCost = INT_MAX, choice = 0, jIdx = -1;

    vector<int> pathA = bfsPath(tip, target);
    if (!pathA.empty()) { bestCost = (int)pathA.size(); choice = 1; }

    for (int j = 0; j < k; j++) if (body[j] == target) jIdx = j;
    if (jIdx >= 0) {
        int cost = k - jIdx;
        if (cost < bestCost) { bestCost = cost; choice = 3; }
    }

    vector<int> pathB;
    if (k >= 1) {
        for (size_t i = 1; i < body.size(); i++) occ[body[i]] = 0;
        pathB = bfsPath(body[0], target);
        for (size_t i = 1; i < body.size(); i++) occ[body[i]] = 1;
        if (!pathB.empty()) {
            int cost = k + (int)pathB.size();
            if (cost < bestCost) { bestCost = cost; choice = 2; }
        }
    }

    if (choice == 0) return false;
    if (choice == 1) extendPath(pathA);
    else if (choice == 3) retractTo(jIdx);
    else { retractTo(0); extendPath(pathB); }
    return true;
}

struct ArmOut {
    int mx, my;
    vector<int> tasks;
    int K;
    string instr;
};

int main() {
    startTime = chrono::steady_clock::now();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> W >> H >> R >> M >> T >> L;
    int N = W * H;
    isMount.assign(N, 0);
    occ.assign(N, 0);
    stampArr.assign(N, 0);
    parArr.assign(N, -1);

    vector<int> mountCells(M);
    for (int i = 0; i < M; i++) {
        int x, y;
        cin >> x >> y;
        mountCells[i] = y * W + x;
        isMount[mountCells[i]] = 1;
    }

    S.assign(T, 0);
    pts.assign(T, {});
    internalD.assign(T, 0);
    taskUsed.assign(T, 0);
    for (int t = 0; t < T; t++) {
        int P;
        cin >> S[t] >> P;
        pts[t].resize(P);
        for (int i = 0; i < P; i++) {
            int x, y;
            cin >> x >> y;
            pts[t][i] = y * W + x;
        }
        int d = 0;
        for (int i = 0; i + 1 < P; i++) d += manh(pts[t][i], pts[t][i + 1]);
        internalD[t] = d;
    }

    vector<char> mountUsed(M, 0);
    int tGlobal = 0;
    vector<ArmOut> outs;
    int armsCount = 0;

    while (armsCount < R) {
        if (elapsedSec() > 8.0) break;

        // Choose mount and task pair with best score per estimated distance.
        int bm = -1;
        double br = -1;
        for (int mi = 0; mi < M; mi++) {
            if (mountUsed[mi]) continue;
            int mc = mountCells[mi];
            for (int t = 0; t < T; t++) {
                if (taskUsed[t]) continue;
                int d = manh(mc, pts[t][0]) + internalD[t];
                if (tGlobal + d > L) continue;
                double r = (double)S[t] / (d + 1.0);
                if (r > br) { br = r; bm = mi; }
            }
        }
        if (bm < 0) break;

        int mc = mountCells[bm];
        mountUsed[bm] = 1;
        body.assign(1, mc);
        occ[mc] = 1;
        moves.clear();
        tStart = tGlobal;

        vector<int> armTasks;
        vector<char> failed(T, 0);

        while (true) {
            if (elapsedSec() > 9.0) break;
            int tip = body.back();
            int now = tStart + (int)moves.size();
            int k = (int)body.size() - 1;
            int best = -1;
            double r2 = -1;
            for (int t = 0; t < T; t++) {
                if (taskUsed[t] || failed[t]) continue;
                int est = manh(tip, pts[t][0]) + internalD[t];
                if (now + est + k > L) continue;
                double r = (double)S[t] / (est + 1.0);
                if (r > r2) { r2 = r; best = t; }
            }
            if (best < 0) break;

            vector<int> savedBody = body;
            size_t sm = moves.size();
            bool ok = true;
            for (int p : pts[best]) {
                if (!moveTo(p)) { ok = false; break; }
                if (tStart + (int)moves.size() + (int)body.size() - 1 > L) { ok = false; break; }
            }
            if (ok) {
                taskUsed[best] = 1;
                armTasks.push_back(best);
            } else {
                setBody(savedBody);
                moves.resize(sm);
                failed[best] = 1;
            }
        }

        if (armTasks.empty()) {
            occ[mc] = 0;
            body.clear();
            continue;
        }

        retractTo(0);
        occ[mc] = 0;
        body.clear();

        ArmOut ao;
        ao.mx = mc % W;
        ao.my = mc / W;
        ao.tasks = armTasks;
        ao.K = tStart + (int)moves.size();
        string s;
        s.reserve(ao.K * 2 + 4);
        for (int i = 0; i < tStart; i++) s += "W ";
        for (size_t i = 0; i < moves.size(); i++) {
            s += moves[i];
            s += ' ';
        }
        if (!s.empty()) s.pop_back();
        ao.instr = s;
        outs.push_back(ao);

        tGlobal = tStart + (int)moves.size();
        armsCount++;
    }

    string out;
    out += to_string(outs.size()) + "\n";
    for (auto& ao : outs) {
        out += to_string(ao.mx) + " " + to_string(ao.my) + " " +
               to_string(ao.tasks.size()) + " " + to_string(ao.K) + "\n";
        for (size_t i = 0; i < ao.tasks.size(); i++) {
            if (i) out += ' ';
            out += to_string(ao.tasks[i]);
        }
        out += "\n";
        out += ao.instr;
        out += "\n";
    }
    fputs(out.c_str(), stdout);
    return 0;
}
