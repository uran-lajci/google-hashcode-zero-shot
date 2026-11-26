#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int B, L, D;
vector<int> score_;
vector<int> Nn, Tt, Mm;
vector<vector<int>> lists; // sorted by score desc
vector<int> ptrIdx;
vector<char> scanned;
ll curDay = 0;

double evalLib(int j) {
    ll start = curDay + Tt[j];
    if (start >= D) return 0.0;
    ll avail = D - start;
    ll need = min(avail * (ll)Mm[j], (ll)Nn[j]);
    int &p = ptrIdx[j];
    while (p < (int)lists[j].size() && scanned[lists[j][p]]) p++;
    ll sum = 0, cnt = 0;
    for (int idx = p; idx < (int)lists[j].size() && cnt < need; idx++) {
        int b = lists[j][idx];
        if (!scanned[b]) {
            sum += score_[b];
            cnt++;
        }
    }
    if (cnt == 0) return 0.0;
    return (double)sum / (double)Tt[j];
}

struct Entry {
    double val;
    int j;
    int stamp;
    bool operator<(const Entry &o) const {
        if (val != o.val) return val < o.val;
        return j > o.j;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> B >> L >> D;
    score_.resize(B);
    for (int i = 0; i < B; i++) cin >> score_[i];

    Nn.resize(L); Tt.resize(L); Mm.resize(L);
    lists.resize(L);
    for (int j = 0; j < L; j++) {
        cin >> Nn[j] >> Tt[j] >> Mm[j];
        lists[j].resize(Nn[j]);
        for (int k = 0; k < Nn[j]; k++) cin >> lists[j][k];
        sort(lists[j].begin(), lists[j].end(), [&](int a, int b) {
            if (score_[a] != score_[b]) return score_[a] > score_[b];
            return a < b;
        });
    }

    ptrIdx.assign(L, 0);
    scanned.assign(B, 0);

    priority_queue<Entry> pq;
    for (int j = 0; j < L; j++) {
        double v = evalLib(j);
        if (v > 0) pq.push({v, j, 0});
    }

    int sel = 0;
    vector<pair<int, vector<int>>> result;

    while (!pq.empty()) {
        if (curDay >= D) break;
        Entry e = pq.top();
        pq.pop();
        if (e.stamp != sel) {
            double v = evalLib(e.j);
            if (v > 0) pq.push({v, e.j, sel});
            continue;
        }
        if (e.val <= 0) break;

        int j = e.j;
        ll start = curDay + Tt[j];
        ll avail = D - start;
        ll need = min(avail * (ll)Mm[j], (ll)Nn[j]);
        vector<int> books;
        for (int idx = ptrIdx[j]; idx < (int)lists[j].size() && (ll)books.size() < need; idx++) {
            int b = lists[j][idx];
            if (!scanned[b]) {
                scanned[b] = 1;
                books.push_back(b);
            }
        }
        if (!books.empty()) result.push_back({j, books});
        curDay += Tt[j];
        sel++;
    }

    string out;
    out.reserve(1 << 20);
    out += to_string(result.size());
    out += '\n';
    for (auto &r : result) {
        out += to_string(r.first);
        out += ' ';
        out += to_string(r.second.size());
        out += '\n';
        for (size_t i = 0; i < r.second.size(); i++) {
            if (i) out += ' ';
            out += to_string(r.second[i]);
        }
        out += '\n';
    }
    cout << out;
    return 0;
}
