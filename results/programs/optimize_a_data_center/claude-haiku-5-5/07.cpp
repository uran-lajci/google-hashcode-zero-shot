#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

struct Result {
    vector<array<int,3>> asg; // r, s, p ; p == -1 if unallocated
    ll score;
};

int R, S, P, M;
vector<char> blockedInit;   // R*S
vector<int> Z, C;

static void recomputeMaxFree(const vector<char>& occ, int r, vector<int>& maxfree) {
    int best = 0, i = 0;
    const char* row = &occ[(size_t)r * S];
    while (i < S) {
        if (row[i]) { i++; continue; }
        int j = i;
        while (j < S && !row[j]) j++;
        best = max(best, j - i);
        i = j;
    }
    maxfree[r] = best;
}

Result solveWithOrder(const vector<int>& order) {
    vector<char> occ = blockedInit;
    vector<int> maxfree(R, 0);
    for (int r = 0; r < R; r++) recomputeMaxFree(occ, r, maxfree);

    vector<ll> load((size_t)P * R, 0);   // load[p*R + r]
    vector<ll> total(P, 0), maxload(P, 0);
    set<pair<ll,int>> st;                 // (guaranteed capacity, pool)
    for (int p = 0; p < P; p++) st.insert({0LL, p});

    Result res;
    res.asg.assign(M, {0, 0, -1});

    for (int idx : order) {
        int z = Z[idx];
        ll c = C[idx];

        bool any = false;
        for (int r = 0; r < R; r++) if (maxfree[r] >= z) { any = true; break; }
        if (!any) continue;

        int p = st.begin()->second;

        // choose row maximizing the new guaranteed capacity of pool p
        int bestR = -1;
        ll bestNg = LLONG_MIN;
        for (int r = 0; r < R; r++) {
            if (maxfree[r] < z) continue;
            ll nl = load[(size_t)p * R + r] + c;
            ll ng = total[p] + c - max(maxload[p], nl);
            if (ng > bestNg) { bestNg = ng; bestR = r; }
        }
        int r = bestR;

        // best-fit segment within row r (smallest run that fits, leftmost)
        const char* row = &occ[(size_t)r * S];
        int bestStart = -1, bestLen = INT_MAX;
        int i = 0;
        while (i < S) {
            if (row[i]) { i++; continue; }
            int j = i;
            while (j < S && !row[j]) j++;
            int len = j - i;
            if (len >= z && len < bestLen) { bestLen = len; bestStart = i; }
            i = j;
        }
        // bestStart must be valid since maxfree[r] >= z
        for (int s = bestStart; s < bestStart + z; s++) occ[(size_t)r * S + s] = 1;
        recomputeMaxFree(occ, r, maxfree);

        ll oldGc = total[p] - maxload[p];
        ll& ld = load[(size_t)p * R + r];
        ld += c;
        total[p] += c;
        maxload[p] = max(maxload[p], ld);
        ll newGc = total[p] - maxload[p];
        st.erase({oldGc, p});
        st.insert({newGc, p});

        res.asg[idx] = {r, bestStart, p};
    }

    ll score = LLONG_MAX;
    for (int p = 0; p < P; p++) score = min(score, total[p] - maxload[p]);
    if (P == 0) score = 0;
    res.score = score;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int U;
    cin >> R >> S >> U >> P >> M;
    blockedInit.assign((size_t)R * S, 0);
    for (int i = 0; i < U; i++) {
        int r, s;
        cin >> r >> s;
        blockedInit[(size_t)r * S + s] = 1;
    }
    Z.resize(M);
    C.resize(M);
    for (int i = 0; i < M; i++) cin >> Z[i] >> C[i];

    // Order A: capacity descending
    vector<int> ordA(M);
    iota(ordA.begin(), ordA.end(), 0);
    stable_sort(ordA.begin(), ordA.end(), [&](int a, int b) {
        if (C[a] != C[b]) return C[a] > C[b];
        return Z[a] < Z[b];
    });

    // Order B: capacity per slot descending
    vector<int> ordB(M);
    iota(ordB.begin(), ordB.end(), 0);
    stable_sort(ordB.begin(), ordB.end(), [&](int a, int b) {
        ll lhs = (ll)C[a] * Z[b], rhs = (ll)C[b] * Z[a];
        if (lhs != rhs) return lhs > rhs;
        return C[a] > C[b];
    });

    Result best = solveWithOrder(ordA);
    Result other = solveWithOrder(ordB);
    if (other.score > best.score) best = other;

    string out;
    out.reserve((size_t)M * 16);
    for (int i = 0; i < M; i++) {
        if (best.asg[i][2] < 0) {
            out += "x\n";
        } else {
            out += to_string(best.asg[i][0]);
            out += ' ';
            out += to_string(best.asg[i][1]);
            out += ' ';
            out += to_string(best.asg[i][2]);
            out += '\n';
        }
    }
    cout << out;
    return 0;
}
