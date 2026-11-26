#include <bits/stdc++.h>
using namespace std;

struct Proj {
    char t;
    int hp, wp, val;
    vector<pair<int,int>> cells; // occupied offsets (dr, dc)
};

int H, W, D, B;
vector<Proj> projs;
vector<char> occ;
long long usedR = 0, usedU = 0;
vector<array<int,3>> placements; // b, r, c

chrono::steady_clock::time_point startT;
double elapsed() {
    return chrono::duration<double>(chrono::steady_clock::now() - startT).count();
}

inline bool fits(int idx, int r0, int c0) {
    const Proj& p = projs[idx];
    if (r0 < 0 || c0 < 0) return false;
    if (r0 + p.hp > H || c0 + p.wp > W) return false;
    for (const auto& d : p.cells) {
        if (occ[(size_t)(r0 + d.first) * W + (c0 + d.second)]) return false;
    }
    return true;
}

inline void place(int idx, int r0, int c0) {
    const Proj& p = projs[idx];
    for (const auto& d : p.cells) {
        occ[(size_t)(r0 + d.first) * W + (c0 + d.second)] = 1;
    }
    if (p.t == 'R') usedR += p.cells.size();
    else usedU += p.cells.size();
    placements.push_back({idx, r0, c0});
}

bool tryList(const vector<int>& lst, size_t& ptr, int K, int r0, int c0) {
    size_t n = lst.size();
    if (n == 0) return false;
    size_t lim = min((size_t)K, n);
    for (size_t k = 0; k < lim; k++) {
        int idx = lst[(ptr + k) % n];
        if (fits(idx, r0, c0)) {
            place(idx, r0, c0);
            ptr = (ptr + k + 1) % n;
            return true;
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    startT = chrono::steady_clock::now();

    cin >> H >> W >> D >> B;
    projs.resize(B);
    for (int i = 0; i < B; i++) {
        Proj& p = projs[i];
        cin >> p.t >> p.hp >> p.wp >> p.val;
        vector<string> rows(p.hp);
        for (int r = 0; r < p.hp; r++) cin >> rows[r];
        for (int r = 0; r < p.hp; r++)
            for (int c = 0; c < p.wp; c++)
                if (rows[r][c] == '#') p.cells.push_back({r, c});
    }

    occ.assign((size_t)H * W, 0);

    vector<int> resList, utList;
    for (int i = 0; i < B; i++) {
        if (projs[i].t == 'R') resList.push_back(i);
        else utList.push_back(i);
    }
    // residential: higher capacity per cell first
    sort(resList.begin(), resList.end(), [&](int a, int b) {
        double da = (double)projs[a].val / projs[a].cells.size();
        double db = (double)projs[b].val / projs[b].cells.size();
        if (da != db) return da > db;
        return a < b;
    });

    // Pass 1: sweep anchors in row-major order, alternating kinds
    size_t ptrR = 0, ptrU = 0;
    const int K = 20;
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            if (usedU <= usedR) {
                tryList(utList, ptrU, K, r, c);
                tryList(resList, ptrR, K, r, c);
            } else {
                tryList(resList, ptrR, K, r, c);
                tryList(utList, ptrU, K, r, c);
            }
        }
        if (elapsed() > 6.0) break;
    }

    // Pass 2: fill remaining space with any fitting project
    vector<int> all = resList;
    all.insert(all.end(), utList.begin(), utList.end());
    bool stop = false;
    for (int r = 0; r < H && !stop; r++) {
        for (int c = 0; c < W; c++) {
            for (int idx : all) {
                if (fits(idx, r, c)) place(idx, r, c);
            }
        }
        if (elapsed() > 8.5) stop = true;
    }

    string out;
    out.reserve(placements.size() * 16 + 16);
    out += to_string(placements.size());
    out += '\n';
    for (auto& pl : placements) {
        out += to_string(pl[0]);
        out += ' ';
        out += to_string(pl[1]);
        out += ' ';
        out += to_string(pl[2]);
        out += '\n';
    }
    cout << out;
    return 0;
}
