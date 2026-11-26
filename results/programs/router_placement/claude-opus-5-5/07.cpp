#include <bits/stdc++.h>
using namespace std;

int H,W,R,Pb,Pr; long long B;
vector<char> wallv,targ;
vector<char> okbuf;
int SZ;

// fills out with non-wall cells covered by router at s (s must be non-wall)
void coverCells(int s, vector<int>& out){
    out.clear();
    int a=s/W,b=s%W;
    for(int adi=0;adi<=R;adi++){
        for(int sr=1;sr>=-1;sr-=2){
            if(adi==0&&sr==-1)continue;
            int di=adi*sr;int x=a+di;
            for(int adj=0;adj<=R;adj++){
                for(int sc=1;sc>=-1;sc-=2){
                    if(adj==0&&sc==-1)continue;
                    int dj=adj*sc;int y=b+dj;
                    int idx=(di+R)*SZ+(dj+R);
                    bool v;
                    if(x<0||x>=H||y<0||y>=W||wallv[x*W+y]) v=false;
                    else{
                        v=true;
                        if(adi>0 && !okbuf[(di-sr+R)*SZ+(dj+R)]) v=false;
                        if(v && adj>0 && !okbuf[(di+R)*SZ+(dj-sc+R)]) v=false;
                    }
                    okbuf[idx]=v;
                    if(v) out.push_back(x*W+y);
                }
            }
        }
    }
}

struct E{double key;int cell;int cov;int dist;};
struct Cmp{bool operator()(const E&a,const E&b)const{
    if(a.key!=b.key)return a.key<b.key; return a.cell>b.cell;}};

int main(){
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    int br,bc;scanf("%d %d",&br,&bc);
    int N=H*W;
    wallv.assign(N,0);targ.assign(N,0);
    {
        static char buf[2000];
        for(int i=0;i<H;i++){
            scanf("%s",buf);
            for(int j=0;j<W;j++){
                char c=buf[j];
                if(c=='#')wallv[i*W+j]=1;
                else if(c=='.')targ[i*W+j]=1;
            }
        }
    }
    SZ=2*R+1; okbuf.assign(SZ*SZ,0);
    vector<int> cov(N,0);
    vector<int> tmp,tmp2;
    for(int s=0;s<N;s++){
        if(wallv[s])continue;
        coverCells(s,tmp);
        int c=0;for(int x:tmp)if(targ[x])c++;
        cov[s]=c;
    }
    const int INF=INT_MAX/2;
    vector<int> dist(N,INF),par(N,-1);
    vector<char> covered(N,0),placed(N,0);
    priority_queue<E,vector<E>,Cmp> pq;
    auto keyOf=[&](int c,int d){return (double)c/(double)(Pr+(long long)Pb*d);};
    auto pushCell=[&](int s){
        if(wallv[s]||placed[s]||cov[s]<=0)return;
        pq.push({keyOf(cov[s],dist[s]),s,cov[s],dist[s]});
    };
    vector<int> q; q.reserve(N);
    auto bfs=[&](){
        size_t h=0;
        while(h<q.size()){
            int u=q[h++];int ux=u/W,uy=u%W;int du=dist[u];
            for(int dx=-1;dx<=1;dx++){int nx=ux+dx;if(nx<0||nx>=H)continue;
                for(int dy=-1;dy<=1;dy++){if(!dx&&!dy)continue;int ny=uy+dy;if(ny<0||ny>=W)continue;
                    int n=nx*W+ny;
                    if(dist[n]>du+1){dist[n]=du+1;par[n]=u;q.push_back(n);pushCell(n);}
                }}
        }
        q.clear();
    };
    int b0=br*W+bc;
    dist[b0]=0;pushCell(b0);q.push_back(b0);bfs();

    long long spent=0;
    vector<int> backbone,routers;
    while(!pq.empty()){
        if(B-spent<Pr)break;
        E e=pq.top();pq.pop();
        int s=e.cell;
        if(placed[s])continue;
        if(dist[s]!=e.dist)continue;
        if(cov[s]!=e.cov){ pushCell(s); continue; }
        if(cov[s]<=0)continue;
        long long cost=Pr+(long long)Pb*dist[s];
        if(spent+cost>B)continue;
        if(1000LL*cov[s]<=cost)continue;
        // place
        vector<int> p;int u=s;
        while(dist[u]!=0){p.push_back(u);u=par[u];}
        reverse(p.begin(),p.end());
        for(int x:p){dist[x]=0;backbone.push_back(x);q.push_back(x);}
        spent+=cost;
        placed[s]=1;routers.push_back(s);
        coverCells(s,tmp);
        for(int t:tmp){
            if(targ[t]&&!covered[t]){
                covered[t]=1;
                coverCells(t,tmp2);
                for(int x:tmp2)cov[x]--;
            }
        }
        bfs();
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(backbone.size())+"\n";
    for(int x:backbone){out+=to_string(x/W)+" "+to_string(x%W)+"\n";}
    out+=to_string(routers.size())+"\n";
    for(int x:routers){out+=to_string(x/W)+" "+to_string(x%W)+"\n";}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
