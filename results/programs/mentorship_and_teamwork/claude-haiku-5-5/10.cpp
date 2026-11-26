#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, P;
    cin >> C >> P;

    unordered_map<string, int> sid;
    auto getId = [&](const string& s) -> int {
        auto it = sid.find(s);
        if (it != sid.end()) return it->second;
        int id = (int)sid.size();
        sid.emplace(s, id);
        return id;
    };

    vector<string> cname(C);
    vector<vector<pair<int, int>>> sk(C);
    for (int c = 0; c < C; c++) {
        int N;
        cin >> cname[c] >> N;
        for (int j = 0; j < N; j++) {
            string s;
            int l;
            cin >> s >> l;
            sk[c].push_back({getId(s), l});
        }
        sort(sk[c].begin(), sk[c].end());
    }

    struct Proj {
        string name;
        int D, S, B;
        vector<pair<int, int>> roles;
    };
    vector<Proj> projs(P);
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

    // Priority: score per day, descending; tie: earlier best-before.
    stable_sort(projs.begin(), projs.end(), [](const Proj& a, const Proj& b) {
        double ra = (double)a.S / a.D, rb = (double)b.S / b.D;
        if (ra != rb) return ra > rb;
        return a.B < b.B;
    });

    int NS = (int)sid.size();
    vector<set<pair<int, int>>> idx(NS);  // (level, contributor) for free contributors, level>=1
    set<int> freeSet;                      // free contributors

    auto getLevel = [&](int c, int s) -> int {
        const auto& v = sk[c];
        auto it = lower_bound(v.begin(), v.end(), make_pair(s, INT_MIN));
        if (it != v.end() && it->first == s) return it->second;
        return 0;
    };
    auto setLevel = [&](int c, int s, int lv) {
        auto& v = sk[c];
        auto it = lower_bound(v.begin(), v.end(), make_pair(s, INT_MIN));
        if (it != v.end() && it->first == s) it->second = lv;
        else v.insert(it, {s, lv});
    };
    auto addFree = [&](int c) {
        for (auto& [s, l] : sk[c])
            if (l >= 1) idx[s].insert({l, c});
        freeSet.insert(c);
    };
    auto removeFree = [&](int c) {
        for (auto& [s, l] : sk[c])
            if (l >= 1) idx[s].erase({l, c});
        freeSet.erase(c);
    };

    for (int c = 0; c < C; c++) addFree(c);

    // Remaining projects by rank in sorted order.
    set<int> rem;
    for (int p = 0; p < P; p++) rem.insert(p);

    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> events;

    // Attempt to staff project p with currently free contributors.
    auto attempt = [&](const Proj& p, vector<int>& asg) -> bool {
        int R = (int)p.roles.size();
        asg.assign(R, -1);
        vector<int> chosen;
        int filled = 0;
        auto isChosen = [&](int c) {
            return find(chosen.begin(), chosen.end(), c) != chosen.end();
        };
        while (filled < R) {
            bool prog = false;
            // Fully qualified assignments first.
            for (int r = 0; r < R; r++) {
                if (asg[r] >= 0) continue;
                int s = p.roles[r].first, L = p.roles[r].second;
                auto& st = idx[s];
                for (auto it = st.lower_bound({L, INT_MIN}); it != st.end(); ++it) {
                    if (!isChosen(it->second)) {
                        asg[r] = it->second;
                        chosen.push_back(it->second);
                        filled++;
                        prog = true;
                        break;
                    }
                }
            }
            if (prog) continue;
            // Mentored assignments (level exactly one below).
            for (int r = 0; r < R && !prog; r++) {
                if (asg[r] >= 0) continue;
                int s = p.roles[r].first, L = p.roles[r].second;
                bool mentorOk = false;
                for (int c : chosen)
                    if (getLevel(c, s) >= L) { mentorOk = true; break; }
                if (!mentorOk) continue;
                int pick = -1;
                if (L >= 2) {
                    auto& st = idx[s];
                    auto it = st.lower_bound({L - 1, INT_MIN});
                    auto e = st.lower_bound({L, INT_MIN});
                    for (; it != e; ++it)
                        if (!isChosen(it->second)) { pick = it->second; break; }
                } else {
                    for (int c : freeSet) {
                        if (isChosen(c)) continue;
                        if (getLevel(c, s) == 0) { pick = c; break; }
                    }
                }
                if (pick >= 0) {
                    asg[r] = pick;
                    chosen.push_back(pick);
                    filled++;
                    prog = true;
                }
            }
            if (!prog) return false;
        }
        return true;
    };

    vector<pair<string, vector<string>>> executed;

    auto tryStart = [&](long long t) {
        const int CAP = 400;
        int attempts = 0;
        vector<int> asg;
        for (auto it = rem.begin(); it != rem.end() && attempts < CAP;) {
            const Proj& p = projs[*it];
            long long late = max(0LL, t + p.D - p.B);
            long long v = p.S - late;
            if (v <= 0) {
                it = rem.erase(it);
                continue;
            }
            if ((int)freeSet.size() < (int)p.roles.size()) {
                ++it;
                continue;
            }
            attempts++;
            if (attempt(p, asg)) {
                vector<string> names;
                int R = (int)p.roles.size();
                for (int r = 0; r < R; r++) removeFree(asg[r]);
                for (int r = 0; r < R; r++) {
                    int c = asg[r];
                    int s = p.roles[r].first, L = p.roles[r].second;
                    int cur = getLevel(c, s);
                    if (cur <= L) setLevel(c, s, cur + 1);
                }
                for (int r = 0; r < R; r++) {
                    events.push({t + p.D, asg[r]});
                    names.push_back(cname[asg[r]]);
                }
                executed.push_back({p.name, names});
                it = rem.erase(it);
            } else {
                ++it;
            }
        }
    };

    long long t = 0;
    while (true) {
        tryStart(t);
        if (events.empty()) break;
        t = events.top().first;
        while (!events.empty() && events.top().first == t) {
            addFree(events.top().second);
            events.pop();
        }
    }

    // Output
    cout << executed.size() << "\n";
    for (auto& [name, names] : executed) {
        cout << name << "\n";
        for (size_t i = 0; i < names.size(); i++) {
            if (i) cout << ' ';
            cout << names[i];
        }
        cout << "\n";
    }
    return 0;
}
