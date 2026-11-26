#include <bits/stdc++.h>
using namespace std;

int W, H, R, M, T, L;
vector<char> obst;          // permanently blocked cells (mounts + finished arm bodies)
vector<int> bidx;           // index of cell in current arm's body, -1 if none
vector<int> stampv, par, dst, qv;
int stampCur = 0;
long long nodeCount = 0;
const long long NODE_LIMIT = 400000000LL;
const int LIM = 8;

struct Task { int S; vector<int> pts; };
vector<Task> tasks;
vector<int> sortedIdx;

vector<int> body;
vector<char> mv;
int budgetG;

const int DX[4] = {1, -1, 0, 0};
const int DY[4] = {0, 0, 1, -1};

bool bfs(int s, int target, int lim, int D, vector<int>& path) {
    path.clear();
    if (s == target) return true;
    if (D <= 0) return false;
    stampCur++;
    qv.clear();
    size_t h = 0;
    qv.push_back(s);
    stampv[s] = stampCur;
    dst[s] = 0;
    bool found = false;
    while (h < qv.size() && !found) {
        int c = qv[h++];
        nodeCount++;
        if (nodeCount > NODE_LIMIT) return false;
        if (dst[c] >= D) continue;
        int cx = c % W, cy = c / W;
        for (int d = 0; d < 4 && !found; d++) {
            int nx = cx + DX[d], ny = cy + DY[d];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            int nb = ny * W + nx;
            if (stampv[nb] == stampCur) continue;
            if (obst[nb]) continue;
            if (bidx[nb] >= 0 && bidx[nb] < lim) continue;
            stampv[nb] = stampCur;
            par[nb] = c;
            dst[nb] = dst[c] + 1;
            qv.push_back(nb);
            if (nb == target) found = true;
        }
    }
    if (!found) return false;
    for (int c = target; c != s; c = par[c]) path.push_back(c);
    reverse(path.begin(), path.end());
    return true;
}

char dirChar(int a, int b) {
    int dx = (b % W) - (a % W);
    int dy = (b / W) - (a / W);
    if (dx == 1) return 'R';
    if (dx == -1) return 'L';
    if (dy == 1) return 'U';
    return 'D';
}

bool tryTask(const Task& tk) {
    for (int p : tk.pts) {
        int k = (int)body.size() - 1;
        if (body[k] == p) continue;
        int best = INT_MAX, bestI = -1;
        vector<int> bestPath;
        int lo = max(0, k - LIM);
        for (int i = k; i >= lo; i--) {
            int rc = k - i;
            int left = budgetG - (int)mv.size() - rc;
            if (left < 0) break;
            vector<int> path;
            if (bfs(body[i], p, i, left, path)) {
                int cost = rc + (int)path.size();
                if (cost < best) {
                    best = cost;
                    bestI = i;
                    bestPath = path;
                }
            }
        }
        if (bestI < 0) return false;
        // retract to bestI
        for (int j = k; j > bestI; j--) {
            int a = body[j], b = body[j - 1];
            mv.push_back(dirChar(a, b));
            bidx[a] = -1;
            body.pop_back();
        }
        int cur = body.back();
        for (int c : bestPath) {
            mv.push_back(dirChar(cur, c));
            body.push_back(c);
            bidx[c] = (int)body.size() - 1;
            cur = c;
        }
    }
    return true;
}

struct Res {
    long long score = 0;
    vector<int> body;
    vector<char> mv;
    vector<int> order;
};

Res simulate(int mc, int t0, const vector<char>& asgIn) {
    vector<char> asg = asgIn;
    body.assign(1, mc);
    bidx[mc] = 0;
    mv.clear();
    budgetG = L - t0;
    Res r;
    for (int ti : sortedIdx) {
        if (asg[ti]) continue;
        if ((int)mv.size() >= budgetG) break;
        vector<int> saveBody = body;
        size_t saveMv = mv.size();
        if (tryTask(tasks[ti])) {
            asg[ti] = 1;
            r.order.push_back(ti);
            r.score += tasks[ti].S;
        } else {
            for (int c : body) bidx[c] = -1;
            body = saveBody;
            for (int idx = 0; idx < (int)body.size(); idx++) bidx[body[idx]] = idx;
            mv.resize(saveMv);
        }
    }
    r.body = body;
    r.mv = mv;
    for (int c : body) bidx[c] = -1;
    body.clear();
    return r;
}

