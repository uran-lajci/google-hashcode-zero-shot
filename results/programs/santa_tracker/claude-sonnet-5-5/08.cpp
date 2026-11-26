#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
struct G{string name;ll s,w,x,y;ll k=0,vx=0,vy=0;bool done=false,ok=false;};
int T,D,W,N;
vector<ll> L,A;
ll amax(ll w){for(int i=0;i<W;i++)if(w<=L[i])return A[i];return 0;}
int main(){
    scanf("%d %d %d %d",&T,&D,&W,&N);
    L.resize(W);A.resize(W);
    for(int i=0;i<W;i++)scanf("%lld %lld",&L[i],&A[i]);
    vector<G> g(N);
    char buf[64];
    for(int i=0;i<N;i++){
        scanf("%s %lld %lld %lld %lld",buf,&g[i].s,&g[i].w,&g[i].x,&g[i].y);
        g[i].name=buf;
    }
    vector<string> out;
    ll D2=(ll)D*D;
    for(auto&q:g){
        if(q.x*q.x+q.y*q.y<=D2){
            out.push_back("LoadGift "+q.name);
            out.push_back("DeliverGift "+q.name);
            q.done=true;
        }
    }
    for(auto&q:g){
        if(q.done)continue;
        ll a=amax(q.w+6);
        ll Aa=a/2;
        if(Aa<=0)continue;
        ll mx=max(llabs(q.x),llabs(q.y));
        if(mx-D>Aa*(ll)T)continue;
        ll k0=max(2LL,(mx-D)/Aa-2);
        for(ll k=k0;k<=T;k++){
            ll vx=llround((double)q.x/k),vy=llround((double)q.y/(k-1));
            if(llabs(vx)>Aa||llabs(vy)>Aa)continue;
            ll dx=k*vx-q.x,dy=(k-1)*vy-q.y;
            if(dx*dx+dy*dy<=D2){q.k=k;q.vx=vx;q.vy=vy;q.ok=true;break;}
        }
    }
    vector<int> ord(N);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return g[a].s>g[b].s;});
    ll cur=0;
    while(true){
        int best=-1;bool one=false;
        for(int i=0;i<N;i++){
            if(g[i].done||!g[i].ok)continue;
            ll c=2*g[i].k+2;
            if(cur+c>T)continue;
            if(best<0||g[i].s*(2*g[best].k+2)>g[best].s*c)best=i;
        }
        if(best<0){
            one=true;
            for(int i=0;i<N;i++){
                if(g[i].done||!g[i].ok)continue;
                if(cur+g[i].k>T)continue;
                if(best<0||g[i].s>g[best].s)best=i;
            }
            if(best<0)break;
        }
        G&p=g[best];
        ll k=p.k,vx=p.vx,vy=p.vy;
        int nz=(vx!=0)+(vy!=0);
        ll mv=max(llabs(vx),llabs(vy));
        vector<int> list={best};
        ll sw=p.w;
        ll px=k*vx,py=(k-1)*vy;
        for(int id:ord){
            if(id==best||g[id].done)continue;
            ll dx=g[id].x-px,dy=g[id].y-py;
            if(dx*dx+dy*dy>D2)continue;
            ll w0=sw+g[id].w+3*nz;
            if(amax(w0)>=2*mv){
                sw+=g[id].w;
                list.push_back(id);
            }
        }
        ll lt=0;
        auto fl=[&](ll to){
            if(to>lt){out.push_back("Float "+to_string(to-lt));lt=to;}
        };
        auto acc=[&](int axis,ll d){
            if(d==0)return;
            if(axis==0)out.push_back((d>0?"AccRight ":"AccLeft ")+to_string(llabs(d)));
            else out.push_back((d>0?"AccUp ":"AccDown ")+to_string(llabs(d)));
        };
        if(nz>0)out.push_back("LoadCarrots "+to_string(3*nz));
        for(int id:list)out.push_back("LoadGift "+g[id].name);
        acc(0,vx);
        fl(1);
        acc(1,vy);
        fl(k);
        for(int id:list){
            out.push_back("DeliverGift "+g[id].name);
            g[id].done=true;
        }
        if(one){break;}
        acc(0,-2*vx);
        fl(k+1);
        acc(1,-2*vy);
        fl(2*k);
        acc(0,vx);
        fl(2*k+1);
        acc(1,vy);
        fl(2*k+2);
        cur+=2*k+2;
    }
    printf("%d\n",(int)out.size());
    for(auto&s:out)printf("%s\n",s.c_str());
}
