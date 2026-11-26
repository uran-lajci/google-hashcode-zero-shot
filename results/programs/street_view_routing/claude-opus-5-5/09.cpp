#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Arc{int to; ll cost; ll len; int e;};
int N,M,C,S; ll T;
vector<vector<Arc>> adj;
vector<char> vis;
ll nodeCount;
// best per first arc
double bestVal; int bestFirst; ll bestGain;

void dfs(int u, ll t, ll gain, ll tim, int depth, int first){
    if(depth==0) return;
    for(int i=0;i<(int)adj[u].size();i++){
        const Arc&a=adj[u][i];
        if(t+a.cost>T) continue;
        nodeCount++;
        int f = first<0? i : first;
        bool nv=!vis[a.e];
        ll g=gain+(nv?a.len:0);
        ll tm=tim+a.cost;
        if(g>0){
            double v=(double)g/((double)tm+1e-6);
            if(v>bestVal+1e-12){bestVal=v;bestFirst=f;bestGain=g;}
        }
        if(nv) vis[a.e]=1;
        dfs(a.to,t+a.cost,g,tm,depth-1,f);
        if(nv) vis[a.e]=0;
    }
}

int main(){
    if(scanf("%d %d %lld %d %d",&N,&M,&T,&C,&S)!=5) return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    adj.assign(N,{});
    vis.assign(M,0);
    for(int j=0;j<M;j++){
        int a,b,d; ll c,l; scanf("%d %d %d %lld %lld",&a,&b,&d,&c,&l);
        adj[a].push_back({b,c,l,j});
        if(d==2) adj[b].push_back({a,c,l,j});
    }
    vector<vector<int>> path(C,vector<int>{S});
    vector<ll> tm(C,0);
    vector<int> pos(C,S);
    vector<char> done(C,0);
    const size_t MAXV=1000000;
    const ll BUDGET=4000;
    const int MAXD=14;
    vector<ll> dist(N,LLONG_MAX); vector<int> par(N,-1), parArc(N,-1);
    vector<int> touched;
    while(true){
        int c=-1;
        for(int i=0;i<C;i++) if(!done[i]&&(c<0||tm[i]<tm[c])) c=i;
        if(c<0) break;
        if(path[c].size()>=MAXV){done[c]=1;continue;}
        int u=pos[c];
        bestVal=-1;bestFirst=-1;bestGain=0;
        int chosen=-1;
        for(int d=1;d<=MAXD;d++){
            nodeCount=0;
            bestVal=-1;bestFirst=-1;
            dfs(u,tm[c],0,0,d,-1);
            chosen=bestFirst;
            if(nodeCount>=BUDGET) break;
        }
        if(chosen>=0){
            const Arc&a=adj[u][chosen];
            vis[a.e]=1; tm[c]+=a.cost; pos[c]=a.to; path[c].push_back(a.to);
            continue;
        }
        // Dijkstra fallback
        for(int x:touched){dist[x]=LLONG_MAX;par[x]=-1;parArc[x]=-1;}
        touched.clear();
        priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
        dist[u]=tm[c]; touched.push_back(u); pq.push({tm[c],u});
        int target=-1,tArc=-1;
        while(!pq.empty()){
            auto [dd,x]=pq.top();pq.pop();
            if(dd!=dist[x]) continue;
            bool found=false;
            for(int i=0;i<(int)adj[x].size();i++){
                const Arc&a=adj[x][i];
                if(!vis[a.e]&&dd+a.cost<=T){target=x;tArc=i;found=true;break;}
            }
            if(found) break;
            for(int i=0;i<(int)adj[x].size();i++){
                const Arc&a=adj[x][i];
                ll nd=dd+a.cost;
                if(nd>T) continue;
                if(nd<dist[a.to]){
                    if(dist[a.to]==LLONG_MAX) touched.push_back(a.to);
                    dist[a.to]=nd;par[a.to]=x;parArc[a.to]=i;pq.push({nd,a.to});
                }
            }
        }
        if(target<0){done[c]=1;continue;}
        vector<int> seq; // arcs indices along path
        vector<int> nodes;
        int x=target;
        while(x!=u){nodes.push_back(x);x=par[x];}
        reverse(nodes.begin(),nodes.end());
        int cur=u;
        bool stop=false;
        for(int y:nodes){
            const Arc&a=adj[par[y]][parArc[y]];
            if(path[c].size()>=MAXV){stop=true;break;}
            vis[a.e]=1; tm[c]+=a.cost; path[c].push_back(y); cur=y;
        }
        pos[c]=cur;
        if(stop){done[c]=1;continue;}
        if(path[c].size()<MAXV){
            const Arc&a=adj[target][tArc];
            vis[a.e]=1; tm[c]+=a.cost; pos[c]=a.to; path[c].push_back(a.to);
        } else done[c]=1;
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(C)+"\n";
    for(int i=0;i<C;i++){
        out+=to_string(path[i].size())+"\n";
        for(int v:path[i]){out+=to_string(v);out+='\n';}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
