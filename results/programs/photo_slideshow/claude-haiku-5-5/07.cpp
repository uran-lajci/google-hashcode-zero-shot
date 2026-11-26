#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> tags;  // sorted unique tag ids
    int a;             // first photo id
    int b;             // second photo id, or -1 if horizontal
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    unordered_map<string, int> tagId;
    tagId.reserve(1 << 16);

    vector<vector<int>> photoTags(n);
    vector<char> orient(n);

    for (int i = 0; i < n; ++i) {
        string o;
        int m;
        cin >> o >> m;
        orient[i] = o[0];
        photoTags[i].reserve(m);
        for (int j = 0; j < m; ++j) {
            string t;
            cin >> t;
            auto it = tagId.find(t);
            int id;
            if (it == tagId.end()) {
                id = (int)tagId.size();
                tagId.emplace(t, id);
            } else {
                id = it->second;
            }
            photoTags[i].push_back(id);
        }
        sort(photoTags[i].begin(), photoTags[i].end());
        photoTags[i].erase(unique(photoTags[i].begin(), photoTags[i].end()), photoTags[i].end());
    }

    vector<Slide> slides;
    vector<int> verts;
    for (int i = 0; i < n; ++i) {
        if (orient[i] == 'H') {
            slides.push_back({photoTags[i], i, -1});
        } else {
            verts.push_back(i);
        }
    }

    // Pair vertical photos: sort by tag count, pair smallest with largest.
    stable_sort(verts.begin(), verts.end(), [&](int x, int y) {
        return photoTags[x].size() < photoTags[y].size();
    });
    int V = (int)verts.size();
    for (int i = 0; i < V / 2; ++i) {
        int x = verts[i];
        int y = verts[V - 1 - i];
        vector<int> u;
        u.reserve(photoTags[x].size() + photoTags[y].size());
        set_union(photoTags[x].begin(), photoTags[x].end(),
                  photoTags[y].begin(), photoTags[y].end(), back_inserter(u));
        slides.push_back({u, x, y});
    }

    int S = (int)slides.size();
    if (S == 0) {
        // Cannot happen with N >= 1 and at least one photo usable, but be safe.
        cout << 1 << "\n";
        cout << 0 << "\n";
        return 0;
    }

    int T = (int)tagId.size();
    vector<vector<int>> post(T);
    for (int s = 0; s < S; ++s) {
        for (int t : slides[s].tags) post[t].push_back(s);
    }

    vector<char> used(S, 0);
    vector<int> cnt(S, 0);
    vector<int> touched;
    touched.reserve(S);

    vector<int> order;
    order.reserve(S);

    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);
    int fallbackPtr = 0;

    const long long PER_STEP_BUDGET = 4000;

    for (int step = 1; step < S; ++step) {
        touched.clear();
        long long budget = PER_STEP_BUDGET;
        const vector<int>& ct = slides[cur].tags;

        for (int t : ct) {
            if (budget <= 0) break;
            vector<int>& p = post[t];
            size_t i = 0;
            while (i < p.size() && budget > 0) {
                int s = p[i];
                if (used[s]) {
                    p[i] = p.back();
                    p.pop_back();
                    continue;
                }
                if (cnt[s] == 0) touched.push_back(s);
                cnt[s]++;
                ++i;
                --budget;
            }
        }

        int best = -1;
        int bestF = -1;
        int curSize = (int)ct.size();
        for (int s : touched) {
            int c = cnt[s];
            int sz = (int)slides[s].tags.size();
            int f = min(c, min(curSize - c, sz - c));
            if (f > bestF) {
                bestF = f;
                best = s;
            }
            cnt[s] = 0;
        }

        int chosen;
        if (best >= 0 && bestF > 0) {
            chosen = best;
        } else {
            while (fallbackPtr < S && used[fallbackPtr]) ++fallbackPtr;
            chosen = fallbackPtr;
        }

        used[chosen] = 1;
        order.push_back(chosen);
        cur = chosen;
    }

    string out;
    out.reserve((size_t)S * 16 + 16);
    out += to_string(S);
    out += '\n';
    for (int s : order) {
        out += to_string(slides[s].a);
        if (slides[s].b >= 0) {
            out += ' ';
            out += to_string(slides[s].b);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
