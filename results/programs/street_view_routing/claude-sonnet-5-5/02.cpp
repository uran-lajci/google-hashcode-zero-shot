#include <bits/stdc++.h>
using namespace std;
struct Arc{int to,id,c;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<Arc>> adj(N);
    vector<int> L(M);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;
        scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        L[j]=l;
        adj[a].push_back({b,j,c});
        if(d==2)adj[b].push_back({a,j,c});
    }
    vector<char> vis(M,0);
    vector<vector<int>> path(C,vector<int>(1,S));
    vector<int> cur(C,S),tm(C,0);
    vector<char> done(C,0);
    vector<long long> stamp(N,-1);
    vector<int> dist(N),par(N),parNode(N);
    long long st=0;
    typedef pair<int,int> P;
    int active=C;
    while(active>0){
        int k=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(k<0||tm[i]<tm[k]))k=i;
        int u=cur[k];
        int rem=T-tm[k];
        // greedy local
        int bestA=-1;double bs=0;
        for(int i=0;i<(int)adj[u].size();i++){
            Arc&a=adj[u][i];
            if(vis[a.id]||a.c>rem)continue;
            double s=(double)L[a.id]/a.c;
            int cnt=0;
            for(auto&b:adj[a.to])if(!vis[b.id])cnt++;
            s*=(1+0.1*cnt);
            if(s>bs){bs=s;bestA=i;}
        }
        if(bestA>=0){
            Arc a=adj[u][bestA];
            vis[a.id]=1;
            tm[k]+=a.c;cur[k]=a.to;path[k].push_back(a.to);
            continue;
        }
        // dijkstra
        st++;
        priority_queue<P,vector<P>,greater<P>> pq;
        stamp[u]=st;dist[u]=0;par[u]=-1;
        pq.push({0,u});
        double best=0;int bu=-1,bi=-1;
        int pops=0;
        vector<char> closed;
        static vector<long long> cl;
        if(cl.size()!=(size_t)N)cl.assign(N,-1);
        while(!pq.empty()&&pops<4000){
            auto [d,x]=pq.top();pq.pop();
            if(cl[x]==st||d!=dist[x])continue;
            cl[x]=st;pops++;
            if(d>=rem)break;
            for(int i=0;i<(int)adj[x].size();i++){
                Arc&a=adj[x][i];
                int nd=d+a.c;
                if(nd>rem)continue;
                if(!vis[a.id]){
                    double s=(double)L[a.id]/nd;
                    if(s>best){best=s;bu=x;bi=i;}
                }
                if(stamp[a.to]!=st||nd<dist[a.to]){
                    stamp[a.to]=st;dist[a.to]=nd;par[a.to]=x;
                    pq.push({nd,a.to});
                }
            }
        }
        if(bu<0){done[k]=1;active--;continue;}
        vector<int> seq;
        int x=bu;
        while(x!=u){seq.push_back(x);x=par[x];}
        reverse(seq.begin(),seq.end());
        for(int y:seq){
            // find arc cur->y cheapest unvisited-or-any
            int bc=INT_MAX,bid=-1;
            for(auto&a:adj[cur[k]])if(a.to==y&&a.c<bc){bc=a.c;bid=a.id;}
            vis[bid]=1;
            tm[k]+=bc;cur[k]=y;path[k].push_back(y);
        }
        Arc a=adj[bu][bi];
        vis[a.id]=1;
        tm[k]+=a.c;cur[k]=a.to;path[k].push_back(a.to);
    }
    printf("%d\n",C);
    for(int i=0;i<C;i++){
        printf("%d\n",(int)path[i].size());
        for(int v:path[i])printf("%d\n",v);
    }
}
