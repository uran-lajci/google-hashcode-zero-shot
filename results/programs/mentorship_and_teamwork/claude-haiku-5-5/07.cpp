#include <bits/stdc++.h>
using namespace std;

struct Proj {
    string name;
    long long D, S, B;
    vector<pair<int,int>> roles; // (skill id, required level)
};

struct Rec {
    int p;
    long long start;
    vector<int> asg; // contributor per role
};

int C, P;
vector<string> cname;
vector<vector<pair<int,int>>> skills; // sorted by skill id: (skill, level)
vector<Proj> projs;
set<int> freeSet;
vector<int> mark;
int stampCur = 0;
long long work = 0;

int getLevel(int c, int s) {
    auto &v = skills[c];
    auto it = lower_bound(v.begin(), v.end(), make_pair(s, INT_MIN));
    if (it != v.end() && it->first == s) return it->second;
    return 0;
}

void setLevel(int c, int s, int val) {
    auto &v = skills[c];
    auto it = lower_bound(v.begin(), v.end(), make_pair(s, INT_MIN));
    if (it != v.end() && it->first == s) {
        it->second = val;
    } else {
        v.insert(it, make_pair(s, val));
    }
}

bool attempt(int p, vector<int> &asg) {
    const auto &roles = projs[p].roles;
    int R = roles.size();
    asg.assign(R, -1);
    stampCur++;

    // Phase 1: direct matches (skill >= required), choose the lowest sufficient level
    for (int r = 0; r < R; r++) {
        int s = roles[r].first, L = roles[r].second;
        int best = -1, bl = INT_MAX;
        work += freeSet.size();
        for (int c : freeSet) {
            if (mark[c] == stampCur) continue;
            int lv = getLevel(c, s);
            if (lv >= L && lv < bl) {
                best = c;
                bl = lv;
                if (lv == L) break;
            }
        }
        if (best >= 0) {
            asg[r] = best;
            mark[best] = stampCur;
        }
    }

    // Phase 2: mentees with skill exactly one level below, requiring a mentor
    for (int r = 0; r < R; r++) {
        if (asg[r] >= 0) continue;
        int s = roles[r].first, L = roles[r].second;
        bool hasMentor = false;
        for (int r2 = 0; r2 < R; r2++) {
            if (r2 == r || asg[r2] < 0) continue;
            if (getLevel(asg[r2], s) >= L) { hasMentor = true; break; }
        }
        if (!hasMentor) return false;
        int pick = -1;
        work += freeSet.size();
        for (int c : freeSet) {
            if (mark[c] == stampCur) continue;
            if (getLevel(c, s) == L - 1) { pick = c; break; }
        }
        if (pick < 0) return false;
        asg[r] = pick;
        mark[pick] = stampCur;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> C >> P;
    unordered_map<string,int> sid;
    auto getId = [&](const string &s) -> int {
        auto it = sid.find(s);
        if (it != sid.end()) return it->second;
        int id = sid.size();
        sid[s] = id;
        return id;
    };

    cname.resize(C);
    skills.resize(C);
    for (int i = 0; i < C; i++) {
        int N;
        cin >> cname[i] >> N;
        for (int k = 0; k < N; k++) {
            string s;
            int l;
            cin >> s >> l;
            skills[i].push_back({getId(s), l});
        }
        sort(skills[i].begin(), skills[i].end());
    }
    mark.assign(C, 0);

    projs.resize(P);
    for (int i = 0; i < P; i++) {
        int R;
        cin >> projs[i].name >> projs[i].D >> projs[i].S >> projs[i].B >> R;
        projs[i].roles.resize(R);
        for (int k = 0; k < R; k++) {
            string s;
            int l;
            cin >> s >> l;
            projs[i].roles[k] = {getId(s), l};
        }
    }

    for (int c = 0; c < C; c++) freeSet.insert(c);

    vector<int> order(P);
    for (int i = 0; i < P; i++) order[i] = i;
    sort(order.begin(), order.end(), [&](int a, int b) {
        double ra = (double)projs[a].S / projs[a].D;
        double rb = (double)projs[b].S / projs[b].D;
        if (ra != rb) return ra > rb;
        if (projs[a].B != projs[b].B) return projs[a].B < projs[b].B;
        return a < b;
    });

    const long long LIMIT = 300000000LL;
    vector<Rec> recs;
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> ev;
    long long t = 0;

    while (true) {
        // Complete projects finishing at or before t: apply learning, free contributors
        while (!ev.empty() && ev.top().first <= t) {
            int ri = ev.top().second;
            ev.pop();
            const Rec &rec = recs[ri];
            const auto &roles = projs[rec.p].roles;
            for (size_t r = 0; r < roles.size(); r++) {
                int c = rec.asg[r];
                int s = roles[r].first, L = roles[r].second;
                int lv = getLevel(c, s);
                if (L >= lv) setLevel(c, s, lv + 1);
                freeSet.insert(c);
            }
        }

        // Try to start projects at time t
        size_t w = 0;
        for (size_t i = 0; i < order.size(); i++) {
            int p = order[i];
            if (freeSet.empty() || work >= LIMIT) {
                order[w++] = p;
                continue;
            }
            const Proj &pr = projs[p];
            long long lateness = max(0LL, t + pr.D - pr.B);
            long long sc = max(0LL, pr.S - lateness);
            if (sc == 0) continue; // lateness only grows over time, drop permanently
            int R = pr.roles.size();
            if (R > (int)freeSet.size()) {
                order[w++] = p;
                continue;
            }
            vector<int> asg;
            if (attempt(p, asg)) {
                for (int c : asg) freeSet.erase(c);
                Rec rec;
                rec.p = p;
                rec.start = t;
                rec.asg = asg;
                recs.push_back(rec);
                ev.push({t + pr.D, (int)recs.size() - 1});
            } else {
                order[w++] = p;
            }
        }
        order.resize(w);

        if (ev.empty()) break;
        t = ev.top().first;
    }

    // Output
    string out;
    out += to_string(recs.size()) + "\n";
    for (const Rec &rec : recs) {
        out += projs[rec.p].name;
        out += "\n";
        for (size_t r = 0; r < rec.asg.size(); r++) {
            if (r) out += " ";
            out += cname[rec.asg[r]];
        }
        out += "\n";
    }
    cout << out;
    return 0;
}
