#include <bits/stdc++.h>
using namespace std;

int B,L,D;
vector<int> S;
vector<int> N,T,M;
vector<vector<int>> books; // sorted desc by score

long long opsGlobal=0;

struct Result{
    long long score=0;
    vector<int> order;
    vector<vector<int>> assign;
};

Result run(double alpha, long long opBudget){
    Result R;
    vector<char> scanned(B,0), used(L,0);
    long long ops=0;
    long long t=0;
    vector<double> powT(L);
    for(int j=0;j<L;j++) powT[j]=pow((double)T[j],alpha);
    auto evalLib=[&](int j,long long tt,long long &sumOut)->double{
        long long days=(long long)D-tt-T[j];
        sumOut=0;
        if(days<=0) return -1;
        long long cap=days*(long long)M[j];
        long long cnt=0;
        for(int b:books[j]){
            ops++;
            if(cnt>=cap) break;
            if(scanned[b]) continue;
            sumOut+=S[b]; cnt++;
        }
        if(sumOut<=0) return -1;
        return (double)sumOut/powT[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        long long s; double v=evalLib(j,0,s);
        if(v>0) pq.push({v,j});
    }
    while(!pq.empty()){
        auto [v,j]=pq.top(); pq.pop();
        if(used[j]) continue;
        long long s; double nv=evalLib(j,t,s);
        if(nv<=0) continue;
        bool take = pq.empty() || nv >= pq.top().first - 1e-12 || ops>opBudget;
        if(!take){ pq.push({nv,j}); continue; }
        // take
        long long days=(long long)D-t-T[j];
        long long cap=days*(long long)M[j];
        vector<int> as;
        for(int b:books[j]){
            if((long long)as.size()>=cap) break;
            if(scanned[b]) continue;
            scanned[b]=1; as.push_back(b); R.score+=S[b];
        }
        if(as.empty()) continue;
        used[j]=1;
        R.order.push_back(j);
        R.assign.push_back(move(as));
        t+=T[j];
        if(t>=D) break;
    }
    opsGlobal+=ops;
    return R;
}

static char buf[1<<25];
int main(){
    // fast input
    auto readInt=[&]()->int{
        int c=getchar_unlocked();
        while(c!='-'&&(c<'0'||c>'9')) c=getchar_unlocked();
        int x=0;
        while(c>='0'&&c<='9'){x=x*10+(c-'0');c=getchar_unlocked();}
        return x;
    };
    B=readInt();L=readInt();D=readInt();
    S.resize(B);
    for(int i=0;i<B;i++) S[i]=readInt();
    N.resize(L);T.resize(L);M.resize(L);books.resize(L);
    for(int j=0;j<L;j++){
        N[j]=readInt();T[j]=readInt();M[j]=readInt();
        books[j].resize(N[j]);
        for(int k=0;k<N[j];k++) books[j][k]=readInt();
        sort(books[j].begin(),books[j].end(),[&](int a,int b){
            if(S[a]!=S[b]) return S[a]>S[b];
            return a<b;
        });
    }
    vector<double> alphas={1.0,0.9,1.1,0.8,1.25,0.7};
    Result best; bool have=false;
    const long long totalBudget=1500000000LL;
    for(double a:alphas){
        if(have && opsGlobal>totalBudget/2) break;
        long long remaining=totalBudget-opsGlobal;
        long long budget=max(200000000LL, remaining/2);
        Result r=run(a,budget);
        if(!have||r.score>best.score){best=move(r);have=true;}
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(best.order.size())+"\n";
    for(size_t i=0;i<best.order.size();i++){
        out+=to_string(best.order[i])+" "+to_string(best.assign[i].size())+"\n";
        auto &v=best.assign[i];
        for(size_t k=0;k<v.size();k++){
            if(k) out+=' ';
            out+=to_string(v[k]);
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
