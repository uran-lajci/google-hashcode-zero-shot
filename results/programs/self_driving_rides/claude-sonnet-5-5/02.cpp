#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,C,F,N,B;
    long long T;
    scanf("%d %d %d %d %d %lld",&R,&C,&F,&N,&B,&T);
    vector<int> a(N),b(N),x(N),y(N);
    vector<long long> s(N),f(N);
    for(int i=0;i<N;i++){
        scanf("%d %d %d %d %lld %lld",&a[i],&b[i],&x[i],&y[i],&s[i],&f[i]);
    }
    vector<char> used(N,0);
    vector<vector<int>> res(F);
    vector<int> pr(F,0),pc(F,0);
    typedef pair<long long,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<F;i++) pq.push({0,i});
    while(!pq.empty()){
        P p=pq.top();pq.pop();
        long long t=p.first;
        int v=p.second;
        int best=-1;
        double bv=-1;
        long long bend=0;
        for(int i=0;i<N;i++){
            if(used[i]) continue;
            long long arrive=t+abs(pr[v]-a[i])+abs(pc[v]-b[i]);
            long long st=max(arrive,s[i]);
            long long d=abs(x[i]-a[i])+abs(y[i]-b[i]);
            long long en=st+d;
            if(en>f[i]) continue;
            double val=(double)(d+(arrive<=s[i]?B:0))/(double)(en-t);
            if(val>bv){
                bv=val;
                best=i;
                bend=en;
            }
        }
        if(best<0) continue;
        used[best]=1;
        res[v].push_back(best);
        pr[v]=x[best];
        pc[v]=y[best];
        if(bend<T) pq.push({bend,v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int r:res[i]) printf(" %d",r);
        printf("\n");
    }
}
