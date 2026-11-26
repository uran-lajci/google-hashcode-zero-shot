#include <bits/stdc++.h>
using namespace std;
int H,W,R;
long long Pb,Pr,B;
int main(){
    scanf("%d %d %d",&H,&W,&R);
    scanf("%lld %lld %lld",&Pb,&Pr,&B);
    int br,bc;scanf("%d %d",&br,&bc);
    vector<string> g(H);
    char buf[2048];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    int S=W+1;
    vector<int> P((size_t)(H+1)*S,0);
    for(int i=0;i<H;i++)for(int j=0;j<W;j++)
        P[(i+1)*S+j+1]=P[i*S+j+1]+P[(i+1)*S+j]-P[i*S+j]+(g[i][j]=='#');
    auto wallsIn=[&](int r1,int c1,int r2,int c2){
        return P[(r2+1)*S+c2+1]-P[r1*S+c2+1]-P[(r2+1)*S+c1]+P[r1*S+c1];
    };
    int N=H*W;
    vector<char> conn(N,0),cov(N,0);
    vector<int> par(N),dist(N);
    conn[br*W+bc]=1;
    vector<int> cl,routers;
    long long spent=0;
    int K=(2*R+1)*(2*R+1);
    double fr[4]={0.5,0.3,0.15,0};
    vector<int> q(N);
    for(int pass=0;pass<4;pass++){
        int T=max(1,(int)(fr[pass]*K));
        int qh=0,qt=0;
        for(int i=0;i<N;i++){
            if(conn[i]){dist[i]=0;par[i]=-1;q[qt++]=i;}
            else dist[i]=-1;
        }
        while(qh<qt){
            int u=q[qh++];
            int r=u/W,c=u%W;
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){
                if(!dr&&!dc)continue;
                int nr=r+dr,nc=c+dc;
                if(nr<0||nc<0||nr>=H||nc>=W)continue;
                int v=nr*W+nc;
                if(dist[v]>=0)continue;
                dist[v]=dist[u]+1;par[v]=u;q[qt++]=v;
            }
        }
        for(int k=0;k<qt;k++){
            int u=q[k];
            int a=u/W,b=u%W;
            if(g[a][b]=='#')continue;
            if(spent+Pr+Pb*(long long)dist[u]>B){
                // estimate may overestimate; check actual only if conn path shorter
                int cnt=0;int x=u;
                while(!conn[x]){cnt++;x=par[x];}
                if(spent+Pr+Pb*cnt>B)continue;
            }
            int r1=max(0,a-R),r2=min(H-1,a+R),c1=max(0,b-R),c2=min(W-1,b+R);
            int gain=0;
            for(int x=r1;x<=r2;x++)for(int y=c1;y<=c2;y++){
                if(g[x][y]!='.'||cov[x*W+y])continue;
                if(wallsIn(min(a,x),min(b,y),max(a,x),max(b,y))==0)gain++;
            }
            if(gain<T)continue;
            int cnt=0;int x=u;
            while(!conn[x]){cnt++;x=par[x];}
            long long cost=Pr+Pb*cnt;
            if(spent+cost>B)continue;
            spent+=cost;
            vector<int> path;
            x=u;
            while(!conn[x]){path.push_back(x);conn[x]=1;x=par[x];}
            for(int i=(int)path.size()-1;i>=0;i--)cl.push_back(path[i]);
            routers.push_back(u);
            for(int x2=r1;x2<=r2;x2++)for(int y=c1;y<=c2;y++){
                if(g[x2][y]!='.')continue;
                if(wallsIn(min(a,x2),min(b,y),max(a,x2),max(b,y))==0)cov[x2*W+y]=1;
            }
        }
    }
    printf("%d\n",(int)cl.size());
    for(int u:cl)printf("%d %d\n",u/W,u%W);
    printf("%d\n",(int)routers.size());
    for(int u:routers)printf("%d %d\n",u/W,u%W);
}
