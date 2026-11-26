#include <bits/stdc++.h>
using namespace std;

static long long WORK = 0;
static inline int scoreT(const vector<int>& A, const vector<int>& B) {
    size_t i = 0, j = 0; int c = 0;
    while (i < A.size() && j < B.size()) {
        if (A[i] < B[j]) i++;
        else if (A[i] > B[j]) j++;
        else { c++; i++; j++; }
    }
    WORK += A.size() + B.size();
    int a = (int)A.size() - c, b = (int)B.size() - c;
    return min(c, min(a, b));
}
static inline void unite(const vector<int>& A, const vector<int>& B, vector<int>& out) {
    out.clear();
    out.reserve(A.size() + B.size());
    set_union(A.begin(), A.end(), B.begin(), B.end(), back_inserter(out));
    WORK += A.size() + B.size();
}
struct Slide { int a, b; vector<int> tags; };

static uint64_t rs = 88172645463325252ULL;
static inline uint64_t rnd() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }

int main() {
    auto T0 = chrono::steady_clock::now();
    auto elapsed = [&]() { return chrono::duration<double>(chrono::steady_clock::now() - T0).count(); };
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int N; if (!(cin >> N)) return 0;
    unordered_map<string, int> tagId; tagId.reserve(N * 4 + 16);
    vector<vector<int>> pt(N); vector<char> isV(N);
    string s;
    for (int i = 0; i < N; i++) {
        char c; int m; cin >> c >> m; isV[i] = (c == 'V');
        pt[i].resize(m);
        for (int k = 0; k < m; k++) {
            cin >> s;
            auto it = tagId.find(s);
            int id;
            if (it == tagId.end()) { id = (int)tagId.size(); tagId.emplace(s, id); } else id = it->second;
            pt[i][k] = id;
        }
        sort(pt[i].begin(), pt[i].end());
        pt[i].erase(unique(pt[i].begin(), pt[i].end()), pt[i].end());
    }
    int TT = (int)tagId.size();
    vector<int> markT(TT + 1, 0); int stamp = 0;

    vector<Slide> sl;
    for (int i = 0; i < N; i++) if (!isV[i]) sl.push_back({i, -1, pt[i]});
    // pair verticals
    vector<int> vs;
    for (int i = 0; i < N; i++) if (isV[i]) vs.push_back(i);
    stable_sort(vs.begin(), vs.end(), [&](int x, int y) { return pt[x].size() > pt[y].size(); });
    {
        int m = vs.size();
        vector<int> nx(m + 1), pv(m + 1);
        // list indices 0..m-1, sentinel m
        for (int i = 0; i < m; i++) { nx[i] = i + 1; pv[i] = i - 1; }
        int head = m > 0 ? 0 : m;
        auto rem = [&](int x) {
            if (pv[x] >= 0) nx[pv[x]] = nx[x]; else head = nx[x];
            if (nx[x] < m) pv[nx[x]] = pv[x];
        };
        while (head < m) {
            int a = head; rem(a);
            if (head >= m) break;
            stamp++;
            for (int t : pt[vs[a]]) markT[t] = stamp;
            int best = -1, bc = INT_MAX, cnt = 0;
            for (int x = head; x < m && cnt < 400; x = nx[x], cnt++) {
                int c = 0;
                for (int t : pt[vs[x]]) if (markT[t] == stamp) c++;
                if (c < bc) { bc = c; best = x; if (c == 0) break; }
            }
            rem(best);
            Slide S; S.a = vs[a]; S.b = vs[best];
            unite(pt[S.a], pt[S.b], S.tags);
            sl.push_back(move(S));
        }
    }
    int n = sl.size();
    if (n == 0) { printf("0\n"); return 0; }

    // greedy ordering
    vector<vector<int>> post(TT);
    vector<vector<int>> ppos(n);
    for (int i = 0; i < n; i++) {
        ppos[i].resize(sl[i].tags.size());
        for (size_t k = 0; k < sl[i].tags.size(); k++) {
            int t = sl[i].tags[k];
            ppos[i][k] = post[t].size();
            post[t].push_back(i);
        }
    }
    vector<char> used(n, 0);
    auto removeS = [&](int x) {
        used[x] = 1;
        for (size_t k = 0; k < sl[x].tags.size(); k++) {
            int t = sl[x].tags[k]; int idx = ppos[x][k];
            int last = post[t].back();
            post[t][idx] = last;
            int kk = lower_bound(sl[last].tags.begin(), sl[last].tags.end(), t) - sl[last].tags.begin();
            ppos[last][kk] = idx;
            post[t].pop_back();
        }
    };
    long long totTags = 0;
    for (auto& x : sl) totTags += x.tags.size();
    long long LIM = max(2000LL, (long long)(1.5e9 / max(1, n)));
    vector<int> vis(n, 0); int vst = 0;
    vector<int> ord; ord.reserve(n);
    int cur = 0; removeS(cur); ord.push_back(cur);
    int gp = 0;
    for (int step = 1; step < n; step++) {
        stamp++; vst++;
        const vector<int>& ct = sl[cur].tags;
        for (int t : ct) markT[t] = stamp;
        int bound = ct.size() / 2;
        int best = -1, bestc = -1, bestsz = INT_MAX;
        long long units = 0; bool stop = false;
        for (int t : ct) {
            for (int x : post[t]) {
                if (vis[x] == vst) continue;
                vis[x] = vst;
                const vector<int>& xt = sl[x].tags;
                int c = 0;
                for (int u : xt) if (markT[u] == stamp) c++;
                units += xt.size();
                int sc = min(c, min((int)ct.size() - c, (int)xt.size() - c));
                if (sc > best || (sc == best && (int)xt.size() < bestsz)) { best = sc; bestc = x; bestsz = xt.size(); }
                if (units > LIM || best >= bound) { stop = true; break; }
            }
            if (stop) break;
        }
        if (bestc == -1) {
            while (used[gp]) gp++;
            bestc = gp;
        }
        removeS(bestc); ord.push_back(bestc); cur = bestc;
    }
    { vector<vector<int>>().swap(post); vector<vector<int>>().swap(ppos); }

    // local search
    WORK = 0;
    const long long WLIM = 2500000000LL;
    vector<int> nt1, nt2;
    long long it = 0;
    int W = 60;
    bool anyV = (int)vs.size() >= 4;
    while (WORK < WLIM && n >= 3) {
        if ((++it & 1023) == 0 && elapsed() > 9.0) break;
        if (anyV && (rnd() & 1)) {
            int i = rnd() % n;
            int off = (int)(rnd() % (2 * W + 1)) - W;
            int j = i + off;
            WORK += 5;
            if (off == 0 || j < 0 || j >= n) continue;
            int si = ord[i], sj = ord[j];
            if (sl[si].b < 0 || sl[sj].b < 0) continue;
            int wi = rnd() & 1, wj = rnd() & 1;
            int pi = wi ? sl[si].b : sl[si].a, oi = wi ? sl[si].a : sl[si].b;
            int pj = wj ? sl[sj].b : sl[sj].a, oj = wj ? sl[sj].a : sl[sj].b;
            unite(pt[oi], pt[pj], nt1);
            unite(pt[oj], pt[pi], nt2);
            int es[4] = {i - 1, i, j - 1, j};
            sort(es, es + 4);
            int oldS = 0, newS = 0;
            for (int q = 0; q < 4; q++) {
                int e = es[q];
                if (q > 0 && es[q - 1] == e) continue;
                if (e < 0 || e + 1 >= n) continue;
                int x = ord[e], y = ord[e + 1];
                oldS += scoreT(sl[x].tags, sl[y].tags);
                const vector<int>& X = (x == si) ? nt1 : (x == sj ? nt2 : sl[x].tags);
                const vector<int>& Y = (y == si) ? nt1 : (y == sj ? nt2 : sl[y].tags);
                newS += scoreT(X, Y);
            }
            if (newS >= oldS) {
                sl[si].a = oi; sl[si].b = pj; sl[si].tags.swap(nt1);
                sl[sj].a = oj; sl[sj].b = pi; sl[sj].tags.swap(nt2);
            }
        } else {
            int i = rnd() % n;
            int j = i + 1 + (int)(rnd() % (2 * W));
            WORK += 5;
            if (j >= n) continue;
            int d = 0;
            if (i > 0) d += scoreT(sl[ord[i - 1]].tags, sl[ord[j]].tags) - scoreT(sl[ord[i - 1]].tags, sl[ord[i]].tags);
            if (j < n - 1) d += scoreT(sl[ord[i]].tags, sl[ord[j + 1]].tags) - scoreT(sl[ord[j]].tags, sl[ord[j + 1]].tags);
            if (d > 0) {
                reverse(ord.begin() + i, ord.begin() + j + 1);
                WORK += (j - i);
            }
        }
    }

    // output
    string out;
    out.reserve(n * 14 + 16);
    out += to_string(n); out += '\n';
    for (int x : ord) {
        out += to_string(sl[x].a);
        if (sl[x].b >= 0) { out += ' '; out += to_string(sl[x].b); }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
