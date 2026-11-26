#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const ll LATMAX = 324000;
const ll LONOFF = 648000;
const ll FULL = 1296000;
const ll CELL = 12960;
const int NR = 50;
const int NC = 100;

ll wrapLon(ll x) {
    x %= FULL;
    if (x > 647999) x -= FULL;
    if (x < -648000) x += FULL;
    return x;
}

ll fdiv(ll a, ll b) {
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

struct Sat {
    ll phi, lam, v, w, d;
    ll lastPhi, lastLam, lastT;
};

void stepSat(Sat& s) {
    ll np = s.phi + s.v;
    ll nl = s.lam - 15;
    if (np > LATMAX) {
        np = 648000 - (s.phi + s.v);
        nl = -648000 + (s.lam - 15);
        s.v = -s.v;
    } else if (np < -LATMAX) {
        np = -648000 - (s.phi + s.v);
        nl = -648000 + (s.lam - 15);
        s.v = -s.v;
    }
    s.phi = np;
    s.lam = wrapLon(nl);
}

int rowOf(ll lat) {
    ll r = (lat + LATMAX) / CELL;
    if (r > NR - 1) r = NR - 1;
    if (r < 0) r = 0;
    return (int)r;
}

int colOf(ll lon) {
    ll c = ((lon + LONOFF) / CELL) % NC;
    return (int)c;
}

struct Photo {
    ll lat, lon, t;
    int s;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll T;
    cin >> T;
    int S;
    cin >> S;
    vector<Sat> sats(S);
    for (int i = 0; i < S; i++) {
        Sat& s = sats[i];
        cin >> s.phi >> s.lam >> s.v >> s.w >> s.d;
        s.lastPhi = 0;
        s.lastLam = 0;
        s.lastT = 0;
    }

    int C;
    cin >> C;

    vector<ll> V(C, 0), rem(C, 0);
    vector<ll> glat, glon;
    vector<int> gcoll;
    vector<char> gdone;
    vector<int> tsv, tev, tg;
    vector<vector<int>> cell(NR * NC);
    unordered_map<ll, vector<int>> coordMap;
    coordMap.reserve(1 << 20);

    for (int c = 0; c < C; c++) {
        ll v;
        int L, R;
        cin >> v >> L >> R;
        V[c] = v;
        rem[c] = L;
        vector<pair<ll, ll>> locs(L);
        for (int j = 0; j < L; j++) cin >> locs[j].first >> locs[j].second;
        vector<pair<int, int>> ranges(R);
        for (int r = 0; r < R; r++) cin >> ranges[r].first >> ranges[r].second;

        for (int j = 0; j < L; j++) {
            int g = (int)glat.size();
            glat.push_back(locs[j].first);
            glon.push_back(locs[j].second);
            gcoll.push_back(c);
            gdone.push_back(0);
            ll lat = locs[j].first, lon = locs[j].second;
            ll key = (lat + LATMAX) * 2000000LL + (lon + LONOFF);
            int cellIdx = rowOf(lat) * NC + colOf(lon);
            for (int r = 0; r < R; r++) {
                int ti = (int)tsv.size();
                tsv.push_back(ranges[r].first);
                tev.push_back(ranges[r].second);
                tg.push_back(g);
                cell[cellIdx].push_back(ti);
                coordMap[key].push_back(ti);
            }
        }
    }

    vector<Photo> photos;
    photos.reserve(1 << 16);

    for (ll t = 0; t < T; t++) {
        for (int si = 0; si < S; si++) {
            Sat& st = sats[si];
            ll phi = st.phi, lam = st.lam, d = st.d, w = st.w;
            ll allow = w * (t - st.lastT);

            ll lo = max(phi - d, -LATMAX);
            ll hi = min(phi + d, LATMAX);
            int r0 = rowOf(lo), r1 = rowOf(hi);
            ll k0 = fdiv(lam - d + LONOFF, CELL);
            ll k1 = fdiv(lam + d + LONOFF, CELL);

            double bestPri = -1.0;
            int bestTri = -1;
            ll bestDp = 0, bestDl = 0;

            for (int r = r0; r <= r1; r++) {
                for (ll k = k0; k <= k1; k++) {
                    int c = (int)(((k % NC) + NC) % NC);
                    vector<int>& lst = cell[r * NC + c];
                    for (size_t i = 0; i < lst.size();) {
                        int tri = lst[i];
                        int g = tg[tri];
                        if (gdone[g] || tev[tri] < t) {
                            lst[i] = lst.back();
                            lst.pop_back();
                            continue;
                        }
                        if (tsv[tri] > t) {
                            i++;
                            continue;
                        }
                        ll dp = glat[g] - phi;
                        ll dl = wrapLon(glon[g] - lam);
                        if (llabs(dp) > d || llabs(dl) > d) {
                            i++;
                            continue;
                        }
                        if (llabs(dp - st.lastPhi) > allow || llabs(dl - st.lastLam) > allow) {
                            i++;
                            continue;
                        }
                        double pri = (double)V[gcoll[g]] / (double)rem[gcoll[g]];
                        if (pri > bestPri) {
                            bestPri = pri;
                            bestTri = tri;
                            bestDp = dp;
                            bestDl = dl;
                        }
                        i++;
                    }
                }
            }

            if (bestTri >= 0) {
                int g = tg[bestTri];
                ll lat = glat[g], lon = glon[g];
                photos.push_back({lat, lon, t, si});
                ll key = (lat + LATMAX) * 2000000LL + (lon + LONOFF);
                auto it = coordMap.find(key);
                if (it != coordMap.end()) {
                    for (int idx : it->second) {
                        int g2 = tg[idx];
                        if (!gdone[g2] && tsv[idx] <= t && t <= tev[idx]) {
                            gdone[g2] = 1;
                            rem[gcoll[g2]]--;
                        }
                    }
                }
                st.lastPhi = bestDp;
                st.lastLam = bestDl;
                st.lastT = t;
            }
        }
        for (int si = 0; si < S; si++) stepSat(sats[si]);
    }

    string out;
    out.reserve(photos.size() * 32 + 16);
    out += to_string(photos.size());
    out += '\n';
    for (const Photo& p : photos) {
        out += to_string(p.lat);
        out += ' ';
        out += to_string(p.lon);
        out += ' ';
        out += to_string(p.t);
        out += ' ';
        out += to_string(p.s);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
