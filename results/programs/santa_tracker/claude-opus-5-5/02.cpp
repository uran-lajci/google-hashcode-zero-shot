#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T, D, W, G;
vector<ll> Lw, Aw;
vector<string> nm; vector<ll> gs, gw, gx, gy;
ll USH;
vector<vector<int>> nbr;
ll OPS = 0;

int amax(ll w){ for(int i=0;i<W;i++) if(w<=Lw[i]) return (int)Aw[i]; return 0; }
ll Fk(ll n, ll K){ if(n<=2*K) return (n+1)*(n+1)/4; return K*(K+1)+(n-2*K)*K; }
ll minN(ll d, ll a, ll K){
    if(d<=0) return 0; ll m=(d+a-1)/a;
    if(m<=K*(K+1)){ ll n=max(0LL,(ll)ceil(2*sqrt((double)m))-3); while(Fk(n,K)<m) n++; return n; }
    return 2*K+(m-K*(K+1)+K-1)/K;
}
const double INF=1e18;
void axisEst(ll d, ll a, ll K, double &tm, ll &c){
    if(d<=0) return; ll n=minN(d,a,K); tm+=n+1; c+=min(n+1,2*K+2);
}
void legEst(ll x1,ll y1,ll x2,ll y2,ll a,ll K,double&tm,ll&c){
    OPS++;
    ll dx=llabs(x2-x1)-USH, dy=llabs(y2-y1)-USH;
    axisEst(dx,a,K,tm,c); axisEst(dy,a,K,tm,c);
}
// full estimate
void estFull(const vector<int>&r,bool ret,ll sx,ll sy,ll baseC,ll K,double&tm,ll&C,ll&A){
    ll Wt=0; for(int g:r) Wt+=gw[g];
    ll c=baseC; tm=INF; C=0; A=0;
    for(int it=0;it<2;it++){
        ll a=amax(Wt+c); if(a==0){tm=INF;return;}
        double t=0; ll cc=0; ll px=sx,py=sy;
        for(int g:r){ legEst(px,py,gx[g],gy[g],a,K,t,cc); px=gx[g];py=gy[g]; }
        if(ret) legEst(px,py,0,0,a,K,t,cc);
        tm=t; C=cc; A=a; c=max(baseC,cc);
    }
}

vector<ll> profile(ll d, ll a, ll K){
    ll n=minN(d,a,K); vector<ll> cap(n), v(n);
    ll mx=0;
    for(ll i=0;i<n;i++){ ll k=min({i+1,n-i,K}); cap[i]=a*k; mx=max(mx,cap[i]); }
    auto f=[&](ll h){ ll s=0; for(ll i=0;i<n;i++) s+=min(cap[i],h); return s; };
    ll lo=0,hi=mx;
    while(lo<hi){ ll mid=(lo+hi+1)/2; if(f(mid)<=d) lo=mid; else hi=mid-1; }
    ll h=lo; ll r=d-f(h);
    for(ll i=0;i<n;i++){ v[i]=min(cap[i],h); if(r>0&&cap[i]>=h+1){v[i]++;r--;} }
    return v;
}

