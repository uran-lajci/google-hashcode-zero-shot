#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int C, P;
vector<string> cname;
vector<map<int,int>> lv;
vector<set<pair<int,int>>> bySkill;
vector<ll> freeAt;
vector<int> usedStamp;
int stampCnt = 0;
int zeroCursor = 0;

struct Project {
    string name;
    ll D, S, B;
    vector<pair<int,int>> roles; // skill id, required level
};
vector<Project> projects;

int getLevel(int c, int s) {
    auto it = lv[c].find(s);
    return it == lv[c].end() ? 0 : it->second;
}

void setLevel(int c, int s, int v) {
    int old = getLevel(c, s);
    if (old >= 1) bySkill[s].erase({old, c});
    lv[c][s] = v;
    if (v >= 1) bySkill[s].insert({v, c});
}

bool evalTeam(int p, ll t, vector<int>& team) {
    const Project& pr = projects[p];
    int R = pr.roles.size();
    team.assign(R, -1);
    stampCnt++;
    vector<int> order(R);
    for (int i = 0; i < R; i++) order[i] = i;
    stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return pr.roles[a].second > pr.roles[b].second;
    });

    // Pass 1: direct fills (level >= required), choose lowest sufficient level
    for (int r : order) {
        int s = pr.roles[r].first, L = pr.roles[r].second;
        auto& st = bySkill[s];
        auto it = st.lower_bound({L, -1});
        int steps = 0;
        for (; it != st.end() && steps < 100; ++it, ++steps) {
            int c = it->second;
            if (freeAt[c] <= t && usedStamp[c] != stampCnt) {
                team[r] = c;
                usedStamp[c] = stampCnt;
                break;
            }
        }
    }

    // Pass 2: mentees (level exactly required-1)
    for (int r : order) {
        if (team[r] >= 0) continue;
        int s = pr.roles[r].first, L = pr.roles[r].second;
        if (L >= 2) {
            auto& st = bySkill[s];
            auto it = st.lower_bound({L - 1, -1});
            auto endIt = st.lower_bound({L, -1});
            int steps = 0;
            for (; it != endIt && steps < 100; ++it, ++steps) {
                int c = it->second;
                if (freeAt[c] <= t && usedStamp[c] != stampCnt) {
                    team[r] = c;
                    usedStamp[c] = stampCnt;
                    break;
                }
            }
        } else {
            // L == 1: need contributor with level 0 in s
            int steps = 0;
            for (; steps < 300 && team[r] < 0; steps++) {
                int c = (zeroCursor + steps) % C;
                if (freeAt[c] <= t && usedStamp[c] != stampCnt && getLevel(c, s) == 0) {
                    team[r] = c;
                    usedStamp[c] = stampCnt;
                }
            }
            zeroCursor = (zeroCursor + steps) % C;
        }
        if (team[r] < 0) return false;
    }

    // Validate mentorship: every role not at required level needs a mentor in team
    for (int r = 0; r < R; r++) {
        int s = pr.roles[r].first, L = pr.roles[r].second;
        if (getLevel(team[r], s) >= L) continue;
        bool found = false;
        for (int r2 = 0; r2 < R && !found; r2++) {
            if (r2 == r) continue;
            if (getLevel(team[r2], s) >= L) found = true;
        }
        if (!found) return false;
    }
    return true;
}

ll valueAt(int p, ll t) {
    const Project& pr = projects[p];
    ll late = max(0LL, t + pr.D - pr.B);
    return max(0LL, pr.S - late);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> C >> P;
    unordered_map<string,int> skillId;
    auto gid = [&](const string& s) {
        auto it = skillId.find(s);
        if (it != skillId.end()) return it->second;
        int id = skillId.size();
        skillId[s] = id;
        return id;
    };

    cname.resize(C);
    lv.resize(C);
    for (int c = 0; c < C; c++) {
        int N;
        cin >> cname[c] >> N;
        for (int i = 0; i < N; i++) {
            string s; int l;
            cin >> s >> l;
            int id = gid(s);
            lv[c][id] = l;
        }
    }

    projects.resize(P);
    for (int p = 0; p < P; p++) {
        int R;
        cin >> projects[p].name >> projects[p].D >> projects[p].S >> projects[p].B >> R;
        projects[p].roles.resize(R);
        for (int r = 0; r < R; r++) {
            string s; int l;
            cin >> s >> l;
            projects[p].roles[r] = {gid(s), l};
        }
    }

    int numSkills = skillId.size();
    bySkill.assign(numSkills, {});
    for (int c = 0; c < C; c++)
        for (auto& kv : lv[c])
            if (kv.second >= 1) bySkill[kv.first].insert({kv.second, c});

    freeAt.assign(C, 0);
    usedStamp.assign(C, -1);

    // Candidate order: by score per day, descending
    vector<int> cand(P);
    for (int i = 0; i < P; i++) cand[i] = i;
    sort(cand.begin(), cand.end(), [&](int a, int b) {
        double ra = (double)projects[a].S / projects[a].D;
        double rb = (double)projects[b].S / projects[b].D;
        if (ra != rb) return ra > rb;
        return a < b;
    });

    vector<bool> started(P, false), dead(P, false);
    priority_queue<ll, vector<ll>, greater<ll>> ev;
    ll t = 0;
    size_t head = 0;
    vector<pair<int, vector<int>>> result;

    while (true) {
        while (true) {
            int bestP = -1;
            double bestR = -1.0;
            vector<int> bestTeam;
            int attempts = 0, scanned = 0;

            while (head < cand.size() && (started[cand[head]] || dead[cand[head]])) head++;

            for (size_t i = head; i < cand.size() && scanned < 1000 && attempts < 40; ++i, ++scanned) {
                int p = cand[i];
                if (started[p] || dead[p]) continue;
                const Project& pr = projects[p];
                double upper = (double)pr.S / pr.D;
                if (bestP >= 0 && upper <= bestR) break;
                ll v = valueAt(p, t);
                if (v <= 0) { dead[p] = true; continue; }
                double ratio = (double)v / pr.D;
                if (bestP >= 0 && ratio <= bestR) continue;
                attempts++;
                vector<int> team;
                if (evalTeam(p, t, team)) {
                    bestP = p;
                    bestR = ratio;
                    bestTeam = team;
                }
            }

            if (bestP < 0) break;

            // Commit the project
            const Project& pr = projects[bestP];
            started[bestP] = true;
            ll endT = t + pr.D;
            for (size_t r = 0; r < pr.roles.size(); r++) {
                int c = bestTeam[r];
                int s = pr.roles[r].first, L = pr.roles[r].second;
                int cur = getLevel(c, s);
                if (cur <= L) setLevel(c, s, cur + 1);
                freeAt[c] = endT;
                ev.push(endT);
            }
            result.push_back({bestP, bestTeam});
        }

        while (!ev.empty() && ev.top() <= t) ev.pop();
        if (ev.empty()) break;
        t = ev.top();
    }

    // Output
    cout << result.size() << "\n";
    for (auto& pr : result) {
        cout << projects[pr.first].name << "\n";
        for (size_t r = 0; r < pr.second.size(); r++) {
            if (r) cout << " ";
            cout << cname[pr.second[r]];
        }
        cout << "\n";
    }
    return 0;
}
