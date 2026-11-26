#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<vector<pair<int,int>>> conn(E);
    vector<int> lat((size_t)E*C,0);
    for(int e=0;e<E;e++){
        int k;
        scanf("%d %d",&LD[e],&k);
        for(int i=0;i<k;i++){
            int c,l;
            scanf("%d %d",&c,&l);
            conn[e].push_back({c,l});
            lat[(size_t)e*C+c]=l;
        }
    }
    vector<int> rv(R),re(R),rn(R),best(R);
    vector<vector<int>> byV(V);
    for(int i=0;i<R;i++){
        scanf("%d %d %d",&rv[i],&re[i],&rn[i]);
        best[i]=LD[re[i]];
        byV[rv[i]].push_back(i);
    }
    vector<long long> gain((size_t)C*V,0);
    for(int i=0;i<R;i++){
        for(auto&p:conn[re[i]]){
            gain[(size_t)p.first*V+rv[i]]+=(long long)rn[i]*(best[i]-p.second);
        }
    }
    struct En{double r;int c,v;};
    auto cmp=[](const En&a,const En&b){return a.r<b.r;};
    vector<En> init;
    init.reserve((size_t)C*V);
    for(int c=0;c<C;c++)for(int v=0;v<V;v++){
        long long g=gain[(size_t)c*V+v];
        if(g>0&&S[v]<=X)init.push_back({(double)g/S[v],c,v});
    }
    vector<long long>().swap(gain);
    priority_queue<En,vector<En>,decltype(cmp)> pq(cmp,std::move(init));
    vector<int> cap(C,X);
    vector<vector<int>> out(C);
    vector<char> has((size_t)C*V,0);
    auto t0=chrono::steady_clock::now();
    int iter=0;
    while(!pq.empty()){
        if((++iter&1023)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>8.0)break;
        }
        En t=pq.top();pq.pop();
        int c=t.c,v=t.v;
        if(has[(size_t)c*V+v]||S[v]>cap[c])continue;
        long long g=0;
        for(int i:byV[v]){
            int l=lat[(size_t)re[i]*C+c];
            if(l&&l<best[i])g+=(long long)rn[i]*(best[i]-l);
        }
        if(g<=0)continue;
        double r=(double)g/S[v];
        if(!pq.empty()&&r<pq.top().r){
            pq.push({r,c,v});
            continue;
        }
        has[(size_t)c*V+v]=1;
        cap[c]-=S[v];
        out[c].push_back(v);
        for(int i:byV[v]){
            int l=lat[(size_t)re[i]*C+c];
            if(l&&l<best[i])best[i]=l;
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
