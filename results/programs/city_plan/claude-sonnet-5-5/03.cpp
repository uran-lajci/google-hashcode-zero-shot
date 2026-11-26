#include <bits/stdc++.h>
using namespace std;
struct P{char t;int h,w,v;vector<pair<int,int>> c;};
int H,W,D,B;
vector<P> ps;
struct Pl{int b,r,c;};
vector<Pl> build(vector<int>&seq){
    vector<vector<char>> g(H,vector<char>(W,0));
    vector<Pl> res;
    int idx=0;
    int r=0;
    while(r<H){
        int c=0;int rowh=0;
        while(c<W){
            bool placed=false;
            for(int t=0;t<(int)seq.size()&&!placed;t++){
                int b=seq[(idx+t)%seq.size()];
                P&p=ps[b];
                if(r+p.h>H||c+p.w>W)continue;
                bool ok=true;
                for(auto&q:p.c)if(g[r+q.first][c+q.second]){ok=false;break;}
                if(!ok)continue;
                for(auto&q:p.c)g[r+q.first][c+q.second]=1;
                res.push_back({b,r,c});
                c+=p.w;rowh=max(rowh,p.h);
                idx=(idx+t+1)%seq.size();
                placed=true;
            }
            if(!placed)break;
        }
        r+=max(1,rowh);
    }
    return res;
}
long long evalS(vector<Pl>&v){
    vector<int> R,U;
    for(int i=0;i<(int)v.size();i++){if(ps[v[i].b].t=='R')R.push_back(i);else U.push_back(i);}
    long long s=0;
    for(int i:R){
        set<int> ty;
        P&a=ps[v[i[0]==0?0:0].b];(void)a;
        P&pa=ps[v[i].b];
        for(int j:U){
            P&pb=ps[v[j].b];
            if(ty.count(pb.v))continue;
            if(abs(v[i].r-v[j].r)>D+50||abs(v[i].c-v[j].c)>D+50)continue;
            int best=1e9;
            for(auto&x:pa.c)for(auto&y:pb.c){
                int d=abs(v[i].r+x.first-v[j].r-y.first)+abs(v[i].c+x.second-v[j].c-y.second);
                if(d<best)best=d;
            }
            if(best<=D)ty.insert(pb.v);
        }
        s+=(long long)pa.v*ty.size();
    }
    return s;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&B);
    ps.resize(B);
    for(int i=0;i<B;i++){
        char t[4];scanf("%s %d %d %d",t,&ps[i].h,&ps[i].w,&ps[i].v);
        ps[i].t=t[0];
        for(int r=0;r<ps[i].h;r++){
            char s[64];scanf("%s",s);
            for(int c=0;c<ps[i].w;c++)if(s[c]=='#')ps[i].c.push_back({r,c});
        }
    }
    int bestR=-1;double br=-1;
    for(int i=0;i<B;i++)if(ps[i].t=='R'){
        double q=(double)ps[i].v/ps[i].c.size();
        if(q>br){br=q;bestR=i;}
    }
    map<int,int> ut;
    for(int i=0;i<B;i++)if(ps[i].t=='U'){
        auto it=ut.find(ps[i].v);
        if(it==ut.end()||ps[i].c.size()<ps[it->second].c.size())ut[ps[i].v]=i;
    }
    vector<int> us;
    for(auto&x:ut)us.push_back(x.second);
    sort(us.begin(),us.end(),[&](int a,int b){return ps[a].c.size()<ps[b].c.size();});
    vector<Pl> best;long long bs=-1;
    for(int K=1;K<=min<int>(4,us.size());K++){
        vector<int> seq;
        for(int k=0;k<K;k++){seq.push_back(bestR);seq.push_back(us[k]);}
        seq.push_back(bestR);
        vector<Pl> v=build(seq);
        long long s=evalS(v);
        if(s>bs){bs=s;best=v;}
    }
    printf("%d\n",(int)best.size());
    for(auto&p:best)printf("%d %d %d\n",p.b,p.r,p.c);
}
