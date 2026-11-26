#include <bits/stdc++.h>
using namespace std;
struct Arc{int to,id,c;};
int main(){
    int N,M,T,C,S;
    scanf("%d %d %d %d %d",&N,&M,&T,&C,&S);
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<Arc>> g(N);
    vector<int> L(M);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;
        scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        L[j]=l;
        g[a].push_back({b,j,c});
        if(d==2)g[b].push_back({a,j,c});
    }
    vector<char> cov(M,0);
    vector<vector<int>> path(C,vector<int>(1,S));
    vector<int> cur(C,S),rem(C,T);
    vector<char> done(C,0);
    vector<long long> dist(N,0);
    vector<int> stamp(N,0),settledStamp(N,0);
    vector<int> parNode(N),parArc(N);
    int st=0;
    int active=C;
    while(active>0){
        int car=-1;
        for(int i=0;i<C;i++)if(!done[i]&&(car<0||rem[i]>rem[car]))car=i;
        int src=cur[car],R=rem[car];
        st++;
        typedef pair<long long,int> P;
        priority_queue<P,vector<P>,greater<P>> pq;
        dist[src]=0;stamp[src]=st;parNode[src]=-1;
        pq.push({0,src});
        double best=-1;int bu=-1,ba=-1;
        int settled=0;
        while(!pq.empty()){
            auto [d,u]=pq.top();pq.pop();
            if(settledStamp[u]==st||d!=dist[u])continue;
            settledStamp[u]=st;
            settled++;
            if(settled>400&&bu>=0)break;
            for(int k=0;k<(int)g[u].size();k++){
                const Arc&a=g[u][k];
                long long nd=d+a.c;
                if(nd>R)continue;
                if(!cov[a.id]){
                    double sc=(double)L[a.id]/(double)nd;
                    if(sc>best){best=sc;bu=u;ba=k;}
                }
                if(stamp[a.to]!=st||nd<dist[a.to]){
                    stamp[a.to]=st;dist[a.to]=nd;
                    parNode[a.to]=u;parArc[a.to]=k;
                    pq.push({nd,a.to});
                }
            }
        }
        if(bu<0){done[car]=1;active--;continue;}
        vector<pair<int,int>> seq;
        int x=bu;
        while(parNode[x]>=0){
            seq.push_back({parNode[x],parArc[x]});
            x=parNode[x];
        }
        reverse(seq.begin(),seq.end());
        seq.push_back({bu,ba});
        for(auto&pr:seq){
            const Arc&a=g[pr.first][pr.second];
            cov[a.id]=1;
            rem[car]-=a.c;
            path[car].push_back(a.to);
            cur[car]=a.to;
        }
    }
    string out;
    out+=to_string(C)+"\n";
    for(int i=0;i<C;i++){
        out+=to_string(path[i].size())+"\n";
        for(int v:path[i])out+=to_string(v)+"\n";
    }
    fputs(out.c_str(),stdout);
}
