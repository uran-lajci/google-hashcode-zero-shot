#include <bits/stdc++.h>
using namespace std;

static long long cdist(long long r1, long long c1, long long r2, long long c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    long long s = dr * dr + dc * dc;
    long long d = (long long)sqrtl((long double)s);
    while (d * d < s) d++;
    while (d > 0 && (d - 1) * (d - 1) >= s) d--;
    return d;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, Cc, D, T, MaxL;
    cin >> R >> Cc >> D >> T >> MaxL;
    int P;
    cin >> P;
    vector<int> wt(P);
    for (int i = 0; i < P; i++) cin >> wt[i];

    int W;
    cin >> W;
    vector<int> wr(W), wc(W);
    vector<uint16_t> stock((size_t)W * P);
    vector<vector<int>> whs(P);
    for (int w = 0; w < W; w++) {
        cin >> wr[w] >> wc[w];
        for (int p = 0; p < P; p++) {
            int x;
            cin >> x;
            stock[(size_t)w * P + p] = (uint16_t)x;
            if (x > 0) whs[p].push_back(w);
        }
    }

    int NC;
    cin >> NC;
    vector<int> orr(NC), occ(NC);
    vector<vector<pair<int, int>>> need(NC); // (type, remaining count)
    vector<long long> rem(NC, 0);
    for (int o = 0; o < NC; o++) {
        cin >> orr[o] >> occ[o];
        int L;
        cin >> L;
        vector<int> types(L);
        for (int i = 0; i < L; i++) cin >> types[i];
        sort(types.begin(), types.end());
        for (int i = 0; i < L;) {
            int j = i;
            while (j < L && types[j] == types[i]) j++;
            need[o].push_back({types[i], j - i});
            i = j;
        }
        rem[o] = L;
    }

    // Drones: priority queue of (free time, drone id)
    vector<int> dr(D, wr[0]), dc(D, wc[0]);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
    for (int d = 0; d < D; d++) pq.push({0LL, d});

    vector<vector<string>> cmds(D);
    vector<int> act;
    for (int o = 0; o < NC; o++) act.push_back(o);

    // Evaluate best trip for order o from position (pr,pc) starting at time t.
    // Returns true if feasible; fills p, w, k, dur.
    auto evaluate = [&](int o, int pr, int pc, long long t,
                        int& bp, int& bw, int& bk, long long& bdur) -> bool {
        bool found = false;
        double bestRatio = -1.0;
        for (auto& pr_pair : need[o]) {
            int p = pr_pair.first;
            int r = pr_pair.second;
            if (r <= 0) continue;
            int cap = MaxL / wt[p];
            if (cap <= 0) continue;
            vector<int>& lst = whs[p];
            long long bestCost = LLONG_MAX;
            int cw = -1;
            for (size_t i = 0; i < lst.size();) {
                int w = lst[i];
                if (stock[(size_t)w * P + p] == 0) {
                    lst[i] = lst.back();
                    lst.pop_back();
                    continue;
                }
                long long d1 = cdist(pr, pc, wr[w], wc[w]);
                long long d2 = cdist(wr[w], wc[w], orr[o], occ[o]);
                long long cost = d1 + d2;
                if (cost < bestCost) {
                    bestCost = cost;
                    cw = w;
                }
                i++;
            }
            if (cw < 0) continue;
            int stk = stock[(size_t)cw * P + p];
            int k = min({r, stk, cap});
            if (k <= 0) continue;
            long long dur = bestCost + 2;
            if (t + dur > T) continue;
            double ratio = (double)k / (double)dur;
            if (!found || ratio > bestRatio) {
                found = true;
                bestRatio = ratio;
                bp = p;
                bw = cw;
                bk = k;
                bdur = dur;
            }
        }
        return found;
    };

    while (!pq.empty()) {
        auto [t, d] = pq.top();
        pq.pop();
        if (t >= T) continue;

        // Compact active orders
        {
            vector<int> na;
            na.reserve(act.size());
            for (int o : act) if (rem[o] > 0) na.push_back(o);
            act.swap(na);
        }
        if (act.empty()) break;

        int pr = dr[d], pc = dc[d];
        vector<pair<long long, int>> cand;
        cand.reserve(act.size());
        for (int o : act) {
            long long dx = pr - orr[o], dy = pc - occ[o];
            cand.push_back({dx * dx + dy * dy, o});
        }
        int K = (int)min<size_t>(8, cand.size());
        partial_sort(cand.begin(), cand.begin() + K, cand.end());

        bool done = false;
        for (int i = 0; i < K && !done; i++) {
            int o = cand[i].second;
            int bp = -1, bw = -1, bk = 0;
            long long bdur = 0;
            if (evaluate(o, pr, pc, t, bp, bw, bk, bdur)) {
                // Apply trip
                stock[(size_t)bw * P + bp] -= (uint16_t)bk;
                for (auto& e : need[o]) {
                    if (e.first == bp) {
                        e.second -= bk;
                        break;
                    }
                }
                rem[o] -= bk;
                cmds[d].push_back(to_string(d) + " L " + to_string(bw) + " " +
                                  to_string(bp) + " " + to_string(bk));
                cmds[d].push_back(to_string(d) + " D " + to_string(o) + " " +
                                  to_string(bp) + " " + to_string(bk));
                dr[d] = orr[o];
                dc[d] = occ[o];
                pq.push({t + bdur, d});
                done = true;
            }
        }
        // If not done, this drone retires.
    }

    // Output
    size_t Q = 0;
    for (int d = 0; d < D; d++) Q += cmds[d].size();
    string out;
    out.reserve(Q * 24 + 16);
    out += to_string(Q);
    out += '\n';
    for (int d = 0; d < D; d++) {
        for (auto& s : cmds[d]) {
            out += s;
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
