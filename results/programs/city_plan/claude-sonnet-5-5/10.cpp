#include <bits/stdc++.h>
using namespace std;
int H,W,D,B;
struct P{char t;int h,w,v;vector<string>g;int cnt;vector<pair<int,int>>c;};
vector<P> ps;
struct Pl{int b,r,c;};
long long evalS(vector<Pl>&pl){
    vector<int> R,U;
    for(int i=0;i<(int)pl.size();i++)(ps[pl[i].b].t=='R'?R:U).push_back(i);
    long long s=0;
    for(int i:R){
        set<int> ty;
        auto&a=ps[pl[i].b];
        for(int j:U){
            auto&b=ps[pl[j].b];
            if(ty.count(b.v))continue;
            if(pl[j].r>pl[i].r+a.h+D||pl[i].r>pl[j].r+b.h+D||pl[j].c>pl[i].c+a.w+D||pl[i].c>pl[j].c+b.w+D)continue;
            bool ok=false;
            for(auto&x:a.c){for(auto&y:b.c){
                if(abs(pl[i].r+x.first-pl[j].r-y.first)+abs(pl[i].c+x.second-pl[j].c-y.second)<=D){ok=true;break;}}
                if(ok)break;}
            if(ok)ty.insert(b.v);
        }
        s+=(long long)a.v*ty.size();
    }
    return s;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&B);
    ps.resize(B);
    for(auto&p:ps){
        char t[3];scanf("%s %d %d %d",t,&p.h,&p.w,&p.v);p.t=t[0];
        p.g.resize(p.h);p.cnt=0;
        for(int i=0;i<p.h;i++){char buf[100];scanf("%s",buf);p.g[i]=buf;
            for(int j=0;j<p.w;j++)if(buf[j]=='#'){p.cnt++;p.c.push_back({i,j});}}
    }
    vector<int> Rs,Us;
    for(int i=0;i<B;i++)(ps[i].t=='R'?Rs:Us).push_back(i);
    sort(Rs.begin(),Rs.end(),[&](int a,int b){return (double)ps[a].v/ps[a].h/ps[a].w>(double)ps[b].v/ps[b].h/ps[b].w;});
    sort(Us.begin(),Us.end(),[&](int a,int b){return ps[a].h*ps[a].w<ps[b].h*ps[b].w;});
    // one utility per type, smallest
    vector<int> Ut;set<int> seen;
    for(int u:Us)if(!seen.count(ps[u].v)){seen.insert(ps[u].v);Ut.push_back(u);}
    vector<Pl> best;long long bs=-1;
    int nr=min<int>(3,Rs.size());
    for(int ri=0;ri<nr;ri++){
        int R=Rs[ri];
        for(int ui=0;ui<(int)min<size_t>(Ut.size(),3);ui++){
            int U=Ut[ui];
            for(int k=1;k<=3;k++){
                vector<Pl> pl;
                int r=0,band=0;
                while(true){
                    bool isU=(band%(k+1)==k);
                    int b=isU?U:R;
                    if(r+ps[b].h>H)break;
                    for(int c=0;c+ps[b].w<=W;c+=ps[b].w)pl.push_back({b,r,c});
                    r+=ps[b].h;band++;
                }
                if(pl.size()>3000)continue;
                long long s=evalS(pl);
                if(s>bs){bs=s;best=pl;}
            }
        }
    }
    printf("%d\n",(int)best.size());
    for(auto&p:best)printf("%d %d %d\n",p.b,p.r,p.c);
}
