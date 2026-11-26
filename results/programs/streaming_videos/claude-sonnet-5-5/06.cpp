#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<vector<pair<int,int>>> ec(E);
    vector<int> lat((size_t)E*C,0);
    for(int e=0;e<E;e++){
        int k;scanf("%d %d",&LD[e],&k);
        for(int i=0;i<k;i++){
            int c,l;scanf("%d %d",&c,&l);
            ec[e].push_back({c,l});
            lat[(size_t)e*C+c]=l;
        }
    }
    vector<vector<pair<int,int>>> rq(V);
    for(int i=0;i<R;i++){
        int v,e,n;scanf("%d %d %d",&v,&e,&n);
        rq[v].push_back({e,n});
    }
    vector<int> best((size_t)E*V);
    for(int e=0;e<E;e++)for(int v=0;v<V;v++)best[(size_t)e*V+v]=LD[e];
    struct Cand{double sc;int v,c;};
    auto cmp=[](const Cand&a,const Cand&b){return a.sc<b.sc;};
    vector<Cand> init;
    vector<long long> g(C);
    for(int v=0;v<V;v++){
        if(S[v]>X)continue;
        fill(g.begin(),g.end(),0);
        for(auto&r:rq[v]){
            int e=r.first;
            for(auto&p:ec[e])g[p.first]+=(long long)r.second*(LD[e]-p.second);
        }
        for(int c=0;c<C;c++)if(g[c]>0)init.push_back({(double)g[c]/S[v],v,c});
    }
    priority_queue<Cand,vector<Cand>,decltype(cmp)> pq(cmp,move(init));
    vector<int> cap(C,X);
    vector<vector<int>> out(C);
    set<pair<int,int>> placed;
    while(!pq.empty()){
        Cand t=pq.top();pq.pop();
        int v=t.v,c=t.c;
        if(cap[c]<S[v])continue;
        long long g2=0;
        for(auto&r:rq[v]){
            int e=r.first;
            int l=lat[(size_t)e*C+c];
            int b=best[(size_t)e*V+v];
            if(l>0&&b>l)g2+=(long long)r.second*(b-l);
        }
        if(g2<=0)continue;
        double sc=(double)g2/S[v];
        if(!pq.empty()&&sc<pq.top().sc){
            pq.push({sc,v,c});
            continue;
        }
        cap[c]-=S[v];
        out[c].push_back(v);
        for(auto&r:rq[v]){
            int e=r.first;
            int l=lat[(size_t)e*C+c];
            if(l>0&&best[(size_t)e*V+v]>l)best[(size_t)e*V+v]=l;
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
