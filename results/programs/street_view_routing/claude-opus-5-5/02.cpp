#include <bits/stdc++.h>
using namespace std;
struct Arc{int v,e,c;};
int N,M,T,C,S;
vector<vector<Arc>> adj;
vector<int> Lw;
vector<char> vis;
long long bestGain; int bestTime, bestFirst;
int D;
void dfs(int u,int depth,int tl,long long gain,int used,int first){
    if(gain>bestGain||(gain==bestGain&&gain>0&&used<bestTime)){bestGain=gain;bestTime=used;bestFirst=first;}
    if(depth==0)return;
    for(auto&a:adj[u]){
        if(a.c>tl)continue;
        int f=first<0?(int)(&a-&adj[u][0]):first;
        if(!vis[a.e]){vis[a.e]=1;dfs(a.v,depth-1,tl-a.c,gain+Lw[a.e],used+a.c,f);vis[a.e]=0;}
        else dfs(a.v,depth-1,tl-a.c,gain,used+a.c,f);
    }
}
int main(){
    auto st=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d",&N,&M,&T,&C,&S)!=5)return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    adj.assign(N,{});Lw.resize(M);vis.assign(M,0);
    for(int j=0;j<M;j++){int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);Lw[j]=l;
        adj[a].push_back({b,j,c});if(d==2)adj[b].push_back({a,j,c});}
    vector<int> pos(C,S),tm(C,0);vector<char> done(C,0);
    vector<vector<int>> path(C,vector<int>{S});
    D=8;
    // adjust depth for dense graphs
    double avgdeg=0;for(int i=0;i<N;i++)avgdeg+=adj[i].size();avgdeg/=max(1,N);
    if(avgdeg>3.0)D=6;
    vector<long long> dist(N);vector<int> par(N),parNode(N);
    long long steps=0;
    while(true){
        int ci=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(ci<0||tm[i]<tm[ci]))ci=i;
        if(ci<0)break;
        if(path[ci].size()>=999990){done[ci]=1;continue;}
        if((++steps&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>7.0)D=1;
            if(el>9.0)D=0;
        }
        int u=pos[ci],tl=T-tm[ci];
        bestGain=0;bestTime=INT_MAX;bestFirst=-1;
        if(D>0)dfs(u,D,tl,0,0,-1);
        if(bestGain>0&&bestFirst>=0){
            Arc a=adj[u][bestFirst];
            vis[a.e]=1;pos[ci]=a.v;tm[ci]+=a.c;path[ci].push_back(a.v);
            continue;
        }
        // Dijkstra to nearest node with feasible unvisited outgoing edge
        fill(dist.begin(),dist.end(),LLONG_MAX);
        priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
        dist[u]=0;pq.push({0,u});par[u]=-1;int target=-1;
        while(!pq.empty()){
            auto [d,x]=pq.top();pq.pop();
            if(d!=dist[x])continue;
            if(d>tl)break;
            bool ok=false;
            for(auto&a:adj[x])if(!vis[a.e]&&d+a.c<=tl){ok=true;break;}
            if(ok){target=x;break;}
            for(auto&a:adj[x]){long long nd=d+a.c;if(nd<=tl&&nd<dist[a.v]){dist[a.v]=nd;parNode[a.v]=x;par[a.v]=a.c;pq.push({nd,a.v});}}
        }
        if(target<0){done[ci]=1;continue;}
        // then take best unvisited edge from target
        vector<int> seq;int x=target;
        while(x!=u){seq.push_back(x);x=parNode[x];}
        reverse(seq.begin(),seq.end());
        tm[ci]+=(int)dist[target];
        for(int y:seq)path[ci].push_back(y);
        pos[ci]=target;
        int tl2=T-tm[ci];int bi=-1;double br=-1;
        for(int k=0;k<(int)adj[target].size();k++){auto&a=adj[target][k];
            if(!vis[a.e]&&a.c<=tl2){double r=(double)Lw[a.e]/max(1,a.c);if(r>br){br=r;bi=k;}}}
        if(bi>=0){Arc a=adj[target][bi];vis[a.e]=1;pos[ci]=a.v;tm[ci]+=a.c;path[ci].push_back(a.v);}
        if(path[ci].size()>1000000){ // safety trim (should not happen)
            path[ci].resize(1000000);done[ci]=1;
        }
    }
    string out;out.reserve(1<<22);
    out+=to_string(C)+"\n";
    for(int i=0;i<C;i++){out+=to_string(path[i].size())+"\n";for(int v:path[i]){out+=to_string(v);out+='\n';}}
    fwrite(out.data(),1,out.size(),stdout);
}
