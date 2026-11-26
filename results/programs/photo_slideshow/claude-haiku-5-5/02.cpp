#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> ids;   // photo ids (1 or 2)
    vector<int> tags;  // sorted unique tag ids
};

static vector<int> mergeUnique(const vector<int>& a, const vector<int>& b) {
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

    int n;
    cin >> n;

    unordered_map<string, int> tagId;
    vector<char> orient(n);
    vector<vector<int>> photoTags(n);

    for (int i = 0; i < n; i++) {
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
        photoTags[i].erase(unique(photoTags[i].begin(), photoTags[i].end()),
                           photoTags[i].end());
    }
    int numTags = (int)tagId.size();

    // Build slides.
    vector<Slide> slides;
    vector<int> verts;
    for (int i = 0; i < n; i++) {
        if (orient[i] == 'H') {
            Slide s;
            s.ids = {i};
            s.tags = photoTags[i];
            slides.push_back(move(s));
        } else {
            verts.push_back(i);
        }
    }

    // Pair verticals: for each unpaired vertical, choose among the next
    // few unpaired verticals the one with the fewest common tags.
    {
        int V = (int)verts.size();
        vector<char> paired(V, 0);
        for (int i = 0; i < V; i++) {
            if (paired[i]) continue;
            int best = -1, bestC = INT_MAX;
            int scanned = 0;
            for (int j = i + 1; j < V && scanned < 100; j++) {
                if (paired[j]) continue;
                scanned++;
                int c = commonCount(photoTags[verts[i]], photoTags[verts[j]]);
                if (c < bestC) {
                    bestC = c;
                    best = j;
                    if (c == 0) break;
                }
            }
            paired[i] = 1;
            if (best != -1) {
                paired[best] = 1;
                Slide s;
                s.ids = {verts[i], verts[best]};
                s.tags = mergeUnique(photoTags[verts[i]], photoTags[verts[best]]);
                slides.push_back(move(s));
            }
        }
    }

    int S = (int)slides.size();
    if (S == 0) {
        cout << 0 << "\n";
        return 0;
    }

    // Inverted index: tag -> slides containing it.
    vector<vector<int>> inv(numTags);
    for (int s = 0; s < S; s++)
        for (int t : slides[s].tags) inv[t].push_back(s);

    vector<int> head(numTags, 0);
    vector<char> used(S, 0);
    vector<int> mark(numTags, 0);
    int markStamp = 0;
    vector<int> seen(S, 0);
    int seenStamp = 0;
    int fallbackPtr = 0;

    vector<int> order;
    order.reserve(S);
    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);

    vector<int> cand;
    cand.reserve(512);

    while ((int)order.size() < S) {
        markStamp++;
        seenStamp++;
        const vector<int>& ctags = slides[cur].tags;
        for (int t : ctags) mark[t] = markStamp;

        cand.clear();
        for (int t : ctags) {
            auto& L = inv[t];
            while (head[t] < (int)L.size() && used[L[head[t]]]) head[t]++;
            int scanned = 0;
            for (int k = head[t]; k < (int)L.size() && scanned < 300 && cand.size() < 400; k++) {
                int x = L[k];
                scanned++;
                if (used[x]) continue;
                if (seen[x] == seenStamp) continue;
                seen[x] = seenStamp;
                cand.push_back(x);
            }
            if (cand.size() >= 400) break;
        }

        int bestX = -1, bestScore = -1;
        int cs = (int)ctags.size();
        for (int x : cand) {
            int common = 0;
            for (int t : slides[x].tags)
                if (mark[t] == markStamp) common++;
            int cx = (int)slides[x].tags.size();
            int sc = min(common, min(cs - common, cx - common));
            if (sc > bestScore) {
                bestScore = sc;
                bestX = x;
            }
        }

        if (bestX == -1) {
            while (fallbackPtr < S && used[fallbackPtr]) fallbackPtr++;
            if (fallbackPtr >= S) break;
            bestX = fallbackPtr;
        }

        used[bestX] = 1;
        order.push_back(bestX);
        cur = bestX;
    }

    // Output.
    string out;
    out.reserve(order.size() * 16 + 16);
    out += to_string(order.size());
    out += '\n';
    for (int s : order) {
        const auto& ids = slides[s].ids;
        for (size_t k = 0; k < ids.size(); k++) {
            if (k) out += ' ';
            out += to_string(ids[k]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
