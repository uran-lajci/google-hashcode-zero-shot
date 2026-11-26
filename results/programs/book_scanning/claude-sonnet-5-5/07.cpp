#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    if(scanf("%d %d %d",&B,&L,&D)!=3) return 0;
    vector<int> S(B);
    for(auto&x:S) scanf("%d",&x);
    vector<vector<int>> bk(L);
    vector<int> T(L),M(L);
    for(int j=0;j<L;j++){
        int n;
        scanf("%d %d %d",&n,&T[j],&M[j]);
        bk[j].resize(n);
        for(auto&x:bk[j]) scanf("%d",&x);
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(S[a]!=S[b]) return S[a]>S[b];
            return a<b;
        });
    }
    vector<char> used(B,0);
    long long t=0;
    auto eval=[&](int j)->double{
        long long rem=D-t-T[j];
        if(rem<=0) return 0;
        long long cap=rem*(long long)M[j];
        long long cnt=0,val=0;
        for(int b:bk[j]){
            if(used[b]) continue;
            val+=S[b];
            if(++cnt>=cap) break;
        }
        return (double)val/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=eval(j);
        if(v>0) pq.push({v,-j});
    }
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()){
        auto [v,nj]=pq.top();pq.pop();
        int j=-nj;
        double nv=eval(j);
        if(nv<=0) continue;
        if(!pq.empty()){
            auto tp=pq.top();
            if(nv<tp.first || (nv==tp.first && -j>tp.second)){
                pq.push({nv,-j});
                continue;
            }
        }
        long long rem=D-t-T[j];
        long long cap=rem*(long long)M[j];
        vector<int> pick;
        for(int b:bk[j]){
            if(used[b]) continue;
            pick.push_back(b);
            used[b]=1;
            if((long long)pick.size()>=cap) break;
        }
        if(pick.empty()) continue;
        t+=T[j];
        res.push_back({j,pick});
    }
    string out=to_string(res.size())+"\n";
    for(auto&r:res){
        out+=to_string(r.first)+" "+to_string(r.second.size())+"\n";
        for(size_t i=0;i<r.second.size();i++){
            if(i) out+=' ';
            out+=to_string(r.second[i]);
        }
        out+='\n';
    }
    fputs(out.c_str(),stdout);
}