struct Sim{
    ll x,y,t,carrots,giftW=0,score=0,cap,used=0,K;
    vector<int> trip; vector<char> dl;
    vector<pair<int,ll>> acts; ll pend=0;
    bool trunc=false, fail=false;
    void flush(){ if(pend){acts.push_back({4,pend});pend=0;} }
    void check(){
        for(size_t i=0;i<trip.size();i++) if(!dl[i]){
            int g=trip[i]; ll dx=gx[g]-x, dy=gy[g]-y;
            if(llabs(dx)>D||llabs(dy)>D) continue;
            if(dx*dx+dy*dy<=(ll)D*D){ flush(); acts.push_back({7,g}); dl[i]=1; giftW-=gw[g]; score+=gs[g]; }
        }
    }
    bool axisMove(int ax, ll d){
        if(d==0) return true;
        ll a=amax(giftW+carrots); if(a==0){fail=true;return false;}
        ll n=minN(llabs(d),a,K);
        if(t+n+1>cap){trunc=true;return false;}
        vector<ll> v=profile(llabs(d),a,K);
        int sg=d>0?1:-1; ll cur=0;
        for(ll i=0;i<=n;i++){
            ll vt=i<n?sg*v[i]:0;
            if(vt!=cur){
                flush(); ll diff=vt-cur; int ty;
                if(ax==0) ty=diff>0?3:2; else ty=diff>0?0:1;
                acts.push_back({ty,llabs(diff)}); used++;
                if(carrots>0) carrots--;
                cur=vt;
            }
            if(t+1>cap){trunc=true;return false;}
            pend++; t++; OPS++;
            if(ax==0) x+=cur; else y+=cur;
            check();
        }
        return true;
    }
    static ll tgt(ll g, ll p){ ll d=g-p; ll s=d>0?1:(d<0?-1:0); return g-s*min(llabs(d),USH); }
    bool runTrip(const vector<int>&route, ll load, bool ret){
        if(load>0){acts.push_back({5,load}); carrots+=load;}
        trip=route; dl.assign(route.size(),0);
        for(int g:route){acts.push_back({6,g}); giftW+=gw[g];}
        check();
        for(size_t i=0;i<route.size();i++){
            if(dl[i]) continue; int g=route[i];
            ll tx=tgt(gx[g],x);
            if(!axisMove(0,tx-x)){flush();return false;}
            if(dl[i]) continue;
            ll ty=tgt(gy[g],y);
            if(!axisMove(1,ty-y)){flush();return false;}
        }
        if(ret){
            ll tx=tgt(0,x); if(!axisMove(0,tx-x)){flush();return false;}
            ll ty=tgt(0,y); if(!axisMove(1,ty-y)){flush();return false;}
        }
        flush(); return true;
    }
};

struct State{ ll x=0,y=0,t=0,carrots=0,score=0; vector<pair<int,ll>> acts; vector<char> delivered, banned; };

vector<char> inR;

vector<int> build(State&S,int seed,bool ret,ll rem,ll K,double alpha){
    vector<int> route={seed}; inR[seed]=1;
    double curT; ll curC,curA; estFull(route,ret,S.x,S.y,S.carrots,K,curT,curC,curA);
    ll Wt=gw[seed], Sc=gs[seed];
    while(true){
        int last=route.back(); int best=-1; double bestR=-1,bT=0; ll bC=0,bA=0;
        for(int c:nbr[last]){
            if(S.delivered[c]||S.banned[c]||inR[c]) continue;
            ll anew=amax(Wt+gw[c]+max(S.carrots,curC+4));
            double nt; ll nc, na;
            if(anew==0) continue;
            if(anew==curA){
                double t2=curT; ll c2=curC;
                double tb=0; ll cb=0;
                if(ret){ legEst(gx[last],gy[last],0,0,curA,K,tb,cb); }
                t2-=tb; c2-=cb;
                legEst(gx[last],gy[last],gx[c],gy[c],curA,K,t2,c2);
                if(ret) legEst(gx[c],gy[c],0,0,curA,K,t2,c2);
                nt=t2; nc=c2; na=curA;
            } else {
                route.push_back(c); estFull(route,ret,S.x,S.y,S.carrots,K,nt,nc,na); route.pop_back();
            }
            if(nt>rem) continue;
            double marg=nt-curT; double r=marg<=1e-9?1e18:gs[c]/marg;
            if(r>bestR){bestR=r;best=c;bT=nt;bC=nc;bA=na;}
        }
        if(best<0) break;
        double thr=alpha*Sc/max(curT,1e-9);
        if(bestR<thr) break;
        route.push_back(best); inR[best]=1; Wt+=gw[best]; Sc+=gs[best];
        curT=bT; curC=bC; curA=bA;
        if(route.size()>2000) break;
    }
    for(int g:route) inR[g]=0;
    return route;
}

// attempt; returns true if committed
bool attempt(State&S, vector<int> route, bool ret, ll K){
    while(!route.empty()){
        ll total=S.carrots; bool popit=false;
        for(int it=0;it<60;it++){
            Sim s; s.x=S.x; s.y=S.y; s.t=S.t; s.carrots=S.carrots; s.cap=T; s.K=K;
            bool ok=s.runTrip(route,total-S.carrots,ret);
            if(s.fail){popit=true;break;}
            if(s.used>total){ total=s.used; continue; }
            if(!ok && ret){popit=true;break;}
            // commit
            for(auto&a:s.acts) S.acts.push_back(a);
            S.x=s.x;S.y=s.y;S.t=s.t;S.carrots=s.carrots;S.score+=s.score;
            for(size_t i=0;i<route.size();i++) if(s.dl[i]) S.delivered[route[i]]=1;
            return true;
        }
        (void)popit;
        route.pop_back();
    }
    return false;
}

