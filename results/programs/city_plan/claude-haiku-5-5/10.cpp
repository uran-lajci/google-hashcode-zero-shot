#include <bits/stdc++.h>
using namespace std;

struct Proj {
    int id;          // index in input
    bool res;        // residential?
    int val;         // capacity (res) or type (utility)
    int h, w;
    vector<pair<int,int>> cells;  // occupied cells relative to top-left
};

struct Placed {
    int proj;
    int r0, c0;
    bool res;
    int type;        // utility type
    int cap;         // residential capacity
    int covIdx;      // index into covs for residences
};

int H, W, D, B;
vector<Proj> P;
vector<int> owner;                  // -1 free, else index into placed
vector<Placed> placed;
vector<bitset<1001>> covs;          // per residence: set of utility types within D
bool typePlaced[1001];
vector<int> stampRes;               // per placed index
int stampType[1001];
int stampCnt = 0;

vector<int> dist_;
int curWH, curWW;
const int INF = 1000000000;

long long work = 0;
const long long WORK_LIMIT = 2000000000LL;

// Two-pass exact L1 distance transform over the window of size (h+2D)x(w+2D),
// sources = occupied cells of the project placed with its top-left at window (D,D).
void computeDist(const Proj& p) {
    curWH = p.h + 2 * D;
    curWW = p.w + 2 * D;
    dist_.assign((size_t)curWH * curWW, INF);
    for (auto [i, j] : p.cells) dist_[(i + D) * curWW + (j + D)] = 0;
    for (int i = 0; i < curWH; i++)
        for (int j = 0; j < curWW; j++) {
            int &v = dist_[i * curWW + j];
            if (i > 0) v = min(v, dist_[(i - 1) * curWW + j] + 1);
            if (j > 0) v = min(v, dist_[i * curWW + j - 1] + 1);
        }
    for (int i = curWH - 1; i >= 0; i--)
        for (int j = curWW - 1; j >= 0; j--) {
            int &v = dist_[i * curWW + j];
            if (i < curWH - 1) v = min(v, dist_[(i + 1) * curWW + j] + 1);
            if (j < curWW - 1) v = min(v, dist_[i * curWW + j + 1] + 1);
        }
    work += 3LL * curWH * curWW;
}

// Calls f(o) for every placed building o having a cell within distance D
// (distance measured from the current candidate, whose dist field is in dist_).
template <class F>
void scanWindow(int r0, int c0, F f) {
    work += (long long)curWH * curWW;
    for (int i = 0; i < curWH; i++) {
        int gr = r0 - D + i;
        if (gr < 0 || gr >= H) continue;
        for (int j = 0; j < curWW; j++) {
            int gc = c0 - D + j;
            if (gc < 0 || gc >= W) continue;
            if (dist_[i * curWW + j] > D) continue;
            int o = owner[(size_t)gr * W + gc];
            if (o >= 0) f(o);
        }
    }
}

long long gainRes(int r0, int c0, int cap) {
    stampCnt++;
    int cnt = 0;
    scanWindow(r0, c0, [&](int o) {
        if (!placed[o].res) {
            int t = placed[o].type;
            if (stampType[t] != stampCnt) {
                stampType[t] = stampCnt;
                cnt++;
            }
        }
    });
    return (long long)cnt * cap;
}

long long gainUtil(int r0, int c0, int t) {
    stampCnt++;
    long long g = 0;
    scanWindow(r0, c0, [&](int o) {
        if (placed[o].res && stampRes[o] != stampCnt) {
            stampRes[o] = stampCnt;
            if (!covs[placed[o].covIdx][t]) g += placed[o].cap;
        }
    });
    return g;
}

bool fits(const Proj& p, int r0, int c0) {
    if (r0 + p.h > H || c0 + p.w > W) return false;
    for (auto [i, j] : p.cells)
        if (owner[(size_t)(r0 + i) * W + (c0 + j)] != -1) return false;
    return true;
}

