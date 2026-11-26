#include <bits/stdc++.h>
using namespace std;

int W, H, R, M, T, L;
vector<pair<int,int>> mountXY;
vector<int> mountId;
vector<char> isMount;
struct Task { long long S; vector<int> pts; };
vector<Task> tasks;

struct ArmPlan { int mx, my; vector<int> tlist; string moves; };
struct Result { long long score; vector<ArmPlan> arms; };

static char dirChar(int a, int b) {
    int ax = a % W, ay = a / W, bx = b % W, by = b / W;
    if (bx == ax + 1 && by == ay) return 'R';
    if (bx == ax - 1 && by == ay) return 'L';
    if (by == ay + 1 && bx == ax) return 'U';
    return 'D';
}

Result solveWith(const vector<int>& chosen) {
    int N = W * H;
    vector<int> label(N, -1), par(N, -1), dist(N, -1);
    vector<int> q;
    q.reserve(N);
    for (int i = 0; i < (int)chosen.size(); i++) {
        int id = mountId[chosen[i]];
        label[id] = i; dist[id] = 0; par[id] = -1;
        q.push_back(id);
    }
    size_t head = 0;
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    while (head < q.size()) {
        int c = q[head++];
        int x = c % W, y = c / W;
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
            int n = ny * W + nx;
            if (isMount[n] || label[n] != -1) continue;
            label[n] = label[c];
            dist[n] = dist[c] + 1;
            par[n] = c;
            q.push_back(n);
        }
    }

    int A = chosen.size();
    vector<vector<int>> cand(A);
    for (int t = 0; t < (int)tasks.size(); t++) {
        int lab = -1;
        bool ok = true;
        for (int p : tasks[t].pts) {
            int l = label[p];
            if (l < 0) { ok = false; break; }
            if (lab == -1) lab = l;
            else if (lab != l) { ok = false; break; }
        }
        if (ok && lab >= 0) cand[lab].push_back(t);
    }

    Result res; res.score = 0;
    vector<char> onBody(N, 0);

    for (int i = 0; i < A; i++) {
        int root = mountId[chosen[i]];
        // order candidates
        vector<pair<double,int>> keyed;
        for (int t : cand[i]) {
            const auto& pts = tasks[t].pts;
            double cost = 0;
            int prev = root;
            for (int p : pts) {
                cost += abs(p % W - prev % W) + abs(p / W - prev / W);
                prev = p;
            }
            cost += 1;
            keyed.push_back({(double)tasks[t].S / cost, t});
        }
        stable_sort(keyed.begin(), keyed.end(), [](const pair<double,int>& a, const pair<double,int>& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });

        vector<int> body = {root};
        onBody[root] = 1;
        string moves;
        vector<int> tlist;
        long long armScore = 0;

        for (auto& kv : keyed) {
            int t = kv.second;
            size_t mark = moves.size();
            vector<pair<int,int>> logv;
            bool ok = true;
            for (int p : tasks[t].pts) {
                if (p == body.back()) continue;
                vector<int> up;
                int c = p;
                while (!onBody[c]) { up.push_back(c); c = par[c]; }
                long long retract = (long long)body.size() - 1 - dist[c];
                if ((long long)moves.size() + retract + (long long)up.size() > L) { ok = false; break; }
                while ((int)body.size() - 1 > dist[c]) {
                    int cell = body.back();
                    body.pop_back();
                    onBody[cell] = 0;
                    logv.push_back({1, cell});
                    moves += dirChar(cell, par[cell]);
                }
                int cur = body.back();
                for (int k = (int)up.size() - 1; k >= 0; k--) {
                    int nd = up[k];
                    moves += dirChar(cur, nd);
                    body.push_back(nd);
                    onBody[nd] = 1;
                    logv.push_back({0, nd});
                    cur = nd;
                }
            }
            if (!ok) {
                for (int k = (int)logv.size() - 1; k >= 0; k--) {
                    if (logv[k].first == 0) {
                        int cell = logv[k].second;
                        body.pop_back();
                        onBody[cell] = 0;
                    } else {
                        int cell = logv[k].second;
                        body.push_back(cell);
                        onBody[cell] = 1;
                    }
                }
                moves.resize(mark);
            } else {
                tlist.push_back(t);
                armScore += tasks[t].S;
            }
        }
        if (!tlist.empty()) {
            ArmPlan ap;
            ap.mx = mountXY[chosen[i]].first;
            ap.my = mountXY[chosen[i]].second;
            ap.tlist = tlist;
            ap.moves = moves;
            res.arms.push_back(ap);
            res.score += armScore;
        }
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> W >> H >> R >> M >> T >> L;
    mountXY.resize(M);
    mountId.resize(M);
    isMount.assign(W * H, 0);
    for (int i = 0; i < M; i++) {
        int x, y;
        cin >> x >> y;
        mountXY[i] = {x, y};
        mountId[i] = y * W + x;
        isMount[mountId[i]] = 1;
    }
    tasks.resize(T);
    for (int t = 0; t < T; t++) {
        int S, P;
        cin >> S >> P;
        tasks[t].S = S;
        tasks[t].pts.resize(P);
        for (int k = 0; k < P; k++) {
            int x, y;
            cin >> x >> y;
            tasks[t].pts[k] = y * W + x;
        }
    }

    // Candidate selections of R mounts
    vector<vector<int>> trials;

    // Trial 1: farthest-point sampling starting near centroid of assembly points
    {
        double cx = 0, cy = 0; long long cnt = 0;
        for (auto& tk : tasks) for (int p : tk.pts) { cx += p % W; cy += p / W; cnt++; }
        if (cnt > 0) { cx /= cnt; cy /= cnt; }
        int start = 0; double bd = 1e18;
        for (int j = 0; j < M; j++) {
            double d = fabs(mountXY[j].first - cx) + fabs(mountXY[j].second - cy);
            if (d < bd) { bd = d; start = j; }
        }
        vector<int> chosen = {start};
        vector<char> used(M, 0);
        used[start] = 1;
        vector<long long> md(M);
        for (int j = 0; j < M; j++)
            md[j] = llabs(mountXY[j].first - mountXY[start].first) + llabs(mountXY[j].second - mountXY[start].second);
        while ((int)chosen.size() < R) {
            int best = -1; long long bv = -1;
            for (int j = 0; j < M; j++) {
                if (used[j]) continue;
                if (md[j] > bv) { bv = md[j]; best = j; }
            }
            if (best < 0) break;
            chosen.push_back(best);
            used[best] = 1;
            for (int j = 0; j < M; j++) {
                long long d = llabs(mountXY[j].first - mountXY[best].first) + llabs(mountXY[j].second - mountXY[best].second);
                md[j] = min(md[j], d);
            }
        }
        trials.push_back(chosen);
    }
    // Trial 2: first R mounts
    {
        vector<int> chosen;
        for (int j = 0; j < R; j++) chosen.push_back(j);
        trials.push_back(chosen);
    }
    // Trials 3,4: deterministic random subsets
    for (int s = 0; s < 2; s++) {
        mt19937 rng(12345 + s);
        vector<int> idx(M);
        iota(idx.begin(), idx.end(), 0);
        shuffle(idx.begin(), idx.end(), rng);
        vector<int> chosen(idx.begin(), idx.begin() + R);
        trials.push_back(chosen);
    }

    Result best; best.score = -1;
    for (auto& ch : trials) {
        Result r = solveWith(ch);
        if (r.score > best.score) best = r;
    }

    string out;
    if (best.arms.empty()) {
        // No feasible task; output a syntactically valid submission.
        out += "1\n";
        out += to_string(mountXY[0].first) + " " + to_string(mountXY[0].second) + " 1 1\n";
        out += "0\n";
        out += "W\n";
    } else {
        out += to_string(best.arms.size()) + "\n";
        for (auto& ap : best.arms) {
            int K = ap.moves.size();
            out += to_string(ap.mx) + " " + to_string(ap.my) + " " +
                   to_string(ap.tlist.size()) + " " + to_string(K == 0 ? 1 : K) + "\n";
            for (size_t k = 0; k < ap.tlist.size(); k++) {
                if (k) out += " ";
                out += to_string(ap.tlist[k]);
            }
            out += "\n";
            if (K == 0) {
                out += "W\n";
            } else {
                for (int k = 0; k < K; k++) {
                    if (k) out += " ";
                    out += ap.moves[k];
                }
                out += "\n";
            }
        }
    }
    cout << out;
    return 0;
}
