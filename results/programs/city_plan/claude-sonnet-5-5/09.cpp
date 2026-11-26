#include <bits/stdc++.h>
using namespace std;
int H,W,D,B;
struct P{char t;int h,w,v;vector<pair<int,int>> cells;};
vector<P> ps;
struct Pl{int b,r,c;};
vector<Pl> run(const vector<int>&items){
    vector<char> g((size_t)H*W,0);
    vector<Pl> res;
    int m=items.size(),idx=0;
    for(int r=0;r<H;r++)for(int c=0;c<W;c++){
        for(int j=0;j<m;j++){
            int b=items[(idx+j)%m];
            const P&p=ps[b];
            if(r+p.h>H||c+p.w>W)continue;
            bool ok=true;
            for(auto&q:p.cells)if(g[(size_t)(r+q.first)*W+c+q.second]){ok=false;break;}
            if(!ok)continue;
            for(auto&q:p.cells)g[(size_t)(r+q.first)*W+c+q.second]=1;
            res.push_back({b,r,c});
            idx=(idx+j+1)%m;
            break;
        }
    }
    return res;
}
long long score(const vector<Pl>&pl){
    map<int,vector<int>> types;
    for(size_t i=0;i<pl.size();i++){
        const P&p=ps[pl[i].b];
        if(p.t=='U')types[p.v].push_back(i);
    }
    vector<vector<int>> dists;
    const int INF=1e8;
    for(auto&kv:types){
        vector<int> d((size_t)H*W,INF);
        for(int i:kv.second){
            const P&p=ps[pl[i].b];
            for(auto&q:p.cells)d[(size_t)(pl[i].r+q.first)*W+pl[i].c+q.second]=0;
        }
        for(int r=0;r<H;r++)for(int c=0;c<W;c++){
            int&x=d[(size_t)r*W+c];
            if(r)x=min(x,d[(size_t)(r-1)*W+c]+1);
            if(c)x=min(x,d[(size_t)r*W+c-1]+1);
        }
        for(int r=H-1;r>=0;r--)for(int c=W-1;c>=0;c--){
            int&x=d[(size_t)r*W+c];
            if(r<H-1)x=min(x,d[(size_t)(r+1)*W+c]+1);
            if(c<W-1)x=min(x,d[(size_t)r*W+c+1]+1);
        }
        dists.push_back(move(d));
    }
    long long s=0;
    for(auto&pp:pl){
        const P&p=ps[pp.b];
        if(p.t!='R')continue;
        int cnt=0;
        for(auto&d:dists){
            bool ok=false;
            for(auto&q:p.cells)if(d[(size_t)(pp.r+q.first)*W+pp.c+q.second]<=D){ok=true;break;}
            if(ok)cnt++;
        }
        s+=(long long)cnt*p.v;
    }
    return s;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&B);
    ps.resize(B);
    for(int i=0;i<B;i++){
        char t;int h,w,v;
        scanf(" %c %d %d %d",&t,&h,&w,&v);
        ps[i].t=t;ps[i].h=h;ps[i].w=w;ps[i].v=v;
        for(int r=0;r<h;r++){
            char buf[128];
            scanf("%s",buf);
            for(int c=0;c<w;c++)if(buf[c]=='#')ps[i].cells.push_back({r,c});
        }
    }
    vector<int> R,U;
    for(int i=0;i<B;i++)(ps[i].t=='R'?R:U).push_back(i);
    sort(R.begin(),R.end(),[&](int a,int b){
        return (long long)ps[a].v*ps[b].cells.size()>(long long)ps[b].v*ps[a].cells.size();
    });
    // smallest per type
    map<int,int> best;
    for(int u:U){
        auto it=best.find(ps[u].v);
        if(it==best.end()||ps[u].cells.size()<ps[it->second].cells.size())best[ps[u].v]=u;
    }
    vector<int> ut;
    for(auto&kv:best)ut.push_back(kv.second);
    sort(ut.begin(),ut.end(),[&](int a,int b){
        if(ps[a].cells.size()!=ps[b].cells.size())return ps[a].cells.size()<ps[b].cells.size();
        return a<b;
    });
    long long bs=-1;
    vector<Pl> bp;
    for(int ri=0;ri<(int)min<size_t>(2,R.size());ri++)
        for(int k=1;k<=3;k++){
            if(k>(int)ut.size())break;
            for(int a=1;a<=4;a++){
                vector<int> items;
                for(int i=0;i<a;i++)items.push_back(R[ri]);
                for(int i=0;i<k;i++)items.push_back(ut[i]);
                auto pl=run(items);
                long long s=score(pl);
                if(s>bs){bs=s;bp=pl;}
            }
        }
    printf("%d\n",(int)bp.size());
    for(auto&p:bp)printf("%d %d %d\n",p.b,p.r,p.c);
}
