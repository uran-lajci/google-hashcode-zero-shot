#include <bits/stdc++.h>
using namespace std;

struct Project {
    bool residential;
    int hp, wp;
    int value;                              // capacity (residential) or type (utility)
    vector<pair<int,int>> cells;            // occupied offsets (dr, dc)
};

int H, W, D, B;
vector<Project> projs;
vector<char> occ;

bool fits(const Project& p, int i, int j) {
    if (i + p.hp > H || j + p.wp > W) return false;
    for (const auto& cd : p.cells) {
        if (occ[(size_t)(i + cd.first) * W + (j + cd.second)]) return false;
    }
    return true;
}

void place(const Project& p, int i, int j) {
    for (const auto& cd : p.cells) {
        occ[(size_t)(i + cd.first) * W + (j + cd.second)] = 1;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> H >> W >> D >> B;
    projs.resize(B);
    for (int b = 0; b < B; ++b) {
        char t;
        cin >> t;
        Project& p = projs[b];
        p.residential = (t == 'R');
        cin >> p.hp >> p.wp >> p.value;
        for (int r = 0; r < p.hp; ++r) {
            string row;
            cin >> row;
            for (int c = 0; c < p.wp; ++c) {
                if (row[c] == '#') p.cells.push_back({r, c});
            }
        }
    }

    // Build residential and utility lists.
    vector<int> R, U;
    for (int b = 0; b < B; ++b) {
        if (projs[b].residential) R.push_back(b);
        else U.push_back(b);
    }
    // Residential: best capacity density first. Utility: smallest first.
    sort(R.begin(), R.end(), [&](int a, int b) {
        double da = (double)projs[a].value / projs[a].cells.size();
        double db = (double)projs[b].value / projs[b].cells.size();
        if (da != db) return da > db;
        return a < b;
    });
    sort(U.begin(), U.end(), [&](int a, int b) {
        if (projs[a].cells.size() != projs[b].cells.size())
            return projs[a].cells.size() < projs[b].cells.size();
        return a < b;
    });

    // Interleaved cyclic sequence: R[k], U[k], R[k+1], U[k+1], ...
    vector<int> seq;
    if (!R.empty() && !U.empty()) {
        size_t m = max(R.size(), U.size());
        for (size_t k = 0; k < m; ++k) {
            seq.push_back(R[k % R.size()]);
            seq.push_back(U[k % U.size()]);
        }
    } else {
        for (int b : R) seq.push_back(b);
        for (int b : U) seq.push_back(b);
    }
    const long long L = (long long)seq.size();

    occ.assign((size_t)H * W, 0);
    vector<array<int,3>> result;   // (project, row, col)

    const int TRIES = 8;
    long long ptr = 0;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            for (int t = 0; t < TRIES && t < L; ++t) {
                long long q = ptr + t;
                int b = seq[q % L];
                if (fits(projs[b], i, j)) {
                    place(projs[b], i, j);
                    result.push_back({b, i, j});
                    ptr = q + 1;
                    break;
                }
            }
        }
    }

    string out;
    out.reserve(result.size() * 16 + 16);
    out += to_string(result.size());
    out += '\n';
    for (const auto& x : result) {
        out += to_string(x[0]);
        out += ' ';
        out += to_string(x[1]);
        out += ' ';
        out += to_string(x[2]);
        out += '\n';
    }
    cout << out;
    return 0;
}