State run(ll K,double alpha){
    State S; S.delivered.assign(G,0); S.banned.assign(G,0);
    vector<double> seedT(G); vector<int> order;
    for(int g=0;g<G;g++){ double tm; ll C,A; estFull({g},true,0,0,0,K,tm,C,A); seedT[g]=tm; if(tm<INF/2&&tm<=T) order.push_back(g); }
    sort(order.begin(),order.end(),[&](int a,int b){ double ra=gs[a]/(seedT[a]+1), rb=gs[b]/(seedT[b]+1); if(ra!=rb) return ra>rb; return a<b; });
    int M=order.size(); vector<int> nxt(M); for(int i=0;i<M;i++) nxt[i]=i+1<M?i+1:-1;
    int head=M?0:-1;
    while(true){
        ll rem=T-S.t; if(rem<=0) break;
        int prev=-1,cur=head,seed=-1;
        while(cur!=-1){
            int g=order[cur];
            if(S.delivered[g]||S.banned[g]||seedT[g]>rem){
                int n2=nxt[cur]; if(prev==-1) head=n2; else nxt[prev]=n2; cur=n2; continue;
            }
            seed=g; break;
        }
        if(seed<0) break;
        vector<int> route=build(S,seed,true,rem,K,alpha);
        if(!attempt(S,route,true,K)) S.banned[seed]=1;
    }
    ll rem=T-S.t;
    if(rem>0){
        int best=-1; double br=-1;
        for(int g=0;g<G;g++){ if(S.delivered[g]) continue; double tm; ll C,A; estFull({g},false,S.x,S.y,S.carrots,K,tm,C,A); if(tm>rem) continue; double r=gs[g]/(tm+1); if(r>br){br=r;best=g;} }
        if(best>=0){ fill(S.banned.begin(),S.banned.end(),0); vector<int> route=build(S,best,false,rem,K,alpha); attempt(S,route,false,K); }
    }
    return S;
}

int main(){
    scanf("%d %d %d %d",&T,&D,&W,&G);
    Lw.resize(W);Aw.resize(W);
    for(int i=0;i<W;i++) scanf("%lld %lld",&Lw[i],&Aw[i]);
    nm.resize(G);gs.resize(G);gw.resize(G);gx.resize(G);gy.resize(G);
    char buf[64];
    for(int i=0;i<G;i++){ scanf("%s %lld %lld %lld %lld",buf,&gs[i],&gw[i],&gx[i],&gy[i]); nm[i]=buf; }
    USH=(ll)floor(D/sqrt(2.0)); while(USH>0&&2*USH*USH>(ll)D*D) USH--; while(2*(USH+1)*(USH+1)<=(ll)D*D) USH++;
    inR.assign(G,0);
    int NB=30; nbr.assign(G,{});
    {
        vector<pair<double,int>> tmp(G);
        for(int i=0;i<G;i++){
            int m=0;
            for(int j=0;j<G;j++) if(j!=i){ double dx=gx[i]-gx[j],dy=gy[i]-gy[j]; tmp[m++]={dx*dx+dy*dy,j}; }
            int k=min(NB,m);
            if(k>0){ nth_element(tmp.begin(),tmp.begin()+k-1,tmp.begin()+m); sort(tmp.begin(),tmp.begin()+k); }
            for(int q=0;q<k;q++) nbr[i].push_back(tmp[q].second);
        }
    }
    vector<ll> Ks={1000000,8,4,20,2,12,40,3,6,1};
    vector<double> alphas={1.0,0.6};
    State best; best.score=-1;
    for(double al:alphas) for(ll K:Ks){
        if(OPS>300000000LL) break;
        State S=run(K,al);
        if(S.score>best.score) best=move(S);
    }
    // output
    string out; out.reserve(best.acts.size()*20+20);
    out+=to_string(best.acts.size()); out+='\n';
    static const char* nmT[]={"AccUp ","AccDown ","AccLeft ","AccRight ","Float ","LoadCarrots ","LoadGift ","DeliverGift "};
    for(auto&a:best.acts){
        out+=nmT[a.first];
        if(a.first==6||a.first==7) out+=nm[a.second]; else out+=to_string(a.second);
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
