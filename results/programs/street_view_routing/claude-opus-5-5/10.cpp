#include <bits/stdc++.h>
using namespace std;

struct Arc{int to,e,c,l;};
int N,M,T,C,S;
vector<vector<Arc>> adj;
vector<int> elen;
vector<char> vis,tmpm;
double bestv;
int baseT;

void dfs(int u,int depth,long long tm,long long gain){
    if(tm>0){double v=(double)gain/tm; if(v>bestv)bestv=v;}
    if(depth==0)return;
    for(auto&a:adj[u]){
        if(baseT+tm+a.c>T)continue;
        int g=0;
        bool mk=false;
        if(!vis[a.e]&&!tmpm[a.e]){g=a.l;tmpm[a.e]=1;mk=true;}
        dfs(a.to,depth-1,tm+a.c,gain+g);
        if(mk)tmpm[a.e]=0;
    }
}

long long runSim(int depth, vector<vector<int>>& paths){
    vis.assign(M,0);tmpm.assign(M,0);
    paths.assign(C,vector<int>(1,S));
    vector<int> pos(C,S); vector<long long> tim(C,0);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    for(int i=0;i<C;i++)pq.push({0,i});
    long long score=0;
    vector<long long> dist(N); vector<int> par(N),pe(N);
    while(!pq.empty()){
        auto [t,ci]=pq.top();pq.pop();
        int u=pos[ci];
        baseT=(int)t;
        // lookahead
        int bestArc=-1;double bv=0;int bimm=-1;int bc=INT_MAX;
        for(int k=0;k<(int)adj[u].size();k++){
            auto&a=adj[u][k];
            if(t+a.c>T)continue;
            int g=0;bool mk=false;
            if(!vis[a.e]){g=a.l;tmpm[a.e]=1;mk=true;}
            bestv=0;
            baseT=(int)t;
            dfs(a.to,depth-1,a.c,g);
            if(mk)tmpm[a.e]=0;
            double v=bestv;
            if(v>bv+1e-12||(v>bv-1e-12&&v>0&&(g>bimm||(g==bimm&&a.c<bc)))){
                bv=v;bestArc=k;bimm=g;bc=a.c;
            }
        }
        if(bestArc>=0&&bv>0){
            auto&a=adj[u][bestArc];
            if(!vis[a.e]){vis[a.e]=1;score+=a.l;}
            pos[ci]=a.to;tim[ci]=t+a.c;paths[ci].push_back(a.to);
            pq.push({tim[ci],ci});
            continue;
        }
        // dijkstra to nearest node with feasible unvisited edge
        fill(dist.begin(),dist.end(),LLONG_MAX);
        dist[u]=0;par[u]=-1;
        priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> dq;
        dq.push({0,u});
        int target=-1;
        long long rem=T-t;
        while(!dq.empty()){
            auto [d,v]=dq.top();dq.pop();
            if(d!=dist[v])continue;
            bool ok=false;
            for(auto&a:adj[v]) if(!vis[a.e]&&d+a.c<=rem){ok=true;break;}
            if(ok&&v!=u){target=v;break;}
            for(auto&a:adj[v]){
                long long nd=d+a.c;
                if(nd>rem)continue;
                if(nd<dist[a.to]){dist[a.to]=nd;par[a.to]=v;pe[a.to]=a.e;dq.push({nd,a.to});}
            }
        }
        if(target<0)continue; // car done
        vector<int> seq;
        for(int v=target;v!=u;v=par[v])seq.push_back(v);
        reverse(seq.begin(),seq.end());
        for(int v:seq){
            int e=pe[v];
            if(!vis[e]){vis[e]=1;score+=elen[e];}
            paths[ci].push_back(v);
        }
        pos[ci]=target;tim[ci]=t+dist[target];
        pq.push({tim[ci],ci});
    }
    return score;
}

int main(){
    if(scanf("%d %d %d %d %d",&N,&M,&T,&C,&S)!=5)return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    adj.assign(N,{});elen.resize(M);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        elen[j]=l;
        adj[a].push_back({b,j,c,l});
        if(d==2)adj[b].push_back({a,j,c,l});
    }
    // choose depths based on graph size to bound work
    double avgdeg=0; for(int i=0;i<N;i++)avgdeg+=adj[i].size(); avgdeg/=max(1,N);
    vector<int> depths;
    for(int d: {3,5,7}){
        double w=pow(max(1.5,avgdeg),d);
        if(w<20000||d==3)depths.push_back(d);
    }
    long long best=-1;vector<vector<int>> bestP,P;
    for(int d:depths){
        long long s=runSim(d,P);
        if(s>best){best=s;bestP=P;}
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(C)+"\n";
    for(int i=0;i<C;i++){
        out+=to_string(bestP[i].size())+"\n";
        for(int v:bestP[i]){out+=to_string(v);out+='\n';}
    }
    fwrite(out.data(),1,out.size(),stdout);
    fprintf(stderr,"score %lld\n",best);
}
