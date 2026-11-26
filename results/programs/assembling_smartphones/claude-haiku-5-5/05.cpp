#include <bits/stdc++.h>
using namespace std;

int W, H, R, M, T, L, N;
vector<int> owner;          // -1 free, -2 mount point (unused), >=0 arm index
vector<int> gsc, stampv, par;
int curStamp = 0;
const int CAP = 2000;

struct P { int pos, task; bool last; };

struct Arm {
    int mount;
    vector<int> body;       // body[0] = mount, back() = gripper head
    deque<int> path;
    vector<P> seq;
    vector<int> assigned;
    int idx = 0, waits = 0, cool = 0;
    vector<int> done;
    vector<char> mv;
    int head() const { return body.back(); }
    void advance() {
        while (idx < (int)seq.size() && head() == seq[idx].pos) {
            if (seq[idx].last) done.push_back(seq[idx].task);
            idx++;
        }
    }
};

int manh(int a, int b) {
    return abs(a % W - b % W) + abs(a / W - b / W);
}

char dirChar(int from, int to) {
    if (to % W != from % W) return (to % W > from % W) ? 'R' : 'L';
    return (to > from) ? 'U' : 'D';
}

bool astar(int start, int goal, deque<int>& out) {
    curStamp++;
    auto h = [&](int c) { return abs(c % W - goal % W) + abs(c / W - goal / W); };
    priority_queue<array<int, 3>, vector<array<int, 3>>, greater<array<int, 3>>> pq;
    stampv[start] = curStamp; gsc[start] = 0; par[start] = -1;
    pq.push({h(start), 0, start});
    int expansions = 0;
    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop();
        int g = cur[1], c = cur[2];
        if (g > gsc[c]) continue;
        if (++expansions > CAP) return false;
        int x = c % W, y = c / W;
        int nbs[4]; int cnt = 0;
        if (x + 1 < W) nbs[cnt++] = c + 1;
        if (x - 1 >= 0) nbs[cnt++] = c - 1;
        if (y + 1 < H) nbs[cnt++] = c + W;
        if (y - 1 >= 0) nbs[cnt++] = c - W;
        for (int k = 0; k < cnt; k++) {
            int nb = nbs[k];
            if (owner[nb] != -1) continue;
            if (nb == goal) {
                par[nb] = c;
                vector<int> v;
                for (int z = goal; z != start; z = par[z]) v.push_back(z);
                reverse(v.begin(), v.end());
                out.assign(v.begin(), v.end());
                return true;
            }
            if (stampv[nb] == curStamp && gsc[nb] <= g + 1) continue;
            stampv[nb] = curStamp;
            gsc[nb] = g + 1;
            par[nb] = c;
            pq.push({g + 1 + h(nb), g + 1, nb});
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> W >> H >> R >> M >> T >> L;
    N = W * H;
    vector<int> mountCells(M);
    for (int i = 0; i < M; i++) {
        int x, y; cin >> x >> y;
        mountCells[i] = y * W + x;
    }
    vector<long long> S(T);
    vector<vector<int>> pts(T);
    for (int t = 0; t < T; t++) {
        int P; cin >> S[t] >> P;
        pts[t].resize(P);
        for (int k = 0; k < P; k++) {
            int x, y; cin >> x >> y;
            pts[t][k] = y * W + x;
        }
    }

    // multi-source BFS distance from assembly points
    vector<int> dist(N, -1);
    {
        queue<int> q;
        for (int t = 0; t < T; t++)
            for (int c : pts[t])
                if (dist[c] == -1) { dist[c] = 0; q.push(c); }
        while (!q.empty()) {
            int c = q.front(); q.pop();
            int x = c % W, y = c / W;
            int nbs[4]; int cnt = 0;
            if (x + 1 < W) nbs[cnt++] = c + 1;
            if (x - 1 >= 0) nbs[cnt++] = c - 1;
            if (y + 1 < H) nbs[cnt++] = c + W;
            if (y - 1 >= 0) nbs[cnt++] = c - W;
            for (int k = 0; k < cnt; k++)
                if (dist[nbs[k]] == -1) { dist[nbs[k]] = dist[c] + 1; q.push(nbs[k]); }
        }
    }

    // choose R mount points nearest to assembly points
    vector<int> mi(M);
    iota(mi.begin(), mi.end(), 0);
    stable_sort(mi.begin(), mi.end(), [&](int a, int b) {
        int da = dist[mountCells[a]], db = dist[mountCells[b]];
        if (da < 0) da = INT_MAX;
        if (db < 0) db = INT_MAX;
        return da < db;
    });

    owner.assign(N, -1);
    for (int i = 0; i < M; i++) owner[mountCells[i]] = -2;
    vector<Arm> arms(R);
    for (int i = 0; i < R; i++) {
        arms[i].mount = mountCells[mi[i]];
        arms[i].body.push_back(arms[i].mount);
        owner[arms[i].mount] = i;
        arms[i].mv.assign(L, 'W');
    }

    // task assignment by greedy estimated time
    vector<int> internal(T, 0);
    for (int t = 0; t < T; t++)
        for (size_t k = 1; k < pts[t].size(); k++)
            internal[t] += manh(pts[t][k - 1], pts[t][k]);

    vector<int> ord(T);
    iota(ord.begin(), ord.end(), 0);
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) { return S[a] > S[b]; });

    vector<long long> armTime(R, 0);
    vector<int> lastPos(R);
    for (int i = 0; i < R; i++) lastPos[i] = arms[i].mount;
    for (int t : ord) {
        int best = -1; long long bestTime = LLONG_MAX;
        for (int a = 0; a < R; a++) {
            long long add = manh(lastPos[a], pts[t][0]) + internal[t];
            long long nt = armTime[a] + add;
            if (nt <= L && nt < bestTime) { bestTime = nt; best = a; }
        }
        if (best >= 0) {
            armTime[best] = bestTime;
            lastPos[best] = pts[t].back();
            arms[best].assigned.push_back(t);
        }
    }
    for (int a = 0; a < R; a++) {
        for (int t : arms[a].assigned) {
            int P = pts[t].size();
            for (int k = 0; k < P; k++)
                arms[a].seq.push_back({pts[t][k], t, k == P - 1});
        }
    }

    gsc.assign(N, 0); stampv.assign(N, 0); par.assign(N, -1);

    for (int a = 0; a < R; a++) arms[a].advance();

    vector<int> frees;
    for (int t = 0; t < L; t++) {
        frees.clear();
        for (int a = 0; a < R; a++) {
            Arm& A = arms[a];
            char mv = 'W';
            if (A.idx < (int)A.seq.size()) {
                int target = A.seq[A.idx].pos;
                int hd = A.head();
                bool decided = false;
                if (A.path.empty()) {
                    if (A.cool > 0) {
                        A.cool--;
                    } else {
                        deque<int> np;
                        if (astar(hd, target, np)) {
                            A.path = np;
                        } else if (owner[target] >= 0 && owner[target] != a) {
                            A.cool = 3;             // blocked by another arm: wait
                        } else if (A.body.size() > 1) {
                            int prev = A.body[A.body.size() - 2];
                            mv = dirChar(hd, prev);
                            frees.push_back(hd);
                            A.body.pop_back();
                            decided = true;
                        } else {
                            A.cool = 2;
                        }
                    }
                }
                if (!decided && !A.path.empty()) {
                    int nxt = A.path.front();
                    if (owner[nxt] == -1) {
                        owner[nxt] = a;
                        A.body.push_back(nxt);
                        A.path.pop_front();
                        mv = dirChar(hd, nxt);
                        A.waits = 0;
                    } else {
                        if (++A.waits >= 3) {
                            A.path.clear();
                            A.waits = 0;
                            A.cool = 0;
                        }
                    }
                }
            }
            A.mv[t] = mv;
            A.advance();
        }
        for (int c : frees) owner[c] = -1;
    }

    // build submission: keep arms that completed at least one task
    vector<int> keep;
    for (int a = 0; a < R; a++)
        if (!arms[a].done.empty()) keep.push_back(a);

    string out;
    if (keep.empty()) {
        int m = arms.empty() ? 0 : arms[0].mount;
        out += "1\n";
        out += to_string(m % W) + " " + to_string(m / W) + " 1 " + to_string(L) + "\n";
        out += "0\n";
        for (int i = 0; i < L; i++) { out += "W"; out += (i + 1 < L ? " " : "\n"); }
    } else {
        out += to_string(keep.size()) + "\n";
        for (int a : keep) {
            const Arm& A = arms[a];
            out += to_string(A.mount % W) + " " + to_string(A.mount / W) + " " +
                   to_string(A.done.size()) + " " + to_string(L) + "\n";
            for (size_t i = 0; i < A.done.size(); i++) {
                out += to_string(A.done[i]);
                out += (i + 1 < A.done.size() ? " " : "\n");
            }
            for (int i = 0; i < L; i++) {
                out += A.mv[i];
                out += (i + 1 < L ? " " : "\n");
            }
        }
    }
    cout << out;
    return 0;
}
