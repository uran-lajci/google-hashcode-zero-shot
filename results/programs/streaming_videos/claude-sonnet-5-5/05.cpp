#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<int> lat((size_t)E*C,0);
    vector<vector<int>> ecs(E);
    for(int e=0;e<E;e++){
        int k;scanf("%d %d",&LD[e],&k);
        while(k--){int c,l;scanf("%d %d",&c,&l);lat[(size_t)e*C+c]=l;ecs[e].push_back(c);}
    }
    map<pair<int,int>,long long> agg;
    vector<array<int,3>> raw(R);
    for(int i=0;i<R;i++){
        scanf("%d %d %d",&raw[i][0],&raw[i][1],&raw[i][2]);
    }
    struct Rq{int v,e;int n;int cur;};
    vector<Rq> rq;
    {
        sort(raw.begin(),raw.end());
        for(int i=0;i<R;i++){
            if(!rq.empty()&&rq.back().v==raw[i][0]&&rq.back().e==raw[i][1])rq.back().n+=raw[i][2];
            else rq.push_back({raw[i][0],raw[i][1],raw[i][2],LD[raw[i][1]]});
        }
    }
    int Q=rq.size();
    vector<vector<int>> byV(V),byE(E);
    for(int i=0;i<Q;i++){byV[rq[i].v].push_back(i);byE[rq[i].e].push_back(i);}
    vector<vector<int>> cEnd(C);
    for(int e=0;e<E;e++)for(int c:ecs[e])cEnd[c].push_back(e);
    auto gain=[&](int c,int v)->long long{
        long long g=0;
        for(int i:byV[v]){
            int l=lat[(size_t)rq[i].e*C+c];
            if(l&&l<rq[i].cur)g+=(long long)rq[i].n*(rq[i].cur-l);
        }
        return g;
    };
    typedef tuple<double,int,int> T;
    priority_queue<T> pq;
    vector<int> stamp(V,-1);
    for(int c=0;c<C;c++){
        for(int e:cEnd[c])for(int i:byE[e]){
            int v=rq[i].v;
            if(stamp[v]==c)continue;
            stamp[v]=c;
            if(S[v]>X)continue;
            long long g=gain(c,v);
            if(g>0)pq.push(T((double)g/S[v],c,v));
        }
    }
    vector<int> rem(C,X);
    vector<vector<int>> out(C);
    while(!pq.empty()){
        auto [sc,c,v]=pq.top();pq.pop();
        if(S[v]>rem[c])continue;
        long long g=gain(c,v);
        if(g<=0)continue;
        double ns=(double)g/S[v];
        if(ns<sc-1e-12){
            pq.push(T(ns,c,v));
            continue;
        }
        rem[c]-=S[v];
        out[c].push_back(v);
        for(int i:byV[v]){
            int l=lat[(size_t)rq[i].e*C+c];
            if(l&&l<rq[i].cur)rq[i].cur=l;
        }
    }
    int N=0;
    for(int c=0;c<C;c++)if(!out[c].empty())N++;
    printf("%d\n",N);
    for(int c=0;c<C;c++)if(!out[c].empty()){
        printf("%d",c);
        for(int v:out[c])printf(" %d",v);
        printf("\n");
    }
}
