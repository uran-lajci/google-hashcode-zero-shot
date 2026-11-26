#include <bits/stdc++.h>
using namespace std;
int H,W,R;
vector<char> g;
vector<char> cov;
vector<int> stampv;
int curStamp=0;
int countOrMark(int a,int b,bool mark){
    int cnt=0;
    curStamp++;
    for(int sr=-1;sr<=1;sr+=2)for(int sc=-1;sc<=1;sc+=2){
        int lim=R;
        for(int i=0;i<=R;i++){
            int r=a+sr*i;
            if(r<0||r>=H)break;
            int nl=lim;
            for(int j=0;j<=lim;j++){
                int c=b+sc*j;
                if(c<0||c>=W)break;
                int id=r*W+c;
                if(g[id]=='#'){nl=j-1;break;}
                if(stampv[id]==curStamp)continue;
                stampv[id]=curStamp;
                if(g[id]=='.'&&!cov[id]){
                    cnt++;
                    if(mark)cov[id]=1;
                }
            }
            lim=nl;
            if(lim<0)break;
        }
    }
    return cnt;
}
int main(){
    long long Pb,Pr,B;
    int br,bc;
    scanf("%d %d %d",&H,&W,&R);
    scanf("%lld %lld %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.assign(H*W,'-');
    {
        static char buf[2000];
        for(int i=0;i<H;i++){
            scanf("%s",buf);
            for(int j=0;j<W;j++)g[i*W+j]=buf[j];
        }
    }
    cov.assign(H*W,0);
    stampv.assign(H*W,0);
    vector<int> dist(H*W);
    for(int i=0;i<H;i++)for(int j=0;j<W;j++)
        dist[i*W+j]=max(abs(i-br),abs(j-bc));
    priority_queue<pair<double,int>> pq;
    for(int i=0;i<H;i++)for(int j=0;j<W;j++){
        int id=i*W+j;
        if(g[id]=='#')continue;
        int gn=countOrMark(i,j,false);
        if(gn==0)continue;
        pq.push({(double)gn/(double)(Pr+Pb*dist[id]),id});
    }
    long long rem=B;
    vector<int> conn,routers;
    vector<int> q;
    while(!pq.empty()){
        auto t=pq.top();pq.pop();
        int id=t.second;
        int a=id/W,b=id%W;
        int gn=countOrMark(a,b,false);
        if(gn==0)continue;
        long long cost=Pr+Pb*(long long)dist[id];
        if(cost>rem)continue;
        double key=(double)gn/(double)cost;
        if(!pq.empty()&&key<pq.top().first-1e-12){
            pq.push({key,id});
            continue;
        }
        // accept
        countOrMark(a,b,true);
        rem-=cost;
        routers.push_back(id);
        // path
        vector<int> path;
        int cur=id;
        while(dist[cur]>0){
            path.push_back(cur);
            int cr=cur/W,cc=cur%W;
            int best=-1;
            for(int dr=-1;dr<=1&&best<0;dr++)for(int dc=-1;dc<=1;dc++){
                if(!dr&&!dc)continue;
                int nr=cr+dr,nc=cc+dc;
                if(nr<0||nr>=H||nc<0||nc>=W)continue;
                if(dist[nr*W+nc]==dist[cur]-1){best=nr*W+nc;break;}
            }
            cur=best;
        }
        reverse(path.begin(),path.end());
        q.clear();
        for(int p:path){
            conn.push_back(p);
            dist[p]=0;
            q.push_back(p);
        }
        size_t h=0;
        while(h<q.size()){
            int u=q[h++];
            int ur=u/W,uc=u%W;
            int nd=dist[u]+1;
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){
                if(!dr&&!dc)continue;
                int nr=ur+dr,nc=uc+dc;
                if(nr<0||nr>=H||nc<0||nc>=W)continue;
                int v=nr*W+nc;
                if(dist[v]>nd){dist[v]=nd;q.push_back(v);}
            }
        }
    }
    string out;
    out+=to_string(conn.size())+"\n";
    for(int p:conn)out+=to_string(p/W)+" "+to_string(p%W)+"\n";
    out+=to_string(routers.size())+"\n";
    for(int p:routers)out+=to_string(p/W)+" "+to_string(p%W)+"\n";
    fputs(out.c_str(),stdout);
}
