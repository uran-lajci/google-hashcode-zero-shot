#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;

    unordered_map<string, int> tagId;
    vector<vector<int>> ptags(N);
    vector<char> orient(N);

    for (int i = 0; i < N; i++) {
        char o;
        int m;
        cin >> o >> m;
        orient[i] = o;
        vector<int> v;
        v.reserve(m);
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
            v.push_back(id);
        }
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
        ptags[i] = move(v);
    }
    int numTags = (int)tagId.size();

    // Build slides
    vector<vector<int>> slideTags;
    vector<vector<int>> slidePhotos;
    vector<int> horiz, vert;
    for (int i = 0; i < N; i++) {
        if (orient[i] == 'H') horiz.push_back(i);
        else vert.push_back(i);
    }
    for (int h : horiz) {
        slideTags.push_back(ptags[h]);
        slidePhotos.push_back({h});
    }
    // pair verticals: sort by tag count, pair adjacent
    sort(vert.begin(), vert.end(), [&](int a, int b) {
        if (ptags[a].size() != ptags[b].size()) return ptags[a].size() < ptags[b].size();
        return a < b;
    });
    for (size_t i = 0; i + 1 < vert.size(); i += 2) {
        int a = vert[i], b = vert[i + 1];
        vector<int> u;
        set_union(ptags[a].begin(), ptags[a].end(), ptags[b].begin(), ptags[b].end(),
                  back_inserter(u));
        slideTags.push_back(move(u));
        slidePhotos.push_back({a, b});
    }

    int S = (int)slideTags.size();
    if (S == 0) {
        printf("0\n");
        return 0;
    }

    // Inverted index
    vector<vector<int>> idx(numTags);
    for (int j = 0; j < S; j++)
        for (int t : slideTags[j]) idx[t].push_back(j);

    vector<char> used(S, 0);
    vector<int> mark(numTags, 0);
    int stamp = 0;
    vector<int> cnt(S, 0);
    vector<int> touched;
    touched.reserve(8192);

    vector<int> order;
    order.reserve(S);
    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);
    int ptr = 0;
    const int BUDGET = 2000;
    const int TOPK = 30;

    for (int step = 1; step < S; step++) {
        stamp++;
        const vector<int> &T = slideTags[cur];
        for (int t : T) mark[t] = stamp;

        // tags of current slide, rarest first
        vector<int> ts = T;
        sort(ts.begin(), ts.end(), [&](int a, int b) {
            if (idx[a].size() != idx[b].size()) return idx[a].size() < idx[b].size();
            return a < b;
        });

        touched.clear();
        int budget = BUDGET;
        for (int t : ts) {
            if (budget <= 0) break;
            auto &lst = idx[t];
            size_t i = 0;
            while (i < lst.size() && budget > 0) {
                int j = lst[i];
                if (used[j]) {
                    lst[i] = lst.back();
                    lst.pop_back();
                    continue;
                }
                if (cnt[j] == 0) touched.push_back(j);
                cnt[j]++;
                i++;
                budget--;
            }
        }

        int best = -1;
        int bestInterest = -1;
        int bestC = -1;
        if (!touched.empty()) {
            int k = min((int)touched.size(), TOPK);
            partial_sort(touched.begin(), touched.begin() + k, touched.end(),
                         [&](int a, int b) {
                             if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
                             return a < b;
                         });
            for (int q = 0; q < k; q++) {
                int j = touched[q];
                int c = 0;
                for (int t : slideTags[j])
                    if (mark[t] == stamp) c++;
                int a = (int)T.size() - c;
                int b = (int)slideTags[j].size() - c;
                int interest = min(c, min(a, b));
                if (interest > bestInterest ||
                    (interest == bestInterest && c > bestC)) {
                    bestInterest = interest;
                    bestC = c;
                    best = j;
                }
            }
        }
        for (int j : touched) cnt[j] = 0;

        if (best == -1) {
            while (ptr < S && used[ptr]) ptr++;
            if (ptr >= S) break;
            best = ptr;
        }
        used[best] = 1;
        order.push_back(best);
        cur = best;
    }

    // Output
    string out;
    out.reserve(order.size() * 16 + 16);
    out += to_string(order.size());
    out += '\n';
    for (int j : order) {
        const auto &ph = slidePhotos[j];
        for (size_t k = 0; k < ph.size(); k++) {
            if (k) out += ' ';
            out += to_string(ph[k]);
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
