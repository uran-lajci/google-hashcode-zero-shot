#include <bits/stdc++.h>
using namespace std;

struct Arm {
    int x, y;
    vector<int> tasks;
    int start;
    vector<char> mv;
};

int W, H, R, M, T, L;
int N;

char dirChar(int a, int b) {
    // move gripper from cell a to adjacent cell b
    if (b == a + W) return 'U';
    if (b == a - W) return 'D';
    if (b == a + 1) return 'R';
    return 'L';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> W >> H >> R >> M >> T >> L;
    N = W * H;

    vector<int> mountCell(M);
    vector<char> isMount(N, 0);
    vector<int> mx(M), my(M);
    for (int i = 0; i < M; ++i) {
        cin >> mx[i] >> my[i];
        mountCell[i] = my[i] * W + mx[i];
        isMount[mountCell[i]] = 1;
    }

    vector<long long> S(T);
    vector<vector<int>> pts(T);
    for (int i = 0; i < T; ++i) {
        int P;
        cin >> S[i] >> P;
        pts[i].resize(P);
        for (int j = 0; j < P; ++j) {
            int x, y;
            cin >> x >> y;
            pts[i][j] = y * W + x;
        }
    }

    vector<char> usedTask(T, 0);
    vector<Arm> arms;
    int t = 0;

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    for (int mi = 0; mi < M && (int)arms.size() < R && t < L; ++mi) {
        int root = mountCell[mi];

        // BFS tree from root over non-mount cells
        vector<int> depth(N, -1), par(N, -1);
        vector<int> q;
        q.reserve(N);
        depth[root] = 0;
        q.push_back(root);
        for (size_t h = 0; h < q.size(); ++h) {
            int c = q[h];
            int x = c % W, y = c / W;
            for (int d = 0; d < 4; ++d) {
                int nx = x + dx[d], ny = y + dy[d];
                if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                int nc = ny * W + nx;
                if (depth[nc] != -1 || isMount[nc]) continue;
                depth[nc] = depth[c] + 1;
                par[nc] = c;
                q.push_back(nc);
            }
        }

        // greedy task selection for this arm
        int g = root;
        int tl = t;
        vector<char> mv;
        vector<int> chosen;

        while (true) {
            int best = -1;
            long long bestS = 0, bestC = 1;
            long long bestCost = 0;
            for (int i = 0; i < T; ++i) {
                if (usedTask[i]) continue;
                long long cost = 0;
                int prev = g;
                bool ok = true;
                for (int p : pts[i]) {
                    if (depth[p] < 0) { ok = false; break; }
                    if (p != prev) cost += depth[prev] + depth[p];
                    prev = p;
                    if (tl + cost > L) { ok = false; break; }
                }
                if (!ok) continue;
                if (tl + cost + depth[prev] > L) continue;
                long long denom = cost + 1;
                // maximize S[i]/denom
                if (best == -1 || S[i] * bestC > bestS * denom) {
                    best = i;
                    bestS = S[i];
                    bestC = denom;
                    bestCost = cost;
                }
            }
            if (best == -1) break;

            // execute chosen task
            for (int p : pts[best]) {
                if (p == g) continue;
                // retract to root
                while (g != root) {
                    int pg = par[g];
                    mv.push_back(dirChar(g, pg));
                    g = pg;
                }
                // descend root -> p
                vector<int> path;
                int x = p;
                while (x != root) {
                    path.push_back(x);
                    x = par[x];
                }
                reverse(path.begin(), path.end());
                int a = root;
                for (int b : path) {
                    mv.push_back(dirChar(a, b));
                    a = b;
                }
                g = p;
            }
            usedTask[best] = 1;
            chosen.push_back(best);
            tl += (int)bestCost;
        }

        if (chosen.empty()) {
            // undo nothing: tasks were not marked used
            continue;
        }

        // final retraction to mount
        while (g != root) {
            int pg = par[g];
            mv.push_back(dirChar(g, pg));
            g = pg;
        }

        Arm arm;
        arm.x = mx[mi];
        arm.y = my[mi];
        arm.tasks = chosen;
        arm.start = t;
        arm.mv = mv;
        arms.push_back(arm);
        t = t + (int)mv.size();
    }

    if (arms.empty()) {
        // Fallback: valid format, no tasks completed.
        cout << 1 << "\n";
        cout << mx[0] << " " << my[0] << " 1 1\n";
        cout << 0 << "\n";
        cout << "W" << "\n";
        return 0;
    }

    string out;
    out += to_string(arms.size()) + "\n";
    for (const Arm& a : arms) {
        int K = a.start + (int)a.mv.size();
        out += to_string(a.x) + " " + to_string(a.y) + " " +
               to_string(a.tasks.size()) + " " + to_string(K) + "\n";
        for (size_t i = 0; i < a.tasks.size(); ++i) {
            if (i) out += " ";
            out += to_string(a.tasks[i]);
        }
        out += "\n";
        bool first = true;
        for (int i = 0; i < a.start; ++i) {
            if (!first) out += " ";
            out += "W";
            first = false;
        }
        for (char c : a.mv) {
            if (!first) out += " ";
            out += c;
            first = false;
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
