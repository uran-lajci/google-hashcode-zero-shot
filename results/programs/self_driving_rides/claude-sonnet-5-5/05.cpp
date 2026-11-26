#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int R,C,F,N;ll B,T;
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    vector<ll> a(N),b(N),x(N),y(N),s(N),f(N),d(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&a[i],&b[i],&x[i],&y[i],&s[i],&f[i]);
        d[i]=llabs(x[i]-a[i])+llabs(y[i]-b[i]);
    }
    vector<int> rem(N);
    for(int i=0;i<N;i++)rem[i]=i;
    vector<ll> pr(F,0),pc(F,0),tm(F,0);
    vector<vector<int>> res(F);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!rem.empty()){
        auto [t,v]=pq.top();pq.pop();
        double best=-1;int bi=-1;
        for(int k=0;k<(int)rem.size();k++){
            int i=rem[k];
            ll st=max(t+llabs(pr[v]-a[i])+llabs(pc[v]-b[i]),s[i]);
            if(st+d[i]>f[i])continue;
            ll pts=d[i]+(st==s[i]?B:0);
            double sc=(double)pts/(double)(st-t+d[i]);
            if(sc>best||(sc==best&&i<rem[bi])){best=sc;bi=k;}
        }
        if(bi<0)continue;
        int i=rem[bi];
        ll st=max(t+llabs(pr[v]-a[i])+llabs(pc[v]-b[i]),s[i]);
        res[v].push_back(i);
        pr[v]=x[i];pc[v]=y[i];tm[v]=st+d[i];
        rem[bi]=rem.back();rem.pop_back();
        pq.push({tm[v],v});
    }
    for(int v=0;v<F;v++){
        printf("%d",(int)res[v].size());
        for(int i:res[v])printf(" %d",i);
        printf("\n");
    }
}
