#include <bits/stdc++.h>
using namespace std;

struct Slide {
    vector<int> tags; // sorted unique
    int a, b;         // b = -1 for horizontal
};

static vector<int> par; // DSU "next unused" structure

int findNext(int x) {
    int r = x;
    while (par[r] != r) r = par[r];
    while (par[x] != r) { int nx = par[x]; par[x] = r; x = nx; }
    return r;
}

int interCount(const vector<int>& a, const vector<int>& b) {
    int i = 0, j = 0, c = 0;
    while (i < (int)a.size() && j < (int)b.size()) {
        if (a[i] == b[j]) { c++; i++; j++; }
        else if (a[i] < b[j]) i++;
        else j++;
    }
    return c;
}

vector<int> unionTags(const vector<int>& a, const vector<int>& b) {
    vector<int> r;
    set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(r));
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    vector<char> ori(N);
    vector<vector<int>> tg(N);
    unordered_map<string, int> tmap;
    tmap.reserve(1 << 18);

    for (int i = 0; i < N; i++) {
        string o;
        int m;
        cin >> o >> m;
        ori[i] = o[0];
        tg[i].reserve(m);
        for (int k = 0; k < m; k++) {
            string t;
            cin >> t;
            auto it = tmap.find(t);
            int id;
            if (it == tmap.end()) {
                id = (int)tmap.size();
                tmap.emplace(t, id);
            } else {
                id = it->second;
            }
            tg[i].push_back(id);
        }
        sort(tg[i].begin(), tg[i].end());
        tg[i].erase(unique(tg[i].begin(), tg[i].end()), tg[i].end());
    }
    int numTags = (int)tmap.size();

    vector<Slide> slides;
    vector<int> H, V;
    for (int i = 0; i < N; i++) {
        if (ori[i] == 'H') H.push_back(i);
        else V.push_back(i);
    }

    for (int h : H) {
        slides.push_back({tg[h], h, -1});
    }

    // Pair verticals greedily: each vertical picks, among the next K unused
    // verticals (sorted by tag count desc), the one with fewest shared tags.
    sort(V.begin(), V.end(), [&](int x, int y) {
        if (tg[x].size() != tg[y].size()) return tg[x].size() > tg[y].size();
        return x < y;
    });
    int VN = (int)V.size();
    vector<char> usedV(VN, 0);
    par.assign(VN + 1, 0);
    for (int i = 0; i <= VN; i++) par[i] = i;
    const int K = 30;

    // mark used helper: remove position p from DSU
    auto removePos = [&](int p) {
        usedV[p] = 1;
        par[p] = p + 1;
    };

    for (int i = 0; i < VN; i++) {
        if (usedV[i]) continue;
        removePos(i);
        int best = -1, bestInter = INT_MAX;
        int seen = 0;
        int j = findNext(i + 1);
        while (j < VN && seen < K) {
            seen++;
            int inter = interCount(tg[V[i]], tg[V[j]]);
            if (inter < bestInter) {
                bestInter = inter;
                best = j;
            }
            j = findNext(j + 1);
        }
        if (best != -1) {
            removePos(best);
            int a = V[i], b = V[best];
            slides.push_back({unionTags(tg[a], tg[b]), a, b});
        }
        // unpaired vertical cannot be used alone; dropped
    }

    int S = (int)slides.size();
    if (S == 0) {
        cout << 0 << "\n";
        return 0;
    }

    // Inverted index
    vector<vector<int>> post(numTags);
    for (int s = 0; s < S; s++)
        for (int t : slides[s].tags) post[t].push_back(s);

    vector<char> used(S, 0);
    vector<int> cnt(S, 0);
    vector<int> touched;
    touched.reserve(1024);
    vector<int> seq;
    seq.reserve(S);

    int cur = 0;
    used[cur] = 1;
    seq.push_back(cur);
    int fp = 0;
    const int PERTAG = 200;
    const int TOTAL = 5000;

    while ((int)seq.size() < S) {
        int budget = TOTAL;
        int curSize = (int)slides[cur].tags.size();
        for (int t : slides[cur].tags) {
            vector<int>& L = post[t];
            size_t i = 0;
            int scanned = 0;
            while (i < L.size() && scanned < PERTAG && budget > 0) {
                int s = L[i];
                if (used[s]) {
                    L[i] = L.back();
                    L.pop_back();
                    continue;
                }
                scanned++;
                budget--;
                if (cnt[s] == 0) touched.push_back(s);
                cnt[s]++;
                i++;
            }
            if (budget <= 0) break;
        }

        int best = -1, bestv = 0;
        for (int s : touched) {
            int c = cnt[s];
            int v = min(c, min(curSize - c, (int)slides[s].tags.size() - c));
            if (v > bestv) {
                bestv = v;
                best = s;
            } else if (v == bestv && best != -1 && s < best) {
                best = s;
            }
        }
        for (int s : touched) cnt[s] = 0;
        touched.clear();

        if (best == -1) {
            while (fp < S && used[fp]) fp++;
            best = fp;
        }
        used[best] = 1;
        seq.push_back(best);
        cur = best;
    }

    string out;
    out.reserve(S * 16);
    out += to_string(S);
    out += '\n';
    for (int s : seq) {
        out += to_string(slides[s].a);
        if (slides[s].b != -1) {
            out += ' ';
            out += to_string(slides[s].b);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
