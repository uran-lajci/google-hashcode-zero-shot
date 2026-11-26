#include <bits/stdc++.h>
using namespace std;

struct Adj{int to,e,c;};
int N,M,C,S; long long T;
vector<vector<Adj>> adj;
vector<long long> Lw;
vector<char> vis;
vector<int> tmpm;
long long ops=0;
int D;
double bestScore; int bestFirst; long long remT;
vector<double> firstNoise;

void dfs(int u,int depth,long long t,long long gain,int fi){
    const auto& A=adj[u];
    for(int i=0;i<(int)A.size();i++){
        const Adj&a=A[i];
        long long nt=t+a.c;
        if(nt>remT) continue;
        ops++;
        long long ng=gain+((vis[a.e]||tmpm[a.e])?0:Lw[a.e]);
        int f=depth==0?i:fi;
        if(ng>0){
            double s=(double)ng/(double)max(1LL,nt)*firstNoise[f];
            if(s>bestScore){bestScore=s;bestFirst=f;}
        }
        if(depth+1<D){
            tmpm[a.e]++;
            dfs(a.to,depth+1,nt,ng,f);
            tmpm[a.e]--;
        }
    }
}

int main(){
    auto st=chrono::steady_clock::now();
    if(scanf("%d %d %lld %d %d",&N,&M,&T,&C,&S)!=5)return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    adj.assign(N,{});Lw.resize(M);
    for(int j=0;j<M;j++){int a,b,d;long long c,l;scanf("%d %d %d %lld %lld",&a,&b,&d,&c,&l);
        Lw[j]=l;adj[a].push_back({b,j,(int)c});if(d==2)adj[b].push_back({a,j,(int)c});}
    vis.assign(M,0);tmpm.assign(M,0);
    vector<long long> dist(N);vector<int> stamp(N,0),par(N),parNode(N);int curStamp=0;
    mt19937 rng(12345);
    long long budget=400000000LL;
    long long bestTotal=-1;vector<vector<int>> bestPaths;
    long long lastRunOps=0;
    for(int run=0;;run++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        if(run>0&&(ops+lastRunOps*1.2>budget||el>9.0))break;
        if(run>200)break;
        long long startOps=ops;
        double noise=run==0?0.0:(0.05+0.3*((run%5)/4.0));
        D=5+(run%3);
        fill(vis.begin(),vis.end(),0);
        vector<int> pos(C,S);vector<long long> tm(C,0);vector<char> done(C,0);
        vector<vector<int>> paths(C,vector<int>{S});
        long long total=0;
        while(true){
            int c=-1;
            for(int i=0;i<C;i++)if(!done[i]&&(c<0||tm[i]<tm[c]))c=i;
            if(c<0)break;
            if(el>9.5){done[c]=1;continue;}
            int u=pos[c];remT=T-tm[c];
            firstNoise.assign(adj[u].size(),1.0);
            if(noise>0)for(auto&x:firstNoise)x=1.0+noise*(rng()%10000)/10000.0;
            bestScore=-1;bestFirst=-1;
            dfs(u,0,0,0,0);
            if(bestFirst>=0){
                const Adj&a=adj[u][bestFirst];
                if(!vis[a.e]){vis[a.e]=1;total+=Lw[a.e];}
                tm[c]+=a.c;pos[c]=a.to;paths[c].push_back(a.to);
                continue;
            }
            // dijkstra to nearest node with unvisited edge reachable
            curStamp++;
            priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
            stamp[u]=curStamp;dist[u]=0;par[u]=-1;pq.push({0,u});
            int target=-1;
            while(!pq.empty()){
                auto [d,x]=pq.top();pq.pop();
                if(d!=dist[x])continue;
                bool ok=false;
                for(auto&a:adj[x]){ops++;if(!vis[a.e]&&d+a.c<=remT){ok=true;break;}}
                if(ok){target=x;break;}
                for(auto&a:adj[x]){
                    long long nd=d+a.c;if(nd>remT)continue;ops++;
                    if(stamp[a.to]!=curStamp||nd<dist[a.to]){
                        stamp[a.to]=curStamp;dist[a.to]=nd;par[a.to]=a.c;parNode[a.to]=x;pq.push({nd,a.to});
                    }
                }
            }
            if(target<0||target==u){done[c]=1;continue;}
            vector<int> seq;int x=target;
            while(x!=u){seq.push_back(x);x=parNode[x];}
            reverse(seq.begin(),seq.end());
            int prev=u;
            for(int y:seq){
                // find edge prev->y
                const Adj*be=nullptr;
                for(auto&a:adj[prev])if(a.to==y){be=&a;break;}
                if(!vis[be->e]){vis[be->e]=1;total+=Lw[be->e];}
                tm[c]+=be->c;paths[c].push_back(y);prev=y;
            }
            pos[c]=target;
        }
        lastRunOps=ops-startOps;
        if(total>bestTotal){bestTotal=total;bestPaths=paths;}
    }
    string out;out.reserve(1<<22);
    out+=to_string(C)+"\n";
    for(int i=0;i<C;i++){
        out+=to_string(bestPaths[i].size())+"\n";
        for(int v:bestPaths[i]){out+=to_string(v);out+='\n';}
    }
    fwrite(out.data(),1,out.size(),stdout);
    fprintf(stderr,"score %lld\n",bestTotal);
}