struct Out {
    int x, y;
    vector<int> tasksOrder;
    vector<char> instr;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> W >> H >> R >> M >> T >> L;
    int N = W * H;
    obst.assign(N, 0);
    bidx.assign(N, -1);
    stampv.assign(N, 0);
    par.assign(N, -1);
    dst.assign(N, 0);

    vector<int> mountCells(M);
    for (int i = 0; i < M; i++) {
        int x, y;
        cin >> x >> y;
        mountCells[i] = y * W + x;
        obst[mountCells[i]] = 1;
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
    }

    sortedIdx.resize(T);
    for (int i = 0; i < T; i++) sortedIdx[i] = i;
    stable_sort(sortedIdx.begin(), sortedIdx.end(), [&](int a, int b) {
        if (tasks[a].S != tasks[b].S) return tasks[a].S > tasks[b].S;
        return tasks[a].pts.size() < tasks[b].pts.size();
    });

    vector<char> asgG(T, 0);
    vector<char> usedMount(N, 0);
    int cursor = 0;
    vector<Out> outs;

    for (int slot = 0; slot < R; slot++) {
        if (cursor >= L) break;
        if (nodeCount > NODE_LIMIT) break;
        int ti0 = -1;
        for (int ti : sortedIdx) if (!asgG[ti]) { ti0 = ti; break; }
        if (ti0 < 0) break;
        int p0 = tasks[ti0].pts[0];
        int px = p0 % W, py = p0 / W;

        vector<pair<int, int>> cand;
        for (int mc : mountCells) {
            if (usedMount[mc]) continue;
            int d = abs(mc % W - px) + abs(mc / W - py);
            cand.push_back({d, mc});
        }
        if (cand.empty()) break;
        sort(cand.begin(), cand.end());
        if (cand.size() > 6) cand.resize(6);

        Res best;
        bool have = false;
        int bestMc = -1;
        for (auto& pr : cand) {
            int mc = pr.second;
            Res r = simulate(mc, cursor, asgG);
            if (!have || r.score > best.score ||
                (r.score == best.score && r.mv.size() < best.mv.size())) {
                best = r;
                bestMc = mc;
                have = true;
            }
        }
        if (!have || best.score <= 0) break;

        usedMount[bestMc] = 1;
        for (int ti : best.order) asgG[ti] = 1;
        for (int c : best.body) obst[c] = 1;

        Out o;
        o.x = bestMc % W;
        o.y = bestMc / W;
        o.tasksOrder = best.order;
        o.instr.assign(cursor, 'W');
        for (char c : best.mv) o.instr.push_back(c);
        cursor += (int)best.mv.size();
        outs.push_back(o);
    }

    ostringstream out;
    if (outs.empty()) {
        // No feasible plan found; emit a syntactically valid (score-zero) placeholder.
        int mc = mountCells[0];
        out << 1 << "\n" << (mc % W) << " " << (mc / W) << " 1 1\n0\nW\n";
    } else {
        out << outs.size() << "\n";
        for (auto& o : outs) {
            out << o.x << " " << o.y << " " << o.tasksOrder.size() << " " << o.instr.size() << "\n";
            for (size_t i = 0; i < o.tasksOrder.size(); i++) {
                if (i) out << " ";
                out << o.tasksOrder[i];
            }
            out << "\n";
            for (size_t i = 0; i < o.instr.size(); i++) {
                if (i) out << " ";
                out << o.instr[i];
            }
            out << "\n";
        }
    }
    cout << out.str();
    return 0;
}
