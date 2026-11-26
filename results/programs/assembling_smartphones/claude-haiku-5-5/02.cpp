#include <bits/stdc++.h>
using namespace std;

static int W, H, R, M, T, L, N;
static vector<int> ownerArr;
static vector<char> markArr;
static vector<int> visArr, parArr, qu;
static int stampCnt = 0;
static long long budgetUsed = 0;
static const long long BUDGET = 700000000LL;
static bool budgetOver = false;
static const long long INF = (long long)4e18;

struct LogE { char type; int c; };
static vector<LogE> lg;

struct Arm {
    int mount;
    vector<int> body;
    vector<char> moves;
    vector<int> tasks;
};

static vector<int> taskScore;
static vector<vector<int>> taskPts;

static char dirChar(int u, int v) {
    if (v == u + W) return 'U';
    if (v == u - W) return 'D';
    if (v == u + 1) return 'R';
    return 'L';
}

static void pushCell(Arm& arm, int c) {
    arm.moves.push_back(dirChar(arm.body.back(), c));
    arm.body.push_back(c);
    markArr[c] = 1;
    lg.push_back({'P', c});
}

static void popCell(Arm& arm) {
    int c = arm.body.back();
    arm.body.pop_back();
    markArr[c] = 0;
    arm.moves.push_back(dirChar(c, arm.body.back()));
    lg.push_back({'Q', c});
}

// BFS within cells owned by 'a'. If useMark, marked (body) cells are blocked.
static bool bfs(int src, int tgt, int a, bool useMark, vector<int>& path) {
    path.clear();
    ++stampCnt;
    int h = 0, tl = 0;
    qu[tl++] = src;
    visArr[src] = stampCnt;
    while (h < tl) {
        int c = qu[h++];
        ++budgetUsed;
        if (budgetUsed > BUDGET) { budgetOver = true; return false; }
        int x = c % W, y = c / W;
        int nbs[4];
        int cnt = 0;
        if (x + 1 < W) nbs[cnt++] = c + 1;
        if (x > 0) nbs[cnt++] = c - 1;
        if (y + 1 < H) nbs[cnt++] = c + W;
        if (y > 0) nbs[cnt++] = c - W;
        for (int k = 0; k < cnt; ++k) {
            int nb = nbs[k];
            if (visArr[nb] == stampCnt) continue;
            if (ownerArr[nb] != a) continue;
            if (useMark && markArr[nb]) continue;
            visArr[nb] = stampCnt;
            parArr[nb] = c;
            if (nb == tgt) {
                for (int v = nb; v != src; v = parArr[v]) path.push_back(v);
                reverse(path.begin(), path.end());
                return true;
            }
            qu[tl++] = nb;
        }
    }
    return false;
}

static bool trySim(Arm& arm, int a, int t) {
    size_t savedMoves = arm.moves.size();
    lg.clear();
    bool ok = true;
    for (int p : taskPts[t]) {
        int g = arm.body.back();
        if (p == g) continue;
        int sz = (int)arm.body.size();
        vector<int> pa, pc;
        bool fa = bfs(g, p, a, true, pa);
        if (budgetOver) { ok = false; break; }
        long long costA = fa ? (long long)pa.size() : INF;

        int j = -1;
        if (markArr[p]) {
            for (int k = sz - 1; k >= 0; --k)
                if (arm.body[k] == p) { j = k; break; }
        }
        long long costB = (j >= 0) ? (long long)(sz - 1 - j) : INF;

        int mount = arm.body[0];
        bool fc = bfs(mount, p, a, false, pc);
        if (budgetOver) { ok = false; break; }
        long long costC = fc ? (long long)(sz - 1) + (long long)pc.size() : INF;

        if (costA >= INF && costB >= INF && costC >= INF) { ok = false; break; }

        if (costA <= costB && costA <= costC) {
            for (int c : pa) pushCell(arm, c);
        } else if (costB <= costC) {
            while ((int)arm.body.size() - 1 > j) popCell(arm);
        } else {
            while (arm.body.size() > 1) popCell(arm);
            for (int c : pc) pushCell(arm, c);
        }
        if (arm.moves.size() > (size_t)L) { ok = false; break; }
    }
    if (!ok) {
        for (int i = (int)lg.size() - 1; i >= 0; --i) {
            if (lg[i].type == 'P') {
                arm.body.pop_back();
                markArr[lg[i].c] = 0;
            } else {
                arm.body.push_back(lg[i].c);
                markArr[lg[i].c] = 1;
            }
        }
        arm.moves.resize(savedMoves);
        lg.clear();
        return false;
    }
    lg.clear();
    arm.tasks.push_back(t);
    return true;
}

