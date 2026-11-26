#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64;

struct Proj {
    bool res;
    int hp, wp, val, ti, area;
    int minr, maxr, minc, maxc;
    vector<pair<int,int>> cells;
};

int H, W, D, B, T, WORDS;
vector<Proj> P;
vector<u64> reach;      // per cell, WORDS words
vector<unsigned char> occ;
vector<int> rid;        // residential building id per cell, -1 if none
vector<int> resCap;
vector<u64> covered;    // per residential building, WORDS words
vector<int> resStamp;
int stampCounter = 0;
long long score = 0;
vector<array<int,3>> placed;
long long ops = 0;
const long long BUDGET = 1500000000LL;
vector<int> dbuf;

bool fits(const Proj& pr, int r0, int c0) {
    ops += pr.cells.size();
    for (auto& e : pr.cells) {
        if (occ[(r0 + e.first) * W + (c0 + e.second)]) return false;
    }
    return true;
}

bool tryResidential(int pi, int r0, int c0) {
    const Proj& pr = P[pi];
    vector<u64> m(WORDS, 0);
    for (auto& e : pr.cells) {
        int idx = (r0 + e.first) * W + (c0 + e.second);
        for (int k = 0; k < WORDS; k++) m[k] |= reach[(size_t)idx * WORDS + k];
    }
    ops += (long long)pr.cells.size() * WORDS;
    long long cnt = 0;
    for (int k = 0; k < WORDS; k++) cnt += __builtin_popcountll(m[k]);
    if (cnt == 0) return false;
    int b = resCap.size();
    resCap.push_back(pr.val);
    for (int k = 0; k < WORDS; k++) covered.push_back(m[k]);
    resStamp.push_back(0);
    for (auto& e : pr.cells) {
        int idx = (r0 + e.first) * W + (c0 + e.second);
        occ[idx] = 1;
        rid[idx] = b;
    }
    score += (long long)pr.val * cnt;
    placed.push_back({pi, r0, c0});
    return true;
}

bool tryUtility(int pi, int r0, int c0) {
    const Proj& pr = P[pi];
    int R0 = max(0, r0 + pr.minr - D), R1 = min(H - 1, r0 + pr.maxr + D);
    int C0 = max(0, c0 + pr.minc - D), C1 = min(W - 1, c0 + pr.maxc + D);
    int bh = R1 - R0 + 1, bw = C1 - C0 + 1;
    const int INF = 100000000;
    dbuf.assign((size_t)bh * bw, INF);
    auto& d = dbuf;
    for (auto& e : pr.cells) {
        d[(r0 + e.first - R0) * bw + (c0 + e.second - C0)] = 0;
    }
    for (int i = 0; i < bh; i++)
        for (int j = 0; j < bw; j++) {
            int v = d[i * bw + j];
            if (i > 0) v = min(v, d[(i - 1) * bw + j] + 1);
            if (j > 0) v = min(v, d[i * bw + j - 1] + 1);
            d[i * bw + j] = v;
        }
    for (int i = bh - 1; i >= 0; i--)
        for (int j = bw - 1; j >= 0; j--) {
            int v = d[i * bw + j];
            if (i < bh - 1) v = min(v, d[(i + 1) * bw + j] + 1);
            if (j < bw - 1) v = min(v, d[i * bw + j + 1] + 1);
            d[i * bw + j] = v;
        }
    ops += (long long)bh * bw * 3;

    int ti = pr.ti;
    int wd = ti >> 6;
    u64 bit = 1ULL << (ti & 63);
    stampCounter++;
    long long gain = 0, freeCnt = 0, ncov = 0;
    for (int i = 0; i < bh; i++)
        for (int j = 0; j < bw; j++) {
            if (d[i * bw + j] > D) continue;
            ncov++;
            int idx = (R0 + i) * W + (C0 + j);
            if (reach[(size_t)idx * WORDS + wd] & bit) continue;
            if (!occ[idx]) freeCnt++;
            else if (rid[idx] >= 0) {
                int b = rid[idx];
                if (!(covered[(size_t)b * WORDS + wd] & bit) && resStamp[b] != stampCounter) {
                    resStamp[b] = stampCounter;
                    gain += resCap[b];
                }
            }
        }
    ops += (long long)bh * bw;
    bool accept = gain > 0 || 3 * freeCnt >= ncov;
    if (!accept) return false;

    for (int i = 0; i < bh; i++)
        for (int j = 0; j < bw; j++) {
            if (d[i * bw + j] > D) continue;
            int idx = (R0 + i) * W + (C0 + j);
            reach[(size_t)idx * WORDS + wd] |= bit;
            if (rid[idx] >= 0) {
                int b = rid[idx];
                if (!(covered[(size_t)b * WORDS + wd] & bit)) {
                    covered[(size_t)b * WORDS + wd] |= bit;
                    score += resCap[b];
                }
            }
        }
    ops += (long long)bh * bw;
    for (auto& e : pr.cells) occ[(r0 + e.first) * W + (c0 + e.second)] = 1;
    placed.push_back({pi, r0, c0});
    return true;
}

