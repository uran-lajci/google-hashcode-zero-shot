#include <bits/stdc++.h>
using namespace std;

struct Proj {
    char tp;
    int hp, wp, val, t, area;
    vector<pair<int,int>> cells;
    vector<pair<int,int>> samp;
};

int main() {
    auto startTime = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    int H, W, D, B;
    cin >> H >> W >> D >> B;
    vector<Proj> pr(B);
    for (int i = 0; i < B; i++) {
        string ts;
        cin >> ts;
        pr[i].tp = ts[0];
        cin >> pr[i].hp >> pr[i].wp >> pr[i].val;
        vector<string> g(pr[i].hp);
        for (int r = 0; r < pr[i].hp; r++) cin >> g[r];
        for (int r = 0; r < pr[i].hp; r++)
            for (int c = 0; c < pr[i].wp; c++)
                if (g[r][c] == '#') pr[i].cells.push_back({r, c});
        pr[i].area = pr[i].cells.size();
        size_t step = max<size_t>(1, pr[i].cells.size() / 32);
        for (size_t k = 0; k < pr[i].cells.size(); k += step)
            pr[i].samp.push_back(pr[i].cells[k]);
        pr[i].t = -1;
    }

    map<int, int> tmap;
    vector<int> utilIdx, resIdx;
    for (int i = 0; i < B; i++) {
        if (pr[i].tp == 'U') {
            auto it = tmap.find(pr[i].val);
            if (it == tmap.end()) {
                int id = tmap.size();
                tmap.emplace(pr[i].val, id);
                pr[i].t = id;
            } else {
                pr[i].t = it->second;
            }
            utilIdx.push_back(i);
        } else {
            resIdx.push_back(i);
        }
    }
    int K = tmap.size();
    int WORDS = max(1, (K + 63) / 64);

    int N = H * W;
    vector<int> ord(N);
    iota(ord.begin(), ord.end(), 0);
    vector<long long> key(N);
    for (int i = 0; i < N; i++) {
        long long r = i / W, c = i % W;
        long long dr = 2 * r - (H - 1), dc = 2 * c - (W - 1);
        key[i] = dr * dr + dc * dc;
    }
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        if (key[a] != key[b]) return key[a] < key[b];
        return a < b;
    });

    vector<char> occ(N, 0);
    vector<array<int,3>> placements;

    auto fits = [&](int r, int c, const Proj& p) -> bool {
        if (r + p.hp > H || c + p.wp > W) return false;
        for (auto& [dr, dc] : p.cells) {
            if (occ[(r + dr) * W + (c + dc)]) return false;
        }
        return true;
    };
    auto place = [&](int idx, int r, int c, const Proj& p) {
        for (auto& [dr, dc] : p.cells) occ[(r + dr) * W + (c + dc)] = 1;
        placements.push_back({idx, r, c});
    };

    // Phase 1: one utility per type, smallest footprint first.
    sort(utilIdx.begin(), utilIdx.end(), [&](int a, int b) {
        if (pr[a].area != pr[b].area) return pr[a].area < pr[b].area;
        return a < b;
    });
    vector<char> typeUsed(K, 0);
    vector<int> chosenUtil;
    for (int i : utilIdx) {
        if (!typeUsed[pr[i].t]) {
            typeUsed[pr[i].t] = 1;
            chosenUtil.push_back(i);
        }
    }
    for (int i : chosenUtil) {
        if (elapsed() > 6.0) break;
        const Proj& p = pr[i];
        for (int a : ord) {
            int r = a / W, c = a % W;
            if (r + p.hp > H || c + p.wp > W) continue;
            if (fits(r, c, p)) {
                place(i, r, c, p);
                break;
            }
        }
    }

    // Accessibility bitsets: for each cell, which utility types are within D.
    vector<uint64_t> acc((size_t)N * WORDS, 0ULL);
    {
        vector<int> seen(N, -1), dist(N, 0), dq;
        dq.reserve(N);
        for (int k = 0; k < (int)placements.size(); k++) {
            const Proj& p = pr[placements[k][0]];
            int r0 = placements[k][1], c0 = placements[k][2];
            int t = p.t;
            dq.clear();
            for (auto& [dr, dc] : p.cells) {
                int s = (r0 + dr) * W + (c0 + dc);
                if (seen[s] != k) {
                    seen[s] = k;
                    dist[s] = 0;
                    dq.push_back(s);
                }
            }
            size_t head = 0;
            while (head < dq.size()) {
                int x = dq[head++];
                acc[(size_t)x * WORDS + (t >> 6)] |= 1ULL << (t & 63);
                if (dist[x] >= D) continue;
                int xr = x / W, xc = x % W;
                int nb[4][2] = {{xr - 1, xc}, {xr + 1, xc}, {xr, xc - 1}, {xr, xc + 1}};
                for (auto& q : nb) {
                    if (q[0] < 0 || q[0] >= H || q[1] < 0 || q[1] >= W) continue;
                    int y = q[0] * W + q[1];
                    if (seen[y] == k) continue;
                    seen[y] = k;
                    dist[y] = dist[x] + 1;
                    dq.push_back(y);
                }
            }
        }
    }

    auto evalTypes = [&](int r, int c, const Proj& p) -> int {
        if (K == 0) return 0;
        uint64_t tmp[64];
        vector<uint64_t> big;
        uint64_t* t = tmp;
        if (WORDS > 64) { big.assign(WORDS, 0); t = big.data(); }
        else for (int w = 0; w < WORDS; w++) tmp[w] = 0;
        for (auto& [dr, dc] : p.samp) {
            size_t idx = (size_t)((r + dr) * W + (c + dc)) * WORDS;
            for (int w = 0; w < WORDS; w++) t[w] |= acc[idx + w];
        }
        int cnt = 0;
        for (int w = 0; w < WORDS; w++) cnt += __builtin_popcountll(t[w]);
        return cnt;
    };

    // Phase 2: residentials.
    int R = resIdx.size();
    if (R > 0) {
        int M = min(6, R);
        int cursor = 0;
        for (int a : ord) {
            if ((a & 1023) == 0 && elapsed() > 8.5) break;
            int r = a / W, c = a % W;
            int bestVal = 0, bestI = -1;
            for (int j = 0; j < M; j++) {
                int i = resIdx[(cursor + j) % R];
                const Proj& p = pr[i];
                if (!fits(r, c, p)) continue;
                int v = p.val * evalTypes(r, c, p);
                if (v > bestVal) {
                    bestVal = v;
                    bestI = i;
                }
            }
            cursor = (cursor + 1) % R;
            if (bestI >= 0) place(bestI, r, c, pr[bestI]);
        }
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
    fputs(out.c_str(), stdout);
    return 0;
}
