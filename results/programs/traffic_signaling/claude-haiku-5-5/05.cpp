#include <bits/stdc++.h>
using namespace std;

int D, I, S, V;
long long F;
vector<int> Bs, Es, Ls;
vector<string> names;
vector<vector<int>> paths;
vector<long long> gStart, gLen, gCyc;

inline bool isGreen(int s, int t) {
    if (gLen[s] == 0) return false;
    long long off = t % gCyc[s];
    return off >= gStart[s] && off < gStart[s] + gLen[s];
}

long long simulate() {
    vector<deque<int>> q(S);
    vector<char> inAct(S, 0);
    vector<int> active;
    vector<int> pos(V, 0);
    vector<vector<int>> arr(D + 1);
    for (int c = 0; c < V; c++) {
        int s = paths[c][0];
        q[s].push_back(c);
        if (!inAct[s]) { inAct[s] = 1; active.push_back(s); }
    }
    long long score = 0;
    vector<int> nxt;
    for (int t = 0; t < D; t++) {
        for (int c : arr[t]) {
            int s = paths[c][pos[c]];
            q[s].push_back(c);
            if (!inAct[s]) { inAct[s] = 1; active.push_back(s); }
        }
        arr[t].clear();
        nxt.clear();
        for (int s : active) {
            if (isGreen(s, t)) {
                int c = q[s].front();
                q[s].pop_front();
                int k = pos[c];
                int ns = paths[c][k + 1];
                if (k + 1 == (int)paths[c].size() - 1) {
                    long long fin = (long long)t + Ls[ns];
                    if (fin <= D) score += F + (D - fin);
                } else {
                    pos[c] = k + 1;
                    long long at = (long long)t + Ls[ns];
                    if (at <= D) arr[at].push_back(c);
                }
            }
            if (!q[s].empty()) nxt.push_back(s);
            else inAct[s] = 0;
        }
        active.swap(nxt);
    }
    return score;
}

void applySchedule(const vector<vector<int>>& ord, const vector<int>& dur) {
    fill(gLen.begin(), gLen.end(), 0LL);
    for (int i = 0; i < I; i++) {
        long long cyc = 0;
        for (int s : ord[i]) {
            gStart[s] = cyc;
            gLen[s] = dur[s];
            cyc += dur[s];
        }
        for (int s : ord[i]) gCyc[s] = cyc;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> D >> I >> S >> V >> F;
    Bs.assign(S, 0); Es.assign(S, 0); Ls.assign(S, 0); names.assign(S, "");
    unordered_map<string, int> id;
    id.reserve(S * 2);
    for (int s = 0; s < S; s++) {
        cin >> Bs[s] >> Es[s] >> names[s] >> Ls[s];
        id[names[s]] = s;
    }
    paths.assign(V, {});
    for (int c = 0; c < V; c++) {
        int P;
        cin >> P;
        paths[c].resize(P);
        for (int k = 0; k < P; k++) {
            string nm;
            cin >> nm;
            paths[c][k] = id[nm];
        }
    }

    // demand and earliest arrival key per street
    const long long INF = LLONG_MAX / 4;
    vector<long long> firstArr(S, INF);
    vector<int> demand(S, 0);
    for (int c = 0; c < V; c++) {
        int P = paths[c].size();
        long long tm = 0;
        for (int k = 0; k <= P - 2; k++) {
            if (k > 0) tm += Ls[paths[c][k]];
            int s = paths[c][k];
            demand[s]++;
            firstArr[s] = min(firstArr[s], tm);
        }
    }
    vector<int> cand;
    for (int s = 0; s < S; s++) if (demand[s] > 0) cand.push_back(s);

    gStart.assign(S, 0);
    gLen.assign(S, 0);
    gCyc.assign(S, 1);

    auto makeOrd = [&](const vector<long long>& keyv) {
        vector<vector<int>> ord(I);
        for (int s : cand) ord[Es[s]].push_back(s);
        for (int i = 0; i < I; i++) {
            sort(ord[i].begin(), ord[i].end(), [&](int a, int b) {
                if (keyv[a] != keyv[b]) return keyv[a] < keyv[b];
                return a < b;
            });
        }
        return ord;
    };
    auto makeDur = [&](double alpha) {
        vector<int> dur(S, 1);
        for (int s = 0; s < S; s++) {
            if (demand[s] == 0) continue;
            long long d = (long long)ceil(demand[s] * alpha);
            if (d < 1) d = 1;
            if (d > D) d = D;
            dur[s] = (int)d;
        }
        return dur;
    };

    vector<long long> keyFA(S), keyDem(S);
    for (int s = 0; s < S; s++) {
        keyFA[s] = firstArr[s];
        keyDem[s] = -(long long)demand[s];
    }

    vector<vector<int>> bestOrd;
    vector<int> bestDur;
    long long bestScore = -1;

    struct Variant { int keyType; double alpha; };
    vector<Variant> variants = {
        {0, 1.0}, {0, 0.5}, {0, 2.0}, {0, 0.0}, {1, 1.0}, {1, 0.0}
    };
    for (auto& vr : variants) {
        vector<vector<int>> ord = makeOrd(vr.keyType == 0 ? keyFA : keyDem);
        vector<int> dur = makeDur(vr.alpha);
        applySchedule(ord, dur);
        long long sc = simulate();
        if (sc > bestScore) {
            bestScore = sc;
            bestOrd = ord;
            bestDur = dur;
        }
    }

    // Deterministic local search with a fixed iteration budget
    {
        mt19937 rng(12345);
        vector<vector<int>> curOrd = bestOrd;
        vector<int> curDur = bestDur;
        long long curScore = bestScore;
        vector<int> multi;
        for (int i = 0; i < I; i++) if (curOrd[i].size() >= 2) multi.push_back(i);
        const int ITER = 80;
        for (int it = 0; it < ITER; it++) {
            vector<vector<int>> ord = curOrd;
            vector<int> dur = curDur;
            bool doSwap = (rng() % 2 == 0);
            if (doSwap && !multi.empty()) {
                int i = multi[rng() % multi.size()];
                int j = rng() % (ord[i].size() - 1);
                swap(ord[i][j], ord[i][j + 1]);
            } else {
                if (cand.empty()) break;
                int s = cand[rng() % cand.size()];
                int delta = (rng() % 2 == 0) ? 1 : -1;
                long long nd = (long long)dur[s] + delta;
                if (nd < 1) nd = 1;
                if (nd > D) nd = D;
                dur[s] = (int)nd;
            }
            applySchedule(ord, dur);
            long long sc = simulate();
            if (sc >= curScore) {
                curScore = sc;
                curOrd = ord;
                curDur = dur;
            }
        }
        if (curScore > bestScore) {
            bestScore = curScore;
            bestOrd = curOrd;
            bestDur = curDur;
        }
    }

    // Output
    vector<int> listed;
    for (int i = 0; i < I; i++) if (!bestOrd[i].empty()) listed.push_back(i);
    string out;
    out += to_string(listed.size()) + "\n";
    for (int i : listed) {
        out += to_string(i) + "\n";
        out += to_string(bestOrd[i].size()) + "\n";
        for (int s : bestOrd[i]) {
            out += names[s];
            out += ' ';
            out += to_string(bestDur[s]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
