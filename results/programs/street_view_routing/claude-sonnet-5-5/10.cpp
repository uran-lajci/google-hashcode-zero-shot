#include <bits/stdc++.h>
using namespace std;
struct E{int to,c,l,id;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<E>> g(N);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        g[a].push_back({b,c,l,j});
        if(d==2)g[b].push_back({a,c,l,j});
    }
    vector<char> cov(M,0);
    vector<vector<int>> route(C,vector<int>{S});
    vector<int> pos(C,S),used(C,0);
    vector<char> done(C,0);
    vector<long long> dist(N,0);
    vector<int> stamp(N,0),par(N,-1),pe(N,-1);
    int cur=0;
    const long long INF=1e18;
    while(true){
        int car=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(car<0||used[i]<used[car]))car=i;
        if(car<0)break;
        int rem=T-used[car];
        int src=pos[car];
        cur++;
        typedef pair<long long,int> P;
        priority_queue<P,vector<P>,greater<P>> pq;
        stamp[src]=cur;dist[src]=0;par[src]=-1;
        pq.push({0,src});
        vector<char> settled; 
        double best=-1;int bu=-1,be=-1;
        int found=0,cnt=0;
        while(!pq.empty()){
            auto [d,u]=pq.top();pq.pop();
            if(d>dist[u])continue;
            if(d>rem)break;
            cnt++;
            for(auto&e:g[u]){
                if(!cov[e.id]&&d+e.c<=rem){
                    double s=(double)e.l/(d+e.c);
                    if(s>best){best=s;bu=u;be=e.id;}
                    found=1;
                }
            }
            if(found){
                // allow some more exploration
                if(cnt>0&&found>0){found++;}
                if(found>40)break;
            }
            for(auto&e:g[u]){
                long long nd=d+e.c;
                if(nd>rem)continue;
                if(stamp[e.to]!=cur||nd<dist[e.to]){
                    stamp[e.to]=cur;dist[e.to]=nd;par[e.to]=u;pe[e.to]=e.id;
                    pq.push({nd,e.to});
                }
            }
        }
        if(bu<0){done[car]=1;continue;}
        // reconstruct path to bu
        vector<int> path;
        int x=bu;
        while(x!=src){
            path.push_back(x);
            cov[pe[x]]=1;
            x=par[x];
        }
        reverse(path.begin(),path.end());
        for(int v:path)route[car].push_back(v);
        used[car]+=(int)dist[bu];
        // final edge
        for(auto&e:g[bu])if(e.id==be){
            cov[be]=1;
            route[car].push_back(e.to);
            used[car]+=e.c;
            pos[car]=e.to;
            break;
        }
    }
    printf("%d\n",C);
    for(int i=0;i<C;i++){
        printf("%d\n",(int)route[i].size());
        for(int v:route[i])printf("%d\n",v);
    }
}
