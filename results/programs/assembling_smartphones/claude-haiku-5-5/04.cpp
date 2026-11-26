#include <bits/stdc++.h>
using namespace std;

int W, H, R, M, T, L;
vector<char> occ;
vector<int> stampv, par, qbuf;
int curStamp = 0;
vector<int> clearsList;

struct Task {
    int S;
    vector<int> pts;
    long long internal;
};
vector<Task> tasks;
vector<char> taken;

struct Arm {
    int mount;
    vector<int> chain;
    int taskId = -1;
    int idx = 0;
    vector<int> done;
    vector<int> plan;
    size_t pos = 0;
    int planTarget = -1;
    int blocked = 0;
    int cooldown = 0;
    vector<char> ins;
};

chrono::steady_clock::time_point startTime;

double elapsedSec() {
    return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
}

inline int manh(int a, int b) {
    return abs(a % W - b % W) + abs(a / W - b / W);
}

char dirChar(int a, int b) {
    int ax = a % W, ay = a / W, bx = b % W, by = b / W;
    if (ay == by) return (bx == ax + 1) ? 'R' : 'L';
    return (by == ay + 1) ? 'U' : 'D';
}

bool bfs(int start, int target, vector<int>& path) {
    curStamp++;
    int head = 0, tail = 0;
    qbuf[tail++] = start;
    stampv[start] = curStamp;
    bool found = false;
    while (head < tail && !found) {
        int c = qbuf[head++];
        int x = c % W, y = c / W;
        int nbs[4];
        int cnt = 0;
        if (x + 1 < W) nbs[cnt++] = c + 1;
        if (x > 0) nbs[cnt++] = c - 1;
        if (y + 1 < H) nbs[cnt++] = c + W;
        if (y > 0) nbs[cnt++] = c - W;
        for (int k = 0; k < cnt; k++) {
            int nb = nbs[k];
            if (stampv[nb] == curStamp || occ[nb]) continue;
            stampv[nb] = curStamp;
            par[nb] = c;
            if (nb == target) { found = true; break; }
            qbuf[tail++] = nb;
        }
    }
    if (!found) return false;
    path.clear();
    for (int c = target; c != start; c = par[c]) path.push_back(c);
    reverse(path.begin(), path.end());
    return true;
}

void chooseTask(Arm& a, int step) {
    int g = a.chain.back();
    int best = -1;
    double bv = -1;
    for (int t = 0; t < T; t++) {
        if (taken[t]) continue;
        long long est = manh(g, tasks[t].pts[0]) + tasks[t].internal;
        if ((long long)step + est > L) continue;
        double v = tasks[t].S / (double)(est + 1);
        if (v > bv) { bv = v; best = t; }
    }
    if (best < 0) return;
    taken[best] = 1;
    a.taskId = best;
    a.idx = 0;
    a.plan.clear();
    a.planTarget = -1;
    a.blocked = 0;
    a.cooldown = 0;
}

void advanceArm(Arm& a, int step) {
    while (a.taskId >= 0) {
        const vector<int>& p = tasks[a.taskId].pts;
        int P = p.size();
        while (a.idx < P && a.chain.back() == p[a.idx]) a.idx++;
        if (a.idx < P) return;
        a.done.push_back(a.taskId);
        a.taskId = -1;
        chooseTask(a, step);
    }
}

char retractArm(Arm& a) {
    int g = a.chain.back();
    int nb = a.chain[a.chain.size() - 2];
    char c = dirChar(g, nb);
    clearsList.push_back(g);
    a.chain.pop_back();
    a.plan.clear();
    a.planTarget = -1;
    return c;
}

