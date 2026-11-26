#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, S, U, P, M;
    cin >> R >> S >> U >> P >> M;
    vector<vector<char>> blocked(R, vector<char>(S, 0));
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blocked[r][s] = 1;
    }
    vector<int> z(M), c(M);
    for (int i = 0; i < M; i++) cin >> z[i] >> c[i];

    // ---------- Placement ----------
    vector<vector<pair<int, int>>> seg(R); // (start, len) of free runs
    vector<int> freeCnt(R, 0), maxSeg(R, 0);
    for (int r = 0; r < R; r++) {
        int s = 0;
        while (s < S) {
            if (blocked[r][s]) { s++; continue; }
            int st = s;
            while (s < S && !blocked[r][s]) s++;
            seg[r].push_back({st, s - st});
            freeCnt[r] += s - st;
            maxSeg[r] = max(maxSeg[r], s - st);
        }
    }

    vector<int> ord(M);
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        ll lhs = (ll)c[a] * z[b], rhs = (ll)c[b] * z[a];
        if (lhs != rhs) return lhs > rhs;
        return a < b;
    });

    vector<int> rowOf(M, -1), slotOf(M, -1);
    for (int idx : ord) {
        int best = -1;
        for (int r = 0; r < R; r++) {
            if (maxSeg[r] >= z[idx] && (best < 0 || freeCnt[r] > freeCnt[best])) best = r;
        }
        if (best < 0) continue;
        // best fit segment
        int bi = -1;
        for (int j = 0; j < (int)seg[best].size(); j++) {
            if (seg[best][j].second >= z[idx]) {
                if (bi < 0 || seg[best][j].second < seg[best][bi].second) bi = j;
            }
        }
        auto &sg = seg[best][bi];
        rowOf[idx] = best;
        slotOf[idx] = sg.first;
        sg.first += z[idx];
        sg.second -= z[idx];
        if (sg.second == 0) seg[best].erase(seg[best].begin() + bi);
        freeCnt[best] -= z[idx];
        int ms = 0;
        for (auto &q : seg[best]) ms = max(ms, q.second);
        maxSeg[best] = ms;
    }

    // ---------- Compact placed servers ----------
    vector<int> pl;
    for (int i = 0; i < M; i++) if (rowOf[i] >= 0) pl.push_back(i);
    sort(pl.begin(), pl.end(), [&](int a, int b) {
        if (c[a] != c[b]) return c[a] > c[b];
        return a < b;
    });
    int K = pl.size();
    vector<int> srow(K), spool(K, 0);
    vector<ll> scap(K);
    vector<int> cidx(M, -1);
    for (int k = 0; k < K; k++) {
        cidx[pl[k]] = k;
        srow[k] = rowOf[pl[k]];
        scap[k] = c[pl[k]];
    }

    // ---------- Initial pool assignment (LPT) ----------
    vector<ll> T(P, 0), L((size_t)P * R, 0);
    {
        priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
        for (int p = 0; p < P; p++) pq.push({0, p});
        for (int k = 0; k < K; k++) {
            auto [t, p] = pq.top();
            pq.pop();
            spool[k] = p;
            T[p] += scap[k];
            L[(size_t)p * R + srow[k]] += scap[k];
            pq.push({T[p], p});
        }
    }

    vector<ll> top1(P), top2(P);
    vector<int> top1row(P);
    auto recompute = [&](int p) {
        ll best = -1;
        int bi = 0;
        for (int r = 0; r < R; r++) {
            ll v = L[(size_t)p * R + r];
            if (v > best) { best = v; bi = r; }
        }
        ll sec = 0;
        for (int r = 0; r < R; r++) {
            if (r == bi) continue;
            sec = max(sec, L[(size_t)p * R + r]);
        }
        top1[p] = max(best, 0LL);
        top1row[p] = bi;
        top2[p] = sec;
    };
    for (int p = 0; p < P; p++) recompute(p);

    // ---------- Local search on pool assignment ----------
    const ll LIMIT = 400000000LL;
    ll evals = 0;
    while (evals < LIMIT) {
        // find pool with minimum guaranteed capacity
        int q = 0;
        ll gcq = T[0] - top1[0];
        for (int p = 1; p < P; p++) {
            ll g = T[p] - top1[p];
            if (g < gcq) { gcq = g; q = p; }
        }
        bool found = false;
        for (int k = 0; k < K; k++) {
            int p = spool[k];
            if (p == q) continue;
            int r = srow[k];
            ll cc = scap[k];
            ll newGcQ = (T[q] + cc) - max(top1[q], L[(size_t)q * R + r] + cc);
            ll newMaxP = (r == top1row[p]) ? max(top2[p], L[(size_t)p * R + r] - cc) : top1[p];
            ll newGcP = (T[p] - cc) - newMaxP;
            if (newGcQ > gcq && newGcP > gcq) {
                // apply move k: p -> q
                L[(size_t)p * R + r] -= cc;
                L[(size_t)q * R + r] += cc;
                T[p] -= cc;
                T[q] += cc;
                spool[k] = q;
                recompute(p);
                recompute(q);
                found = true;
                break;
            }
        }
        evals += K + 1;
        if (!found) break;
    }

    // ---------- Output ----------
    string out;
    out.reserve((size_t)M * 16);
    for (int i = 0; i < M; i++) {
        if (rowOf[i] < 0) {
            out += "x\n";
        } else {
            int k = cidx[i];
            out += to_string(rowOf[i]);
            out += ' ';
            out += to_string(slotOf[i]);
            out += ' ';
            out += to_string(spool[k]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