void applyPlace(int pi, int r0, int c0) {
    const Proj& p = P[pi];
    computeDist(p);
    int id = placed.size();
    Placed pl;
    pl.proj = pi;
    pl.r0 = r0;
    pl.c0 = c0;
    pl.res = p.res;
    pl.type = p.res ? -1 : p.val;
    pl.cap = p.res ? p.val : 0;
    pl.covIdx = -1;
    if (p.res) {
        pl.covIdx = covs.size();
        covs.emplace_back();
        int ci = pl.covIdx;
        scanWindow(r0, c0, [&](int o) {
            if (!placed[o].res) covs[ci].set(placed[o].type);
        });
    } else {
        int t = p.val;
        scanWindow(r0, c0, [&](int o) {
            if (placed[o].res) covs[placed[o].covIdx].set(t);
        });
        typePlaced[t] = true;
    }
    for (auto [i, j] : p.cells) owner[(size_t)(r0 + i) * W + (c0 + j)] = id;
    placed.push_back(pl);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> D >> B;
    P.resize(B);
    for (int i = 0; i < B; i++) {
        char tp;
        int hp, wp, val;
        cin >> tp >> hp >> wp >> val;
        Proj p;
        p.id = i;
        p.res = (tp == 'R');
        p.val = val;
        p.h = hp;
        p.w = wp;
        for (int r = 0; r < hp; r++) {
            string s;
            cin >> s;
            for (int c = 0; c < wp; c++)
                if (s[c] == '#') p.cells.push_back({r, c});
        }
        P[i] = p;
    }

    owner.assign((size_t)H * W, -1);
    stampRes.assign((size_t)H * W + 5, 0);

    vector<int> resList, utilList;
    for (int i = 0; i < B; i++) {
        if (P[i].cells.empty()) continue;
        if (P[i].res) resList.push_back(i);
        else utilList.push_back(i);
    }
    // residential: best capacity per occupied cell first
    sort(resList.begin(), resList.end(), [&](int a, int b) {
        long long lhs = (long long)P[a].val * P[b].cells.size();
        long long rhs = (long long)P[b].val * P[a].cells.size();
        if (lhs != rhs) return lhs > rhs;
        return a < b;
    });
    // utility: smallest footprint first
    sort(utilList.begin(), utilList.end(), [&](int a, int b) {
        if (P[a].cells.size() != P[b].cells.size()) return P[a].cells.size() < P[b].cells.size();
        return a < b;
    });

    int R = resList.size(), U = utilList.size();
    bool stop = false;
    for (int pass = 0; !stop; pass++) {
        bool changed = false;
        for (int r0 = 0; r0 < H && !stop; r0++) {
            for (int c0 = 0; c0 < W; c0++) {
                if (work > WORK_LIMIT) { stop = true; break; }
                double bestS = 0;
                int bestP = -1;
                auto consider = [&](int pi) {
                    const Proj& p = P[pi];
                    if (!fits(p, r0, c0)) return;
                    computeDist(p);
                    long long g;
                    if (p.res) {
                        g = gainRes(r0, c0, p.val);
                    } else {
                        g = gainUtil(r0, c0, p.val);
                        if (!typePlaced[p.val]) g += 1;  // bootstrap: place each type once
                    }
                    if (g <= 0) return;
                    double s = (double)g / (double)p.cells.size();
                    if (s > bestS) {
                        bestS = s;
                        bestP = pi;
                    }
                };
                for (int k = 0; k < min(R, 3); k++) consider(resList[k]);
                if (U > 0) {
                    int off = (int)(((long long)r0 * W + c0 + (long long)pass * 131) % U);
                    for (int k = 0; k < min(U, 2); k++) consider(utilList[(off + k) % U]);
                }
                if (bestP >= 0) {
                    applyPlace(bestP, r0, c0);
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }

    // Output
    string out;
    out += to_string(placed.size()) + "\n";
    for (const auto& pl : placed) {
        out += to_string(P[pl.proj].id) + " " + to_string(pl.r0) + " " + to_string(pl.c0) + "\n";
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
