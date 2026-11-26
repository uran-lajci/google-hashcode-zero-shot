#include <bits/stdc++.h>
using namespace std;
int H,W,R,Pb,Pr;long long B;
vector<string> g;
vector<int> PW,PT,dist_;
vector<char> cov;
int S;
inline int idx(int r,int c){return r*W+c;}
inline int pw(int r,int c){return PW[(r)*(W+1)+c];}
inline int rectW(int r1,int c1,int r2,int c2){ // inclusive
    return PW[(r2+1)*(W+1)+c2+1]-PW[r1*(W+1)+c2+1]-PW[(r2+1)*(W+1)+c1]+PW[r1*(W+1)+c1];
}
int gainOf(int a,int b,bool mark){
    int g_=0;
    int x0=max(0,a-R),x1=min(H-1,a+R),y0=max(0,b-R),y1=min(W-1,b+R);
    for(int x=x0;x<=x1;x++){
        int r1=min(a,x),r2=max(a,x);
        for(int y=y0;y<=y1;y++){
            if(g[x][y]!='.'||cov[idx(x,y)])continue;
            if(rectW(r1,min(b,y),r2,max(b,y)))continue;
            g_++;
            if(mark)cov[idx(x,y)]=1;
        }
    }
    return g_;
}
int main(){
    int br,bc;
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    char *buf=new char[2000];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    PW.assign((H+1)*(W+1),0);PT=PW;
    for(int i=0;i<H;i++)for(int j=0;j<W;j++){
        int k=(i+1)*(W+1)+j+1;
        PW[k]=PW[k-1]+PW[k-(W+1)]-PW[k-(W+1)-1]+(g[i][j]=='#');
        PT[k]=PT[k-1]+PT[k-(W+1)]-PT[k-(W+1)-1]+(g[i][j]=='.');
    }
    cov.assign(H*W,0);
    dist_.assign(H*W,0);
    for(int i=0;i<H;i++)for(int j=0;j<W;j++)dist_[idx(i,j)]=max(abs(i-br),abs(j-bc));
    typedef pair<double,int> P;
    priority_queue<P> pq;
    {
        vector<P> init;init.reserve(H*W);
        for(int i=0;i<H;i++)for(int j=0;j<W;j++){
            if(g[i][j]=='#')continue;
            int x0=max(0,i-R),x1=min(H-1,i+R),y0=max(0,j-R),y1=min(W-1,j+R);
            int t=PT[(x1+1)*(W+1)+y1+1]-PT[x0*(W+1)+y1+1]-PT[(x1+1)*(W+1)+y0]+PT[x0*(W+1)+y0];
            if(t<=0)continue;
            double key=1000.0*t/(Pr+(double)Pb*dist_[idx(i,j)]);
            init.push_back({key,idx(i,j)});
        }
        pq=priority_queue<P>(less<P>(),move(init));
    }
    long long spent=0;
    vector<pair<int,int>> conn,routers;
    long long evals=0;
    while(!pq.empty()&&evals<2500000){
        P t=pq.top();pq.pop();
        int id=t.second;int a=id/W,b=id%W;
        int d=dist_[id];
        long long cost=Pr+(long long)Pb*d;
        if(spent+cost>B)continue;
        evals++;
        int gn=gainOf(a,b,false);
        if(gn<=0)continue;
        double key=1000.0*gn/cost;
        if(1000LL*gn<cost)continue;
        if(!pq.empty()&&key<pq.top().first){pq.push({key,id});continue;}
        // accept
        gainOf(a,b,true);
        spent+=cost;
        routers.push_back({a,b});
        if(d>0){
            vector<int> path;
            int cur=id;
            while(dist_[cur]>0){
                path.push_back(cur);
                int cr=cur/W,cc=cur%W,cd=dist_[cur];
                int nxt=-1;
                for(int dr=-1;dr<=1&&nxt<0;dr++)for(int dc=-1;dc<=1;dc++){
                    if(!dr&&!dc)continue;
                    int nr=cr+dr,nc=cc+dc;
                    if(nr<0||nc<0||nr>=H||nc>=W)continue;
                    if(dist_[idx(nr,nc)]==cd-1){nxt=idx(nr,nc);break;}
                }
                cur=nxt;
            }
            deque<int> q;
            for(int k=(int)path.size()-1;k>=0;k--){
                conn.push_back({path[k]/W,path[k]%W});
            }
            for(int p:path){dist_[p]=0;q.push_back(p);}
            while(!q.empty()){
                int c=q.front();q.pop_front();
                int cr=c/W,cc=c%W;
                for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){
                    if(!dr&&!dc)continue;
                    int nr=cr+dr,nc=cc+dc;
                    if(nr<0||nc<0||nr>=H||nc>=W)continue;
                    int ni=idx(nr,nc);
                    if(dist_[ni]>dist_[c]+1){dist_[ni]=dist_[c]+1;q.push_back(ni);}
                }
            }
        }
    }
    printf("%d\n",(int)conn.size());
    for(auto&p:conn)printf("%d %d\n",p.first,p.second);
    printf("%d\n",(int)routers.size());
    for(auto&p:routers)printf("%d %d\n",p.first,p.second);
}
