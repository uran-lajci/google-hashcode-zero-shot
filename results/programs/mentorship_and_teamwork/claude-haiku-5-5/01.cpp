#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int C, P;
vector<string> names;
vector<vector<pair<int,int>>> sk; // sorted by skill id: (skillId, level)
struct Proj {
    string name;
    int D;
    ll S;
    int B;
    vector<pair<int,int>> roles; // (skillId, requiredLevel)
};
vector<Proj> projs;
vector<char> inAvail;
vector<int> usedStamp;
int stampCounter = 0;

int getLevel(int c, int s) {
    auto it = lower_bound(sk[c].begin(), sk[c].end(), make_pair(s, INT_MIN));
    if (it != sk[c].end() && it->first == s) return it->second;
    return 0;
}

void incLevel(int c, int s) {
    auto it = lower_bound(sk[c].begin(), sk[c].end(), make_pair(s, INT_MIN));
    if (it != sk[c].end() && it->first == s) {
        it->second++;
    } else {
        sk[c].insert(it, make_pair(s, 1));
    }
}

// Tries to staff project p using contributors currently available.
bool staff(const Proj& pr, const vector<int>& avail, vector<int>& assign) {
    int R = pr.roles.size();
    assign.assign(R, -1);
    stampCounter++;
    vector<int> idx(R);
    for (int i = 0; i < R; i++) idx[i] = i;
    sort(idx.begin(), idx.end(), [&](int a, int b) {
        return pr.roles[a].second > pr.roles[b].second;
    });
    vector<int> pending;
    // Pass 1: direct fits, best fit (lowest sufficient level)
    for (int r : idx) {
        int s = pr.roles[r].first, L = pr.roles[r].second;
        int best = -1, bl = INT_MAX;
        for (int c : avail) {
            if (!inAvail[c] || usedStamp[c] == stampCounter) continue;
            int lv = getLevel(c, s);
            if (lv >= L && lv < bl) {
                bl = lv;
                best = c;
                if (lv == L) break;
            }
        }
        if (best == -1) pending.push_back(r);
        else {
            assign[r] = best;
            usedStamp[best] = stampCounter;
        }
    }
    // Pass 2: mentored roles (level exactly one below, with a direct mentor)
    for (int r : pending) {
        int s = pr.roles[r].first, L = pr.roles[r].second;
        bool hasMentor = false;
        for (int q = 0; q < R; q++) {
            if (assign[q] != -1 && getLevel(assign[q], s) >= L) {
                hasMentor = true;
                break;
            }
        }
        if (!hasMentor) return false;
        int pick = -1;
        for (int c : avail) {
            if (!inAvail[c] || usedStamp[c] == stampCounter) continue;
            if (getLevel(c, s) == L - 1) {
                pick = c;
                break;
            }
        }
        if (pick == -1) return false;
        assign[r] = pick;
        usedStamp[pick] = stampCounter;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto startTime = chrono::steady_clock::now();

    cin >> C >> P;
    unordered_map<string,int> sid;
    sid.reserve(1 << 16);
    auto getId = [&](const string& s) -> int {
        auto it = sid.find(s);
        if (it != sid.end()) return it->second;
        int id = (int)sid.size();
        sid.emplace(s, id);
        return id;
    };

    names.resize(C);
    sk.assign(C, {});
    for (int c = 0; c < C; c++) {
        int n;
        cin >> names[c] >> n;
        sk[c].resize(n);
        for (int j = 0; j < n; j++) {
            string s;
            int l;
            cin >> s >> l;
            sk[c][j] = {getId(s), l};
        }
        sort(sk[c].begin(), sk[c].end());
    }

    projs.resize(P);
    for (int p = 0; p < P; p++) {
        int R;
        cin >> projs[p].name >> projs[p].D >> projs[p].S >> projs[p].B >> R;
        projs[p].roles.resize(R);
        for (int r = 0; r < R; r++) {
            string s;
            int l;
            cin >> s >> l;
            projs[p].roles[r] = {getId(s), l};
        }
    }

    // Project priority: score per day, descending
    vector<int> ord(P);
    for (int i = 0; i < P; i++) ord[i] = i;
    sort(ord.begin(), ord.end(), [&](int a, int b) {
        ll lhs = projs[a].S * (ll)projs[b].D;
        ll rhs = projs[b].S * (ll)projs[a].D;
        if (lhs != rhs) return lhs > rhs;
        return projs[a].B < projs[b].B;
    });

    inAvail.assign(C, 1);
    usedStamp.assign(C, 0);
    vector<int> avail(C);
    for (int c = 0; c < C; c++) avail[c] = c;
    vector<char> planned(P, 0);

    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> heap;
    vector<pair<int, vector<int>>> result; // project index, assignment
    ll T = 0;
    bool timeout = false;

    while (true) {
        // Remove busy contributors from avail
        size_t w = 0;
        for (int c : avail) if (inAvail[c]) avail[w++] = c;
        avail.resize(w);
        // Release contributors whose work finished
        while (!heap.empty() && heap.top().first <= T) {
            int c = heap.top().second;
            heap.pop();
            if (!inAvail[c]) {
                inAvail[c] = 1;
                avail.push_back(c);
            }
        }
        ll availCount = avail.size();

        bool committed = false;
        for (int p : ord) {
            if (planned[p]) continue;
            auto now = chrono::steady_clock::now();
            if (chrono::duration<double>(now - startTime).count() > 8.5) {
                timeout = true;
                break;
            }
            const Proj& pr = projs[p];
            ll late = max(0LL, T + pr.D - pr.B);
            if (pr.S - late <= 0) continue;
            if (availCount < (ll)pr.roles.size()) continue;

            vector<int> assign;
            if (staff(pr, avail, assign)) {
                // Commit: learning and busy marking
                for (size_t r = 0; r < pr.roles.size(); r++) {
                    int c = assign[r];
                    int s = pr.roles[r].first, L = pr.roles[r].second;
                    if (getLevel(c, s) <= L) incLevel(c, s);
                    inAvail[c] = 0;
                    availCount--;
                    heap.push({T + pr.D, c});
                }
                planned[p] = 1;
                result.push_back({p, assign});
                committed = true;
            }
        }
        if (timeout) break;
        if (heap.empty()) break;
        T = heap.top().first;
        (void)committed;
    }

    // Output
    string out;
    out += to_string(result.size()) + "\n";
    for (auto& [p, assign] : result) {
        out += projs[p].name;
        out += "\n";
        for (size_t r = 0; r < assign.size(); r++) {
            if (r) out += " ";
            out += names[assign[r]];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