bool scanProject(int pi) {
    const Proj& pr = P[pi];
    bool any = false;
    for (int r0 = 0; r0 + pr.hp <= H; r0++) {
        for (int c0 = 0; c0 + pr.wp <= W; c0++) {
            if (ops > BUDGET) return any;
            if (!fits(pr, r0, c0)) continue;
            bool ok = pr.res ? tryResidential(pi, r0, c0) : tryUtility(pi, r0, c0);
            if (ok) any = true;
        }
    }
    return any;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> H >> W >> D >> B;
    P.resize(B);
    map<int, int> tmap;
    for (int i = 0; i < B; i++) {
        char t;
        int hp, wp, v;
        cin >> t >> hp >> wp >> v;
        Proj& pr = P[i];
        pr.res = (t == 'R');
        pr.hp = hp;
        pr.wp = wp;
        pr.val = v;
        pr.area = hp * wp;
        vector<string> g(hp);
        for (int r = 0; r < hp; r++) cin >> g[r];
        pr.minr = INT_MAX; pr.minc = INT_MAX; pr.maxr = INT_MIN; pr.maxc = INT_MIN;
        for (int r = 0; r < hp; r++)
            for (int c = 0; c < wp; c++)
                if (g[r][c] == '#') {
                    pr.cells.push_back({r, c});
                    pr.minr = min(pr.minr, r); pr.maxr = max(pr.maxr, r);
                    pr.minc = min(pr.minc, c); pr.maxc = max(pr.maxc, c);
                }
        if (!pr.res) {
            auto it = tmap.find(v);
            if (it == tmap.end()) {
                int id = tmap.size();
                tmap[v] = id;
                pr.ti = id;
            } else {
                pr.ti = it->second;
            }
        } else {
            pr.ti = -1;
        }
    }
    T = max(1, (int)tmap.size());
    WORDS = (T + 63) / 64;
    reach.assign((size_t)H * W * WORDS, 0);
    occ.assign((size_t)H * W, 0);
    rid.assign((size_t)H * W, -1);

    vector<int> ulist, rlist;
    for (int i = 0; i < B; i++) {
        if (P[i].cells.empty()) continue;
        (P[i].res ? rlist : ulist).push_back(i);
    }
    sort(ulist.begin(), ulist.end(), [&](int a, int b) {
        if (P[a].area != P[b].area) return P[a].area < P[b].area;
        return a < b;
    });
    sort(rlist.begin(), rlist.end(), [&](int a, int b) {
        long long lhs = (long long)P[a].val * P[b].area;
        long long rhs = (long long)P[b].val * P[a].area;
        if (lhs != rhs) return lhs > rhs;
        return a < b;
    });

    bool changed = true;
    while (changed && ops <= BUDGET) {
        changed = false;
        for (int p : ulist) {
            if (ops > BUDGET) break;
            if (scanProject(p)) changed = true;
        }
        for (int p : rlist) {
            if (ops > BUDGET) break;
            if (scanProject(p)) changed = true;
        }
    }

    // Output: placed list uses project index into P; map to original index.
    string out;
    out.reserve(placed.size() * 16 + 16);
    out += to_string(placed.size());
    out += '\n';
    for (auto& a : placed) {
        out += to_string(a[0]);
        out += ' ';
        out += to_string(a[1]);
        out += ' ';
        out += to_string(a[2]);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
