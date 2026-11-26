#include <bits/stdc++.h>
using namespace std;

struct E{int to,id,cost,len;};
int N,M,T,C,S;
vector<vector<E>> adj;
vector<char> vis;
double bestR; int bestFirst;
long long nodesExplored;
const long long NODE_CAP=20000;

void dfs(int u,int rem,int depth,long long gain,long long tim,int first){
    if(depth==0) return;
    for(auto&e:adj[u]){
        if(e.cost>rem) continue;
        if(nodesExplored>=NODE_CAP) return;
        nodesExplored++;
        int f=(first<0?(int)(&e-&adj[u][0]):first);
        bool nv=!vis[e.id];
        long long g=gain+(nv?e.len:0);
        long long t=tim+e.cost;
        if(g>0){
            double r=(double)g/(double)max(1LL,t);
            if(r>bestR){bestR=r;bestFirst=f;}
        }
        if(nv) vis[e.id]=1;
        dfs(e.to,rem-e.cost,depth-1,g,t,f);
        if(nv) vis[e.id]=0;
    }
}

int main(){
    if(scanf("%d %d %d %d %d",&N,&M,&T,&C,&S)!=5) return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    adj.assign(N,{});
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        adj[a].push_back({b,j,c,l});
        if(d==2) adj[b].push_back({a,j,c,l});
    }
    vis.assign(M,0);
    vector<vector<int>> path(C,vector<int>{S});
    vector<int> pos(C,S), tm(C,0);
    vector<char> done(C,0);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
    for(int i=0;i<C;i++) pq.push({0,i});
    vector<long long> dist(N); vector<int> par(N), parE(N);
    const int DEPTH=7;
    while(!pq.empty()){
        auto [t0,c]=pq.top();pq.pop();
        int u=pos[c]; int rem=T-tm[c];
        bestR=0;bestFirst=-1;nodesExplored=0;
        dfs(u,rem,DEPTH,0,0,-1);
        if(bestFirst>=0){
            E&e=adj[u][bestFirst];
            vis[e.id]=1; tm[c]+=e.cost; pos[c]=e.to; path[c].push_back(e.to);
            pq.push({tm[c],c});
            continue;
        }
        // Dijkstra to nearest node with an unvisited outgoing edge that fits
        const long long INF=LLONG_MAX/4;
        fill(dist.begin(),dist.end(),INF);
        dist[u]=0; par[u]=-1;
        priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<pair<long long,int>>> q;
        q.push({0,u});
        int target=-1;
        while(!q.empty()){
            auto [d,x]=q.top();q.pop();
            if(d!=dist[x]) continue;
            if(d>rem) break;
            bool ok=false;
            for(auto&e:adj[x]) if(!vis[e.id] && d+e.cost<=rem){ok=true;break;}
            if(ok){target=x;break;}
            for(auto&e:adj[x]){
                long long nd=d+e.cost;
                if(nd<dist[e.to] && nd<=rem){dist[e.to]=nd;par[e.to]=x;parE[e.to]=e.cost;q.push({nd,e.to});}
            }
        }
        if(target<0||target==u){done[c]=1;continue;}
        vector<int> seq;
        for(int x=target;x!=u;x=par[x]) seq.push_back(x);
        reverse(seq.begin(),seq.end());
        // follow path, marking edges
        int cur=u;
        for(int x:seq){
            // find edge cur->x with matching minimal cost
            int bi=-1;
            for(int k=0;k<(int)adj[cur].size();k++) if(adj[cur][k].to==x){bi=k;break;}
            E&e=adj[cur][bi];
            vis[e.id]=1; tm[c]+=e.cost; cur=x; path[c].push_back(x);
        }
        pos[c]=cur;
        pq.push({tm[c],c});
    }
    string out;
    out+=to_string(C)+"\n";
    for(int i=0;i<C;i++){
        out+=to_string(path[i].size())+"\n";
        for(int x:path[i]){out+=to_string(x);out+='\n';}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
