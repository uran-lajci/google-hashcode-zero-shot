#include <bits/stdc++.h>
using namespace std;

const long long FULL = 1296000LL;

long long wrapL(long long a) {
    // wrap into [-648000, 647999]
    a = ((a + 648000) % FULL + FULL) % FULL;
    return a - 648000;
}

struct Sat {
    int lam0, v0, w, d;
    int phi0;
};

struct Shot {
    int x, y, dp, dl;
};

struct Coll {
    long long V;
    vector<pair<int,int>> locs;
    vector<pair<int,int>> ranges;
};

int T, S, C;
vector<Sat> sats;
vector<vector<int>> PH, LM;
vector<map<int, Shot>> sched;
unordered_map<long long, vector<pair<int,int>>> locImgs; // key -> (turn, sat)

long long keyOf(int x, int y) {
    return (long long)(x + 400000) * 1000000LL + (long long)(y + 700000);
}

bool orientOK(int s, int t, int dp, int dl) {
    const auto &m = sched[s];
    int W = sats[s].w;
    auto it = m.lower_bound(t);
    if (it != m.end()) {
        long long dt = it->first - t;
        if (llabs((long long)dp - it->second.dp) > (long long)W * dt) return false;
        if (llabs((long long)dl - it->second.dl) > (long long)W * dt) return false;
    }
    if (it != m.begin()) {
        auto p = prev(it);
        long long dt = t - p->first;
        if (llabs((long long)dp - p->second.dp) > (long long)W * dt) return false;
        if (llabs((long long)dl - p->second.dl) > (long long)W * dt) return false;
    }
    return true;
}

bool findSlot(int x, int y, int ts, int te, int &bs, int &bt, int &bdp, int &bdl) {
    for (int s = 0; s < S; s++) {
        int d = sats[s].d;
        long long t = ts;
        while (t <= te) {
            long long g = wrapL((long long)y - LM[s][t]);
            if (llabs(g) > d) {
                long long need = (g < -d) ? (-d - g) : (FULL - d - g);
                long long step = (need + 14) / 15;
                if (step < 1) step = 1;
                t += step;
                continue;
            }
            long long dp = (long long)x - PH[s][t];
            if (llabs(dp) <= d && !sched[s].count((int)t) && orientOK(s, (int)t, (int)dp, (int)g)) {
                bs = s; bt = (int)t; bdp = (int)dp; bdl = (int)g;
                return true;
            }
            t++;
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> T;
    cin >> S;
    sats.resize(S);
    PH.assign(S, vector<int>(T));
    LM.assign(S, vector<int>(T));
    for (int s = 0; s < S; s++) {
        cin >> sats[s].phi0 >> sats[s].lam0 >> sats[s].v0 >> sats[s].w >> sats[s].d;
        long long phi = sats[s].phi0, lam = sats[s].lam0, v = sats[s].v0;
        for (int t = 0; t < T; t++) {
            PH[s][t] = (int)phi;
            LM[s][t] = (int)lam;
            long long np = phi + v;
            long long nv = v;
            bool flip = false;
            if (np > 324000) {
                np = 648000 - np;
                nv = -v;
                flip = true;
            } else if (np < -324000) {
                np = -648000 - np;
                nv = -v;
                flip = true;
            }
            long long nl = lam - 15 - (flip ? 648000 : 0);
            nl = wrapL(nl);
            phi = np;
            v = nv;
            lam = nl;
        }
    }

    cin >> C;
    vector<Coll> colls(C);
    for (int c = 0; c < C; c++) {
        int L, R;
        cin >> colls[c].V >> L >> R;
        colls[c].locs.resize(L);
        for (int i = 0; i < L; i++) cin >> colls[c].locs[i].first >> colls[c].locs[i].second;
        colls[c].ranges.resize(R);
        for (int i = 0; i < R; i++) cin >> colls[c].ranges[i].first >> colls[c].ranges[i].second;
    }

    sched.assign(S, map<int, Shot>());

    vector<int> order(C);
    for (int i = 0; i < C; i++) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        double ra = (double)colls[a].V / colls[a].locs.size();
        double rb = (double)colls[b].V / colls[b].locs.size();
        return ra > rb;
    });

    // record of accepted shots
    for (int ci : order) {
        const Coll &cl = colls[ci];
        // added: key, turn, sat
        vector<tuple<long long, int, int>> added;
        bool ok = true;
        for (auto &lc : cl.locs) {
            int x = lc.first, y = lc.second;
            long long key = keyOf(x, y);
            bool covered = false;
            auto itv = locImgs.find(key);
            if (itv != locImgs.end()) {
                for (auto &ts : itv->second) {
                    for (auto &r : cl.ranges) {
                        if (ts.first >= r.first && ts.first <= r.second) { covered = true; break; }
                    }
                    if (covered) break;
                }
            }
            if (covered) continue;

            bool found = false;
            for (auto &r : cl.ranges) {
                int bs, bt, bdp, bdl;
                if (findSlot(x, y, r.first, r.second, bs, bt, bdp, bdl)) {
                    sched[bs][bt] = Shot{x, y, bdp, bdl};
                    locImgs[key].push_back({bt, bs});
                    added.emplace_back(key, bt, bs);
                    found = true;
                    break;
                }
            }
            if (!found) { ok = false; break; }
        }
        if (!ok) {
            for (int i = (int)added.size() - 1; i >= 0; i--) {
                long long key;
                int t, s;
                tie(key, t, s) = added[i];
                sched[s].erase(t);
                auto &vec = locImgs[key];
                // the entry was the last pushed for this key at rollback time
                for (int j = (int)vec.size() - 1; j >= 0; j--) {
                    if (vec[j].first == t && vec[j].second == s) {
                        vec.erase(vec.begin() + j);
                        break;
                    }
                }
            }
        }
    }

    long long total = 0;
    for (int s = 0; s < S; s++) total += sched[s].size();
    string out;
    out.reserve(total * 32 + 16);
    out += to_string(total);
    out += '\n';
    for (int s = 0; s < S; s++) {
        for (auto &p : sched[s]) {
            out += to_string(p.second.x);
            out += ' ';
            out += to_string(p.second.y);
            out += ' ';
            out += to_string(p.first);
            out += ' ';
            out += to_string(s);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
