#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    unordered_map<string, int> tagId;
    vector<char> orient(N);
    vector<vector<int>> ptags(N);

    for (int i = 0; i < N; i++) {
        string o;
        int m;
        cin >> o >> m;
        orient[i] = o[0];
        for (int j = 0; j < m; j++) {
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
            ptags[i].push_back(id);
        }
        sort(ptags[i].begin(), ptags[i].end());
        ptags[i].erase(unique(ptags[i].begin(), ptags[i].end()), ptags[i].end());
    }
    int numTags = (int)tagId.size();

    // Build slides.
    vector<vector<int>> stags;
    vector<vector<int>> sphotos;
    vector<int> H, V;
    for (int i = 0; i < N; i++) {
        if (orient[i] == 'H') H.push_back(i);
        else V.push_back(i);
    }
    for (int h : H) {
        stags.push_back(ptags[h]);
        sphotos.push_back({h});
    }
    sort(V.begin(), V.end(), [&](int a, int b) {
        if (ptags[a].size() != ptags[b].size()) return ptags[a].size() > ptags[b].size();
        return a < b;
    });
    for (size_t i = 0; i + 1 < V.size(); i += 2) {
        int a = V[i], b = V[i + 1];
        vector<int> u;
        set_union(ptags[a].begin(), ptags[a].end(), ptags[b].begin(), ptags[b].end(),
                  back_inserter(u));
        stags.push_back(u);
        sphotos.push_back({a, b});
    }

    int S = (int)stags.size();
    if (S == 0) {
        // No slide possible (cannot happen under constraints with N>=1 unless only one V).
        cout << 0 << "\n";
        return 0;
    }

    // Inverted index.
    vector<vector<int>> post(numTags);
    for (int s = 0; s < S; s++)
        for (int t : stags[s]) post[t].push_back(s);
    vector<int> ptr(numTags, 0);

    const int L = 200;
    vector<char> used(S, 0);
    vector<int> cnt(S, 0);
    vector<int> touched;
    vector<int> order;
    order.reserve(S);

    int cur = 0;
    used[cur] = 1;
    order.push_back(cur);
    int gp = 0;

    for (int step = 1; step < S; step++) {
        touched.clear();
        const vector<int>& ct = stags[cur];
        int csz = (int)ct.size();
        for (int t : ct) {
            auto& pl = post[t];
            int &p = ptr[t];
            while (p < (int)pl.size() && used[pl[p]]) p++;
            int end = min((int)pl.size(), p + L);
            for (int k = p; k < end; k++) {
                int s = pl[k];
                if (used[s]) continue;
                if (cnt[s] == 0) touched.push_back(s);
                cnt[s]++;
            }
        }

        int best = -1, bestVal = 0;
        for (int s : touched) {
            int c = cnt[s];
            int ssz = (int)stags[s].size();
            int val = min(c, min(csz - c, ssz - c));
            if (val > bestVal) {
                bestVal = val;
                best = s;
            }
        }
        for (int s : touched) cnt[s] = 0;

        int nxt;
        if (best >= 0) {
            nxt = best;
        } else {
            while (gp < S && used[gp]) gp++;
            nxt = gp;
        }
        used[nxt] = 1;
        order.push_back(nxt);
        cur = nxt;
    }

    string out;
    out.reserve((size_t)S * 16);
    out += to_string(order.size());
    out += '\n';
    for (int s : order) {
        for (size_t k = 0; k < sphotos[s].size(); k++) {
            if (k) out += ' ';
            out += to_string(sphotos[s][k]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
