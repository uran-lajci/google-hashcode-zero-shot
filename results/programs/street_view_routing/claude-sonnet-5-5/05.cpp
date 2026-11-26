#include <bits/stdc++.h>
using namespace std;
struct Arc{int to,id,c,l;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<Arc>> g(N);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;
        scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        g[a].push_back({b,j,c,l});
        if(d==2)g[b].push_back({a,j,c,l});
    }
    vector<char> cov(M,0);
    vector<vector<int>> route(C,vector<int>(1,S));
    vector<int> pos(C,S),tm(C,0);
    vector<char> done(C,0);
    vector<int> stamp(N,0),par(N,-1),parArc(N,-1);
    vector<long long> dist(N),pg(N);
    int cur=0;
    auto search=[&](int car,int limit)->bool{
        int src=pos[car];
        int rem=T-tm[car];
        cur++;
        typedef pair<long long,int> P;
        priority_queue<P,vector<P>,greater<P>> pq;
        stamp[src]=cur;dist[src]=0;pg[src]=0;par[src]=-1;
        pq.push({0,src});
        vector<char> fin;
        vector<int> popped;
        double best=0;int bu=-1;int bArc=-1;
        int cnt=0;
        unordered_set<int> seen;
        while(!pq.empty()){
            auto [d,u]=pq.top();pq.pop();
            if(d!=dist[u])continue;
            if(seen.count(u))continue;
            seen.insert(u);
            cnt++;
            for(int k=0;k<(int)g[u].size();k++){
                Arc&a=g[u][k];
                long long nd=d+a.c;
                if(nd>rem)continue;
                if(!cov[a.id]){
                    double gain=pg[u]+a.l;
                    double r=gain/(double)nd;
                    if(r>best){best=r;bu=u;bArc=k;}
                }
                if(stamp[a.to]!=cur||nd<dist[a.to]){
                    stamp[a.to]=cur;dist[a.to]=nd;
                    pg[a.to]=pg[u]+(cov[a.id]?0:a.l);
                    par[a.to]=u;parArc[a.to]=a.id;
                    pq.push({nd,a.to});
                }
            }
            if(cnt>=limit&&bu>=0)break;
        }
        if(bu<0)return false;
        vector<int> path;
        int v=bu;
        while(v!=src){path.push_back(v);v=par[v];}
        path.push_back(src);
        reverse(path.begin(),path.end());
        path.push_back(g[bu][bArc].to);
        for(size_t i=1;i<path.size();i++){
            int a=path[i-1],b=path[i];
            const Arc*best2=nullptr;
            for(auto&x:g[a])if(x.to==b){
                if(!best2||x.c<best2->c)best2=&x;
            }
            cov[best2->id]=1;
            tm[car]+=best2->c;
            route[car].push_back(b);
        }
        pos[car]=path.back();
        return true;
    };
    while(true){
        int c=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(c<0||tm[i]<tm[c]))c=i;
        if(c<0)break;
        if(!search(c,300)){
            if(!search(c,INT_MAX))done[c]=1;
        }
    }
    printf("%d\n",C);
    for(int i=0;i<C;i++){
        printf("%d\n",(int)route[i].size());
        for(int v:route[i])printf("%d\n",v);
    }
}