char decideArm(Arm& a) {
    if (a.taskId < 0) {
        if (a.chain.size() > 1) return retractArm(a);
        return 'W';
    }
    int target = tasks[a.taskId].pts[a.idx];
    int g = a.chain.back();
    if (a.planTarget != target || a.pos >= a.plan.size()) {
        if (a.cooldown > 0) {
            a.cooldown--;
            if (a.chain.size() > 1) return retractArm(a);
            return 'W';
        }
        vector<int> path;
        if (bfs(g, target, path)) {
            a.plan = path;
            a.pos = 0;
            a.planTarget = target;
            a.blocked = 0;
        } else {
            a.cooldown = 4;
            a.planTarget = -1;
            a.plan.clear();
            if (a.chain.size() > 1) return retractArm(a);
            return 'W';
        }
    }
    int nxt = a.plan[a.pos];
    if (!occ[nxt]) {
        occ[nxt] = 1;
        a.chain.push_back(nxt);
        a.pos++;
        a.blocked = 0;
        return dirChar(g, nxt);
    }
    a.blocked++;
    if (a.blocked >= 3) {
        a.plan.clear();
        a.planTarget = -1;
        a.blocked = 0;
    }
    return 'W';
}

int main() {
    startTime = chrono::steady_clock::now();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> W >> H >> R >> M >> T >> L;
    int N = W * H;
    occ.assign(N, 0);
    stampv.assign(N, 0);
    par.assign(N, -1);
    qbuf.assign(N, 0);

    vector<int> mounts(M);
    for (int i = 0; i < M; i++) {
        int x, y;
        cin >> x >> y;
        mounts[i] = y * W + x;
        occ[mounts[i]] = 1;
    }

    tasks.resize(T);
    for (int t = 0; t < T; t++) {
        int S, P;
        cin >> S >> P;
        tasks[t].S = S;
        tasks[t].pts.resize(P);
        for (int i = 0; i < P; i++) {
            int x, y;
            cin >> x >> y;
            tasks[t].pts[i] = y * W + x;
        }
        long long in = 0;
        for (int i = 0; i + 1 < P; i++) in += manh(tasks[t].pts[i], tasks[t].pts[i + 1]);
        tasks[t].internal = in;
    }
    taken.assign(T, 0);

    // Choose mounts: those with highest value for nearby high-score tasks.
    vector<double> mval(M, 0.0);
    for (int m = 0; m < M; m++) {
        double v = 0;
        for (int t = 0; t < T; t++) {
            v += tasks[t].S / (1.0 + manh(mounts[m], tasks[t].pts[0]));
        }
        mval[m] = v;
    }
    vector<int> order(M);
    for (int i = 0; i < M; i++) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        if (mval[a] != mval[b]) return mval[a] > mval[b];
        return a < b;
    });

    vector<Arm> arms(R);
    for (int a = 0; a < R; a++) {
        arms[a].mount = mounts[order[a]];
        arms[a].chain.push_back(arms[a].mount);
    }

    for (int a = 0; a < R; a++) chooseTask(arms[a], 0);

    int steps = 0;
    for (int s = 0; s < L; s++) {
        if (elapsedSec() > 8.0) break;
        for (int a = 0; a < R; a++) {
            advanceArm(arms[a], s);
            char c = decideArm(arms[a]);
            arms[a].ins.push_back(c);
        }
        for (int c : clearsList) occ[c] = 0;
        clearsList.clear();
        steps = s + 1;
    }

    for (int a = 0; a < R; a++) advanceArm(arms[a], steps);

    vector<int> used;
    for (int a = 0; a < R; a++) {
        if (!arms[a].done.empty() && steps > 0) used.push_back(a);
    }

    if (used.empty()) {
        cout << 0 << "\n";
        return 0;
    }

    string out;
    out += to_string(used.size()) + "\n";
    for (int a : used) {
        const Arm& ar = arms[a];
        out += to_string(ar.mount % W) + " " + to_string(ar.mount / W) + " " +
               to_string(ar.done.size()) + " " + to_string(steps) + "\n";
        for (size_t i = 0; i < ar.done.size(); i++) {
            if (i) out += " ";
            out += to_string(ar.done[i]);
        }
        out += "\n";
        for (int i = 0; i < steps; i++) {
            if (i) out += " ";
            out += ar.ins[i];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
