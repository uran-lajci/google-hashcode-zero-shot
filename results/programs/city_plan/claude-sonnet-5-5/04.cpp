#include <bits/stdc++.h>
using namespace std;
struct P{char t;int h,w,v;vector<pair<int,int>> cells;};
int H,W,D,B;
vector<P> ps;
struct Pl{int b,r,c;};
static int gapd(int a1,int a2,int b1,int b2){ // intervals [a1,a2],[b1,b2]
    if(a2<b1)return b1-a2; if(b2<a1)return a1-b2; return 0;}
long long score(const vector<Pl>&pl){
    vector<int> us;vector<int> rs;
    for(int i=0;i<(int)pl.size();i++){ if(ps[pl[i].b].t=='U')us.push_back(i);else rs.push_back(i);}
    sort(us.begin(),us.end(),[&](int a,int b){return pl[a].r<pl[b].r;});
    vector<int> tops;for(int u:us)tops.push_back(pl[u].r);
    long long tot=0;
    for(int ri:rs){
        const Pl&R=pl[ri];const P&pr=ps[R.b];
        int rt=R.r,rb=R.r+pr.h-1,cl=R.c,cr=R.c+pr.w-1;
        int lo=rt-50-D,hi=rb+D;
        size_t s=lower_bound(tops.begin(),tops.end(),lo)-tops.begin();
        set<int> found;
        for(size_t k=s;k<us.size()&&tops[k]<=hi;k++){
            const Pl&U=pl[us[k]];const P&pu=ps[U.b];
            if(found.count(pu.v))continue;
            int g=gapd(rt,rb,U.r,U.r+pu.h-1)+gapd(cl,cr,U.c,U.c+pu.w-1);
            if(g>D)continue;
            bool ok=false;
            for(auto&a:pr.cells){
                for(auto&b:pu.cells){
                    if(abs(R.r+a.first-U.r-b.first)+abs(R.c+a.second-U.c-b.second)<=D){ok=true;break;}
                }
                if(ok)break;
            }
            if(ok)found.insert(pu.v);
        }
        tot+=(long long)pr.v*found.size();
    }
    return tot;
}
vector<Pl> build(int R,const vector<int>&u){
    vector<Pl> out;int r=0,c=0,sh=0;int k=u.size();
    for(long long i=0;;i++){
        int b=(i%2==0)?R:u[(i/2)%k];
        int h=ps[b].h,w=ps[b].w;
        if(c+w>W){r+=sh;c=0;sh=0;}
        if(r+h>H)break;
        out.push_back({b,r,c});
        c+=w;sh=max(sh,h);
    }
    return out;
}
int main(){
    auto t0=chrono::steady_clock::now();
    scanf("%d %d %d %d",&H,&W,&D,&B);
    ps.resize(B);
    for(int i=0;i<B;i++){
        char t[4];P&p=ps[i];
        scanf("%s %d %d %d",t,&p.h,&p.w,&p.v);p.t=t[0];
        for(int r=0;r<p.h;r++){
            char buf[128];scanf("%s",buf);
            for(int c=0;c<p.w;c++)if(buf[c]=='#')p.cells.push_back({r,c});
        }
    }
    vector<int> Rs;
    map<int,int> best; // type -> project with smallest area
    for(int i=0;i<B;i++){
        if(ps[i].t=='R')Rs.push_back(i);
        else{
            auto it=best.find(ps[i].v);
            if(it==best.end()||ps[i].h*ps[i].w<ps[it->second].h*ps[it->second].w)best[ps[i].v]=i;
        }
    }
    vector<int> ul;for(auto&kv:best)ul.push_back(kv.second);
    sort(ul.begin(),ul.end(),[&](int a,int b){
        int x=ps[a].h*ps[a].w,y=ps[b].h*ps[b].w;if(x!=y)return x<y;return a<b;});
    vector<int> rc;
    {
        vector<int> a=Rs;
        sort(a.begin(),a.end(),[&](int x,int y){
            return (long long)ps[x].v*ps[y].h*ps[y].w>(long long)ps[y].v*ps[x].h*ps[x].w;});
        for(int i=0;i<(int)a.size()&&i<3;i++)rc.push_back(a[i]);
        sort(a.begin(),a.end(),[&](int x,int y){return ps[x].v>ps[y].v;});
        if(find(rc.begin(),rc.end(),a[0])==rc.end())rc.push_back(a[0]);
    }
    vector<Pl> bestPl;long long bs=-1;
    for(int R:rc){
        for(int k=1;k<=(int)ul.size()&&k<=10;k++){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>7.5&&bs>=0)goto done;
            vector<int> u(ul.begin(),ul.begin()+k);
            auto pl=build(R,u);
            long long s=score(pl);
            if(s>bs){bs=s;bestPl=pl;}
        }
    }
    done:
    printf("%d\n",(int)bestPl.size());
    for(auto&p:bestPl)printf("%d %d %d\n",p.b,p.r,p.c);
}
