#include <bits/stdc++.h>
using namespace std;

int W, H, R, M, T, L;
vector<char> isMount, occ;
vector<int> visStamp, parentCell;
int stampCur = 0;
long long bfsBudget = 600000000LL;

struct Task {
    int S;
    vector<int> pts;
};
vector<Task> tasks;

vector<int> body;   // body[0] = mount, body.back() = gripper
vector<char> mv;    // instruction characters

char dirChar(int a, int b) {
    int ax = a % W, ay = a / W, bx = b % W, by = b / W;
    int dx = bx - ax, dy = by - ay;
    if (dx == 1 && dy == 0) return 'R';
    if (dx == -1 && dy == 0) return 'L';
    if (dx == 0 && dy == 1) return 'U';
    return 'D';
}

// Move gripper to adjacent cell b. Either retract (b is previous body cell) or expand (b must be free).
void moveTo(int b) {
    char c = dirChar(body.back(), b);
    if (body.size() >= 2 && b == body[body.size() - 2]) {
        occ[body.back()] = 0;
        body.pop_back();
    } else {
        occ[b] = 1;
        body.push_back(b);
    }
    mv.push_back(c);
}

bool goTo(int t, long long limit) {
    while (true) {
        if (body.back() == t) return true;
        if ((long long)mv.size() >= limit) return false;
        if (occ[t]) {
            // t is part of our body: retract toward it
            moveTo(body[body.size() - 2]);
            continue;
        }
        if (bfsBudget <= 0) return false;
        int h = body.back();
        stampCur++;
        vector<int> q;
        q.reserve(64);
        q.push_back(h);
        visStamp[h] = stampCur;
        bool found = false;
        size_t qi = 0;
        while (qi < q.size() && !found) {
            int u = q[qi++];
            int ux = u % W, uy = u / W;
            const int dxs[4] = {1, -1, 0, 0};
            const int dys[4] = {0, 0, 1, -1};
            for (int d = 0; d < 4; d++) {
                int nx = ux + dxs[d], ny = uy + dys[d];
                if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                int v = ny * W + nx;
                if (occ[v] || isMount[v] || visStamp[v] == stampCur) continue;
                visStamp[v] = stampCur;
                parentCell[v] = u;
                if (v == t) { found = true; break; }
                q.push_back(v);
            }
        }
        bfsBudget -= (long long)q.size() + 1;
        if (found) {
            vector<int> path;
            int cur = t;
            while (cur != h) {
                path.push_back(cur);
                cur = parentCell[cur];
            }
            reverse(path.begin(), path.end());
            for (int c : path) {
                if ((long long)mv.size() >= limit) return false;
                moveTo(c);
            }
            return true;
        }
        if (body.size() == 1) return false;
        moveTo(body[body.size() - 2]);
    }
}

bool runTask(const Task& tk, long long limit) {
    for (int p : tk.pts) {
        if (!goTo(p, limit)) return false;
    }
    return true;
}

