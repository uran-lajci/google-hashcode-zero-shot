#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> tags;
    int a, b; // b = -1 for a single horizontal photo
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<char> ori(n);
    vector<vector<int>> ptags(n);
    unordered_map<string, int> dict;
    dict.reserve(1 << 16);

    for (int i = 0; i < n; i++) {
        char c;
        int m;
        cin >> c >> m;
        ori[i] = c;
        ptags[i].reserve(m);
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
            ptags[i].push_back(id);
        }
        sort(ptags[i].begin(), ptags[i].end());
        ptags[i].erase(unique(ptags[i].begin(), ptags[i].end()), ptags[i].end());
    }
    const int T = (int)dict.size();

    vector<int> H, V;
    for (int i = 0; i < n; i++) {
        if (ori[i] == 'H') H.push_back(i);
        else V.push_back(i);
    }

    vector<Slide> slides;
    slides.reserve(n);
    for (int h : H) slides.push_back({ptags[h], h, -1});

    // Pair vertical photos: sort by tag count, pick partner in a window
    // maximizing the union size (so slide tag sets are richer).
    sort(V.begin(), V.end(), [&](int x, int y) {
        if (ptags[x].size() != ptags[y].size()) return ptags[x].size() < ptags[y].size();
        return x < y;
    });
    vector<char> vused(V.size(), 0);
    vector<int> mk(max(T, 1), 0);
    int stamp = 0;
    const int WINDOW = 30;
    for (size_t i = 0; i < V.size(); i++) {
        if (vused[i]) continue;
        stamp++;
        const vector<int>& A = ptags[V[i]];
        for (int t : A) mk[t] = stamp;
        int best = -1, bestU = -1;
        int considered = 0;
        for (size_t j = i + 1; j < V.size() && considered < WINDOW; j++) {
            if (vused[j]) continue;
            considered++;
            const vector<int>& B = ptags[V[j]];
            int common = 0;
            for (int t : B) if (mk[t] == stamp) common++;
            int U = (int)A.size() + (int)B.size() - common;
            if (U > bestU) {
                bestU = U;
                best = (int)j;
            }
        }
        if (best >= 0) {
            vused[i] = vused[best] = 1;
            vector<int> u = A;
            for (int t : ptags[V[best]]) u.push_back(t);
            sort(u.begin(), u.end());
            u.erase(unique(u.begin(), u.end()), u.end());
            slides.push_back({u, V[i], V[best]});
        }
    }

    const int K = (int)slides.size();
    if (K == 0) {
        cout << 0 << "\n";
        return 0;
    }

    // Inverted index: tag -> slides containing it
    vector<vector<int>> inv(max(T, 1));
    for (int s = 0; s < K; s++)
        for (int t : slides[s].tags) inv[t].push_back(s);

    vector<char> used(K, 0);
    vector<int> seen(K, 0);
    vector<int> mk2(max(T, 1), 0);
    vector<int> order;
    order.reserve(K);

    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);
    int remaining = K - 1;
    int fallback = 0;
    int step = 0;
    const size_t C = 50;

    while (remaining > 0) {
        step++;
        const Slide& cs = slides[cur];
        for (int t : cs.tags) mk2[t] = step;

        vector<int> cand;
        cand.reserve(C);
        bool full = false;
        for (int t : cs.tags) {
            if (full) break;
            vector<int>& L = inv[t];
            size_t k = 0;
            while (k < L.size()) {
                int s = L[k];
                if (used[s]) {
                    L[k] = L.back();
                    L.pop_back();
                    continue;
                }
                if (seen[s] != step) {
                    seen[s] = step;
                    cand.push_back(s);
                    if (cand.size() >= C) {
                        full = true;
                        break;
                    }
                }
                k++;
            }
        }

        int best = -1;
        int bestF = -1;
        if (!cand.empty()) {
            for (int s : cand) {
                const vector<int>& st = slides[s].tags;
                int common = 0;
                for (int t : st) if (mk2[t] == step) common++;
                int sz = (int)st.size();
                int f = min({common, (int)cs.tags.size() - common, sz - common});
                if (f > bestF) {
                    bestF = f;
                    best = s;
                }
            }
        } else {
            while (fallback < K && used[fallback]) fallback++;
            best = fallback;
        }

        used[best] = 1;
        order.push_back(best);
        remaining--;
        cur = best;
    }

    string out;
    out.reserve((size_t)K * 16 + 16);
    out += to_string(order.size());
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
