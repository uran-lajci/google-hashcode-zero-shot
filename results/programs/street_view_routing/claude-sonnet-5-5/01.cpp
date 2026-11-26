#include <bits/stdc++.h>
using namespace std;
struct E{int to,id,c;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<E>> g(N);
    vector<int> L(M),cost(M);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        L[j]=l;cost[j]=c;
        g[a].push_back({b,j,c});
        if(d==2)g[b].push_back({a,j,c});
    }
    vector<char> cov(M,0);
    vector<vector<int>> route(C,vector<int>{S});
    vector<int> cur(C,S),rem(C,T);
    vector<char> done(C,0);
    vector<int> dist(N,INT_MAX),par(N,-1),pe(N,-1),stamp(N,0);
    int st=0;
    int active=C;
    clock_t t0=clock();
    while(active>0){
        if((double)(clock()-t0)/CLOCKS_PER_SEC>8.0)break;
        for(int k=0;k<C;k++){
            if(done[k])continue;
            for(int pass=0;pass<2;pass++){
                int limitNodes=pass==0?400:INT_MAX;
                st++;
                int s=cur[k],R=rem[k];
                priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
                stamp[s]=st;dist[s]=0;par[s]=-1;pe[s]=-1;
                pq.push({0,s});
                double best=-1;int bu=-1,be=-1,bv=-1;
                int settled=0;
                while(!pq.empty()){
                    auto [d,u]=pq.top();pq.pop();
                    if(d>dist[u])continue;
                    if(d>R)break;
                    if(++settled>limitNodes&&bu>=0)break;
                    for(auto&e:g[u]){
                        if(d+e.c>R)continue;
                        if(!cov[e.id]){
                            double r=(double)L[e.id]/(d+e.c);
                            if(r>best){best=r;bu=u;be=e.id;bv=e.to;}
                        }
                        int nd=d+e.c;
                        if(stamp[e.to]!=st||nd<dist[e.to]){
                            stamp[e.to]=st;dist[e.to]=nd;par[e.to]=u;pe[e.to]=e.id;
                            pq.push({nd,e.to});
                        }
                    }
                }
                if(bu>=0){
                    vector<int> path;
                    int x=bu;
                    while(x!=s){path.push_back(x);x=par[x];}
                    // mark edges along path
                    int x2=bu;
                    while(x2!=s){cov[pe[x2]]=1;x2=par[x2];}
                    reverse(path.begin(),path.end());
                    for(int v:path)route[k].push_back(v);
                    route[k].push_back(bv);
                    cov[be]=1;
                    rem[k]-=dist[bu]+cost[be];
                    cur[k]=bv;
                    break;
                }
                if(pass==1){done[k]=1;active--;}
            }
        }
    }
    printf("%d\n",C);
    for(int k=0;k<C;k++){
        printf("%d\n",(int)route[k].size());
        for(int v:route[k])printf("%d\n",v);
    }
}
