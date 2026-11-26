#include <bits/stdc++.h>
using namespace std;
struct Ride{int a,b,x,y,s,f,id,d;};
int main(){
    int R,C,F,N,B;long long T;
    scanf("%d %d %d %d %d %lld",&R,&C,&F,&N,&B,&T);
    vector<Ride> rs(N);
    for(int i=0;i<N;i++){
        Ride&r=rs[i];
        scanf("%d %d %d %d %d %d",&r.a,&r.b,&r.x,&r.y,&r.s,&r.f);
        r.id=i;r.d=abs(r.x-r.a)+abs(r.y-r.b);
    }
    vector<vector<int>> out(F);
    vector<int> px(F,0),py(F,0);
    vector<long long> tm(F,0);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!rs.empty()){
        auto [t,v]=pq.top();pq.pop();
        int n=rs.size();
        int best=-1;long long bn=0,bd=1;
        for(int i=0;i<n;i++){
            const Ride&r=rs[i];
            long long arr=t+abs(px[v]-r.a)+abs(py[v]-r.b);
            long long st=max(arr,(long long)r.s);
            long long fin=st+r.d;
            if(fin>r.f)continue;
            long long num=r.d+(st==r.s?B:0);
            long long den=fin-t;
            if(best<0||num*bd>bn*den){best=i;bn=num;bd=den;}
        }
        if(best<0)continue;
        Ride r=rs[best];
        long long arr=t+abs(px[v]-r.a)+abs(py[v]-r.b);
        long long st=max(arr,(long long)r.s);
        tm[v]=st+r.d;
        px[v]=r.x;py[v]=r.y;
        out[v].push_back(r.id);
        rs[best]=rs.back();rs.pop_back();
        if(tm[v]<T)pq.push({tm[v],v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)out[i].size());
        for(int id:out[i])printf(" %d",id);
        printf("\n");
    }
}
