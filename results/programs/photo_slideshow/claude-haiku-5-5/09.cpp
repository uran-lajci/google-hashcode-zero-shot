#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> tags;   // sorted unique tag ids
    vector<int> ids;    // photo ids
};

static vector<int> mergeUnion(const vector<int>& a, const vector<int>& b) {
    vector<int> r;
    r.reserve(a.size() + b.size());
    set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(r));
    return r;
}

static int commonCount(const vector<int>& a, const vector<int>& b) {
    int i = 0, j = 0, c = 0;
    while (i < (int)a.size() && j < (int)b.size()) {
        if (a[i] == b[j]) { c++; i++; j++; }
        else if (a[i] < b[j]) i++;
        else j++;
    }
    return c;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;

    unordered_map<string, int> tagId;
    tagId.reserve(1 << 16);

    vector<vector<int>> photoTags(N);
    vector<char> orient(N);

    for (int i = 0; i < N; i++) {
        char o;
        int m;
        cin >> o >> m;
        orient[i] = o;
        photoTags[i].reserve(m);
        for (int k = 0; k < m; k++) {
            string s;
            cin >> s;
            auto it = tagId.find(s);
            int id;
            if (it == tagId.end()) {
                id = (int)tagId.size();
                tagId.emplace(s, id);
            } else {
                id = it->second;
            }
            photoTags[i].push_back(id);
        }
        sort(photoTags[i].begin(), photoTags[i].end());
        photoTags[i].erase(unique(photoTags[i].begin(), photoTags[i].end()), photoTags[i].end());
    }
    int T = (int)tagId.size();

    vector<Slide> slides;
    vector<int> verts;
    for (int i = 0; i < N; i++) {
        if (orient[i] == 'H') {
            Slide s;
            s.tags = photoTags[i];
            s.ids = {i};
            slides.push_back(move(s));
        } else {
            verts.push_back(i);
        }
    }

    // Pair verticals: order by tag count descending, pick partner in a window
    // that minimizes shared tags (more diverse slide).
    sort(verts.begin(), verts.end(), [&](int a, int b) {
        if (photoTags[a].size() != photoTags[b].size())
            return photoTags[a].size() > photoTags[b].size();
        return a < b;
    });
    int V = (int)verts.size();
    vector<char> vused(V, 0);
    const int W = 30;
    for (int i = 0; i < V; i++) {
        if (vused[i]) continue;
        vused[i] = 1;
        int bestK = -1, bestC = INT_MAX;
        int collected = 0;
        for (int k = i + 1; k < V && collected < W; k++) {
            if (vused[k]) continue;
            collected++;
            int c = commonCount(photoTags[verts[i]], photoTags[verts[k]]);
            if (c < bestC) { bestC = c; bestK = k; }
        }
        if (bestK >= 0) {
            vused[bestK] = 1;
            Slide s;
            s.tags = mergeUnion(photoTags[verts[i]], photoTags[verts[bestK]]);
            s.ids = {verts[i], verts[bestK]};
            slides.push_back(move(s));
        }
        // An unpaired last vertical photo is left unused (allowed).
    }

    int S = (int)slides.size();
    if (S == 0) {
        // Should not happen since N >= 1, but handle gracefully.
        cout << 0 << "\n";
        return 0;
    }

    // Inverted index: tag -> slides containing it.
    vector<vector<int>> lists(T);
    for (int s = 0; s < S; s++)
        for (int t : slides[s].tags)
            lists[t].push_back(s);

    vector<char> used(S, 0);
    vector<int> cnt(S, 0);
    vector<int> touched;
    touched.reserve(4096);

    vector<int> order;
    order.reserve(S);

    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);
    int fallbackPtr = 0;

    const long long BUDGET = 3000;

    for (int step = 1; step < S; step++) {
        const Slide& cs = slides[cur];
        int a = (int)cs.tags.size();
        int perTag = (int)(BUDGET / max(1, a)) + 10;

        touched.clear();
        for (int t : cs.tags) {
            vector<int>& lst = lists[t];
            size_t j = 0;
            int examined = 0;
            while (j < lst.size() && examined < perTag) {
                int s = lst[j];
                if (used[s]) {
                    lst[j] = lst.back();
                    lst.pop_back();
                    continue;
                }
                examined++;
                if (cnt[s] == 0) touched.push_back(s);
                cnt[s]++;
                j++;
            }
        }

        int best = -1, bestScore = -1;
        for (int s : touched) {
            int c = cnt[s];
            int b = (int)slides[s].tags.size();
            int sc = min(c, min(a - c, b - c));
            if (sc > bestScore) {
                bestScore = sc;
                best = s;
            }
        }
        for (int s : touched) cnt[s] = 0;

        int nxt = -1;
        if (best >= 0) {
            nxt = best;
        } else {
            while (fallbackPtr < S && used[fallbackPtr]) fallbackPtr++;
            if (fallbackPtr < S) nxt = fallbackPtr;
        }
        if (nxt < 0) break;

        used[nxt] = 1;
        order.push_back(nxt);
        cur = nxt;
    }

    // Output
    string out;
    out.reserve(order.size() * 16 + 16);
    out += to_string(order.size());
    out += '\n';
    for (int s : order) {
        const Slide& sl = slides[s];
        for (size_t k = 0; k < sl.ids.size(); k++) {
            if (k) out += ' ';
            out += to_string(sl.ids[k]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
