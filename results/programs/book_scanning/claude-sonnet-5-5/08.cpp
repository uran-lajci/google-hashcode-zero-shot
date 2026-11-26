#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    scanf("%d %d %d",&B,&L,&D);
    vector<int> S(B);
    for(auto&x:S)scanf("%d",&x);
    vector<int> T(L),M(L);
    vector<vector<int>> bk(L);
    for(int j=0;j<L;j++){
        int n;
        scanf("%d %d %d",&n,&T[j],&M[j]);
        bk[j].resize(n);
        for(auto&x:bk[j])scanf("%d",&x);
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];
            return a<b;});
    }
    vector<char> used(B,0);
    long long ops=0;
    const long long BUDGET=300000000LL;
    long long t=0;
    auto eval=[&](int j,long long t)->double{
        long long rem=D-t-T[j];
        if(rem<=0)return -1;
        long long cap=rem*(long long)M[j];
        long long taken=0,val=0;
        for(int b:bk[j]){
            ops++;
            if(taken>=cap)break;
            if(!used[b]){taken++;val+=S[b];}
        }
        return (double)val/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double p=eval(j,0);
        if(p>0)pq.push({p,j});
    }
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()){
        auto [p,j]=pq.top();pq.pop();
        if(t+T[j]>=D)continue;
        if(ops<BUDGET){
            double np=eval(j,t);
            if(np<=0)continue;
            if(!pq.empty()){
                auto tp=pq.top();
                if(np<tp.first||(np==tp.first&&j>tp.second)){
                    pq.push({np,j});
                    continue;
                }
            }
        }
        long long rem=D-t-T[j];
        long long cap=rem*(long long)M[j];
        vector<int> sel;
        for(int b:bk[j]){
            if((long long)sel.size()>=cap)break;
            if(!used[b]){sel.push_back(b);}
        }
        if(sel.empty())continue;
        for(int b:sel)used[b]=1;
        res.push_back({j,sel});
        t+=T[j];
    }
    string out;
    out+=to_string(res.size())+"\n";
    for(auto&r:res){
        out+=to_string(r.first)+" "+to_string(r.second.size())+"\n";
        for(size_t i=0;i<r.second.size();i++){
            if(i)out+=' ';
            out+=to_string(r.second[i]);
        }
        out+="\n";
    }
    fputs(out.c_str(),stdout);
}
