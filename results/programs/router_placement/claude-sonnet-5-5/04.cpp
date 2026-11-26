#include <bits/stdc++.h>
using namespace std;
int H,W,R;
vector<string> g;
vector<char> cov,conn;
static int dxs[8]={-1,-1,-1,0,0,1,1,1},dys[8]={-1,0,1,-1,1,-1,0,1};
int gainOf(int r,int c,bool mark){
    int cnt=0;
    bool ok[11][11];
    for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
        for(int dx=0;dx<=R;dx++)for(int dy=0;dy<=R;dy++){
            int x=r+sx*dx,y=c+sy*dy;
            bool o;
            if(x<0||y<0||x>=H||y>=W||g[x][y]=='#')o=false;
            else o=(dx==0||ok[dx-1][dy])&&(dy==0||ok[dx][dy-1]);
            ok[dx][dy]=o;
            if(o&&(sx==1||dx>0)&&(sy==1||dy>0)&&g[x][y]=='.'&&!cov[x*W+y]){
                cnt++;
                if(mark)cov[x*W+y]=1;
            }
        }
    }
    return cnt;
}
int main(){
    long long Pb,Pr,B;int br,bc;
    cin>>H>>W>>R>>Pb>>Pr>>B>>br>>bc;
    g.resize(H);
    for(auto&s:g)cin>>s;
    int n=H*W;
    vector<int> par(n,-1),dist(n,-1);
    vector<int> q;q.reserve(n);
    int root=br*W+bc;
    dist[root]=0;q.push_back(root);
    for(size_t i=0;i<q.size();i++){
        int u=q[i];int r=u/W,c=u%W;
        for(int k=0;k<8;k++){
            int x=r+dxs[k],y=c+dys[k];
            if(x<0||y<0||x>=H||y>=W)continue;
            int v=x*W+y;
            if(dist[v]>=0)continue;
            dist[v]=dist[u]+1;par[v]=u;q.push_back(v);
        }
    }
    cov.assign(n,0);conn.assign(n,0);
    conn[root]=1;
    auto marg=[&](int v){
        int m=0;
        while(!conn[v]){m++;v=par[v];}
        return m;
    };
    typedef pair<double,int> P;
    priority_queue<P> pq;
    for(int v=0;v<n;v++){
        if(dist[v]<0||g[v/W][v%W]=='#')continue;
        int gn=gainOf(v/W,v%W,false);
        if(gn<=0)continue;
        double key=gn*1000.0/(Pr+Pb*(double)dist[v]);
        pq.push({key,v});
    }
    long long spent=0;
    vector<int> routers,cells;
    long long evals=0;
    while(!pq.empty()&&evals<4000000){
        auto [k,v]=pq.top();pq.pop();
        evals++;
        int m=marg(v);
        long long cost=Pr+Pb*(long long)m;
        if(spent+cost>B)continue;
        int gn=gainOf(v/W,v%W,false);
        if(gn<=0)continue;
        double key=gn*1000.0/cost;
        if(!pq.empty()&&key<pq.top().first){pq.push({key,v});continue;}
        gainOf(v/W,v%W,true);
        int u=v;
        while(!conn[u]){conn[u]=1;cells.push_back(u);u=par[u];}
        spent+=cost;
        routers.push_back(v);
    }
    sort(cells.begin(),cells.end(),[&](int a,int b){
        if(dist[a]!=dist[b])return dist[a]<dist[b];
        return a<b;});
    string out=to_string(cells.size())+"\n";
    for(int v:cells)out+=to_string(v/W)+" "+to_string(v%W)+"\n";
    out+=to_string(routers.size())+"\n";
    for(int v:routers)out+=to_string(v/W)+" "+to_string(v%W)+"\n";
    fputs(out.c_str(),stdout);
}
