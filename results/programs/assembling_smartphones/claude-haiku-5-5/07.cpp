#include <bits/stdc++.h>
using namespace std;

int W, H, R, M, T, L;
vector<char> isMount, occ;
vector<int> stampArr, parArr;
int stampCur = 0;
long long budgetBFS = 300000000LL;
vector<int> bq;

vector<int> S;
vector<vector<int>> pts;
vector<long long> internalCost;
vector<int> status; // 0 free, 1 claimed, 2 closed (done or banned)
long long totalScore = 0;

struct Arm {
    int mount;
    vector<int> path;
    int task = -1;
    int pi = 0;
    vector<int> done;
    deque<int> plan;
    int blocked = 0;
    int stall = 0;
    string act;
};
vector<Arm> arms;

inline int X(int c) { return c % W; }
inline int Y(int c) { return c / W; }
inline bool freeCell(int c) { return !occ[c] && !isMount[c]; }

long long manh(int a, int b) {
    return llabs((long long)X(a) - X(b)) + llabs((long long)Y(a) - Y(b));
}

char dirChar(int from, int to) {
    int dx = X(to) - X(from), dy = Y(to) - Y(from);
    if (dx == 1) return 'R';
    if (dx == -1) return 'L';
    if (dy == 1) return 'U';
    return 'D';
}

bool bfs(int src, int dst, vector<int>& out) {
    if (budgetBFS < 0) return false;
    stampCur++;
    bq.clear();
    bq.push_back(src);
    stampArr[src] = stampCur;
    size_t head = 0;
    bool found = false;
    while (head < bq.size()) {
        if (budgetBFS < 0) break;
        int c = bq[head++];
        budgetBFS--;
        if (c == dst) { found = true; break; }
        int x = X(c), y = Y(c);
        int nb[4];
        int cnt = 0;
        if (x + 1 < W) nb[cnt++] = c + 1;
        if (x > 0) nb[cnt++] = c - 1;
        if (y + 1 < H) nb[cnt++] = c + W;
        if (y > 0) nb[cnt++] = c - W;
        for (int k = 0; k < cnt; k++) {
            int n = nb[k];
            if (stampArr[n] == stampCur) continue;
            if (!freeCell(n)) continue;
            stampArr[n] = stampCur;
            parArr[n] = c;
            bq.push_back(n);
        }
    }
    if (!found) return false;
    out.clear();
    for (int c = dst; c != src; c = parArr[c]) out.push_back(c);
    reverse(out.begin(), out.end());
    return true;
}

bool pick(Arm& a, int s) {
    int tip = a.path.back();
    int bt = -1;
    double bestRatio = -1.0;
    for (int t = 0; t < T; t++) {
        if (status[t] != 0) continue;
        long long cost = manh(tip, pts[t][0]) + internalCost[t];
        if ((long long)s + cost > L) continue;
        double ratio = (double)S[t] / (double)(cost + 1);
        if (ratio > bestRatio) { bestRatio = ratio; bt = t; }
    }
    if (bt < 0) return false;
    status[bt] = 1;
    a.task = bt;
    a.pi = 0;
    a.plan.clear();
    a.blocked = 0;
    a.stall = 0;
    return true;
}

void zeroCost(Arm& a, int s) {
    for (;;) {
        if (a.task < 0) {
            if (!pick(a, s)) return;
        }
        int target = pts[a.task][a.pi];
        if (a.path.back() != target) return;
        a.pi++;
        a.plan.clear();
        a.blocked = 0;
        a.stall = 0;
        if (a.pi == (int)pts[a.task].size()) {
            a.done.push_back(a.task);
            totalScore += S[a.task];
            status[a.task] = 2;
            a.task = -1;
            a.pi = 0;
        }
    }
}

void doExpand(Arm& a, int s, int nx) {
    int tip = a.path.back();
    a.act[s] = dirChar(tip, nx);
    occ[nx] = 1;
    a.path.push_back(nx);
}

void doRetract(Arm& a, int s) {
    int tip = a.path.back();
    int prev = a.path[a.path.size() - 2];
    a.act[s] = dirChar(tip, prev);
    occ[tip] = 0;
    a.path.pop_back();
}

