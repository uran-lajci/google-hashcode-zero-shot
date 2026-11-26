#include <bits/stdc++.h>
using namespace std;
int H,W,R;
vector<string> g;
vector<short> rd,ld;
vector<int> pre;
vector<char> cov,placed;
vector<int> dist_;
template<class F> void forRows(int a,int b,F f){
    for(int dx=-1;dx<=1;dx+=2){
        int cr=min<int>(R,rd[a*W+b]),cl=min<int>(R,ld[a*W+b]);
        for(int k=(dx==-1?1:0);k<=R;k++){
            int x=a+dx*(dx==-1?k:k);
            if(dx==-1) x=a-k;
            if(x<0||x>=H)break;
            if(g[x][b]=='#')break;
            cr=min<int>(cr,rd[x*W+b]);
            cl=min<int>(cl,ld[x*W+b]);
            f(x,b-cl,b+cr);
        }
    }
}
int main(){
    int Pb,Pr;long long B;int br,bc;
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    char buf[2048];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    rd.assign(H*W,0);ld.assign(H*W,0);
    pre.assign(H*(W+1),0);
    for(int r=0;r<H;r++){
        for(int c=W-1;c>=0;c--){
            if(g[r][c]=='#')rd[r*W+c]=-1;
            else rd[r*W+c]=(c+1<W&&g[r][c+1]!='#')?rd[r*W+c+1]+1:0;
        }
        for(int c=0;c<W;c++){
            if(g[r][c]=='#')ld[r*W+c]=-1;
            else ld[r*W+c]=(c>0&&g[r][c-1]!='#')?ld[r*W+c-1]+1:0;
        }
        for(int c=0;c<W;c++)pre[r*(W+1)+c+1]=pre[r*(W+1)+c]+(g[r][c]=='.');
    }
    cov.assign(H*W,0);placed.assign(H*W,0);
    const int INF=1e9;
    dist_.assign(H*W,INF);
    vector<int> q;
    auto bfs=[&](){
        size_t h=0;
        while(h<q.size()){
            int u=q[h++];int ur=u/W,uc=u%W;int d=dist_[u];
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){
                int nr=ur+dr,nc=uc+dc;
                if(nr<0||nc<0||nr>=H||nc>=W)continue;
                int v=nr*W+nc;
                if(dist_[v]>d+1){dist_[v]=d+1;q.push_back(v);}
            }
        }
        q.clear();
    };
    dist_[br*W+bc]=0;q.push_back(br*W+bc);bfs();
    priority_queue<pair<double,int>> pq;
    for(int r=0;r<H;r++)for(int c=0;c<W;c++){
        if(g[r][c]=='#')continue;
        int gain=0;
        forRows(r,c,[&](int x,int c1,int c2){
            gain+=pre[x*(W+1)+c2+1]-pre[x*(W+1)+c1];
        });
        if(gain<=0)continue;
        double cost=Pr+(double)dist_[r*W+c]*Pb;
        pq.push({gain/cost,r*W+c});
    }
    long long rem=B;
    vector<pair<int,int>> bb,routers;
    while(!pq.empty()){
        auto [key,id]=pq.top();pq.pop();
        if(placed[id])continue;
        int r=id/W,c=id%W;
        int gain=0;
        forRows(r,c,[&](int x,int c1,int c2){
            for(int y=c1;y<=c2;y++)if(g[x][y]=='.'&&!cov[x*W+y])gain++;
        });
        if(gain<=0)continue;
        long long cost=Pr+(long long)dist_[id]*Pb;
        if(cost>rem)continue;
        if(1000LL*gain<=cost)continue;
        double nk=gain/(double)cost;
        if(!pq.empty()&&nk<pq.top().first){pq.push({nk,id});continue;}
        // accept
        rem-=cost;
        placed[id]=1;
        routers.push_back({r,c});
        forRows(r,c,[&](int x,int c1,int c2){
            for(int y=c1;y<=c2;y++)cov[x*W+y]=1;
        });
        vector<int> path;
        int cur=id;
        while(dist_[cur]>0){
            path.push_back(cur);
            int d=dist_[cur];int cr=cur/W,cc=cur%W;
            int nxt=-1;
            for(int dr=-1;dr<=1&&nxt<0;dr++)for(int dc=-1;dc<=1;dc++){
                int nr=cr+dr,nc=cc+dc;
                if(nr<0||nc<0||nr>=H||nc>=W)continue;
                if(dist_[nr*W+nc]==d-1){nxt=nr*W+nc;break;}
            }
            cur=nxt;
        }
        reverse(path.begin(),path.end());
        for(int p:path){
            bb.push_back({p/W,p%W});
            dist_[p]=0;q.push_back(p);
        }
        bfs();
    }
    printf("%d\n",(int)bb.size());
    for(auto&p:bb)printf("%d %d\n",p.first,p.second);
    printf("%d\n",(int)routers.size());
    for(auto&p:routers)printf("%d %d\n",p.first,p.second);
}
