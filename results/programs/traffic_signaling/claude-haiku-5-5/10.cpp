#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long D;
    int I, S, V;
    long long F;
    cin >> D >> I >> S >> V >> F;

    vector<int> B(S), E(S);
    vector<long long> Lt(S);
    vector<string> name(S);
    unordered_map<string, int> id;
    id.reserve(S * 2 + 1);
    for (int s = 0; s < S; ++s) {
        cin >> B[s] >> E[s] >> name[s] >> Lt[s];
        id[name[s]] = s;
    }

    // For each street: number of feasible cars needing its light, and earliest arrival.
    vector<long long> cnt(S, 0), firstArr(S, LLONG_MAX);

    for (int v = 0; v < V; ++v) {
        int P;
        cin >> P;
        vector<int> path(P);
        for (int k = 0; k < P; ++k) {
            string nm;
            cin >> nm;
            path[k] = id[nm];
        }
        // suffix sums: suf[k] = sum of L[path[j]] for j >= k
        vector<long long> suf(P + 1, 0);
        for (int k = P - 1; k >= 0; --k) suf[k] = suf[k + 1] + Lt[path[k]];
        // arrival at end of path[k]: a_0 = 0, a_k = sum_{j=1..k} L[path[j]]
        long long a = 0;
        for (int k = 0; k <= P - 2; ++k) {
            if (k >= 1) a += Lt[path[k]];
            // remaining time after end of path[k] = sum_{j>k} L[path[j]] = suf[k+1]
            if (a + suf[k + 1] <= D) {
                cnt[path[k]]++;
                firstArr[path[k]] = min(firstArr[path[k]], a);
            }
        }
    }

    // Group candidate streets by their end intersection.
    vector<vector<int>> perInter(I);
    for (int s = 0; s < S; ++s) {
        if (cnt[s] > 0) perInter[E[s]].push_back(s);
    }

    int A = 0;
    string body;
    for (int i = 0; i < I; ++i) {
        auto &lst = perInter[i];
        if (lst.empty()) continue;
        sort(lst.begin(), lst.end(), [&](int x, int y) {
            if (firstArr[x] != firstArr[y]) return firstArr[x] < firstArr[y];
            return x < y;
        });
        A++;
        body += to_string(i) + "\n";
        body += to_string(lst.size()) + "\n";
        for (int s : lst) {
            long long T = min(cnt[s], D);
            if (T < 1) T = 1;
            body += name[s] + " " + to_string(T) + "\n";
        }
    }

    cout << A << "\n" << body;
    return 0;
}
