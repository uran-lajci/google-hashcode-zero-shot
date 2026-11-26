#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,C,F,N,B;long long T;
    scanf("%d %d %d %d %d %lld",&R,&C,&F,&N,&B,&T);
    vector<int> a(N),b(N),x(N),y(N);
    vector<long long> s(N),f(N);
    for(int i=0;i<N;i++){
        scanf("%d %d %d %d %lld %lld",&a[i],&b[i],&x[i],&y[i],&s[i],&f[i]);
    }
    vector<int> rem(N);
    for(int i=0;i<N;i++)rem[i]=i;
    vector<int> px(F,0),py(F,0);
    vector<vector<int>> res(F);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<pair<long long,int>>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!rem.empty()){
        auto [t,v]=pq.top();pq.pop();
        double best=-1;int bi=-1;long long bend=0;
        for(size_t k=0;k<rem.size();k++){
            int r=rem[k];
            long long d=abs(x[r]-a[r])+abs(y[r]-b[r]);
            long long arr=t+abs(px[v]-a[r])+abs(py[v]-b[r]);
            long long st=max(arr,s[r]);
            if(st+d>f[r])continue;
            long long sc=d+(st==s[r]?B:0);
            long long spent=st+d-t;
            double val=(double)sc/(double)spent;
            if(val>best){best=val;bi=k;bend=st+d;}
        }
        if(bi<0)continue;
        int r=rem[bi];
        res[v].push_back(r);
        px[v]=x[r];py[v]=y[r];
        rem[bi]=rem.back();rem.pop_back();
        pq.push({bend,v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int r:res[i])printf(" %d",r);
        printf("\n");
    }
}
