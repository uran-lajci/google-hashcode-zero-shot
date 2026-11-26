#include <bits/stdc++.h>
using namespace std;
int main(){
    long long R,C,F,N,B,T;
    scanf("%lld %lld %lld %lld %lld %lld",&R,&C,&F,&N,&B,&T);
    vector<array<long long,6>> r(N);
    for(auto&q:r)for(int k=0;k<6;k++)scanf("%lld",&q[k]);
    vector<long long> px(F,0),py(F,0),tm(F,0);
    vector<vector<int>> res(F);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    vector<int> rem(N);
    iota(rem.begin(),rem.end(),0);
    while(!pq.empty()){
        auto [t,v]=pq.top();pq.pop();
        double best=-1;int bi=-1;size_t bj=0;
        size_t w=0;
        for(size_t j=0;j<rem.size();j++){
            int i=rem[j];
            auto&q=r[i];
            long long d=llabs(q[2]-q[0])+llabs(q[3]-q[1]);
            long long arr=t+llabs(q[0]-px[v])+llabs(q[1]-py[v]);
            long long st=max(arr,q[4]);
            long long fin=st+d;
            if(t+d>q[5]) continue; // infeasible for all later pops
            rem[w++]=i;
            if(fin>q[5])continue;
            long long sc=d+(arr<=q[4]?B:0);
            double val=(double)sc/(double)(fin-t);
            if(val>best){best=val;bi=i;bj=w-1;}
        }
        rem.resize(w);
        if(bi<0)continue;
        auto&q=r[bi];
        long long d=llabs(q[2]-q[0])+llabs(q[3]-q[1]);
        long long arr=t+llabs(q[0]-px[v])+llabs(q[1]-py[v]);
        long long fin=max(arr,q[4])+d;
        res[v].push_back(bi);
        px[v]=q[2];py[v]=q[3];
        rem.erase(rem.begin()+bj);
        pq.push({fin,v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int x:res[i])printf(" %d",x);
        printf("\n");
    }
}
