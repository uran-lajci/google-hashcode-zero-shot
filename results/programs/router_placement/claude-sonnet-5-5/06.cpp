#include <bits/stdc++.h>
using namespace std;
int H,W,R;
long long Pb,Pr,B;
vector<string> g;
vector<char> cov;
static inline bool wallc(int r,int c){return g[r][c]=='#';}
int evalg(int r,int c,bool apply){
    int cnt=0;
    bool ok[11][11];
    for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
        for(int i=0;i<=R;i++)for(int j=0;j<=R;j++){
            int x=r+sx*i,y=c+sy*j;
            bool o=x>=0&&x<H&&y>=0&&y<W&&g[x][y]!='#';
            if(o&&i>0)o=ok[i-1][j];
            if(o&&j>0)o=ok[i][j-1];
            ok[i][j]=o;
            if(o&&(i>0||sx==1)&&(j>0||sy==1)&&g[x][y]=='.'&&!cov[x*W+y]){
                cnt++;
                if(apply)cov[x*W+y]=1;
            }
        }
    }
    return cnt;
}
int main(){
    auto t0=chrono::steady_clock::now();
    int br,bc;
    scanf("%d %d %d",&H,&W,&R);
    scanf("%lld %lld %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    static char buf[2000];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    int n=H*W;
    cov.assign(n,0);
    vector<int> par(n,-2),dist(n,0);
    vector<char> conn(n,0);
    {
        vector<int> q;q.reserve(n);
        int s=br*W+bc;par[s]=-1;q.push_back(s);
        for(size_t h=0;h<q.size();h++){
            int u=q[h];int r=u/W,c=u%W;
            for(int dx=-1;dx<=1;dx++)for(int dy=-1;dy<=1;dy++){
                if(!dx&&!dy)continue;
                int x=r+dx,y=c+dy;
                if(x<0||y<0||x>=H||y>=W)continue;
                int v=x*W+y;
                if(par[v]!=-2)continue;
                par[v]=u;dist[v]=dist[u]+1;q.push_back(v);
            }
        }
        conn[s]=1;
    }
    typedef pair<double,int> P;
    priority_queue<P> pq;
    {
        vector<P> init;init.reserve(n);
        for(int r=0;r<H;r++)for(int c=0;c<W;c++){
            if(g[r][c]=='#')continue;
            int gn=evalg(r,c,false);
            if(gn<=0)continue;
            double cost=Pr+Pb*dist[r*W+c];
            init.push_back({gn*1000.0/cost,r*W+c});
        }
        pq=priority_queue<P>(less<P>(),std::move(init));
    }
    long long rem=B;
    vector<int> outc,routers;
    int iter=0;
    bool timeup=false;
    while(!pq.empty()&&!timeup){
        if((++iter&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>8.3)break;
        }
        P tp=pq.top();pq.pop();
        int v=tp.second;int r=v/W,c=v%W;
        int gn=evalg(r,c,false);
        if(gn<=0)continue;
        int k=0;int u=v;
        while(!conn[u]){k++;u=par[u];}
        long long cost=Pr+Pb*k;
        double key=gn*1000.0/cost;
        if(!pq.empty()&&key<pq.top().first-1e-12){pq.push({key,v});continue;}
        if(1000LL*gn<=cost||cost>rem)continue;
        // accept
        vector<int> path;
        u=v;
        while(!conn[u]){path.push_back(u);conn[u]=1;u=par[u];}
        for(int i=(int)path.size()-1;i>=0;i--)outc.push_back(path[i]);
        routers.push_back(v);
        rem-=cost;
        evalg(r,c,true);
    }
    string o;
    o+=to_string(outc.size())+"\n";
    for(int u:outc)o+=to_string(u/W)+" "+to_string(u%W)+"\n";
    o+=to_string(routers.size())+"\n";
    for(int u:routers)o+=to_string(u/W)+" "+to_string(u%W)+"\n";
    fputs(o.c_str(),stdout);
}
