#include <bits/stdc++.h>
using namespace std;

int L, G, S, B, F, N;
vector<string> sname;
vector<int> sbin;
unordered_map<string, int> sid;
vector<int> R;
vector<string> fname;
vector<int> fD;
vector<long long> fU;
vector<vector<int>> fBins;

struct Res {
    long long score;
    string out;
};

struct Op {
    int id, t, fin;
    bool idle;
};

Res simulate(const vector<int>& ord) {
    set<pair<int, int>> ms;   // (free time, engineer id) for engineers already created
    int nextId = 0;           // engineers with id < nextId have been created
    vector<vector<pair<int, int>>> iv(B);  // committed job intervals per binary
    vector<vector<string>> eng(G);
    long long score = 0;

    for (int f : ord) {
        vector<Op> ops;
        vector<int> jb, jt, jd;
        bool ok = true;
        int launch = 0;
        for (int b : fBins[f]) {
            int id, t;
            bool idle = false;
            if (nextId < G) {
                id = nextId++;
                t = 0;
                idle = true;
            } else {
                auto it = ms.begin();
                t = it->first;
                id = it->second;
                ms.erase(it);
            }
            int C = 0;
            for (const auto& p : iv[b])
                if (p.first <= t && t < p.second) C++;
            int d = fD[f] + R[b] + C;
            int fin = t + d;
            ops.push_back({id, t, fin, idle});
            ms.insert({fin, id});
            jb.push_back(b);
            jt.push_back(t);
            jd.push_back(d);
            launch = max(launch, fin);
            if (fin >= L) {
                ok = false;
                break;
            }
        }
        if (!ok) {
            for (int k = (int)ops.size() - 1; k >= 0; k--) {
                const Op& op = ops[k];
                ms.erase(ms.find({op.fin, op.id}));
                if (op.idle) {
                    nextId--;
                } else {
                    ms.insert({op.t, op.id});
                }
            }
            continue;
        }
        // commit
        for (size_t k = 0; k < ops.size(); k++) {
            eng[ops[k].id].push_back("impl " + fname[f] + " " + to_string(jb[k]));
            iv[jb[k]].push_back({jt[k], jt[k] + jd[k]});
        }
        score += fU[f] * (long long)(L - launch);
    }

    ostringstream os;
    int cnt = 0;
    for (int i = 0; i < G; i++)
        if (!eng[i].empty()) cnt++;
    os << cnt << "\n";
    for (int i = 0; i < G; i++) {
        if (eng[i].empty()) continue;
        os << eng[i].size() << "\n";
        for (const auto& s : eng[i]) os << s << "\n";
    }
    return {score, os.str()};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> L >> G >> S >> B >> F >> N;
    sname.resize(S);
    sbin.resize(S);
    R.assign(B, 0);
    for (int i = 0; i < S; i++) {
        cin >> sname[i] >> sbin[i];
        sid[sname[i]] = i;
        R[sbin[i]]++;
    }
    fname.resize(F);
    fD.resize(F);
    fU.resize(F);
    fBins.resize(F);
    for (int f = 0; f < F; f++) {
        int M;
        cin >> fname[f] >> M >> fD[f] >> fU[f];
        set<int> bs;
        for (int m = 0; m < M; m++) {
            string s;
            cin >> s;
            bs.insert(sbin[sid[s]]);
        }
        fBins[f] = vector<int>(bs.begin(), bs.end());
    }

    // Compute ordering keys
    vector<double> cost(F, 0.0);
    for (int f = 0; f < F; f++) {
        for (int b : fBins[f]) cost[f] += fD[f] + R[b];
    }

    vector<vector<double>> keys(4, vector<double>(F));
    for (int f = 0; f < F; f++) {
        double U = (double)fU[f];
        double k = (double)fBins[f].size();
        keys[0][f] = U / max(1.0, cost[f]);
        keys[1][f] = U;
        keys[2][f] = U / k;
        keys[3][f] = U / ((double)fD[f] * k);
    }

    Res best{-1, ""};
    for (int o = 0; o < 4; o++) {
        vector<int> ord(F);
        for (int f = 0; f < F; f++) ord[f] = f;
        sort(ord.begin(), ord.end(), [&](int a, int b) {
            if (keys[o][a] != keys[o][b]) return keys[o][a] > keys[o][b];
            return a < b;
        });
        Res r = simulate(ord);
        if (r.score > best.score) best = r;
    }

    if (best.score < 0) {
        cout << 0 << "\n";
    } else {
        cout << best.out;
    }
    return 0;
}