void undo(const vector<int>& savedBody, size_t mvs) {
    for (int c : body) occ[c] = 0;
    body = savedBody;
    for (int c : body) occ[c] = 1;
    mv.resize(mvs);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> W >> H >> R >> M >> T >> L;
    isMount.assign(W * H, 0);
    occ.assign(W * H, 0);
    visStamp.assign(W * H, 0);
    parentCell.assign(W * H, -1);
    vector<int> mc(M);
    for (int i = 0; i < M; i++) {
        int x, y;
        cin >> x >> y;
        mc[i] = y * W + x;
        isMount[mc[i]] = 1;
    }
    tasks.resize(T);
    for (int i = 0; i < T; i++) {
        int S, P;
        cin >> S >> P;
        tasks[i].S = S;
        tasks[i].pts.resize(P);
        for (int k = 0; k < P; k++) {
            int x, y;
            cin >> x >> y;
            tasks[i].pts[k] = y * W + x;
        }
    }

    vector<char> assigned(T, 0), usedM(M, 0);
    int time = 0;
    int armCount = 0;
    string out;

    for (int a = 0; a < R && time < L; a++) {
        // choose mount point
        int bestMi = -1;
        double bestV = 0;
        for (int i = 0; i < M; i++) {
            if (usedM[i]) continue;
            int m = mc[i];
            int mx = m % W, my = m / W;
            double v = 0;
            for (int j = 0; j < T; j++) {
                if (assigned[j]) continue;
                int p = tasks[j].pts[0];
                double d = abs(mx - p % W) + abs(my - p / W);
                v = max(v, tasks[j].S / (1.0 + d));
            }
            if (v > bestV) { bestV = v; bestMi = i; }
        }
        if (bestMi < 0) break;
        usedM[bestMi] = 1;
        int m = mc[bestMi];

        body.clear();
        body.push_back(m);
        occ[m] = 1;
        mv.clear();
        vector<int> armTasks;

        while (true) {
            int avail = L - time;
            int hx = body.back() % W, hy = body.back() / W;
            vector<pair<double, int>> cand;
            for (int j = 0; j < T; j++) {
                if (assigned[j]) continue;
                long long est = 0;
                int cx = hx, cy = hy;
                for (int p : tasks[j].pts) {
                    int px = p % W, py = p / W;
                    est += abs(px - cx) + abs(py - cy);
                    cx = px; cy = py;
                }
                if (est + (long long)body.size() - 1 > avail) continue;
                cand.push_back({(double)tasks[j].S / (est + 1.0), j});
            }
            if (cand.empty()) break;
            sort(cand.begin(), cand.end(), [](const pair<double, int>& x, const pair<double, int>& y) {
                if (x.first != y.first) return x.first > y.first;
                return x.second < y.second;
            });
            if (cand.size() > 10) cand.resize(10);

            int bestJ = -1;
            double bestVal = -1;
            for (auto& pr : cand) {
                int j = pr.second;
                vector<int> saved = body;
                size_t mvs = mv.size();
                bool ok = runTask(tasks[j], avail);
                if (ok && (long long)mv.size() + (long long)body.size() - 1 <= avail) {
                    double cost = (double)(mv.size() - mvs) + (double)body.size() - 1;
                    double val = tasks[j].S / (cost + 1.0);
                    if (val > bestVal) { bestVal = val; bestJ = j; }
                }
                undo(saved, mvs);
            }
            if (bestJ < 0) break;

            vector<int> saved = body;
            size_t mvs = mv.size();
            bool ok = runTask(tasks[bestJ], avail);
            if (!ok || (long long)mv.size() + (long long)body.size() - 1 > avail) {
                undo(saved, mvs);
                break;
            }
            assigned[bestJ] = 1;
            armTasks.push_back(bestJ);
        }

        // retract fully back to mount point
        while (body.size() > 1) moveTo(body[body.size() - 2]);

        if (!armTasks.empty()) {
            armCount++;
            long long K = (long long)time + (long long)mv.size();
            out += to_string(m % W) + " " + to_string(m / W) + " " +
                   to_string(armTasks.size()) + " " + to_string(K) + "\n";
            for (size_t k = 0; k < armTasks.size(); k++) {
                if (k) out += " ";
                out += to_string(armTasks[k]);
            }
            out += "\n";
            string instr;
            instr.reserve(2 * K);
            for (int s = 0; s < time; s++) {
                instr += "W ";
            }
            for (size_t k = 0; k < mv.size(); k++) {
                instr += mv[k];
                if (k + 1 < mv.size()) instr += " ";
            }
            if (!instr.empty() && instr.back() == ' ') instr.pop_back();
            out += instr + "\n";
            time += (int)mv.size();
        }
        occ[m] = 0;
        body.clear();
        mv.clear();
    }

    cout << armCount << "\n" << out;
    return 0;
}
