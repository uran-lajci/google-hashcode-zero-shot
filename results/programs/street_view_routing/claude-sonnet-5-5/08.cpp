#include <bits/stdc++.h>
using namespace std;
struct E{int to,id,c,l;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<E>> g(N);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        g[a].push_back({b,j,c,l});
        if(d==2)g[b].push_back({a,j,c,l});
    }
    vector<char> vis(M,0);
    vector<int> pos(C,S),rem(C,T);
    vector<vector<int>> route(C,vector<int>{S});
    vector<char> done(C,0);
    vector<int> dist(N,INT_MAX),par(N,-1),pe(N,-1);
    vector<long long> gs(N,0);
    vector<int> touched;
    auto t0=chrono::steady_clock::now();
    while(true){
        int best=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(best<0||rem[i]>rem[best]))best=i;
        if(best<0)break;
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(el>8.5)break;
        int car=best;
        int cap=rem[car];
        int tgt=-1;
        for(int attempt=0;attempt<2&&tgt<0;attempt++){
            int lim=attempt==0?min(rem[car],1200):rem[car];
            for(int v:touched){dist[v]=INT_MAX;gs[v]=0;}
            touched.clear();
            priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
            dist[pos[car]]=0;par[pos[car]]=-1;gs[pos[car]]=0;
            touched.push_back(pos[car]);
            pq.push({0,pos[car]});
            double bs=0;
            while(!pq.empty()){
                auto [d,u]=pq.top();pq.pop();
                if(d>dist[u])continue;
                if(u!=pos[car]&&gs[u]>0){
                    double s=(double)gs[u]/(d+10);
                    if(s>bs){bs=s;tgt=u;}
                }
                for(auto&e:g[u]){
                    int nd=d+e.c;
                    if(nd>lim)continue;
                    long long ng=gs[u]+(vis[e.id]?0:e.l);
                    if(nd<dist[e.to]){
                        if(dist[e.to]==INT_MAX)touched.push_back(e.to);
                        dist[e.to]=nd;par[e.to]=u;pe[e.to]=e.id;gs[e.to]=ng;
                        pq.push({nd,e.to});
                    }
                }
            }
        }
        (void)cap;
        if(tgt<0){done[car]=1;continue;}
        vector<int> path;
        int v=tgt;
        while(v!=pos[car]){path.push_back(v);vis[pe[v]]=1;v=par[v];}
        reverse(path.begin(),path.end());
        for(int x:path)route[car].push_back(x);
        rem[car]-=dist[tgt];
        pos[car]=tgt;
    }
    printf("%d\n",C);
    for(int i=0;i<C;i++){
        printf("%d\n",(int)route[i].size());
        for(int x:route[i])printf("%d\n",x);
    }
}
