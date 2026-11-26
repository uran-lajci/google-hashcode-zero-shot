#include <bits/stdc++.h>
using namespace std;

struct Proj {
    bool res;
    int hp, wp, val, a; // a = column of first occupied cell in row 0
    vector<uint64_t> pm;                 // per plan row bitmask (bit j = column j occupied)
    vector<pair<int,int>> cells;         // occupied cells (dr, dc)
};

struct Placement { int b, r, c; };

int H, W, D, B;
int NW;
vector<uint64_t> occ;
long long ops = 0;
const long long LIMIT = 400000000LL;

inline uint64_t getWin(const uint64_t* row, int pos) {
    int w = pos >> 6, s = pos & 63;
    uint64_t v = row[w] >> s;
    if (s) v |= row[w + 1] << (64 - s);
    return v;
}

bool fits(const Proj& p, int r, int tl) {
    ops++;
    if (tl < 0 || tl + p.wp > W || r + p.hp > H) return false;
    for (int i = 0; i < p.hp; i++) {
        const uint64_t* row = &occ[(size_t)(r + i) * NW];
        ops++;
        if (getWin(row, tl) & p.pm[i]) return false;
    }
    return true;
}

void place(const Proj& p, int r, int tl) {
    for (auto& cell : p.cells) {
        int rr = r + cell.first, cc = tl + cell.second;
        occ[(size_t)rr * NW + (cc >> 6)] |= 1ULL << (cc & 63);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> D >> B;
    vector<Proj> projs(B);
    for (int i = 0; i < B; i++) {
        char tp;
        int hp, wp, val;
        cin >> tp >> hp >> wp >> val;
        vector<string> plan(hp);
        for (int r = 0; r < hp; r++) cin >> plan[r];
        Proj& p = projs[i];
        p.res = (tp == 'R');
        p.hp = hp;
        p.wp = wp;
        p.val = val;
        p.a = 0;
        for (int j = 0; j < wp; j++) if (plan[0][j] == '#') { p.a = j; break; }
        p.pm.assign(hp, 0);
        for (int r = 0; r < hp; r++)
            for (int j = 0; j < wp; j++)
                if (plan[r][j] == '#') {
                    p.pm[r] |= 1ULL << j;
                    p.cells.push_back({r, j});
                }
    }

    NW = W / 64 + 2;
    occ.assign((size_t)H * NW, 0ULL);

    vector<int> resOrder, utilOrder;
    set<int> types;
    for (int i = 0; i < B; i++) {
        if (projs[i].res) resOrder.push_back(i);
        else {
            utilOrder.push_back(i);
            types.insert(projs[i].val);
        }
    }
    // residential: best capacity per occupied cell first
    stable_sort(resOrder.begin(), resOrder.end(), [&](int x, int y) {
        double rx = (double)projs[x].val / projs[x].cells.size();
        double ry = (double)projs[y].val / projs[y].cells.size();
        if (rx != ry) return rx > ry;
        return x < y;
    });

    double T = (double)types.size();
    double rho = 2.5 * T / (2.0 * D * D);
    rho = min(0.5, max(0.05, rho));

    vector<Placement> result;
    long long U = 0; // utility occupied cells
    int nU = utilOrder.size();
    int ptr = 0;
    long long HW = (long long)H * W;

    auto tryRes = [&](int r, int c) -> bool {
        for (int idx : resOrder) {
            if (ops > LIMIT) return false;
            const Proj& p = projs[idx];
            int tl = c - p.a;
            if (fits(p, r, tl)) {
                place(p, r, tl);
                result.push_back({idx, r, tl});
                return true;
            }
        }
        return false;
    };

    auto tryUtil = [&](int r, int c) -> bool {
        if (nU == 0) return false;
        for (int k = 0; k < nU; k++) {
            if (ops > LIMIT) return false;
            int idx = utilOrder[(ptr + k) % nU];
            const Proj& p = projs[idx];
            int tl = c - p.a;
            if (fits(p, r, tl)) {
                place(p, r, tl);
                result.push_back({idx, r, tl});
                U += p.cells.size();
                ptr = (ptr + k + 1) % nU;
                return true;
            }
        }
        return false;
    };

    for (int pass = 0; pass < 5 && ops <= LIMIT; pass++) {
        size_t before = result.size();
        for (int r = 0; r < H && ops <= LIMIT; r++) {
            for (int c = 0; c < W && ops <= LIMIT; c++) {
                long long scanned = (pass == 0) ? (long long)r * W + c + 1 : HW;
                bool preferUtil = (double)U < rho * (double)scanned;
                if (preferUtil) {
                    if (!tryUtil(r, c)) tryRes(r, c);
                } else {
                    if (!tryRes(r, c)) tryUtil(r, c);
                }
            }
        }
        if (result.size() == before) break;
    }

    // Output
    string out;
    out += to_string(result.size());
    out += '\n';
    for (const auto& pl : result) {
        out += to_string(pl.b);
        out += ' ';
        out += to_string(pl.r);
        out += ' ';
        out += to_string(pl.c);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
