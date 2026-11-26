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
    long long t=0;
    auto eval=[&](int j)->double{
        long long rem=D-t-T[j];
        if(rem<=0)return -1;
        long long cap=rem*(long long)M[j];
        long long v=0,c=0;
        for(int b:bk[j]){
            if(c>=cap)break;
            if(!used[b]){v+=S[b];c++;}
        }
        return (double)v/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double p=eval(j);
        if(p>0)pq.push({p,j});
    }
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()){
        auto [p,j]=pq.top();pq.pop();
        double f=eval(j);
        if(f<=0)continue;
        if(!pq.empty()&&f<pq.top().first){
            pq.push({f,j});
            continue;
        }
        long long rem=D-t-T[j];
        long long cap=rem*(long long)M[j];
        vector<int> ch;
        for(int b:bk[j]){
            if((long long)ch.size()>=cap)break;
            if(!used[b]){used[b]=1;ch.push_back(b);}
        }
        if(ch.empty())continue;
        t+=T[j];
        res.push_back({j,ch});
    }
    printf("%d\n",(int)res.size());
    for(auto&r:res){
        printf("%d %d\n",r.first,(int)r.second.size());
        for(size_t i=0;i<r.second.size();i++)
            printf("%d%c",r.second[i],i+1==r.second.size()?'\n':' ');
    }
}
