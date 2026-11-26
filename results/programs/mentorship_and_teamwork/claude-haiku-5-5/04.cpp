#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
using Clock = chrono::steady_clock;

int C, P;
vector<string> cname;
vector<vector<pair<int,int>>> cskill0;   // (skillId, level>=1)
struct Proj {
    string name;
    int D;
    ll S, B;
    vector<pair<int,int>> roles;          // (skillId, requiredLevel)
};
vector<Proj> pr;
unordered_map<string,int> skillId;
int numSkills = 0;

int getSkill(const string& s) {
    auto it = skillId.find(s);
    if (it != skillId.end()) return it->second;
    int id = (int)skillId.size();
    skillId[s] = id;
    return id;
}

struct Run {
    ll score = 0;
    vector<pair<int, vector<int>>> plan;  // (project index, contributor per role)
};

Run simulate(const vector<int>& order, Clock::time_point dl) {
    const int K = 100;
    vector<map<int,int>> lev(C);          // only levels >= 1 stored
    vector<ll> avail(C, 0);
    vector<set<pair<ll,int>>> bySkill(numSkills);
    set<pair<ll,int>> all;
    for (int c = 0; c < C; c++) {
        all.insert({0LL, c});
        for (auto& [s, l] : cskill0[c]) {
            lev[c][s] = l;
            bySkill[s].insert({0LL, c});
        }
    }
    auto getLev = [&](int c, int s) -> int {
        auto it = lev[c].find(s);
        return it == lev[c].end() ? 0 : it->second;
    };

    vector<int> mark(C, -1);
    int stamp = 0;
    Run res;

    for (int pi : order) {
        if (Clock::now() > dl) break;
        const Proj& p = pr[pi];
        int R = (int)p.roles.size();
        vector<int> who(R, -1);
        vector<char> ment(R, 0);
        stamp++;

        // Pass 1: direct roles (contributor already has the skill at required level or higher)
        for (int r = 0; r < R; r++) {
            int s = p.roles[r].first, L = p.roles[r].second;
            auto& st = bySkill[s];
            int cnt = 0;
            for (auto it = st.begin(); it != st.end() && cnt < K; ++it, ++cnt) {
                int c = it->second;
                if (mark[c] == stamp) continue;
                if (getLev(c, s) >= L) {
                    who[r] = c;
                    mark[c] = stamp;
                    break;
                }
            }
        }

        // Pass 2: mentored roles (contributor at exactly required-1)
        bool ok = true;
        for (int r = 0; r < R && ok; r++) {
            if (who[r] != -1) continue;
            int s = p.roles[r].first, L = p.roles[r].second;
            ment[r] = 1;
            int cnt = 0;
            if (L - 1 >= 1) {
                auto& st = bySkill[s];
                for (auto it = st.begin(); it != st.end() && cnt < K; ++it, ++cnt) {
                    int c = it->second;
                    if (mark[c] == stamp) continue;
                    if (getLev(c, s) == L - 1) {
                        who[r] = c;
                        mark[c] = stamp;
                        break;
                    }
                }
            } else {
                for (auto it = all.begin(); it != all.end() && cnt < K; ++it, ++cnt) {
                    int c = it->second;
                    if (mark[c] == stamp) continue;
                    if (getLev(c, s) == 0) {
                        who[r] = c;
                        mark[c] = stamp;
                        break;
                    }
                }
            }
            if (who[r] == -1) ok = false;
        }
        if (!ok) continue;

        // Validate mentorship: another assigned contributor must have the skill at >= required level
        for (int r = 0; r < R && ok; r++) {
            if (!ment[r]) continue;
            int s = p.roles[r].first, L = p.roles[r].second;
            bool f = false;
            for (int r2 = 0; r2 < R; r2++) {
                if (r2 == r) continue;
                if (getLev(who[r2], s) >= L) { f = true; break; }
            }
            if (!f) ok = false;
        }
        if (!ok) continue;

        ll start = 0;
        for (int r = 0; r < R; r++) start = max(start, avail[who[r]]);
        ll late = max(0LL, start + p.D - p.B);
        ll sc = max(0LL, p.S - late);
        if (sc <= 0) continue;

        ll end = start + p.D;
        // Remove members from index structures
        for (int r = 0; r < R; r++) {
            int c = who[r];
            for (auto& [s, l] : lev[c]) bySkill[s].erase({avail[c], c});
            all.erase({avail[c], c});
        }
        // Learning: role level >= current level -> improve by one
        for (int r = 0; r < R; r++) {
            int c = who[r];
            int s = p.roles[r].first, L = p.roles[r].second;
            int l = getLev(c, s);
            if (L >= l) lev[c][s] = l + 1;
        }
        // Reinsert with new availability
        for (int r = 0; r < R; r++) {
            int c = who[r];
            avail[c] = end;
            for (auto& [s, l] : lev[c]) bySkill[s].insert({end, c});
            all.insert({end, c});
        }
        res.score += sc;
        res.plan.push_back({pi, who});
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto t0 = Clock::now();
    auto dl = t0 + chrono::milliseconds(8000);

    cin >> C >> P;
    cname.resize(C);
    cskill0.resize(C);
    for (int c = 0; c < C; c++) {
        int N;
        cin >> cname[c] >> N;
        for (int i = 0; i < N; i++) {
            string s;
            int l;
            cin >> s >> l;
            cskill0[c].push_back({getSkill(s), l});
        }
    }
    pr.resize(P);
    for (int i = 0; i < P; i++) {
        Proj& p = pr[i];
        int R;
        cin >> p.name >> p.D >> p.S >> p.B >> R;
        p.roles.resize(R);
        for (int r = 0; r < R; r++) {
            string s;
            int l;
            cin >> s >> l;
            p.roles[r] = {getSkill(s), l};
        }
    }
    numSkills = (int)skillId.size();

    vector<vector<int>> orders;
    {
        vector<int> o(P);
        iota(o.begin(), o.end(), 0);
        sort(o.begin(), o.end(), [&](int a, int b) {
            if (pr[a].S != pr[b].S) return pr[a].S > pr[b].S;
            return pr[a].D < pr[b].D;
        });
        orders.push_back(o);
    }
    {
        vector<int> o(P);
        iota(o.begin(), o.end(), 0);
        sort(o.begin(), o.end(), [&](int a, int b) {
            ll lhs = pr[a].S * pr[b].D, rhs = pr[b].S * pr[a].D;
            if (lhs != rhs) return lhs > rhs;
            return pr[a].D < pr[b].D;
        });
        orders.push_back(o);
    }
    {
        vector<int> o(P);
        iota(o.begin(), o.end(), 0);
        sort(o.begin(), o.end(), [&](int a, int b) {
            if (pr[a].B != pr[b].B) return pr[a].B < pr[b].B;
            return pr[a].S > pr[b].S;
        });
        orders.push_back(o);
    }

    Run best;
    bool have = false;
    for (auto& o : orders) {
        if (Clock::now() > dl) break;
        Run r = simulate(o, dl);
        if (!have || r.score > best.score) {
            best = move(r);
            have = true;
        }
    }

    string out;
    out += to_string(best.plan.size()) + "\n";
    for (auto& [pi, who] : best.plan) {
        out += pr[pi].name;
        out += "\n";
        for (size_t r = 0; r < who.size(); r++) {
            if (r) out += " ";
            out += cname[who[r]];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
