#include <bits/stdc++.h>
using namespace std;
int H,W,R;
long long Pb,Pr,B;
vector<string> g;
vector<char> cov,con;
vector<int> dist_;
static bool okk[21][21];
// mode 0: count uncovered targets; mode 1: mark covered
int evalc(int a,int b,int mode){
    int cnt=0;
    for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
        for(int dx=0;dx<=R;dx++)for(int dy=0;dy<=R;dy++){
            int x=a+sx*dx,y=b+sy*dy;
            bool ok;
            if(x<0||x>=H||y<0||y>=W||g[x][y]=='#')ok=false;
            else if(dx==0&&dy==0)ok=true;
            else ok=(dx==0||okk[dx-1][dy])&&(dy==0||okk[dx][dy-1]);
            okk[dx][dy]=ok;
            if(!ok)continue;
            if(dx==0&&sx==-1)continue;
            if(dy==0&&sy==-1)continue;
            if(g[x][y]=='.'){
                int id=x*W+y;
                if(!cov[id]){
                    cnt++;
                    if(mode)cov[id]=1;
                }
            }
        }
    }
    return cnt;
}
int main(){
    int br,bc;
    scanf("%d %d %d",&H,&W,&R);
    scanf("%lld %lld %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    char buf[2048];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    cov.assign(H*W,0);
    con.assign(H*W,0);
    dist_.assign(H*W,0);
    for(int i=0;i<H;i++)for(int j=0;j<W;j++)
        dist_[i*W+j]=max(abs(i-br),abs(j-bc));
    con[br*W+bc]=1;
    typedef pair<double,int> PI;
    priority_queue<PI> pq;
    for(int i=0;i<H;i++)for(int j=0;j<W;j++){
        if(g[i][j]=='#')continue;
        int gn=evalc(i,j,0);
        if(gn<=0)continue;
        double c=Pr+Pb*dist_[i*W+j];
        pq.push({gn/c,i*W+j});
    }
    long long rem=B;
    vector<int> backbone,routers;
    vector<char> hasR(H*W,0);
    while(!pq.empty()){
        auto t=pq.top();pq.pop();
        int id=t.second;
        if(hasR[id])continue;
        int a=id/W,b=id%W;
        int gn=evalc(a,b,0);
        if(gn<=0)continue;
        long long cost=Pr+Pb*(long long)dist_[id];
        if(cost>rem)continue;
        double key=gn/(double)cost;
        if(!pq.empty()&&key<pq.top().first-1e-12){
            pq.push({key,id});
            continue;
        }
        // accept
        rem-=cost;
        evalc(a,b,1);
        hasR[id]=1;
        routers.push_back(id);
        vector<int> path;
        int cur=id;
        while(dist_[cur]>0){
            int cx=cur/W,cy=cur%W;
            int d=dist_[cur];
            path.push_back(cur);
            int nxt=-1;
            for(int dx=-1;dx<=1&&nxt<0;dx++)for(int dy=-1;dy<=1;dy++){
                if(!dx&&!dy)continue;
                int x=cx+dx,y=cy+dy;
                if(x<0||y<0||x>=H||y>=W)continue;
                if(dist_[x*W+y]==d-1){nxt=x*W+y;break;}
            }
            cur=nxt;
            // mark so loop proceeds: we don't change dist yet
        }
        reverse(path.begin(),path.end());
        queue<int> q;
        for(int p:path){
            con[p]=1;
            backbone.push_back(p);
            dist_[p]=0;
            q.push(p);
        }
        while(!q.empty()){
            int c=q.front();q.pop();
            int cx=c/W,cy=c%W;
            for(int dx=-1;dx<=1;dx++)for(int dy=-1;dy<=1;dy++){
                if(!dx&&!dy)continue;
                int x=cx+dx,y=cy+dy;
                if(x<0||y<0||x>=H||y>=W)continue;
                int n=x*W+y;
                if(dist_[n]>dist_[c]+1){
                    dist_[n]=dist_[c]+1;
                    q.push(n);
                }
            }
        }
    }
    string out;
    out+=to_string(backbone.size())+"\n";
    for(int p:backbone)out+=to_string(p/W)+" "+to_string(p%W)+"\n";
    out+=to_string(routers.size())+"\n";
    for(int p:routers)out+=to_string(p/W)+" "+to_string(p%W)+"\n";
    fputs(out.c_str(),stdout);
}
