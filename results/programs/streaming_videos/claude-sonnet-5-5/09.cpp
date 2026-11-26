#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<vector<pair<int,int>>> con(E);
    vector<int> lat((size_t)E*C,0);
    for(int e=0;e<E;e++){
        int k;
        scanf("%d %d",&LD[e],&k);
        for(int i=0;i<k;i++){
            int c,l;
            scanf("%d %d",&c,&l);
            con[e].push_back({c,l});
            lat[(size_t)e*C+c]=l;
        }
    }
    vector<int> rv(R),re(R),rn(R);
    vector<vector<int>> byv(V);
    for(int i=0;i<R;i++){
        scanf("%d %d %d",&rv[i],&re[i],&rn[i]);
        byv[rv[i]].push_back(i);
    }
    vector<int> cur(R);
    for(int i=0;i<R;i++)cur[i]=LD[re[i]];
    vector<long long> gain((size_t)C*V,0);
    for(int i=0;i<R;i++){
        for(auto&p:con[re[i]]){
            gain[(size_t)p.first*V+rv[i]]+=(long long)rn[i]*(LD[re[i]]-p.second);
        }
    }
    typedef pair<double,pair<int,int>> T;
    priority_queue<T> pq;
    for(int c=0;c<C;c++)
        for(int v=0;v<V;v++){
            long long g=gain[(size_t)c*V+v];
            if(g>0&&S[v]<=X)pq.push({(double)g/S[v],{c,v}});
        }
    vector<long long>().swap(gain);
    vector<int> rem(C,X);
    vector<vector<int>> out(C);
    while(!pq.empty()){
        T t=pq.top();pq.pop();
        int c=t.second.first,v=t.second.second;
        if(S[v]>rem[c])continue;
        long long g=0;
        for(int r:byv[v]){
            int l=lat[(size_t)re[r]*C+c];
            if(l&&cur[r]>l)g+=(long long)rn[r]*(cur[r]-l);
        }
        if(g<=0)continue;
        double ratio=(double)g/S[v];
        if(!pq.empty()&&ratio<pq.top().first){
            pq.push({ratio,{c,v}});
            continue;
        }
        rem[c]-=S[v];
        out[c].push_back(v);
        for(int r:byv[v]){
            int l=lat[(size_t)re[r]*C+c];
            if(l&&cur[r]>l)cur[r]=l;
        }
    }
    printf("%d\n",C);
    for(int c=0;c<C;c++){
        printf("%d",c);
        for(int v:out[c])printf(" %d",v);
        printf("\n");
    }
}