// Multi-source BFS. srcs[k] gets owner k. blocked cells get owner -2.
static void computeOwner(const vector<int>& srcs, const vector<int>& blocked) {
    ownerArr.assign(N, -1);
    for (int b : blocked) ownerArr[b] = -2;
    vector<int> q;
    q.reserve(N);
    for (int k = 0; k < (int)srcs.size(); ++k) {
        ownerArr[srcs[k]] = k;
        q.push_back(srcs[k]);
    }
    size_t h = 0;
    while (h < q.size()) {
        int c = q[h++];
        int own = ownerArr[c];
        int x = c % W, y = c / W;
        int nbs[4];
        int cnt = 0;
        if (x + 1 < W) nbs[cnt++] = c + 1;
        if (x > 0) nbs[cnt++] = c - 1;
        if (y + 1 < H) nbs[cnt++] = c + W;
        if (y > 0) nbs[cnt++] = c - W;
        for (int k = 0; k < cnt; ++k) {
            int nb = nbs[k];
            if (ownerArr[nb] == -1) {
                ownerArr[nb] = own;
                q.push_back(nb);
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> W >> H >> R >> M >> T >> L;
    N = W * H;
    vector<int> mounts(M);
    for (int i = 0; i < M; ++i) {
        int x, y;
        cin >> x >> y;
        mounts[i] = y * W + x;
    }
    taskScore.resize(T);
    taskPts.resize(T);
    for (int t = 0; t < T; ++t) {
        int S, P;
        cin >> S >> P;
        taskScore[t] = S;
        taskPts[t].resize(P);
        for (int k = 0; k < P; ++k) {
            int x, y;
            cin >> x >> y;
            taskPts[t][k] = y * W + x;
        }
    }

    markArr.assign(N, 0);
    visArr.assign(N, 0);
    parArr.assign(N, -1);
    qu.assign(N, 0);

    // Pass 1: regions from all mount points; count feasible tasks per mount.
    computeOwner(mounts, {});
    vector<int> cnt(M, 0);
    for (int t = 0; t < T; ++t) {
        int o = ownerArr[taskPts[t][0]];
        if (o < 0) continue;
        bool all = true;
        for (int p : taskPts[t]) if (ownerArr[p] != o) { all = false; break; }
        if (all) cnt[o]++;
    }
    vector<int> ord;
    for (int i = 0; i < M; ++i) if (cnt[i] > 0) ord.push_back(i);
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
        return a < b;
    });
    if ((int)ord.size() > R) ord.resize(R);

    if (ord.empty()) {
        // No feasible task at all; output a minimal syntactically valid submission.
        int m = mounts[0];
        cout << 1 << "\n" << (m % W) << " " << (m / W) << " 1 1\n0\nW\n";
        return 0;
    }

    vector<int> chosenCells;
    vector<char> isChosen(M, 0);
    for (int i : ord) { chosenCells.push_back(mounts[i]); isChosen[i] = 1; }
    vector<int> blocked;
    for (int i = 0; i < M; ++i) if (!isChosen[i]) blocked.push_back(mounts[i]);

    // Pass 2: regions from chosen mounts only.
    computeOwner(chosenCells, blocked);
    int A = (int)chosenCells.size();

    vector<Arm> arms(A);
    for (int a = 0; a < A; ++a) {
        arms[a].mount = chosenCells[a];
        arms[a].body.push_back(chosenCells[a]);
        markArr[chosenCells[a]] = 1;
    }

    for (int a = 0; a < A; ++a) {
        vector<int> cand;
        for (int t = 0; t < T; ++t) {
            bool all = true;
            for (int p : taskPts[t]) if (ownerArr[p] != a) { all = false; break; }
            if (all) cand.push_back(t);
        }
        Arm& arm = arms[a];
        while (!cand.empty() && !budgetOver) {
            int g = arm.body.back();
            int gx = g % W, gy = g / W;
            int bestK = -1;
            double bestRatio = -1.0;
            for (int k = 0; k < (int)cand.size(); ++k) {
                int t = cand[k];
                long long est = 0;
                int px = gx, py = gy;
                for (int p : taskPts[t]) {
                    int qx = p % W, qy = p / W;
                    est += abs(qx - px) + abs(qy - py);
                    px = qx; py = qy;
                }
                double ratio = (double)taskScore[t] / (double)(est + 1);
                if (ratio > bestRatio) { bestRatio = ratio; bestK = k; }
            }
            int t = cand[bestK];
            cand.erase(cand.begin() + bestK);
            trySim(arm, a, t);
        }
    }

    // Output
    vector<int> used;
    for (int a = 0; a < A; ++a) if (!arms[a].tasks.empty()) used.push_back(a);
    if (used.empty()) {
        int m = mounts[0];
        cout << 1 << "\n" << (m % W) << " " << (m / W) << " 1 1\n0\nW\n";
        return 0;
    }
    string out;
    out += to_string(used.size()) + "\n";
    for (int a : used) {
        Arm& arm = arms[a];
        out += to_string(arm.mount % W) + " " + to_string(arm.mount / W) + " " +
               to_string(arm.tasks.size()) + " " + to_string(arm.moves.size()) + "\n";
        for (size_t i = 0; i < arm.tasks.size(); ++i) {
            if (i) out += " ";
            out += to_string(arm.tasks[i]);
        }
        out += "\n";
        for (size_t i = 0; i < arm.moves.size(); ++i) {
            if (i) out += " ";
            out += arm.moves[i];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
