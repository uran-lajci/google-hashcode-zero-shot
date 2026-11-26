#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<int> lat((size_t)C*E,0);
    vector<vector<int>> ec(E);
    for(int e=0;e<E;e++){
        int k;
        scanf("%d %d",&LD[e],&k);
        while(k--){
            int c,l;
            scanf("%d %d",&c,&l);
            lat[(size_t)c*E+e]=l;
            ec[e].push_back(c);
        }
    }
    struct Rq{int e;int n;int best;};
    vector<vector<Rq>> rv(V);
    {
        map<pair<int,int>,int> dummy;
        for(int i=0;i<R;i++){
            int v,e,n;
            scanf("%d %d %d",&v,&e,&n);
            rv[v].push_back({e,n,LD[e]});
        }
    }
    vector<long long> gain((size_t)C*V,0);
    for(int v=0;v<V;v++)
        for(auto&q:rv[v])
            for(int c:ec[q.e])
                gain[(size_t)c*V+v]+=(long long)q.n*(LD[q.e]-lat[(size_t)c*E+q.e]);
    typedef pair<double,int> P;
    priority_queue<P> pq;
    for(int c=0;c<C;c++)
        for(int v=0;v<V;v++){
            long long g=gain[(size_t)c*V+v];
            if(g>0&&S[v]<=X)pq.push({(double)g/S[v],c*V+v});
        }
    vector<int> used(C,0);
    vector<vector<int>> out(C);
    vector<char> in((size_t)C*V,0);
    while(!pq.empty()){
        auto [r,id]=pq.top();pq.pop();
        int c=id/V,v=id%V;
        if(in[id]||used[c]+S[v]>X)continue;
        long long g=0;
        for(auto&q:rv[v]){
            int l=lat[(size_t)c*E+q.e];
            if(l&&q.best>l)g+=(long long)q.n*(q.best-l);
        }
        if(g<=0)continue;
        double nr=(double)g/S[v];
        if(!pq.empty()&&nr<pq.top().first){
            pq.push({nr,id});
            continue;
        }
        in[id]=1;
        used[c]+=S[v];
        out[c].push_back(v);
        for(auto&q:rv[v]){
            int l=lat[(size_t)c*E+q.e];
            if(l&&q.best>l)q.best=l;
        }
    }
    printf("%d\n",C);
    for(int c=0;c<C;c++){
        printf("%d",c);
        for(int v:out[c])printf(" %d",v);
        printf("\n");
    }
}
