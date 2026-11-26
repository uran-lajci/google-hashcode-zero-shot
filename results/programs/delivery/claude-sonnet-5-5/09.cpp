#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int ceilDist(ll dx, ll dy){
    ll x=dx*dx+dy*dy;
    ll d=(ll)sqrtl((long double)x);
    while(d*d<x)d++;
    while(d>0&&(d-1)*(d-1)>=x)d--;
    return (int)d;
}
struct Cmd{int drone;char t;int a,b,c;};
int main(){
    int R,C_,D,T,MAXL;
    if(scanf("%d %d %d %d %d",&R,&C_,&D,&T,&MAXL)!=5)return 0;
    int P;scanf("%d",&P);
    vector<int> wt(P);
    for(auto&x:wt)scanf("%d",&x);
    int W;scanf("%d",&W);
    vector<int> wr(W),wc(W);
    vector<uint16_t> stock((size_t)W*P);
    vector<vector<int>> has(P);
    for(int w=0;w<W;w++){
        scanf("%d %d",&wr[w],&wc[w]);
        for(int p=0;p<P;p++){
            int v;scanf("%d",&v);
            stock[(size_t)w*P+p]=v;
            if(v>0)has[p].push_back(w);
        }
    }
    int N;scanf("%d",&N);
    vector<int> orr(N),occ(N);
    vector<vector<int>> items(N);
    for(int i=0;i<N;i++){
        scanf("%d %d",&orr[i],&occ[i]);
        int L;scanf("%d",&L);
        items[i].resize(L);
        for(auto&x:items[i])scanf("%d",&x);
    }
    vector<int> ord(N);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return items[a].size()<items[b].size();});
    vector<int> dt(D,0),dr(D,wr[0]),dc(D,wc[0]);
    typedef pair<int,int> pii;
    priority_queue<pii,vector<pii>,greater<pii>> pq;
    for(int i=0;i<D;i++)pq.push({0,i});
    vector<Cmd> out;
    for(int o:ord){
        auto it=items[o];
        sort(it.begin(),it.end(),[&](int a,int b){return wt[a]>wt[b];});
        vector<vector<int>> trips;
        vector<int> room;
        for(int p:it){
            bool placed=false;
            for(size_t k=0;k<trips.size();k++){
                if(room[k]>=wt[p]){trips[k].push_back(p);room[k]-=wt[p];placed=true;break;}
            }
            if(!placed){trips.push_back({p});room.push_back(MAXL-wt[p]);}
        }
        vector<Cmd> tmp;
        struct Undo{int d,t,r,c;};
        vector<Undo> undo;
        vector<array<int,3>> stockUndo; // w,p,count
        bool ok=true;
        for(auto&trip:trips){
            int d=-1;
            while(!pq.empty()){
                auto [t,id]=pq.top();pq.pop();
                if(dt[id]==t){d=id;break;}
            }
            if(d<0){ok=false;break;}
            undo.push_back({d,dt[d],dr[d],dc[d]});
            // choose warehouses
            map<int,map<int,int>> lw; // warehouse->product->count
            map<int,int> dp;
            for(int p:trip){
                int best=-1;ll bd=LLONG_MAX;
                for(int w:has[p]){
                    if(stock[(size_t)w*P+p]==0)continue;
                    ll dx=wr[w]-orr[o],dy=wc[w]-occ[o];
                    ll v=dx*dx+dy*dy;
                    if(v<bd){bd=v;best=w;}
                }
                if(best<0){ok=false;break;}
                stock[(size_t)best*P+p]--;
                stockUndo.push_back({best,p,1});
                lw[best][p]++;
                dp[p]++;
            }
            if(!ok)break;
            int t=dt[d],r=dr[d],c=dc[d];
            for(auto&[w,m]:lw){
                for(auto&[p,n]:m){
                    t+=ceilDist(r-wr[w],c-wc[w])+1;r=wr[w];c=wc[w];
                    tmp.push_back({d,'L',w,p,n});
                }
            }
            for(auto&[p,n]:dp){
                t+=ceilDist(r-orr[o],c-occ[o])+1;r=orr[o];c=occ[o];
                tmp.push_back({d,'D',o,p,n});
            }
            if(t>T){ok=false;break;}
            dt[d]=t;dr[d]=r;dc[d]=c;
            pq.push({t,d});
        }
        if(ok){
            for(auto&x:tmp)out.push_back(x);
        } else {
            for(int i=(int)undo.size()-1;i>=0;i--){
                auto&u=undo[i];
                dt[u.d]=u.t;dr[u.d]=u.r;dc[u.d]=u.c;
                pq.push({u.t,u.d});
            }
            for(auto&s:stockUndo)stock[(size_t)s[0]*P+s[1]]+=s[2];
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&c:out)printf("%d %c %d %d %d\n",c.drone,c.t,c.a,c.b,c.c);
}
