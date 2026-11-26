#include <bits/stdc++.h>
using namespace std;
int H,W,R,Pb,Pr;long long B;
vector<string> g;
vector<int> dst;
vector<char> cov,conn;
template<class F> void scan(int r,int c,F f){
    for(int sr=-1;sr<=1;sr+=2)for(int sc=-1;sc<=1;sc+=2){
        int lim=R;
        for(int i=0;i<=R;i++){
            int rr=r+sr*i;
            if(rr<0||rr>=H)break;
            for(int j=0;j<=lim;j++){
                int cc=c+sc*j;
                if(cc<0||cc>=W)break;
                if(g[rr][cc]=='#'){lim=j-1;break;}
                if(sr==-1&&i==0)continue;
                if(sc==-1&&j==0)continue;
                f(rr*W+cc);
            }
        }
    }
}
int main(){
    int br,bc;
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    char buf[1105];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    int n=H*W;
    dst.assign(n,INT_MAX);cov.assign(n,0);conn.assign(n,0);
    vector<int> q;
    q.push_back(br*W+bc);dst[br*W+bc]=0;conn[br*W+bc]=1;
    auto relax=[&](vector<int>&qq){
        size_t h=0;
        while(h<qq.size()){
            int u=qq[h++];int r=u/W,c=u%W;
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){
                int a=r+dr,b=c+dc;
                if(a<0||b<0||a>=H||b>=W)continue;
                int v=a*W+b;
                if(dst[v]>dst[u]+1){dst[v]=dst[u]+1;qq.push_back(v);}
            }
        }
    };
    relax(q);
    auto gainOf=[&](int u){
        int cnt=0;
        scan(u/W,u%W,[&](int v){if(g[v/W][v%W]=='.'&&!cov[v])cnt++;});
        return cnt;
    };
    typedef pair<double,int> P;
    priority_queue<P> pq;
    for(int u=0;u<n;u++){
        if(g[u/W][u%W]=='#')continue;
        int gn=gainOf(u);
        if(gn==0)continue;
        pq.push({(double)gn/(Pr+(double)Pb*dst[u]),u});
    }
    long long rem=B;
    vector<pair<int,int>> bb,routers;
    auto t0=chrono::steady_clock::now();
    int it=0;
    while(!pq.empty()){
        if((++it&255)==0){
            double e=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(e>8.5)break;
        }
        P t=pq.top();pq.pop();
        int u=t.second;
        long long cost=Pr+(long long)Pb*dst[u];
        if(cost>rem)continue;
        int gn=gainOf(u);
        if(gn==0)continue;
        double key=(double)gn/cost;
        if(!pq.empty()&&key<pq.top().first){pq.push({key,u});continue;}
        // accept
        rem-=cost;
        vector<int> path;
        int cur=u;
        while(dst[cur]>0){
            path.push_back(cur);
            int r=cur/W,c=cur%W;int nx=-1;
            for(int dr=-1;dr<=1&&nx<0;dr++)for(int dc=-1;dc<=1;dc++){
                int a=r+dr,b=c+dc;
                if(a<0||b<0||a>=H||b>=W)continue;
                int v=a*W+b;
                if(dst[v]==dst[cur]-1){nx=v;break;}
            }
            cur=nx;
        }
        vector<int> nq;
        for(int i=path.size()-1;i>=0;i--){
            int v=path[i];
            bb.push_back({v/W,v%W});
        }
        for(int v:path){dst[v]=0;conn[v]=1;nq.push_back(v);}
        relax(nq);
        routers.push_back({u/W,u%W});
        scan(u/W,u%W,[&](int v){if(g[v/W][v%W]=='.')cov[v]=1;});
    }
    printf("%d\n",(int)bb.size());
    for(auto&p:bb)printf("%d %d\n",p.first,p.second);
    printf("%d\n",(int)routers.size());
    for(auto&p:routers)printf("%d %d\n",p.first,p.second);
}
