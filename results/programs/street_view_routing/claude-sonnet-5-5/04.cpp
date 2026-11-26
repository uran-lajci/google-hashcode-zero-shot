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
    vector<char> cov(M,0);
    vector<vector<int>> route(C,vector<int>(1,S));
    vector<int> cur(C,S),used(C,0);
    vector<char> done(C,0);
    auto t0=clock();
    vector<long long> dist(N);
    vector<int> par(N),pe(N),stamp(N,0);
    int st=0;
    int active=C;
    while(active>0){
        for(int k=0;k<C;k++){
            if(done[k])continue;
            double el=(double)(clock()-t0)/CLOCKS_PER_SEC;
            if(el>8.5){done[k]=1;active--;continue;}
            int u=cur[k];
            int rem=T-used[k];
            int best=-1;double bs=-1;
            for(int i=0;i<(int)g[u].size();i++){
                E&e=g[u][i];
                if(cov[e.id]||e.c>rem)continue;
                double s=(double)e.l/(e.c+1);
                if(s>bs){bs=s;best=i;}
            }
            if(best>=0){
                E&e=g[u][best];
                cov[e.id]=1;used[k]+=e.c;cur[k]=e.to;route[k].push_back(e.to);
                continue;
            }
            // dijkstra
            st++;
            priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
            dist[u]=0;stamp[u]=st;par[u]=-1;
            pq.push({0,u});
            vector<char> fin;
            int target=-1;
            static vector<int> fst(N,0);
            while(!pq.empty()){
                auto [d,x]=pq.top();pq.pop();
                if(d>dist[x]||fst[x]==st)continue;
                fst[x]=st;
                if(d>rem)break;
                bool ok=false;
                for(E&e:g[x])if(!cov[e.id]&&d+e.c<=rem){ok=true;break;}
                if(ok&&x!=u){target=x;break;}
                for(E&e:g[x]){
                    long long nd=d+e.c;
                    if(nd>rem)continue;
                    if(stamp[e.to]!=st||nd<dist[e.to]){
                        stamp[e.to]=st;dist[e.to]=nd;par[e.to]=x;
                        pq.push({nd,e.to});
                    }
                }
            }
            if(target<0){done[k]=1;active--;continue;}
            vector<int> path;
            int x=target;
            while(x!=u){path.push_back(x);x=par[x];}
            reverse(path.begin(),path.end());
            int p=u;
            for(int y:path){
                for(E&e:g[p])if(e.to==y){
                    // choose min cost edge (only one exists per pair)
                    cov[e.id]=1;used[k]+=e.c;break;
                }
                route[k].push_back(y);
                p=y;
            }
            cur[k]=target;
        }
    }
    string out;
    out+=to_string(C)+"\n";
    for(int k=0;k<C;k++){
        out+=to_string(route[k].size())+"\n";
        for(int v:route[k])out+=to_string(v)+"\n";
    }
    fputs(out.c_str(),stdout);
}
