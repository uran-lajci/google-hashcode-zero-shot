#include <bits/stdc++.h>
using namespace std;

int R, S, U, P, M;
vector<int> Z, C;
vector<vector<char>> blocked;

struct Result {
    long long score;
    vector<int> row, slot, pool;
};

Result attempt(const vector<int>& order) {
    // Build free segments per row
    vector<vector<pair<int,int>>> segs(R);
    for (int r = 0; r < R; r++) {
        int start = -1;
        for (int s = 0; s < S; s++) {
            if (!blocked[r][s]) {
                if (start < 0) start = s;
            } else {
                if (start >= 0) {
                    segs[r].push_back({start, s - start});
                    start = -1;
                }
            }
        }
        if (start >= 0) segs[r].push_back({start, S - start});
    }
    vector<int> maxSeg(R, 0);
    for (int r = 0; r < R; r++)
        for (auto& p : segs[r]) maxSeg[r] = max(maxSeg[r], p.second);

    vector<long long> rowCap(R, 0);
    vector<int> row(M, -1), slot(M, -1), pool(M, -1);
    vector<int> placed;

    // Phase 1: placement into rows
    for (int idx : order) {
        int z = Z[idx];
        int best = -1;
        for (int r = 0; r < R; r++) {
            if (maxSeg[r] >= z && (best < 0 || rowCap[r] < rowCap[best])) best = r;
        }
        if (best < 0) continue;
        auto& sg = segs[best];
        int k = -1;
        for (int j = 0; j < (int)sg.size(); j++) {
            if (sg[j].second >= z && (k < 0 || sg[j].second < sg[k].second)) k = j;
        }
        int st = sg[k].first;
        if (sg[k].second == z) {
            sg.erase(sg.begin() + k);
        } else {
            sg[k].first += z;
            sg[k].second -= z;
        }
        maxSeg[best] = 0;
        for (auto& p : sg) maxSeg[best] = max(maxSeg[best], p.second);
        rowCap[best] += C[idx];
        row[idx] = best;
        slot[idx] = st;
        placed.push_back(idx);
    }

    // Phase 2: pool assignment
    vector<long long> load((size_t)R * P, 0), total(P, 0), maxr(P, 0);
    for (int idx : placed) {
        int r = row[idx];
        long long c = C[idx];
        int best = -1;
        long long bestG = 0, bestNew = 0;
        for (int i = 0; i < P; i++) {
            long long g = total[i] - maxr[i];
            long long nm = max(maxr[i], load[(size_t)r * P + i] + c);
            long long newg = total[i] + c - nm;
            if (best < 0 || g < bestG || (g == bestG && newg > bestNew)) {
                best = i;
                bestG = g;
                bestNew = newg;
            }
        }
        pool[idx] = best;
        load[(size_t)r * P + best] += c;
        maxr[best] = max(maxr[best], load[(size_t)r * P + best]);
        total[best] += c;
    }

    long long score = LLONG_MAX;
    for (int i = 0; i < P; i++) score = min(score, total[i] - maxr[i]);

    Result res;
    res.score = score;
    res.row = move(row);
    res.slot = move(slot);
    res.pool = move(pool);
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> R >> S >> U >> P >> M;
    blocked.assign(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blocked[r][s] = 1;
    }
    Z.resize(M);
    C.resize(M);
    for (int i = 0; i < M; i++) cin >> Z[i] >> C[i];

    auto start = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - start).count();
    };

    vector<vector<int>> orders;
    {
        vector<int> o(M);
        iota(o.begin(), o.end(), 0);
        // o1: capacity descending
        vector<int> o1 = o;
        sort(o1.begin(), o1.end(), [&](int a, int b) {
            if (C[a] != C[b]) return C[a] > C[b];
            return a < b;
        });
        orders.push_back(o1);
        // o2: density descending
        vector<int> o2 = o;
        sort(o2.begin(), o2.end(), [&](int a, int b) {
            long long l = (long long)C[a] * Z[b], rr = (long long)C[b] * Z[a];
            if (l != rr) return l > rr;
            return a < b;
        });
        orders.push_back(o2);
        // o3: size ascending, capacity descending
        vector<int> o3 = o;
        sort(o3.begin(), o3.end(), [&](int a, int b) {
            if (Z[a] != Z[b]) return Z[a] < Z[b];
            if (C[a] != C[b]) return C[a] > C[b];
            return a < b;
        });
        orders.push_back(o3);
        // o4: capacity descending, size ascending
        vector<int> o4 = o;
        sort(o4.begin(), o4.end(), [&](int a, int b) {
            if (C[a] != C[b]) return C[a] > C[b];
            if (Z[a] != Z[b]) return Z[a] < Z[b];
            return a < b;
        });
        orders.push_back(o4);
    }

    Result best;
    best.score = -1;
    double lastDur = 0;
    for (size_t k = 0; k < orders.size(); k++) {
        if (k > 0 && elapsed() + lastDur * 1.2 > 7.0) break;
        double t0 = elapsed();
        Result res = attempt(orders[k]);
        lastDur = elapsed() - t0;
        if (res.score > best.score) best = move(res);
    }

    string out;
    out.reserve((size_t)M * 16);
    for (int i = 0; i < M; i++) {
        if (best.row[i] < 0) {
            out += "x\n";
        } else {
            out += to_string(best.row[i]);
            out += ' ';
            out += to_string(best.slot[i]);
            out += ' ';
            out += to_string(best.pool[i]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
