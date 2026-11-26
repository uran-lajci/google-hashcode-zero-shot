#include <bits/stdc++.h>
using namespace std;
int H,W,R;
long long Pb,Pr,B;
vector<string> g;
vector<int> d;
vector<char> cov;
static int outc[500];
int evalCell(int a,int b,int &cnt,bool mark){
    // returns gain
    int gain=0;cnt=0;
    bool ok[11][11];
    for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
        for(int i=0;i<=R;i++)for(int j=0;j<=R;j++){
            int x=a+sx*i,y=b+sy*j;
            bool o=x>=0&&x<H&&y>=0&&y<W&&g[x][y]!='#';
            if(o&&i>0&&!ok[i-1][j])o=false;
            if(o&&j>0&&!ok[i][j-1])o=false;
            ok[i][j]=o;
            if(!o)continue;
            if(sx==-1&&i==0)continue;
            if(sy==-1&&j==0)continue;
            if(g[x][y]=='.'&&!cov[x*W+y]){
                gain++;
                if(mark)cov[x*W+y]=1;
            }
        }
    }
    return gain;
}
int main(){
    auto t0=chrono::steady_clock::now();
    scanf("%d %d %d",&H,&W,&R);
    scanf("%lld %lld %lld",&Pb,&Pr,&B);
    int br,bc;scanf("%d %d",&br,&bc);
    g.resize(H);
    char buf[1105];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    d.assign(H*W,0);cov.assign(H*W,0);
    for(int i=0;i<H;i++)for(int j=0;j<W;j++)d[i*W+j]=max(abs(i-br),abs(j-bc));
    typedef pair<double,int> P;
    priority_queue<P> pq;
    int c;
    for(int i=0;i<H;i++)for(int j=0;j<W;j++){
        if(g[i][j]=='#')continue;
        int gn=evalCell(i,j,c,false);
        if(gn>0)pq.push({(double)gn/(Pr+Pb*d[i*W+j]),i*W+j});
    }
    long long spent=0;
    vector<int> bb,routers;
    int dx[8]={-1,-1,-1,0,0,1,1,1},dy[8]={-1,0,1,-1,1,-1,0,1};
    int iter=0;
    while(!pq.empty()){
        if((++iter&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>8.5)break;
        }
        P t=pq.top();pq.pop();
        int id=t.second;int a=id/W,b=id%W;
        int gn=evalCell(a,b,c,false);
        if(gn<=0)continue;
        long long cost=Pr+Pb*(long long)d[id];
        double key=(double)gn/cost;
        if(!pq.empty()&&key<pq.top().first){pq.push({key,id});continue;}
        if(spent+cost>B)continue;
        spent+=cost;
        evalCell(a,b,c,true);
        routers.push_back(id);
        // connect path
        vector<int> path;
        int cur=id;
        vector<int> newc;
        while(d[cur]>0){
            path.push_back(cur);
            int x=cur/W,y=cur%W;
            int nxt=-1;
            for(int k=0;k<8;k++){
                int nx=x+dx[k],ny=y+dy[k];
                if(nx<0||ny<0||nx>=H||ny>=W)continue;
                if(d[nx*W+ny]==d[cur]-1){nxt=nx*W+ny;break;}
            }
            cur=nxt;
        }
        for(int k=path.size()-1;k>=0;k--)bb.push_back(path[k]);
        // set d=0 and relax
        vector<int> q;
        for(int p:path){d[p]=0;q.push_back(p);}
        for(size_t h=0;h<q.size();h++){
            int u=q[h];int x=u/W,y=u%W;
            for(int k=0;k<8;k++){
                int nx=x+dx[k],ny=y+dy[k];
                if(nx<0||ny<0||nx>=H||ny>=W)continue;
                int v=nx*W+ny;
                if(d[v]>d[u]+1){d[v]=d[u]+1;q.push_back(v);}
            }
        }
    }
    printf("%d\n",(int)bb.size());
    for(int p:bb)printf("%d %d\n",p/W,p%W);
    printf("%d\n",(int)routers.size());
    for(int p:routers)printf("%d %d\n",p/W,p%W);
}
