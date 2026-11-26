#include <bits/stdc++.h>
using namespace std;

// Greedy scheduler for the satellite imaging problem.
// Precomputes all satellite positions, then assigns collections in order of
// value per location, rolling back any collection that cannot be fully served.

static const double TIME_LIMIT = 8.0;

int T, S;
vector<int> LAT, LON;          // index t*S + s
vector<int> D;                 // max orientation per satellite
vector<long long> WMAX;        // max orientation change per turn per satellite
vector<map<int, pair<int,int>>> sp; // per satellite: turn -> (lat, lon) of target

chrono::steady_clock::time_point startT;
bool timeUp = false;

double elapsed() {
    return chrono::duration<double>(chrono::steady_clock::now() - startT).count();
}

static inline int normLon(long long x) {
    while (x < -648000) x += 1296000;
    while (x > 647999) x -= 1296000;
    return (int)x;
}

// Check whether satellite s can photograph (pl,po) at turn t given its existing picks.
bool canTake(int s, int t, int pl, int po) {
    int idx = t * S + s;
    long long dla = (long long)pl - LAT[idx];
    long long dlo = (long long)po - LON[idx];
    if (llabs(dla) > D[s] || llabs(dlo) > D[s]) return false;
    auto &m = sp[s];
    auto it = m.lower_bound(t);
    if (it != m.end() && it->first == t) return false;
    long long W = WMAX[s];
    if (it != m.begin()) {
        auto p = prev(it);
        int tp = p->first;
        long long pa = (long long)p->second.first - LAT[tp * S + s];
        long long pb = (long long)p->second.second - LON[tp * S + s];
        long long dt = t - tp;
        if (llabs(dla - pa) > W * dt || llabs(dlo - pb) > W * dt) return false;
    } else {
        // orientation starts at 0 at turn 0
        if (llabs(dla) > W * t || llabs(dlo) > W * t) return false;
    }
    if (it != m.end()) {
        int tn = it->first;
        long long na = (long long)it->second.first - LAT[tn * S + s];
        long long nb = (long long)it->second.second - LON[tn * S + s];
        long long dt = tn - t;
        if (llabs(na - dla) > W * dt || llabs(nb - dlo) > W * dt) return false;
    }
    return true;
}

struct LogEntry {
    long long key;
    int s, t;
};

long long makeKey(int pl, int po) {
    return (long long)(pl + 400000) * 2000000LL + (po + 700000);
}

unordered_map<long long, vector<pair<int,int>>> locPicks; // key -> (t, s)

bool scanLocation(int pl, int po, long long key,
                  const vector<pair<int,int>>& ranges, vector<LogEntry>& lg) {
    for (auto [a, b] : ranges) {
        for (int t = a; t <= b; ++t) {
            if (((t - a) & 255) == 0 && elapsed() > TIME_LIMIT) {
                timeUp = true;
                return false;
            }
            for (int s = 0; s < S; ++s) {
                if (canTake(s, t, pl, po)) {
                    sp[s][t] = {pl, po};
                    locPicks[key].push_back({t, s});
                    lg.push_back({key, s, t});
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    startT = chrono::steady_clock::now();

    cin >> T >> S;
    D.assign(S, 0);
    WMAX.assign(S, 0);
    vector<int> phi0(S), lam0(S), v0(S);
    for (int s = 0; s < S; ++s) {
        long long w;
        cin >> phi0[s] >> lam0[s] >> v0[s] >> w >> D[s];
        WMAX[s] = w;
    }

    // Simulate satellites
    LAT.assign((size_t)T * S, 0);
    LON.assign((size_t)T * S, 0);
    for (int s = 0; s < S; ++s) {
        long long la = phi0[s], lo = lam0[s], v = v0[s];
        for (int t = 0; t < T; ++t) {
            LAT[(size_t)t * S + s] = (int)la;
            LON[(size_t)t * S + s] = (int)lo;
            long long nl = la + v;
            long long nlo = lo - 15;
            long long nv = v;
            if (nl > 324000) {
                nl = 648000 - nl;
                nlo = -648000 + (lo - 15);
                nv = -v;
            } else if (nl < -324000) {
                nl = -648000 - nl;
                nlo = -648000 + (lo - 15);
                nv = -v;
            }
            la = nl;
            lo = normLon(nlo);
            v = nv;
        }
    }

    sp.assign(S, {});

    int C;
    cin >> C;
    struct Coll {
        int V;
        vector<pair<int,int>> locs;
        vector<pair<int,int>> ranges;
    };
    vector<Coll> colls(C);
    for (int c = 0; c < C; ++c) {
        int L, R;
        cin >> colls[c].V >> L >> R;
        colls[c].locs.resize(L);
        for (int i = 0; i < L; ++i) cin >> colls[c].locs[i].first >> colls[c].locs[i].second;
        colls[c].ranges.resize(R);
        for (int i = 0; i < R; ++i) {
            cin >> colls[c].ranges[i].first >> colls[c].ranges[i].second;
            if (colls[c].ranges[i].second > T - 1) colls[c].ranges[i].second = T - 1;
        }
    }

    vector<int> order(C);
    for (int i = 0; i < C; ++i) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        double ra = (double)colls[a].V / colls[a].locs.size();
        double rb = (double)colls[b].V / colls[b].locs.size();
        return ra > rb;
    });

    vector<LogEntry> lg;
    for (int c : order) {
        if (timeUp || elapsed() > TIME_LIMIT) break;
        lg.clear();
        bool allOk = true;
        for (auto [pl, po] : colls[c].locs) {
            long long key = makeKey(pl, po);
            bool sat = false;
            auto itL = locPicks.find(key);
            if (itL != locPicks.end()) {
                for (auto [t, s] : itL->second) {
                    for (auto [a, b] : colls[c].ranges) {
                        if (t >= a && t <= b) { sat = true; break; }
                    }
                    if (sat) break;
                }
            }
            if (!sat) {
                sat = scanLocation(pl, po, key, colls[c].ranges, lg);
            }
            if (!sat) {
                allOk = false;
                break;
            }
        }
        if (!allOk) {
            // rollback in reverse order
            for (int i = (int)lg.size() - 1; i >= 0; --i) {
                sp[lg[i].s].erase(lg[i].t);
                auto &vec = locPicks[lg[i].key];
                vec.pop_back();
            }
            lg.clear();
        }
    }

    // Output
    long long P = 0;
    for (int s = 0; s < S; ++s) P += (long long)sp[s].size();
    string out;
    out.reserve((size_t)P * 32 + 16);
    out += to_string(P);
    out += '\n';
    for (int s = 0; s < S; ++s) {
        for (auto &kv : sp[s]) {
            out += to_string(kv.second.first);
            out += ' ';
            out += to_string(kv.second.second);
            out += ' ';
            out += to_string(kv.first);
            out += ' ';
            out += to_string(s);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
