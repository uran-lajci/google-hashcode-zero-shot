#include <bits/stdc++.h>
using namespace std;

int C, T, S;
vector<string> nm;
vector<long long> ct, rt;
vector<vector<int>> deps;
vector<char> done_;
vector<long long> fin, glob;
vector<int> srv;
vector<long long> freeT;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto t0 = chrono::steady_clock::now();
    cin >> C >> T >> S;
    nm.resize(C); ct.resize(C); rt.resize(C); deps.resize(C);
    unordered_map<string, int> id;
    id.reserve(C * 2);
    for (int i = 0; i < C; i++) {
        cin >> nm[i] >> ct[i] >> rt[i];
        id[nm[i]] = i;
        int n; cin >> n;
        deps[i].resize(n);
        for (int j = 0; j < n; j++) { string s; cin >> s; deps[i][j] = id[s]; }
    }
    vector<int> tf(T); vector<long long> td(T), tg(T);
    for (int i = 0; i < T; i++) { string s; cin >> s >> td[i] >> tg[i]; tf[i] = id[s]; }
    vector<int> ord(T);
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        if (td[a] != td[b]) return td[a] < td[b];
        return tg[a] > tg[b];
    });
    done_.assign(C, 0); fin.assign(C, 0); glob.assign(C, 0); srv.assign(C, -1);
    freeT.assign(S, 0);
    vector<int> stamp(C, -1);
    vector<pair<int,int>> out; // (file, server)
    vector<int> needed, stk;
    vector<pair<int,long long>> freeLog;
    vector<int> fileLog;
    vector<char> isCand(S, 0);
    vector<int> cands;
    for (int oi = 0; oi < T; oi++) {
        if (chrono::duration<double>(chrono::steady_clock::now() - t0).count() > 8.5) break;
        int ti = ord[oi];
        int tgt = tf[ti];
        long long D = td[ti];
        if (done_[tgt]) continue;
        // collect needed
        needed.clear(); stk.clear();
        stk.push_back(tgt); stamp[tgt] = oi;
        while (!stk.empty()) {
            int f = stk.back(); stk.pop_back();
            needed.push_back(f);
            for (int d : deps[f]) if (!done_[d] && stamp[d] != oi) { stamp[d] = oi; stk.push_back(d); }
        }
        sort(needed.begin(), needed.end());
        freeLog.clear(); fileLog.clear();
        bool fail = false;
        for (int f : needed) {
            long long G = 0;
            cands.clear();
            for (int d : deps[f]) {
                G = max(G, glob[d]);
                int s = srv[d];
                if (!isCand[s]) { isCand[s] = 1; cands.push_back(s); }
            }
            // best non-candidate: min free server
            int bestS = -1; long long bestF = LLONG_MAX;
            {
                int ms = -1; long long mf = LLONG_MAX;
                for (int s = 0; s < S; s++) if (!isCand[s] && freeT[s] < mf) { mf = freeT[s]; ms = s; }
                if (ms >= 0) { bestS = ms; bestF = max(mf, G) + ct[f]; }
            }
            for (int s : cands) {
                long long ready = 0;
                for (int d : deps[f]) {
                    long long a = (srv[d] == s) ? fin[d] : glob[d];
                    if (a > ready) ready = a;
                }
                long long F = max(ready, freeT[s]) + ct[f];
                if (F < bestF) { bestF = F; bestS = s; }
            }
            for (int s : cands) isCand[s] = 0;
            freeLog.push_back({bestS, freeT[bestS]});
            freeT[bestS] = bestF;
            done_[f] = 1; fin[f] = bestF; glob[f] = bestF + rt[f]; srv[f] = bestS;
            fileLog.push_back(f);
            if (bestF > D) { fail = true; break; }
        }
        if (fail) {
            for (int i = (int)freeLog.size() - 1; i >= 0; i--) freeT[freeLog[i].first] = freeLog[i].second;
            for (int f : fileLog) { done_[f] = 0; srv[f] = -1; }
        } else {
            for (int f : fileLog) out.push_back({f, srv[f]});
        }
    }
    string res;
    if (out.empty()) {
        int f = 0;
        for (int i = 0; i < C; i++) if (deps[i].empty()) { f = i; break; }
        res = "1\n" + nm[f] + " 0\n";
    } else {
        res = to_string(out.size()) + "\n";
        for (auto& p : out) { res += nm[p.first]; res += ' '; res += to_string(p.second); res += '\n'; }
    }
    fwrite(res.data(), 1, res.size(), stdout);
    return 0;
}
