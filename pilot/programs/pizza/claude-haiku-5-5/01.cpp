#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, L, H;
    cin >> R >> C >> L >> H;
    vector<string> g(R);
    for (int i = 0; i < R; i++) cin >> g[i];

    // 2D prefix sums of mushrooms
    vector<int> P((size_t)(R + 1) * (C + 1), 0);
    auto idxP = [&](int r, int c) { return (size_t)r * (C + 1) + c; };
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            P[idxP(r + 1, c + 1)] = P[idxP(r, c + 1)] + P[idxP(r + 1, c)] - P[idxP(r, c)] +
                                    (g[r][c] == 'M' ? 1 : 0);
        }
    }
    auto mcount = [&](int r, int c, int h, int w) {
        return P[idxP(r + h, c + w)] - P[idxP(r, c + w)] - P[idxP(r + h, c)] + P[idxP(r, c)];
    };

    // run[j*R + i] = number of consecutive free cells starting at (i,j) going right
    vector<int> run((size_t)C * R);
    for (int j = 0; j < C; j++)
        for (int i = 0; i < R; i++)
            run[(size_t)j * R + i] = C - j;

    struct Slice { int r1, c1, r2, c2; };
    vector<Slice> slices;

    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (run[(size_t)c * R + r] == 0) continue;  // used cell

            int bestA = 0, bestH = 0, bestW = 0;
            int minRun = INT_MAX;
            int maxH = min(H, R - r);
            for (int h = 1; h <= maxH; h++) {
                int i = r + h - 1;
                minRun = min(minRun, run[(size_t)c * R + i]);
                if (minRun == 0) break;
                int wmax = min(minRun, min(H / h, C - c));
                if (wmax <= 0) continue;
                int area = h * wmax;
                if (area <= bestA) continue;
                int m = mcount(r, c, h, wmax);
                int t = area - m;
                if (m >= L && t >= L) {
                    bestA = area;
                    bestH = h;
                    bestW = wmax;
                    if (bestA == H) break;
                }
            }

            if (bestA == 0) continue;

            // place slice
            slices.push_back({r, c, r + bestH - 1, c + bestW - 1});
            for (int i = r; i < r + bestH; i++) {
                for (int j = c; j < c + bestW; j++) {
                    run[(size_t)j * R + i] = 0;
                }
                // update cells to the left in this row
                int k = c - 1;
                while (k >= 0 && run[(size_t)k * R + i] > c - k) {
                    run[(size_t)k * R + i] = c - k;
                    k--;
                }
            }
        }
    }

    string out;
    out.reserve(slices.size() * 24 + 16);
    out += to_string(slices.size());
    out += '\n';
    for (const auto& s : slices) {
        out += to_string(s.r1); out += ' ';
        out += to_string(s.c1); out += ' ';
        out += to_string(s.r2); out += ' ';
        out += to_string(s.c2); out += '\n';
    }
    cout << out;
    return 0;
}
