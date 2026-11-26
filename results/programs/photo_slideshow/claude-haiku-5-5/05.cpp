#include <bits/stdc++.h>
using namespace std;

static int interSize(const vector<int>& a, const vector<int>& b) {
    int i = 0, j = 0, c = 0;
    while (i < (int)a.size() && j < (int)b.size()) {
        if (a[i] == b[j]) { c++; i++; j++; }
        else if (a[i] < b[j]) i++;
        else j++;
    }
    return c;
}

static vector<int> unionTags(const vector<int>& a, const vector<int>& b) {
    vector<int> r;
    r.reserve(a.size() + b.size());
    set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(r));
    return r;
}

struct Slide {
    vector<int> tags;
    int a, b;  // b == -1 for a horizontal slide
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    unordered_map<string, int> tid;
    vector<char> ori(n);
    vector<vector<int>> tg(n);

    for (int i = 0; i < n; i++) {
        int m;
        cin >> ori[i] >> m;
        tg[i].reserve(m);
        for (int k = 0; k < m; k++) {
            string s;
            cin >> s;
            auto it = tid.find(s);
            int id;
            if (it == tid.end()) {
                id = (int)tid.size();
                tid.emplace(s, id);
            } else {
                id = it->second;
            }
            tg[i].push_back(id);
        }
        sort(tg[i].begin(), tg[i].end());
        tg[i].erase(unique(tg[i].begin(), tg[i].end()), tg[i].end());
    }

    vector<int> H, V;
    for (int i = 0; i < n; i++) {
        if (ori[i] == 'H') H.push_back(i);
        else V.push_back(i);
    }

    vector<Slide> sl;
    sl.reserve(n);
    for (int h : H) sl.push_back({tg[h], h, -1});

    // Pair verticals: sort by tag count, then for each vertical pick the
    // partner within a small forward window that minimizes shared tags.
    sort(V.begin(), V.end(), [&](int x, int y) {
        if (tg[x].size() != tg[y].size()) return tg[x].size() < tg[y].size();
        return x < y;
    });
    {
        const int K = 30;
        int vs = (int)V.size();
        vector<char> vu(vs, 0);
        for (int i = 0; i < vs; i++) {
            if (vu[i]) continue;
            vu[i] = 1;
            int best = -1, bi = INT_MAX, seen = 0;
            for (int j = i + 1; j < vs && seen < K; j++) {
                if (vu[j]) continue;
                seen++;
                int inter = interSize(tg[V[i]], tg[V[j]]);
                if (inter < bi) { bi = inter; best = j; }
            }
            if (best == -1) break;  // no unused verticals remain
            vu[best] = 1;
            sl.push_back({unionTags(tg[V[i]], tg[V[best]]), V[i], V[best]});
        }
    }

    int S = (int)sl.size();
    if (S == 0) {
        cout << 0 << "\n";
        return 0;
    }

    int T = (int)tid.size();
    vector<vector<int>> post(T);
    for (int s = 0; s < S; s++)
        for (int t : sl[s].tags) post[t].push_back(s);
    vector<int> st(T, 0);

    vector<char> used(S, 0);
    vector<int> cnt(S, 0);
    vector<int> touched;
    touched.reserve(S);

    vector<int> order;
    order.reserve(S);

    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);
    int fp = 0;  // fallback pointer over unused slides

    for (int step = 1; step < S; step++) {
        const vector<int>& ct = sl[cur].tags;
        int t = (int)ct.size();
        touched.clear();
        long long budget = 3000;

        for (int tag : ct) {
            vector<int>& lst = post[tag];
            int& s0 = st[tag];
            while (s0 < (int)lst.size() && used[lst[s0]]) s0++;
            for (int k = s0; k < (int)lst.size() && budget > 0; k++) {
                budget--;
                int id = lst[k];
                if (used[id]) continue;
                if (cnt[id] == 0) touched.push_back(id);
                cnt[id]++;
            }
        }

        int best = -1, bestF = -1;
        for (int id : touched) {
            int c = cnt[id];
            int s = (int)sl[id].tags.size();
            int f = min(c, min(t - c, s - c));
            if (f > bestF) { bestF = f; best = id; }
        }
        for (int id : touched) cnt[id] = 0;

        int nxt;
        if (best != -1) {
            nxt = best;
        } else {
            while (fp < S && used[fp]) fp++;
            if (fp >= S) break;
            nxt = fp;
        }
        used[nxt] = 1;
        order.push_back(nxt);
        cur = nxt;
    }

    string out;
    out.reserve(order.size() * 16 + 16);
    out += to_string(order.size());
    out += '\n';
    for (int idx : order) {
        const Slide& s = sl[idx];
        out += to_string(s.a);
        if (s.b != -1) {
            out += ' ';
            out += to_string(s.b);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
