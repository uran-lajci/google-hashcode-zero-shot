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
    vector<char> taken(B,0);
    auto t0=chrono::steady_clock::now();
    long long cur=0;
    auto eval=[&](int j)->double{
        long long days=D-cur-T[j];
        if(days<=0)return 0;
        long long cap=days*M[j];
        long long sum=0,cnt=0;
        for(int b:bk[j]){
            if(taken[b])continue;
            sum+=S[b];
            if(++cnt>=cap)break;
        }
        return (double)sum/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=eval(j);
        if(v>0)pq.push({v,j});
    }
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()){
        auto [p,j]=pq.top();pq.pop();
        double elapsed=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        double v=p;
        if(elapsed<8.0){
            v=eval(j);
            if(v<=0)continue;
            if(!pq.empty()&&v<pq.top().first){
                pq.push({v,j});
                continue;
            }
        }
        long long days=D-cur-T[j];
        if(days<=0)continue;
        long long cap=days*M[j];
        vector<int> lst;
        for(int b:bk[j]){
            if(taken[b])continue;
            lst.push_back(b);
            taken[b]=1;
            if((long long)lst.size()>=cap)break;
        }
        if(lst.empty())continue;
        res.push_back({j,lst});
        cur+=T[j];
    }
    string out=to_string(res.size())+"\n";
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
