#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, L, H;
    cin >> R >> C >> L >> H;
    vector<string> g(R);
    for (int i = 0; i < R; i++) cin >> g[i];

    // If no slice can satisfy both constraints, output zero slices.
    if (2 * L > H) {
        printf("0\n");
        return 0;
    }

    // 2D prefix sums of mushrooms.
    int W1 = C + 1;
    vector<int> P((size_t)(R + 1) * W1, 0);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            int m = (g[i][j] == 'M') ? 1 : 0;
            P[(size_t)(i + 1) * W1 + (j + 1)] =
                P[(size_t)i * W1 + (j + 1)] + P[(size_t)(i + 1) * W1 + j] -
                P[(size_t)i * W1 + j] + m;
        }
    }
    auto mushRect = [&](int r, int c, int h, int w) -> int {
        int r2 = r + h, c2 = c + w;
        return P[(size_t)r2 * W1 + c2] - P[(size_t)r * W1 + c2] -
               P[(size_t)r2 * W1 + c] + P[(size_t)r * W1 + c];
    };

    // used grid and run[i][j] = number of consecutive free cells starting at (i,j) going right.
    vector<char> used((size_t)R * C, 0);
    vector<int> run((size_t)R * (C + 1), 0);
    auto RUN = [&](int i, int j) -> int& { return run[(size_t)i * (C + 1) + j]; };
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++) RUN(i, j) = C - j;

    vector<array<int, 4>> slices;

    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (used[(size_t)r * C + c]) continue;
            int fw = INT_MAX;
            int bestArea = 0, bh = 0, bw = 0;
            int maxH = min(R - r, H);
            for (int h = 1; h <= maxH; h++) {
                fw = min(fw, RUN(r + h - 1, c));
                if (fw == 0) break;
                int w = min(fw, H / h);
                if (w <= 0) break;
                int area = h * w;
                if (area <= bestArea) continue;
                int mush = mushRect(r, c, h, w);
                int tom = area - mush;
                if (mush >= L && tom >= L) {
                    bestArea = area;
                    bh = h;
                    bw = w;
                }
            }
            if (bestArea > 0) {
                slices.push_back({r, c, r + bh - 1, c + bw - 1});
                for (int i = r; i < r + bh; i++) {
                    for (int j = c; j < c + bw; j++) used[(size_t)i * C + j] = 1;
                    for (int j = c + bw - 1; j >= 0; j--) {
                        RUN(i, j) = used[(size_t)i * C + j] ? 0 : RUN(i, j + 1) + 1;
                    }
                }
            }
        }
    }

    string out;
    out.reserve(slices.size() * 24 + 16);
    out += to_string(slices.size());
    out += '\n';
    for (auto& s : slices) {
        out += to_string(s[0]); out += ' ';
        out += to_string(s[1]); out += ' ';
        out += to_string(s[2]); out += ' ';
        out += to_string(s[3]); out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
