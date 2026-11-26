#include <bits/stdc++.h>
using namespace std;
int main(){
    int V,E,R,C,X;
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    vector<int> S(V);
    for(auto&s:S)scanf("%d",&s);
    vector<int> LD(E);
    vector<vector<pair<int,int>>> conn(E);
    vector<vector<int>> lat(E,vector<int>(C,0));
    for(int e=0;e<E;e++){
        int k;
        scanf("%d %d",&LD[e],&k);
        for(int j=0;j<k;j++){
            int c,l;
            scanf("%d %d",&c,&l);
            conn[e].push_back({c,l});
            lat[e][c]=l;
        }
    }
    vector<map<int,int>> agg(V);
    for(int i=0;i<R;i++){
        int v,e,n;
        scanf("%d %d %d",&v,&e,&n);
        agg[v][e]+=n;
    }
    struct Rq{int e,n,best;};
    vector<vector<Rq>> rq(V);
    for(int v=0;v<V;v++)
        for(auto&p:agg[v])
            rq[v].push_back({p.first,p.second,LD[p.first]});
    typedef tuple<double,int,int> T;
    priority_queue<T> pq;
    vector<long long> tmp(C,0);
    for(int v=0;v<V;v++){
        if(S[v]>X)continue;
        vector<int> touched;
        for(auto&r:rq[v]){
            for(auto&cl:conn[r.e]){
                if(tmp[cl.first]==0)touched.push_back(cl.first);
                tmp[cl.first]+=(long long)r.n*(r.best-cl.second);
            }
        }
        for(int c:touched){
            if(tmp[c]>0)pq.push(T((double)tmp[c]/S[v],v,c));
            tmp[c]=0;
        }
    }
    vector<int> cap(C,X);
    vector<vector<int>> out(C);
    set<pair<int,int>> has;
    while(!pq.empty()){
        auto [sc,v,c]=pq.top();
        pq.pop();
        if(cap[c]<S[v])continue;
        if(has.count({v,c}))continue;
        long long g=0;
        for(auto&r:rq[v]){
            int l=lat[r.e][c];
            if(l>0&&l<r.best)g+=(long long)r.n*(r.best-l);
        }
        if(g<=0)continue;
        double ns=(double)g/S[v];
        if(!pq.empty()&&ns<get<0>(pq.top())){
            pq.push(T(ns,v,c));
            continue;
        }
        has.insert({v,c});
        cap[c]-=S[v];
        out[c].push_back(v);
        for(auto&r:rq[v]){
            int l=lat[r.e][c];
            if(l>0&&l<r.best)r.best=l;
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
