#include <bits/stdc++.h>
using namespace std;
int H,W,R;
long long Pb,Pr,B;
vector<string> g;
vector<char> cov,conn;
vector<int> par;
int ok_[21][21];
int evalCell(int r,int c,bool apply){
    int n=2*R+1;
    int cnt=0;
    // fill quadrants
    ok_[R][R]=1;
    for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
        for(int a=0;a<=R;a++)for(int b=0;b<=R;b++){
            if(a==0&&b==0)continue;
            int x=r+sx*a,y=c+sy*b;
            int v;
            if(x<0||y<0||x>=H||y>=W||g[x][y]=='#')v=0;
            else{
                v=1;
                if(a>0)v&=ok_[R+sx*(a-1)][R+sy*b];
                if(b>0)v&=ok_[R+sx*a][R+sy*(b-1)];
            }
            ok_[R+sx*a][R+sy*b]=v;
        }
    }
    for(int i=0;i<n;i++)for(int j=0;j<n;j++){
        if(!ok_[i][j])continue;
        int x=r+i-R,y=c+j-R;
        int id=x*W+y;
        if(g[x][y]=='.'&&!cov[id]){
            cnt++;
            if(apply)cov[id]=1;
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
    char *buf=new char[W+5];
    for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;}
    int N=H*W;
    cov.assign(N,0);conn.assign(N,0);par.assign(N,-1);
    vector<int> dep(N,-1);
    int s=br*W+bc;
    vector<int> q;q.reserve(N);
    q.push_back(s);dep[s]=0;
    for(size_t h=0;h<q.size();h++){
        int u=q[h];int x=u/W,y=u%W;
        for(int dx=-1;dx<=1;dx++)for(int dy=-1;dy<=1;dy++){
            if(!dx&&!dy)continue;
            int a=x+dx,b=y+dy;
            if(a<0||b<0||a>=H||b>=W)continue;
            int v=a*W+b;
            if(dep[v]>=0)continue;
            dep[v]=dep[u]+1;par[v]=u;q.push_back(v);
        }
    }
    conn[s]=1;
    auto costOf=[&](int v){
        long long k=0;
        while(!conn[v]){k++;v=par[v];}
        return Pr+Pb*k;
    };
    priority_queue<pair<double,int>> pq;
    for(int i=0;i<H;i++)for(int j=0;j<W;j++){
        if(g[i][j]=='#')continue;
        int id=i*W+j;
        int gn=evalCell(i,j,false);
        if(gn<=0)continue;
        double c=Pr+Pb*dep[id];
        pq.push({gn/c,id});
    }
    long long spent=0;
    vector<int> bb,rt;
    while(!pq.empty()){
        auto t=pq.top();pq.pop();
        int id=t.second;
        int x=id/W,y=id%W;
        int gn=evalCell(x,y,false);
        if(gn<=0)continue;
        long long c=costOf(id);
        if(spent+c>B)continue;
        double ratio=(double)gn/c;
        if(!pq.empty()&&ratio<pq.top().first){
            pq.push({ratio,id});
            continue;
        }
        evalCell(x,y,true);
        spent+=c;
        vector<int> chain;
        int v=id;
        while(!conn[v]){chain.push_back(v);conn[v]=1;v=par[v];}
        for(int i=(int)chain.size()-1;i>=0;i--)bb.push_back(chain[i]);
        rt.push_back(id);
    }
    string out;
    out+=to_string(bb.size())+"\n";
    for(int v:bb)out+=to_string(v/W)+" "+to_string(v%W)+"\n";
    out+=to_string(rt.size())+"\n";
    for(int v:rt)out+=to_string(v/W)+" "+to_string(v%W)+"\n";
    fputs(out.c_str(),stdout);
}
