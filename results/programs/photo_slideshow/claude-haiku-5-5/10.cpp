#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> tags;  // sorted unique tag ids
    int p1, p2;        // photo ids; p2 = -1 for horizontal
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    vector<char> ori(N);
    vector<vector<int>> tg(N);
    unordered_map<string, int> dict;
    dict.reserve(1 << 20);

    for (int i = 0; i < N; i++) {
        char c;
        int m;
        cin >> c >> m;
        ori[i] = c;
        tg[i].reserve(m);
        for (int k = 0; k < m; k++) {
            string s;
            cin >> s;
            auto it = dict.find(s);
            int id;
            if (it == dict.end()) {
                id = (int)dict.size();
                dict.emplace(s, id);
            } else {
                id = it->second;
            }
            tg[i].push_back(id);
        }
        sort(tg[i].begin(), tg[i].end());
        tg[i].erase(unique(tg[i].begin(), tg[i].end()), tg[i].end());
    }
    int T = (int)dict.size();

    vector<int> H, V;
    for (int i = 0; i < N; i++) {
        if (ori[i] == 'H') H.push_back(i);
        else V.push_back(i);
    }

    // Pair verticals with similar tag counts (sorted by size descending).
    sort(V.begin(), V.end(), [&](int a, int b) {
        if (tg[a].size() != tg[b].size()) return tg[a].size() > tg[b].size();
        return a < b;
    });

    vector<Slide> slides;
    slides.reserve(H.size() + V.size() / 2 + 1);
    for (int h : H) {
        slides.push_back({tg[h], h, -1});
    }
    for (size_t k = 0; k + 1 < V.size(); k += 2) {
        int a = V[k], b = V[k + 1];
        vector<int> u;
        u.reserve(tg[a].size() + tg[b].size());
        set_union(tg[a].begin(), tg[a].end(), tg[b].begin(), tg[b].end(),
                  back_inserter(u));
        slides.push_back({u, a, b});
    }

    int S = (int)slides.size();
    if (S == 0) {
        cout << 0 << "\n";
        return 0;
    }

    // Inverted index: tag -> slides containing it.
    vector<vector<int>> lists(T);
    for (int s = 0; s < S; s++) {
        for (int t : slides[s].tags) lists[t].push_back(s);
    }

    // Slides sorted by tag count for fallback.
    vector<int> bySize(S);
    for (int s = 0; s < S; s++) bySize[s] = s;
    sort(bySize.begin(), bySize.end(), [&](int a, int b) {
        if (slides[a].tags.size() != slides[b].tags.size())
            return slides[a].tags.size() < slides[b].tags.size();
        return a < b;
    });
    size_t fbPtr = 0;

    vector<char> used(S, 0);
    vector<int> mark(T, 0);
    vector<int> seen(S, 0);
    int stamp = 0;
    int candStamp = 0;

    vector<int> order;
    order.reserve(S);
    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);

    const int PER_TAG = 4;
    const size_t MAX_CAND = 40;

    vector<int> cand;
    cand.reserve(64);

    while ((int)order.size() < S) {
        stamp++;
        const vector<int>& ct = slides[cur].tags;
        for (int t : ct) mark[t] = stamp;

        candStamp++;
        cand.clear();
        for (int t : ct) {
            vector<int>& L = lists[t];
            size_t j = 0;
            int cnt = 0;
            while (j < L.size() && cnt < PER_TAG) {
                int s = L[j];
                if (used[s]) {
                    L[j] = L.back();
                    L.pop_back();
                    continue;
                }
                if (seen[s] != candStamp) {
                    seen[s] = candStamp;
                    cand.push_back(s);
                }
                j++;
                cnt++;
            }
            if (cand.size() >= MAX_CAND) break;
        }

        if (cand.empty()) {
            while (fbPtr < bySize.size() && used[bySize[fbPtr]]) fbPtr++;
            if (fbPtr < bySize.size()) cand.push_back(bySize[fbPtr]);
            else break;
        }

        int cs = (int)ct.size();
        int best = -1;
        int bestScore = -1;
        for (int s : cand) {
            const vector<int>& st = slides[s].tags;
            int ss = (int)st.size();
            int c = 0;
            for (int t : st) if (mark[t] == stamp) c++;
            int sc = min(c, min(cs - c, ss - c));
            if (sc > bestScore) {
                bestScore = sc;
                best = s;
            }
        }

        if (best < 0) break;
        used[best] = 1;
        order.push_back(best);
        cur = best;
    }

    // Output.
    string out;
    out.reserve(order.size() * 14 + 16);
    out += to_string(order.size());
    out += '\n';
    for (int s : order) {
        out += to_string(slides[s].p1);
        if (slides[s].p2 >= 0) {
            out += ' ';
            out += to_string(slides[s].p2);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
