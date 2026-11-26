#include <bits/stdc++.h>
using namespace std;

typedef bitset<1024> BS;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto start = chrono::steady_clock::now();

    int H, W, D, B;
    cin >> H >> W >> D >> B;

    vector<int> hp(B), wp(B), fr(B, -1), fc(B, -1);
    vector<vector<BS>> pat(B);

    for (int i = 0; i < B; i++) {
        char t;
        int h, w, val;
        cin >> t >> h >> w >> val;
        hp[i] = h;
        wp[i] = w;
        pat[i].assign(h, BS());
        for (int row = 0; row < h; row++) {
            string s;
            cin >> s;
            for (int j = 0; j < w; j++) {
                if (s[j] == '#') {
                    pat[i][row][j] = 1;
                    if (fr[i] < 0) {
                        fr[i] = row;
                        fc[i] = j;
                    }
                }
            }
        }
    }

    vector<BS> occ(H, BS());
    vector<array<int, 3>> placed;
    int rot = 0;

    for (int r = 0; r < H; r++) {
        double el = chrono::duration<double>(chrono::steady_clock::now() - start).count();
        if (el > 8.0) break;
        for (int c = 0; c < W; c++) {
            for (int k = 0; k < B; k++) {
                int idx = (rot + k) % B;
                if (r + hp[idx] > H || c + wp[idx] > W) continue;
                // quick reject: first occupied cell of the plan must be free
                if (occ[r + fr[idx]][c + fc[idx]]) continue;
                bool ok = true;
                for (int i = 0; i < hp[idx]; i++) {
                    BS shifted = pat[idx][i] << c;
                    if ((occ[r + i] & shifted).any()) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    for (int i = 0; i < hp[idx]; i++) {
                        occ[r + i] |= (pat[idx][i] << c);
                    }
                    placed.push_back({idx, r, c});
                    rot = (idx + 1) % B;
                    break;
                }
            }
        }
    }

    string out;
    out += to_string(placed.size());
    out += '\n';
    for (auto &p : placed) {
        out += to_string(p[0]);
        out += ' ';
        out += to_string(p[1]);
        out += ' ';
        out += to_string(p[2]);
        out += '\n';
    }
    cout << out;
    return 0;
}
