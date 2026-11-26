#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<vector<pair<int,int>>> conn(E);
    vector<int> lat((size_t)C*E,0);
    for(int e=0;e<E;e++){
        int k;
        scanf("%d %d",&LD[e],&k);
        for(int i=0;i<k;i++){
            int c,l;
            scanf("%d %d",&c,&l);
            conn[e].push_back({c,l});
            lat[(size_t)c*E+e]=l;
        }
    }
    vector<map<int,long long>> mp(V);
    for(int i=0;i<R;i++){
        int v,e,n;
        scanf("%d %d %d",&v,&e,&n);
        mp[v][e]+=n;
    }
    struct Ent{int e;long long n;int best;};
    vector<vector<Ent>> ent(V);
    for(int v=0;v<V;v++)
        for(auto&p:mp[v])ent[v].push_back({p.first,p.second,LD[p.first]});
    mp.clear();
    mp.shrink_to_fit();
    typedef tuple<double,int,int> T;
    priority_queue<T> pq;
    vector<long long> g(C,0);
    for(int v=0;v<V;v++){
        if(S[v]>X)continue;
        vector<int> touched;
        for(auto&en:ent[v]){
            for(auto&pc:conn[en.e]){
                if(g[pc.first]==0)touched.push_back(pc.first);
                g[pc.first]+=en.n*(long long)(LD[en.e]-pc.second);
            }
        }
        for(int c:touched){
            if(g[c]>0)pq.push(T((double)g[c]/S[v],v,c));
            g[c]=0;
        }
    }
    vector<int> used(C,0);
    vector<vector<int>> out(C);
    vector<set<int>> has(C);
    while(!pq.empty()){
        auto [r,v,c]=pq.top();
        pq.pop();
        if(used[c]+S[v]>X)continue;
        if(has[c].count(v))continue;
        long long gn=0;
        for(auto&en:ent[v]){
            int l=lat[(size_t)c*E+en.e];
            if(l>0&&l<en.best)gn+=en.n*(long long)(en.best-l);
        }
        if(gn<=0)continue;
        double nr=(double)gn/S[v];
        if(!pq.empty()&&nr<get<0>(pq.top())){
            pq.push(T(nr,v,c));
            continue;
        }
        used[c]+=S[v];
        has[c].insert(v);
        out[c].push_back(v);
        for(auto&en:ent[v]){
            int l=lat[(size_t)c*E+en.e];
            if(l>0&&l<en.best)en.best=l;
        }
    }
    int N=0;
    for(int c=0;c<C;c++)if(!out[c].empty())N++;
    printf("%d\n",N);
    for(int c=0;c<C;c++){
        if(out[c].empty())continue;
        printf("%d",c);
        for(int v:out[c])printf(" %d",v);
        printf("\n");
    }
}
