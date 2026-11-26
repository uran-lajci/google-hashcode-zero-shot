#include <bits/stdc++.h>
using namespace std;

static char buf[1 << 25];
int bufLen = 0, bufPos = 0;
inline int readChar() {
    if (bufPos == bufLen) {
        bufLen = (int)fread(buf, 1, sizeof(buf), stdin);
        bufPos = 0;
        if (bufLen <= 0) return -1;
    }
    return buf[bufPos++];
}
inline long long readInt() {
    int c = readChar();
    while (c != -1 && c != '-' && (c < '0' || c > '9')) c = readChar();
    bool neg = false;
    if (c == '-') { neg = true; c = readChar(); }
    long long x = 0;
    while (c >= '0' && c <= '9') {
        x = x * 10 + (c - '0');
        c = readChar();
    }
    return neg ? -x : x;
}

// ceil of Euclidean distance between two cells
static inline long long dist(long long r1, long long c1, long long r2, long long c2) {
    long long dr = r1 - r2, dc = c1 - c2;
    long long d2 = dr * dr + dc * dc;
    long long r = (long long)sqrtl((long double)d2);
    while (r * r < d2) r++;
    while (r > 0 && (r - 1) * (r - 1) >= d2) r--;
    return r;
}

struct Need {
    int type;
    int cnt;
};

int main() {
    long long R = readInt();
    long long Cc = readInt();
    int D = (int)readInt();
    long long T = readInt();
    long long maxLoad = readInt();

    int P = (int)readInt();
    vector<long long> weight(P);
    for (int i = 0; i < P; i++) weight[i] = readInt();

    int W = (int)readInt();
    vector<long long> wr(W), wc(W);
    vector<vector<int>> stock(W, vector<int>(P, 0));
    for (int w = 0; w < W; w++) {
        wr[w] = readInt();
        wc[w] = readInt();
        for (int p = 0; p < P; p++) stock[w][p] = (int)readInt();
    }

    int C = (int)readInt();
    vector<long long> orr(C), oc(C);
    vector<vector<Need>> need(C);
    vector<long long> total(C, 0);
    for (int o = 0; o < C; o++) {
        orr[o] = readInt();
        oc[o] = readInt();
        int L = (int)readInt();
        vector<int> types(L);
        for (int i = 0; i < L; i++) types[i] = (int)readInt();
        sort(types.begin(), types.end());
        for (int i = 0; i < L;) {
            int j = i;
            while (j < L && types[j] == types[i]) j++;
            need[o].push_back({types[i], j - i});
            i = j;
        }
        total[o] = L;
    }

    // Serve smaller orders first.
    vector<int> ord(C);
    for (int i = 0; i < C; i++) ord[i] = i;
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) { return total[a] < total[b]; });

    // Drones start at warehouse 0.
    vector<long long> dpr(D, wr[0]), dpc(D, wc[0]);
    vector<long long> dtime(D, 0);
    vector<vector<string>> cmds(D);

    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
    for (int d = 0; d < D; d++) pq.push({0, d});

    size_t ptr = 0;
    while (true) {
        while (ptr < ord.size() && need[ord[ptr]].empty()) ptr++;
        if (ptr >= ord.size()) break;
        if (pq.empty()) break;
        auto [t, d] = pq.top();
        pq.pop();
        if (t >= T) break;

        int o = ord[ptr];
        long long pr = dpr[d], pc = dpc[d];

        // Choose warehouse minimizing round trip for this order.
        int bestW = -1;
        long long bestCost = LLONG_MAX;
        for (int w = 0; w < W; w++) {
            bool has = false;
            for (const Need& nd : need[o]) {
                if (nd.cnt > 0 && stock[w][nd.type] > 0) { has = true; break; }
            }
            if (!has) continue;
            long long cost = dist(pr, pc, wr[w], wc[w]) + dist(wr[w], wc[w], orr[o], oc[o]);
            if (cost < bestCost) {
                bestCost = cost;
                bestW = w;
            }
        }
        if (bestW < 0) {
            need[o].clear();
            pq.push({t, d});
            continue;
        }

        // Build the load list for this trip.
        long long cap = maxLoad;
        vector<pair<int, int>> loads; // (type, count)
        for (const Need& nd : need[o]) {
            if (nd.cnt <= 0) continue;
            int avail = min(stock[bestW][nd.type], nd.cnt);
            long long kmax = cap / weight[nd.type];
            int k = (int)min<long long>(avail, kmax);
            if (k > 0) {
                loads.push_back({nd.type, k});
                cap -= (long long)k * weight[nd.type];
            }
        }
        if (loads.empty()) {
            // Cannot carry anything; shouldn't happen with valid input.
            need[o].clear();
            pq.push({t, d});
            continue;
        }

        // Simulate timing.
        long long tt = t;
        long long cr = pr, cc = pc;
        for (size_t i = 0; i < loads.size(); i++) {
            long long dd = dist(cr, cc, wr[bestW], wc[bestW]);
            tt += dd + 1;
            cr = wr[bestW];
            cc = wc[bestW];
        }
        for (size_t i = 0; i < loads.size(); i++) {
            long long dd = dist(cr, cc, orr[o], oc[o]);
            tt += dd + 1;
            cr = orr[o];
            cc = oc[o];
        }
        if (tt > T) {
            // Trip does not fit within the deadline; stop planning.
            break;
        }

        // Commit trip.
        for (auto [type, k] : loads) {
            stock[bestW][type] -= k;
            for (Need& nd : need[o]) {
                if (nd.type == type) { nd.cnt -= k; break; }
            }
            cmds[d].push_back(to_string(d) + " L " + to_string(bestW) + " " +
                              to_string(type) + " " + to_string(k));
        }
        for (auto [type, k] : loads) {
            cmds[d].push_back(to_string(d) + " D " + to_string(o) + " " +
                              to_string(type) + " " + to_string(k));
        }
        dpr[d] = orr[o];
        dpc[d] = oc[o];
        dtime[d] = tt;
        pq.push({tt, d});

        // Drop exhausted needs.
        auto& nv = need[o];
        nv.erase(remove_if(nv.begin(), nv.end(), [](const Need& x) { return x.cnt <= 0; }), nv.end());
    }

    // Output.
    size_t Q = 0;
    for (int d = 0; d < D; d++) Q += cmds[d].size();
    string out;
    out.reserve(Q * 24 + 16);
    out += to_string(Q);
    out += '\n';
    for (int d = 0; d < D; d++) {
        for (const string& s : cmds[d]) {
            out += s;
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
