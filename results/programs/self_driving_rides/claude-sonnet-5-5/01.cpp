#include <bits/stdc++.h>
using namespace std;
struct Ride{int a,b,x,y,s,f,d;};
int main(){
    int R,C,F,N,B;long long T;
    scanf("%d %d %d %d %d %lld",&R,&C,&F,&N,&B,&T);
    vector<Ride> r(N);
    for(int i=0;i<N;i++){
        scanf("%d %d %d %d %d %d",&r[i].a,&r[i].b,&r[i].x,&r[i].y,&r[i].s,&r[i].f);
        r[i].d=abs(r[i].x-r[i].a)+abs(r[i].y-r[i].b);
    }
    vector<int> un(N);
    for(int i=0;i<N;i++)un[i]=i;
    vector<vector<int>> res(F);
    vector<int> px(F,0),py(F,0);
    vector<long long> tm(F,0);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!un.empty()){
        auto [t,v]=pq.top();pq.pop();
        double best=-1;int bi=-1;
        for(int k=0;k<(int)un.size();k++){
            const Ride&q=r[un[k]];
            long long arr=t+abs(px[v]-q.a)+abs(py[v]-q.b);
            long long st=max(arr,(long long)q.s);
            if(st+q.d>q.f)continue;
            double pts=q.d+(st==q.s?B:0);
            double cost=st+q.d-t;
            double val=pts/cost;
            if(val>best){best=val;bi=k;}
        }
        if(bi<0)continue;
        int id=un[bi];
        const Ride&q=r[id];
        long long arr=t+abs(px[v]-q.a)+abs(py[v]-q.b);
        long long st=max(arr,(long long)q.s);
        res[v].push_back(id);
        px[v]=q.x;py[v]=q.y;
        tm[v]=st+q.d;
        un[bi]=un.back();un.pop_back();
        pq.push({tm[v],v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int x:res[i])printf(" %d",x);
        printf("\n");
    }
}
