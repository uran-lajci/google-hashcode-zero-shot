#include <bits/stdc++.h>
using namespace std;
struct E{int to,id,c;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<E>> g(N);
    vector<int> L(M);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;
        scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        L[j]=l;
        g[a].push_back({b,j,c});
        if(d==2)g[b].push_back({a,j,c});
    }
    vector<char> vis(M,0);
    vector<int> pos(C,S),tm(C,0);
    vector<char> done(C,0);
    vector<vector<int>> path(C,vector<int>{S});
    vector<int> dist(N,INT_MAX),par(N,-1);
    vector<int> touched;
    int active=C;
    while(active>0){
        for(int c=0;c<C;c++){
            if(done[c])continue;
            int rem=T-tm[c];
            bool found=false;
            for(int pass=0;pass<2&&!found;pass++){
                int limit=pass==0?300:N+5;
                touched.clear();
                priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
                dist[pos[c]]=0;par[pos[c]]=-1;touched.push_back(pos[c]);
                pq.push({0,pos[c]});
                int pops=0;
                double best=0;int bu=-1,be=-1;
                vector<char> closed;
                while(!pq.empty()&&pops<limit){
                    auto [d,u]=pq.top();pq.pop();
                    if(d>dist[u])continue;
                    pops++;
                    for(int k=0;k<(int)g[u].size();k++){
                        E&e=g[u][k];
                        int nd=d+e.c;
                        if(nd>rem)continue;
                        if(!vis[e.id]){
                            double v=(double)L[e.id]/nd;
                            if(v>best){best=v;bu=u;be=k;}
                        }
                        if(nd<dist[e.to]){
                            if(dist[e.to]==INT_MAX)touched.push_back(e.to);
                            dist[e.to]=nd;par[e.to]=u;
                            pq.push({nd,e.to});
                        }
                    }
                }
                if(bu>=0){
                    found=true;
                    vector<int> seq;
                    int x=bu;
                    while(x!=-1){seq.push_back(x);x=par[x];}
                    reverse(seq.begin(),seq.end());
                    seq.push_back(g[bu][be].to);
                    int totalT=dist[bu]+g[bu][be].c;
                    // mark visited along the path
                    for(size_t i=0;i+1<seq.size();i++){
                        int u=seq[i],w=seq[i+1];
                        int bi=-1,bc=INT_MAX;
                        for(auto&e:g[u])if(e.to==w&&e.c<bc){bc=e.c;bi=e.id;}
                        if(i+2==seq.size()){bi=g[bu][be].id;}
                        vis[bi]=1;
                        path[c].push_back(w);
                    }
                    tm[c]+=totalT;
                    pos[c]=seq.back();
                }
                for(int t:touched){dist[t]=INT_MAX;par[t]=-1;}
            }
            if(!found){done[c]=1;active--;}
        }
    }
    printf("%d\n",C);
    for(int c=0;c<C;c++){
        printf("%d\n",(int)path[c].size());
        for(int x:path[c])printf("%d\n",x);
    }
}
