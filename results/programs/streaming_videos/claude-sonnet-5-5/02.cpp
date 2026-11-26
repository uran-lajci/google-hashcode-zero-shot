#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<vector<short>> lat(E,vector<short>(C,-1));
    vector<vector<pair<int,int>>> conn(E);
    for(int e=0;e<E;e++){
        int k;scanf("%d %d",&LD[e],&k);
        while(k--){int c,l;scanf("%d %d",&c,&l);lat[e][c]=l;conn[e].push_back({c,l});}
    }
    vector<unordered_map<int,int>> agg(E);
    for(int i=0;i<R;i++){
        int v,e,n;scanf("%d %d %d",&v,&e,&n);
        agg[e][v]+=n;
    }
    struct Rq{int e;int n;int id;};
    vector<vector<Rq>> reqs(V);
    vector<int> cur;
    for(int e=0;e<E;e++){
        vector<pair<int,int>> tmp(agg[e].begin(),agg[e].end());
        sort(tmp.begin(),tmp.end());
        for(auto&p:tmp){
            int id=cur.size();
            cur.push_back(LD[e]);
            reqs[p.first].push_back({e,p.second,id});
        }
    }
    vector<long long> gain((size_t)C*V,0);
    for(int v=0;v<V;v++){
        for(auto&r:reqs[v]){
            for(auto&pr:conn[r.e]){
                gain[(size_t)pr.first*V+v]+=(long long)r.n*(LD[r.e]-pr.second);
            }
        }
    }
    typedef tuple<double,int,int> T;
    auto cmp=[](const T&a,const T&b){
        if(get<0>(a)!=get<0>(b))return get<0>(a)<get<0>(b);
        if(get<1>(a)!=get<1>(b))return get<1>(a)>get<1>(b);
        return get<2>(a)>get<2>(b);
    };
    vector<T> init;
    for(int c=0;c<C;c++)for(int v=0;v<V;v++){
        long long g=gain[(size_t)c*V+v];
        if(g>0&&S[v]<=X)init.push_back(T((double)g/S[v],c,v));
    }
    vector<long long>().swap(gain);
    priority_queue<T,vector<T>,decltype(cmp)> pq(cmp,move(init));
    vector<int> rem(C,X);
    vector<vector<int>> out(C);
    vector<vector<char>> has(C,vector<char>(V,0));
    while(!pq.empty()){
        auto [r,c,v]=pq.top();pq.pop();
        if(has[c][v]||S[v]>rem[c])continue;
        long long g=0;
        for(auto&q:reqs[v]){
            int l=lat[q.e][c];
            if(l>=0&&l<cur[q.id])g+=(long long)q.n*(cur[q.id]-l);
        }
        if(g<=0)continue;
        double nr=(double)g/S[v];
        if(nr<r){
            if(!pq.empty()){
                T t(nr,c,v);
                if(cmp(t,pq.top())){pq.push(t);continue;}
            }
        }
        has[c][v]=1;rem[c]-=S[v];out[c].push_back(v);
        for(auto&q:reqs[v]){
            int l=lat[q.e][c];
            if(l>=0&&l<cur[q.id])cur[q.id]=l;
        }
    }
    printf("%d\n",C);
    for(int c=0;c<C;c++){
        printf("%d",c);
        for(int v:out[c])printf(" %d",v);
        printf("\n");
    }
}
