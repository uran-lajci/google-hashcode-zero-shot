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
    unordered_map<long long,int> idx;
    vector<int> rv,re;
    vector<long long> rn;
    for(int i=0;i<R;i++){
        int v,e,n;
        scanf("%d %d %d",&v,&e,&n);
        long long key=(long long)v*E+e;
        auto it=idx.find(key);
        if(it==idx.end()){
            idx[key]=rv.size();
            rv.push_back(v);
            re.push_back(e);
            rn.push_back(n);
        } else rn[it->second]+=n;
    }
    int M=rv.size();
    vector<vector<int>> byv(V);
    for(int i=0;i<M;i++)byv[rv[i]].push_back(i);
    vector<int> best(M);
    for(int i=0;i<M;i++)best[i]=LD[re[i]];
    vector<long long> gain((size_t)V*C,0);
    for(int i=0;i<M;i++){
        for(auto&p:conn[re[i]]){
            gain[(size_t)rv[i]*C+p.first]+=rn[i]*(LD[re[i]]-p.second);
        }
    }
    typedef tuple<double,int,int,long long> T;
    priority_queue<T> pq;
    for(int v=0;v<V;v++)for(int c=0;c<C;c++){
        long long g=gain[(size_t)v*C+c];
        if(g>0&&S[v]<=X)pq.push(T((double)g/S[v],v,c,g));
    }
    vector<long long> used(C,0);
    vector<vector<int>> out(C);
    while(!pq.empty()){
        auto [sc,v,c,g]=pq.top();
        pq.pop();
        if(used[c]+S[v]>X)continue;
        long long ng=0;
        for(int r:byv[v]){
            int l=lat[(size_t)c*E+re[r]];
            if(l&&l<best[r])ng+=rn[r]*(best[r]-l);
        }
        if(ng<=0)continue;
        if(ng<g){
            pq.push(T((double)ng/S[v],v,c,ng));
            continue;
        }
        used[c]+=S[v];
        out[c].push_back(v);
        for(int r:byv[v]){
            int l=lat[(size_t)c*E+re[r]];
            if(l&&l<best[r])best[r]=l;
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
