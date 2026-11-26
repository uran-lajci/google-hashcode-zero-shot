#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Ev {
    ll end;
    int e;
    int kind; // 0 = impl, 1 = move
    int f, j, k, s;
};

int main() {
    ll L;
    int G, S, B, F, N;
    cin >> L >> G >> S >> B >> F >> N;

    vector<string> svcName(S);
    vector<int> svcBin(S);
    unordered_map<string, int> sid;
    for (int s = 0; s < S; s++) {
        cin >> svcName[s] >> svcBin[s];
        sid[svcName[s]] = s;
    }
    vector<int> R(B, 0);
    for (int s = 0; s < S; s++) R[svcBin[s]]++;

    vector<string> fName(F);
    vector<int> M(F), D(F);
    vector<ll> U(F);
    vector<vector<int>> fs(F);
    vector<vector<char>> impl(F);
    vector<int> rem(F);
    vector<char> launched(F, 0);
    vector<vector<pair<int, int>>> svcFeat(S);
    for (int f = 0; f < F; f++) {
        cin >> fName[f] >> M[f] >> D[f] >> U[f];
        fs[f].resize(M[f]);
        for (int p = 0; p < M[f]; p++) {
            string nm;
            cin >> nm;
            fs[f][p] = sid[nm];
            svcFeat[fs[f][p]].push_back({f, p});
        }
        impl[f].assign(M[f], 0);
        rem[f] = M[f];
    }

    vector<int> C(B, 0);
    vector<char> moveActive(B, 0);
    unordered_set<ll> inProg;
    vector<vector<string>> tasks;
    vector<ll> freeSince;
    priority_queue<int, vector<int>, greater<int>> pool;
    auto cmp = [](const Ev& a, const Ev& b) {
        if (a.end != b.end) return a.end > b.end;
        return a.e > b.e;
    };
    priority_queue<Ev, vector<Ev>, decltype(cmp)> ev(cmp);
    int created = 0;

    auto takeEng = [&]() -> int {
        if (!pool.empty()) {
            int e = pool.top();
            pool.pop();
            return e;
        }
        if (created < G) {
            tasks.push_back({});
            freeSince.push_back(0);
            return created++;
        }
        return -1;
    };
    auto addTask = [&](int e, ll t, const string& line, ll dur) {
        if (t > freeSince[e]) tasks[e].push_back("wait " + to_string(t - freeSince[e]));
        tasks[e].push_back(line);
        freeSince[e] = t + dur;
    };
    auto distinctCount = [&](int f, int ovS, int newBin) -> int {
        vector<int> v;
        for (int p = 0; p < M[f]; p++) {
            if (impl[f][p]) continue;
            int s = fs[f][p];
            int b = (s == ovS) ? newBin : svcBin[s];
            v.push_back(b);
        }
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
        return (int)v.size();
    };
    auto moveOK = [&](int s, int selfF, int k) -> bool {
        for (auto& pr : svcFeat[s]) {
            int f = pr.first, pos = pr.second;
            if (f == selfF || launched[f] || impl[f][pos]) continue;
            int before = distinctCount(f, -1, -1);
            int after = distinctCount(f, s, k);
            if (after > before) return false;
        }
        return true;
    };
    auto process = [&](const Ev& x) {
        if (x.kind == 0) {
            int f = x.f, j = x.j;
            C[j]--;
            inProg.erase((ll)f * B + j);
            for (int p = 0; p < M[f]; p++) {
                if (!impl[f][p] && svcBin[fs[f][p]] == j) {
                    impl[f][p] = 1;
                    rem[f]--;
                }
            }
            if (rem[f] == 0) launched[f] = 1;
        } else {
            moveActive[x.j] = 0;
            moveActive[x.k] = 0;
        }
        pool.push(x.e);
    };

    struct Cand {
        double key;
        int type, f, j, k;
    };

    auto batch = [&](ll t) {
        if (pool.empty() && created >= G) return;
        vector<Cand> cs;
        for (int f = 0; f < F; f++) {
            if (launched[f] || rem[f] == 0) continue;
            vector<int> bins;
            for (int p = 0; p < M[f]; p++)
                if (!impl[f][p]) bins.push_back(svcBin[fs[f][p]]);
            sort(bins.begin(), bins.end());
            vector<pair<int, int>> bc;
            for (size_t a = 0; a < bins.size();) {
                size_t b = a;
                while (b < bins.size() && bins[b] == bins[a]) b++;
                bc.push_back({bins[a], (int)(b - a)});
                a = b;
            }
            int m = (int)bc.size();
            for (auto& pr : bc) {
                int j = pr.first;
                double c = (double)D[f] + R[j] + C[j];
                cs.push_back({(double)U[f] / (c * m), 0, f, j, -1});
            }
            if (m >= 2) {
                int kb = -1, kc = -1;
                for (auto& pr : bc) {
                    if (pr.second > kc) {
                        kc = pr.second;
                        kb = pr.first;
                    }
                }
                for (auto& pr : bc) {
                    int j = pr.first, cnt = pr.second;
                    if (j == kb) continue;
                    double tot = (double)cnt * max(R[j], R[kb]);
                    double key = (double)U[f] / ((tot + D[f] + R[kb] + C[kb]) * m);
                    cs.push_back({key, 1, f, j, kb});
                }
            }
        }
        sort(cs.begin(), cs.end(), [](const Cand& a, const Cand& b) {
            if (a.key != b.key) return a.key > b.key;
            if (a.type != b.type) return a.type < b.type;
            if (a.f != b.f) return a.f < b.f;
            return a.j < b.j;
        });

        for (const Cand& c : cs) {
            if (pool.empty() && created >= G) break;
            if (c.type == 0) {
                int f = c.f, j = c.j;
                if (moveActive[j]) continue;
                if (inProg.count((ll)f * B + j)) continue;
                ll dur = (ll)D[f] + R[j] + C[j];
                ll end = t + dur;
                if (end >= L) continue;
                int e = takeEng();
                if (e < 0) break;
                addTask(e, t, "impl " + fName[f] + " " + to_string(j), dur);
                C[j]++;
                inProg.insert((ll)f * B + j);
                ev.push({end, e, 0, f, j, -1, -1});
            } else {
                int f = c.f, j = c.j, k = c.k;
                if (moveActive[j] || moveActive[k] || C[j] || C[k]) continue;
                ll dur = max(R[j], R[k]);
                ll end = t + dur;
                if (end >= L) continue;
                int chosen = -1;
                for (int p = 0; p < M[f]; p++) {
                    if (impl[f][p]) continue;
                    int s = fs[f][p];
                    if (svcBin[s] != j) continue;
                    if (moveOK(s, f, k)) {
                        chosen = s;
                        break;
                    }
                }
                if (chosen < 0) continue;
                int e = takeEng();
                if (e < 0) break;
                int s = chosen;
                addTask(e, t, "move " + svcName[s] + " " + to_string(k), dur);
                R[j]--;
                R[k]++;
                svcBin[s] = k;
                moveActive[j] = 1;
                moveActive[k] = 1;
                ev.push({end, e, 1, f, j, k, s});
            }
        }
    };

    ll t = 0;
    while (true) {
        while (!ev.empty() && ev.top().end <= t) {
            Ev x = ev.top();
            ev.pop();
            process(x);
        }
        if (t < L) batch(t);
        if (ev.empty()) break;
        t = ev.top().end;
        if (t >= L) break;
    }

    string out;
    out += to_string(tasks.size()) + "\n";
    for (auto& tv : tasks) {
        out += to_string(tv.size()) + "\n";
        for (auto& line : tv) {
            out += line;
            out += "\n";
        }
    }
    fputs(out.c_str(), stdout);
    return 0;
}
