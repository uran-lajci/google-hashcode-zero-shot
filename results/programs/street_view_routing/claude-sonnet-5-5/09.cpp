#include <bits/stdc++.h>
using namespace std;
struct Arc{int to,id,c,l;};
int main(){
    int N,M,T,C,S;
    if(scanf("%d %d %d %d %d",&N,&M,&T,&C,&S)!=5)return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<Arc>> g(N);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        g[a].push_back({b,j,c,l});
        if(d==2)g[b].push_back({a,j,c,l});
    }
    vector<char> cov(M,0);
    vector<vector<int>> path(C,vector<int>(1,S));
    vector<int> cur(C,S),rem(C,T);
    vector<char> done(C,0);
    vector<long long> dist(N,0);
    vector<int> stamp(N,0),par(N,-1);
    vector<int> parArc(N,-1);
    int st=0;
    while(true){
        int car=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(car<0||rem[i]>rem[car]))car=i;
        if(car<0)break;
        int u=cur[car];
        // best adjacent uncovered arc
        int best=-1;double bs=-1;
        for(int k=0;k<(int)g[u].size();k++){
            auto&a=g[u][k];
            if(cov[a.id]||a.c>rem[car])continue;
            double s=(double)a.l/(a.c+1e-9);
            if(s>bs){bs=s;best=k;}
        }
        if(best>=0){
            auto&a=g[u][best];
            cov[a.id]=1;rem[car]-=a.c;cur[car]=a.to;path[car].push_back(a.to);
            continue;
        }
        // Dijkstra to nearest node with uncovered arc that fits
        st++;
        typedef pair<long long,int> P;
        priority_queue<P,vector<P>,greater<P>> pq;
        stamp[u]=st;dist[u]=0;par[u]=-1;pq.push({0,u});
        vector<char> fin;
        set<int> closed;
        int target=-1;
        while(!pq.empty()){
            auto [d,x]=pq.top();pq.pop();
            if(d!=dist[x]||stamp[x]!=st)continue;
            if(closed.count(x))continue;
            closed.insert(x);
            if(d>rem[car])break;
            bool ok=false;
            if(x!=u){
                for(auto&a:g[x])if(!cov[a.id]&&d+a.c<=rem[car]){ok=true;break;}
            }
            if(ok){target=x;break;}
            for(auto&a:g[x]){
                long long nd=d+a.c;
                if(nd>rem[car])continue;
                if(stamp[a.to]!=st||nd<dist[a.to]){
                    stamp[a.to]=st;dist[a.to]=nd;par[a.to]=x;
                    pq.push({nd,a.to});
                }
            }
        }
        if(target<0){done[car]=1;continue;}
        vector<int> p;
        for(int x=target;x!=u;x=par[x])p.push_back(x);
        reverse(p.begin(),p.end());
        int from=u;
        for(int x:p){
            // find cheapest arc from->x
            int bc=INT_MAX;
            for(auto&a:g[from])if(a.to==x&&a.c<bc)bc=a.c;
            for(auto&a:g[from])if(a.to==x&&a.c==bc){cov[a.id]=1;break;}
            rem[car]-=bc;
            path[car].push_back(x);
            from=x;
        }
        cur[car]=target;
    }
    printf("%d\n",C);
    for(int i=0;i<C;i++){
        printf("%d\n",(int)path[i].size());
        for(int x:path[i])printf("%d\n",x);
    }
}
