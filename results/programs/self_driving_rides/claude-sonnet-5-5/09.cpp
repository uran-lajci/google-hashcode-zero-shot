#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int R,C,F,N;ll B,T;
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    vector<int> a(N),b(N),x(N),y(N);vector<ll> s(N),f(N);
    for(int i=0;i<N;i++){
        scanf("%d %d %d %d %lld %lld",&a[i],&b[i],&x[i],&y[i],&s[i],&f[i]);
    }
    vector<int> rem(N);
    for(int i=0;i<N;i++)rem[i]=i;
    vector<int> px(F,0),py(F,0);
    vector<vector<int>> res(F);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!rem.empty()){
        auto [t,v]=pq.top();pq.pop();
        ll bn=-1,bd=1;int bi=-1;
        for(int k=0;k<(int)rem.size();k++){
            int r=rem[k];
            ll d=abs(x[r]-a[r])+abs(y[r]-b[r]);
            ll arr=t+abs(px[v]-a[r])+abs(py[v]-b[r]);
            ll st=max(arr,s[r]);
            ll fin=st+d;
            if(fin>f[r])continue;
            ll num=d+(st==s[r]?B:0);
            ll den=fin-t;
            if(bi<0||num*bd>bn*den){bn=num;bd=den;bi=k;}
        }
        if(bi<0)continue;
        int r=rem[bi];
        ll d=abs(x[r]-a[r])+abs(y[r]-b[r]);
        ll arr=t+abs(px[v]-a[r])+abs(py[v]-b[r]);
        ll st=max(arr,s[r]);
        res[v].push_back(r);
        px[v]=x[r];py[v]=y[r];
        pq.push({st+d,v});
        rem[bi]=rem.back();rem.pop_back();
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int r:res[i])printf(" %d",r);
        printf("\n");
    }
}
