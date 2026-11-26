#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int L, G, S, NB, F, N;
    cin >> L >> G >> S >> NB >> F >> N;

    // Services and their initial binaries.
    unordered_map<string, int> sid;
    vector<int> sbin(S);
    vector<int> R(NB, 0);
    for (int i = 0; i < S; i++) {
        string nm;
        int b;
        cin >> nm >> b;
        sid[nm] = i;
        sbin[i] = b;
        R[b]++;
    }

    struct Feat {
        string name;
        int D;
        long long U;
        vector<int> bins;
    };
    vector<Feat> feats(F);
    for (int f = 0; f < F; f++) {
        int M;
        cin >> feats[f].name >> M >> feats[f].D >> feats[f].U;
        set<int> bs;
        for (int k = 0; k < M; k++) {
            string sn;
            cin >> sn;
            bs.insert(sbin[sid[sn]]);
        }
        feats[f].bins.assign(bs.begin(), bs.end());
    }

    // Fenwick trees per binary over days 0..L (index day+1).
    const int W = L + 2;
    vector<int> fenS((size_t)NB * W, 0), fenE((size_t)NB * W, 0);

    auto upd = [&](vector<int>& fen, int b, int day, int v) {
        for (int i = day + 1; i <= L + 1; i += i & -i) fen[(size_t)b * W + i] += v;
    };
    auto qry = [&](vector<int>& fen, int b, int day) {
        int r = 0;
        for (int i = day + 1; i > 0; i -= i & -i) r += fen[(size_t)b * W + i];
        return r;
    };

    // Runs greedy planner in the given feature order.
    auto run = [&](const vector<int>& order,
                   vector<vector<pair<int, int>>>& out) -> long long {
        fill(fenS.begin(), fenS.end(), 0);
        fill(fenE.begin(), fenE.end(), 0);
        set<pair<int, int>> pool;  // (free time, engineer)
        for (int e = 0; e < G; e++) pool.insert({0, e});
        out.assign(G, {});
        long long score = 0;

        struct Op {
            int e, prevT, b, s, end;
        };

        for (int fi : order) {
            const Feat& f = feats[fi];
            vector<Op> ops;
            bool ok = true;
            int finish = 0;
            for (int b : f.bins) {
                if (pool.empty()) { ok = false; break; }
                auto it = pool.begin();
                int t = it->first, e = it->second;
                if (t >= L) { ok = false; break; }
                pool.erase(it);
                int c = qry(fenS, b, t) - qry(fenE, b, t);
                int end = t + f.D + R[b] + c;
                upd(fenS, b, t, 1);
                if (end <= L) upd(fenE, b, end, 1);
                pool.insert({end, e});
                ops.push_back({e, t, b, t, end});
                finish = max(finish, end);
            }
            if (ok && finish < L) {
                for (const Op& op : ops) out[op.e].push_back({fi, op.b});
                score += f.U * (long long)(L - finish);
            } else {
                for (int k = (int)ops.size() - 1; k >= 0; k--) {
                    const Op& op = ops[k];
                    pool.erase({op.end, op.e});
                    pool.insert({op.prevT, op.e});
                    upd(fenS, op.b, op.s, -1);
                    if (op.end <= L) upd(fenE, op.b, op.end, -1);
                }
            }
        }
        return score;
    };

    // Priority keys for ordering features.
    vector<double> k0(F), k1(F), k2(F);
    for (int f = 0; f < F; f++) {
        long long sumW = 0, maxW = 0;
        for (int b : feats[f].bins) {
            long long w = feats[f].D + R[b];
            sumW += w;
            maxW = max(maxW, w);
        }
        double U = (double)feats[f].U;
        if (sumW <= 0) sumW = 1;
        k0[f] = U / (double)sumW;
        k1[f] = (maxW > 0) ? U / (double)maxW : 0.0;
        k2[f] = (L - maxW > 0) ? U * (double)(L - maxW) / (double)sumW : 0.0;
    }

    vector<vector<double>*> keys = {&k0, &k1, &k2};
    long long bestScore = -1;
    vector<vector<pair<int, int>>> bestOut;
    for (auto kp : keys) {
        const vector<double>& key = *kp;
        vector<int> order(F);
        for (int f = 0; f < F; f++) order[f] = f;
        stable_sort(order.begin(), order.end(), [&](int a, int b) {
            if (key[a] != key[b]) return key[a] > key[b];
            return a < b;
        });
        vector<vector<pair<int, int>>> out;
        long long sc = run(order, out);
        if (sc > bestScore) {
            bestScore = sc;
            bestOut = out;
        }
    }

    // Output submission.
    int E = 0;
    for (int e = 0; e < G; e++)
        if (!bestOut[e].empty()) E++;

    string res;
    res += to_string(E) + "\n";
    for (int e = 0; e < G; e++) {
        if (bestOut[e].empty()) continue;
        res += to_string(bestOut[e].size()) + "\n";
        for (auto& pr : bestOut[e]) {
            res += "impl " + feats[pr.first].name + " " + to_string(pr.second) + "\n";
        }
    }
    cout << res;
    return 0;
}
