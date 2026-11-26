#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int T,D,W,G;
vector<ll> L,A;
struct Gift{string name;int s,w;ll c,r;};
int accOf(ll w){for(int i=0;i<W;i++)if(w<=L[i])return (int)A[i];return 0;}
int main(){
    scanf("%d %d %d %d",&T,&D,&W,&G);
    L.resize(W);A.resize(W);
    for(int i=0;i<W;i++)scanf("%lld %lld",&L[i],&A[i]);
    vector<Gift> g(G);
    char buf[64];
    for(auto&x:g){scanf("%s %d %d %lld %lld",buf,&x.s,&x.w,&x.c,&x.r);x.name=buf;}
    vector<string> out;
    vector<int> rest;
    for(int i=0;i<G;i++){
        ll d2=g[i].c*g[i].c+g[i].r*g[i].r;
        if(d2<=(ll)D*D){
            out.push_back("LoadGift "+g[i].name);
            out.push_back("DeliverGift "+g[i].name);
        } else rest.push_back(i);
    }
    sort(rest.begin(),rest.end(),[&](int a,int b){
        if(g[a].s!=g[b].s)return g[a].s>g[b].s;return a<b;});
    int cand=min<int>(40,rest.size());
    int Ks[5]={10,30,100,300,1000};
    bool done=false;
    for(int ci=0;ci<cand&&!done;ci++){
        const Gift&gf=g[rest[ci]];
        ll X=gf.c,Y=gf.r;
        for(ll Tt=1;Tt<=T&&!done;){
            for(int ki=0;ki<5&&!done;ki++){
                int K=Ks[ki];
                ll w=gf.w+K;
                if(accOf(w)==0)continue;
                ll px=0,py=0,vx=0,vy=0;
                int car=K;
                vector<pair<char,int>> ops;
                bool bad=false;
                for(ll t=0;t<Tt;t++){
                    ll rem=Tt-t;
                    ll ex=X-(px+vx*rem),ey=Y-(py+vy*rem);
                    int am=car>0?accOf(w):0;
                    bool useX=llabs(ex)>=llabs(ey);
                    ll e=useX?ex:ey;
                    ll dv=0;
                    if(am>0){
                        // round e/rem
                        ll q=(llabs(e)*2+rem)/(2*rem);
                        if(e<0)q=-q;
                        dv=max<ll>(-am,min<ll>(am,q));
                    }
                    if(dv!=0){
                        if(useX){vx+=dv;ops.push_back({dv>0?'R':'L',(int)llabs(dv)});}
                        else{vy+=dv;ops.push_back({dv>0?'U':'D',(int)llabs(dv)});}
                        car--;w--;
                    }
                    ops.push_back({'F',1});
                    px+=vx;py+=vy;
                }
                __int128 dx=X-px,dy=Y-py;
                if(dx*dx+dy*dy<=(__int128)D*D){
                    out.push_back("LoadCarrots "+to_string(K));
                    out.push_back("LoadGift "+gf.name);
                    int pend=0;
                    auto flush=[&](){if(pend){out.push_back("Float "+to_string(pend));pend=0;}};
                    for(auto&o:ops){
                        if(o.first=='F'){pend+=o.second;continue;}
                        flush();
                        const char*nm=o.first=='R'?"AccRight ":o.first=='L'?"AccLeft ":o.first=='U'?"AccUp ":"AccDown ";
                        out.push_back(string(nm)+to_string(o.second));
                    }
                    flush();
                    out.push_back("DeliverGift "+gf.name);
                    done=true;
                }
                (void)bad;
            }
            ll nt=max(Tt+1,(ll)(Tt*1.08));
            if(Tt<T&&nt>T)nt=T;
            if(Tt==T)break;
            Tt=nt;
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&s:out)printf("%s\n",s.c_str());
}
