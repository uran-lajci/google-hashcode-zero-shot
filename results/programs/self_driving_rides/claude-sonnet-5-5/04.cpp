#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int R,C,F,N;ll B,T;
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    vector<array<ll,6>> r(N);
    for(auto&x:r)for(int k=0;k<6;k++)scanf("%lld",&x[k]);
    vector<int> rem(N);
    for(int i=0;i<N;i++)rem[i]=i;
    vector<vector<int>> res(F);
    vector<ll> px(F,0),py(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!rem.empty()){
        auto [t,v]=pq.top();pq.pop();
        double best=-1;int bi=-1;ll bfin=0;
        for(int k=0;k<(int)rem.size();k++){
            auto&q=r[rem[k]];
            ll d=llabs(q[2]-q[0])+llabs(q[3]-q[1]);
            ll arr=t+llabs(q[0]-px[v])+llabs(q[1]-py[v]);
            ll st=max(arr,q[4]);
            if(st+d>q[5])continue;
            ll sc=d+(st==q[4]?B:0);
            double val=(double)sc/(double)(st+d-t)+1e-9/(1.0+st+d);
            if(val>best){best=val;bi=k;bfin=st+d;}
        }
        if(bi<0)continue;
        int id=rem[bi];
        rem[bi]=rem.back();rem.pop_back();
        res[v].push_back(id);
        px[v]=r[id][2];py[v]=r[id][3];
        pq.push({bfin,v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int x:res[i])printf(" %d",x);
        printf("\n");
    }
}
