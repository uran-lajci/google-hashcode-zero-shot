#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> tags;  // sorted unique tag ids
    int a, b;          // photo ids (b = -1 for horizontal)
};

static int commonCount(const vector<int>& x, const vector<int>& y) {
    size_t i = 0, j = 0;
    int c = 0;
    while (i < x.size() && j < y.size()) {
        if (x[i] == y[j]) { c++; i++; j++; }
        else if (x[i] < y[j]) i++;
        else j++;
    }
    return c;
}

static vector<int> unionTags(const vector<int>& x, const vector<int>& y) {
    vector<int> r;
    r.reserve(x.size() + y.size());
    set_union(x.begin(), x.end(), y.begin(), y.end(), back_inserter(r));
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    unordered_map<string, int> tagId;
    vector<vector<int>> photoTags(N);
    vector<char> orient(N);
    for (int i = 0; i < N; i++) {
        string o;
        int m;
        cin >> o >> m;
        orient[i] = o[0];
        photoTags[i].reserve(m);
        for (int k = 0; k < m; k++) {
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
    int numTags = (int)tagId.size();

    vector<Slide> slides;
    vector<int> verts;
    for (int i = 0; i < N; i++) {
        if (orient[i] == 'H') {
            slides.push_back({photoTags[i], i, -1});
        } else {
            verts.push_back(i);
        }
    }

    // Greedy pairing of vertical photos: pair with a nearby unpaired vertical
    // that shares the fewest tags (maximizing the slide's tag union).
    {
        int V = (int)verts.size();
        vector<char> paired(V, 0);
        for (int i = 0; i < V; i++) {
            if (paired[i]) continue;
            int bestJ = -1, bestC = INT_MAX;
            int checked = 0, scanned = 0;
            for (int j = i + 1; j < V && checked < 20 && scanned < 60; j++) {
                scanned++;
                if (paired[j]) continue;
                checked++;
                int c = commonCount(photoTags[verts[i]], photoTags[verts[j]]);
                if (c < bestC) { bestC = c; bestJ = j; }
            }
            paired[i] = 1;
            if (bestJ >= 0) {
                paired[bestJ] = 1;
                int pa = verts[i], pb = verts[bestJ];
                slides.push_back({unionTags(photoTags[pa], photoTags[pb]), pa, pb});
            } else {
                // Unpaired leftover vertical: cannot form a slide alone; skip.
            }
        }
    }

    int S = (int)slides.size();
    if (S == 0) {
        // No slides possible (should not happen since N >= 1 and a photo exists
        // only if it is horizontal or paired); output a minimal valid answer.
        printf("1\n");
        if (N > 0) {
            // Fallback: a single vertical photo cannot be shown alone; output nothing valid.
            // Use photo 0 as a horizontal slide to remain syntactically valid.
            printf("0\n");
        }
        return 0;
    }

    vector<vector<int>> post(max(numTags, 1));
    for (int s = 0; s < S; s++)
        for (int t : slides[s].tags) post[t].push_back(s);

    vector<char> used(S, 0);
    vector<int> order;
    order.reserve(S);

    int cur = 0;
    used[0] = 1;
    order.push_back(0);
    int fallbackPtr = 1;

    for (int step = 1; step < S; step++) {
        const vector<int>& A = slides[cur].tags;
        int bestS = -1, bestV = -1;
        int scanned = 0, evaluated = 0;
        int na = (int)A.size();

        for (int t : A) {
            if (evaluated >= 30 || scanned >= 300) break;
            auto& p = post[t];
            size_t k = 0;
            while (k < p.size() && scanned < 300 && evaluated < 30) {
                int id = p[k];
                if (used[id]) {
                    p[k] = p.back();
                    p.pop_back();
                    continue;
                }
                scanned++;
                evaluated++;
                const vector<int>& B = slides[id].tags;
                int c = commonCount(A, B);
                int nb = (int)B.size();
                int v = min(c, min(na - c, nb - c));
                if (v > bestV) { bestV = v; bestS = id; }
                k++;
            }
        }

        if (bestS < 0) {
            while (fallbackPtr < S && used[fallbackPtr]) fallbackPtr++;
            if (fallbackPtr >= S) break;
            bestS = fallbackPtr;
        }

        used[bestS] = 1;
        order.push_back(bestS);
        cur = bestS;
    }

    string out;
    out.reserve(order.size() * 16 + 16);
    out += to_string(order.size());
    out += '\n';
    for (int s : order) {
        const Slide& sl = slides[s];
        if (sl.b < 0) {
            out += to_string(sl.a);
        } else {
            out += to_string(sl.a);
            out += ' ';
            out += to_string(sl.b);
        }
        out += '\n';
    }
    fputs(out.c_str(), stdout);
    return 0;
}