void action(Arm& a, int s) {
    if (a.task < 0) {
        if (a.path.size() > 1) doRetract(a, s);
        return;
    }
    int tip = a.path.back();
    int tg = pts[a.task][a.pi];
    a.stall++;
    if (a.stall > 2000) {
        status[a.task] = 2;
        a.task = -1;
        a.pi = 0;
        a.plan.clear();
        a.stall = 0;
        a.blocked = 0;
        if (a.path.size() > 1) doRetract(a, s);
        return;
    }
    if (!a.plan.empty()) {
        int nx = a.plan.front();
        if (freeCell(nx)) {
            doExpand(a, s, nx);
            a.plan.pop_front();
            a.blocked = 0;
            return;
        }
        a.blocked++;
        if (a.blocked < 3) return; // wait
        a.plan.clear();
        a.blocked = 0;
    }
    vector<int> p;
    if (bfs(tip, tg, p) && !p.empty()) {
        a.plan.assign(p.begin(), p.end());
        int nx = a.plan.front();
        if (freeCell(nx)) {
            doExpand(a, s, nx);
            a.plan.pop_front();
        }
        return;
    }
    if (a.path.size() > 1) doRetract(a, s);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> W >> H >> R >> M >> T >> L;
    isMount.assign((size_t)W * H, 0);
    occ.assign((size_t)W * H, 0);
    stampArr.assign((size_t)W * H, 0);
    parArr.assign((size_t)W * H, -1);

    vector<int> mountCells(M);
    for (int i = 0; i < M; i++) {
        int x, y;
        cin >> x >> y;
        mountCells[i] = y * W + x;
        isMount[mountCells[i]] = 1;
    }

    S.resize(T);
    pts.resize(T);
    internalCost.assign(T, 0);
    status.assign(T, 0);
    vector<char> isAssembly((size_t)W * H, 0);
    for (int t = 0; t < T; t++) {
        int P;
        cin >> S[t] >> P;
        pts[t].resize(P);
        for (int i = 0; i < P; i++) {
            int x, y;
            cin >> x >> y;
            pts[t][i] = y * W + x;
            isAssembly[pts[t][i]] = 1;
        }
        for (int i = 1; i < P; i++) internalCost[t] += manh(pts[t][i - 1], pts[t][i]);
    }

    // prefix sums of assembly points
    vector<int> pre((size_t)(H + 1) * (W + 1), 0);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            pre[(y + 1) * (W + 1) + (x + 1)] = pre[y * (W + 1) + (x + 1)] + pre[(y + 1) * (W + 1) + x] - pre[y * (W + 1) + x] + (isAssembly[y * W + x] ? 1 : 0);

    const int K = 3;
    vector<pair<long long, int>> scored;
    for (int i = 0; i < M; i++) {
        int c = mountCells[i];
        int x = X(c), y = Y(c);
        int x0 = max(0, x - K), x1 = min(W - 1, x + K);
        int y0 = max(0, y - K), y1 = min(H - 1, y + K);
        long long cnt = pre[(y1 + 1) * (W + 1) + (x1 + 1)] - pre[y0 * (W + 1) + (x1 + 1)] - pre[(y1 + 1) * (W + 1) + x0] + pre[y0 * (W + 1) + x0];
        scored.push_back({-cnt, c});
    }
    sort(scored.begin(), scored.end());

    int useArms = min(R, M);
    for (int i = 0; i < useArms; i++) {
        Arm a;
        a.mount = scored[i].second;
        a.path.push_back(a.mount);
        occ[a.mount] = 1;
        a.act.assign(L, 'W');
        arms.push_back(a);
    }

    for (int s = 0; s < L; s++) {
        for (auto& a : arms) {
            zeroCost(a, s);
            action(a, s);
        }
    }
    for (auto& a : arms) zeroCost(a, L);

    vector<int> outIdx;
    for (int i = 0; i < (int)arms.size(); i++)
        if (!arms[i].done.empty()) outIdx.push_back(i);

    string out;
    if (outIdx.empty()) {
        out += "1\n";
        out += to_string(X(mountCells.empty() ? 0 : mountCells[0])) + " " + to_string(Y(mountCells.empty() ? 0 : mountCells[0])) + " 1 1\n";
        out += "0\n";
        out += "W\n";
        cout << out;
        return 0;
    }

    out += to_string(outIdx.size()) + "\n";
    for (int idx : outIdx) {
        Arm& a = arms[idx];
        int last = 0;
        for (int i = 0; i < L; i++)
            if (a.act[i] != 'W') last = i + 1;
        int Kc = max(1, last);
        out += to_string(X(a.mount)) + " " + to_string(Y(a.mount)) + " " + to_string(a.done.size()) + " " + to_string(Kc) + "\n";
        for (size_t i = 0; i < a.done.size(); i++) {
            if (i) out += " ";
            out += to_string(a.done[i]);
        }
        out += "\n";
        for (int i = 0; i < Kc; i++) {
            if (i) out += " ";
            out += a.act[i];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
