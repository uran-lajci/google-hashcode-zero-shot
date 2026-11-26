#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T, D, W, G;
vector<ll> L, A;
vector<string> nm; vector<ll> sc, wt, gx, gy;
ll D2, R0;
chrono::steady_clock::time_point START;
double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-START).count(); }

ll accFor(ll w){ for(int i=0;i<W;i++) if(w<=L[i]) return A[i]; return 0; }
static inline ll isq(ll v){ if(v<=0) return 0; ll r=(ll)sqrtl((long double)v); while(r>0 && r*r>v) r--; while((r+1)*(r+1)<=v) r++; return r; }
static inline bool within(ll x,ll y,ll px,ll py){ __int128 dx=x-px, dy=y-py; return dx*dx+dy*dy <= (__int128)D2; }
void offsets(ll dx, ll dy, ll&ox, ll&oy){ ox=min(dx,R0); oy=min(dy,isq(D2-ox*ox)); ox=min(dx,isq(D2-oy*oy)); }

struct Act{ char type; ll val; };
struct Est{ double t,c; };
Est estAxis(ll d, double a, double m){
    if(d<=0) return {0,0};
    double vm=m*a; double tri=sqrt(d/a);
    if(a*tri<=vm) return {2*tri+2, 2*tri+2};
    return {d/vm+vm/a+2, 2*vm/a+2};
}
Est estMove(ll px,ll py,ll qx,ll qy,double a,double m){
    if(within(px,py,qx,qy)) return {0,0};
    ll dx=llabs(qx-px), dy=llabs(qy-py), ox,oy; offsets(dx,dy,ox,oy);
    Est e1=estAxis(dx-ox,a,m), e2=estAxis(dy-oy,a,m);
    return {e1.t+e2.t, e1.c+e2.c};
}
void targetPt(ll x,ll y,ll qx,ll qy,ll&tx,ll&ty){
    ll dx=qx-x, dy=qy-y, ox,oy; offsets(llabs(dx),llabs(dy),ox,oy);
    tx = qx - (dx>0?ox:(dx<0?-ox:0));
    ty = qy - (dy>0?oy:(dy<0?-oy:0));
}
static inline ll fval(ll v, ll a){ if(v<=0) return 0; ll k=(v-1)/a; return v + k*v - a*k*(k+1)/2; }

struct Sim{
    ll x,y,vx,vy; int t; ll wg,C,used; bool needFloat; vector<Act> acts; double vmMul; int failType=0;
    vector<ll> buf;
    ll weight(){ return wg+C-used; }
    void flt(int n){ t+=n; x+=vx*n; y+=vy*n; if(!acts.empty()&&acts.back().type=='F') acts.back().val+=n; else acts.push_back({'F',(ll)n}); needFloat=false; }
    void acc(int axis, ll diff){ if(needFloat) flt(1); char ty = axis==0 ? (diff>0?'R':'L') : (diff>0?'U':'D'); acts.push_back({ty,llabs(diff)}); used++; if(axis==0) vx+=diff; else vy+=diff; needFloat=true; }
    bool moveAxis(int axis, ll d){
        if(d==0) return true;
        ll a=accFor(weight()); if(a==0){failType=2;return false;}
        ll vm = vmMul>=1e14 ? (LLONG_MAX/4) : max<ll>(a,(ll)(vmMul*a));
        int maxSteps = T - t - (needFloat?1:0);
        if(maxSteps<=0){failType=1;return false;}
        ll ad=llabs(d);
        if(2*sqrt((double)ad/a)-2 > maxSteps){failType=1;return false;}
        buf.clear(); ll prev=0, rem=ad;
        while(rem>0){
            if((int)buf.size()>=maxSteps){failType=1;return false;}
            ll lo=max(0LL,prev-a), hi=min(prev+a,vm);
            while(lo<hi){ ll mid=lo+(hi-lo+1)/2; if(fval(mid,a)<=rem) lo=mid; else hi=mid-1; }
            buf.push_back(lo); rem-=lo; prev=lo;
        }
        ll sg=d>0?1:-1; prev=0;
        for(ll v:buf){ if(v!=prev) acc(axis, sg*(v-prev)); flt(1); prev=v; }
        if(prev!=0) acc(axis,-sg*prev);
        return true;
    }
};
struct GState{ ll x,y; int t; ll car; bool nf; };
struct TripOut{ bool ok=false; int failType=0, failIdx=0; Sim s; vector<int> deliv; ll score=0; bool returned=false; ll C=0; };

TripOut simTrip(const GState& g0, const vector<int>& route, ll C, double vm, bool needRet){
    TripOut o; Sim& s=o.s; s.x=g0.x; s.y=g0.y; s.vx=s.vy=0; s.t=g0.t; s.wg=0; s.C=C; s.used=0; s.needFloat=g0.nf; s.vmMul=vm; o.C=C;
    if(C>g0.car) s.acts.push_back({'C',C-g0.car});
    int k=route.size();
    for(int j=0;j<k;j++){ s.acts.push_back({'G',(ll)route[j]}); s.wg+=wt[route[j]]; }
    vector<char> dl(k,0);
    auto dh=[&](){ for(int j=0;j<k;j++){ int g=route[j]; if(!dl[j]&&within(s.x,s.y,gx[g],gy[g])){ dl[j]=1; s.acts.push_back({'X',(ll)g}); s.wg-=wt[g]; o.score+=sc[g]; o.deliv.push_back(g);} } };
    dh();
    for(int j=0;j<k;j++){
        if(dl[j]) continue; int g=route[j]; ll tx,ty; targetPt(s.x,s.y,gx[g],gy[g],tx,ty);
        if(!s.moveAxis(0,tx-s.x)||!s.moveAxis(1,ty-s.y)){ o.failType=s.failType; o.failIdx=j; return o; }
        dh(); if(!dl[j]){ o.failType=1; o.failIdx=j; return o; }
    }
    if(needRet){
        if(!within(s.x,s.y,0,0)){
            ll tx,ty; targetPt(s.x,s.y,0,0,tx,ty);
            if(!s.moveAxis(0,tx-s.x)||!s.moveAxis(1,ty-s.y)){ o.failType=1; o.failIdx=k-1; return o; }
        }
        o.returned=true;
    }
    if(s.used>C){ o.failType=3; return o; }
    o.ok=true; return o;
}

bool refine(const GState& st, vector<int> route, double carEst, double vm, bool needRet, TripOut& out){
    ll C=max(st.car,(ll)ceil(carEst));
    for(int it=0; it<80 && !route.empty(); it++){
        TripOut o=simTrip(st,route,C,vm,needRet);
        if(o.ok){ out=move(o); return true; }
        if(o.failType==3){ C=max(C+1,o.s.used); continue; }
        route.resize(o.failIdx);
    }
    return false;
}

vector<int> build(const GState& st, const vector<int>& pool, int cap, double vm, bool needRet, double& carEstOut){
    double a=A[cap]; ll Wc=L[cap]; int P=pool.size();
    vector<Est> back(P); for(int i=0;i<P;i++){ int g=pool[i]; back[i]=estMove(gx[g],gy[g],0,0,a,vm); }
    vector<char> usd(P,0);
    ll cx=st.x, cy=st.y; Est cb=estMove(cx,cy,0,0,a,vm);
    double tU=0,cE=0; ll gW=0; vector<int> route; double rem=T-st.t;
    while(true){
        int bi=-1; double br=-1; Est be{0,0};
        for(int i=0;i<P;i++){
            if(usd[i]) continue; int g=pool[i];
            Est e1=estMove(cx,cy,gx[g],gy[g],a,vm);
            double tn=tU+e1.t+(needRet?back[i].t:0); if(tn>rem) continue;
            double cn=cE+e1.c+(needRet?back[i].c:0); if((double)(gW+wt[g])+cn>(double)Wc) continue;
            double cost=e1.t+(needRet?back[i].t-cb.t:0);
            double r=sc[g]/max(cost,0.5);
            if(r>br){br=r;bi=i;be=e1;}
        }
        if(bi<0) break;
        usd[bi]=1; int g=pool[bi]; route.push_back(g); tU+=be.t; cE+=be.c; gW+=wt[g];
        if(!within(cx,cy,gx[g],gy[g])){ ll tx,ty; targetPt(cx,cy,gx[g],gy[g],tx,ty); cx=tx; cy=ty; cb=estMove(cx,cy,0,0,a,vm); }
    }
    carEstOut=cE+(needRet?cb.c:0);
    return route;
}

vector<int> activeSorted;

pair<ll,vector<Act>> run(double vm){
    GState st{0,0,0,0,false}; vector<char> done(G,0); vector<Act> acts; ll score=0;
    vector<int> act=activeSorted; int lastCap=W-1; const int PS=800;
    while(st.t<T){
        double el=elapsed(); if(el>9.2) break;
        if(!within(st.x,st.y,0,0)) break;
        for(int g:act) if(!done[g]&&within(st.x,st.y,gx[g],gy[g])){ acts.push_back({'G',(ll)g}); acts.push_back({'X',(ll)g}); done[g]=1; score+=sc[g]; }
        act.erase(remove_if(act.begin(),act.end(),[&](int g){return (bool)done[g];}),act.end());
        vector<int> pool; for(int g:act){ if((int)pool.size()>=PS) break; pool.push_back(g); }
        if(pool.empty()) break;
        vector<int> caps; if(el>7.5) caps={lastCap}; else for(int i=0;i<W;i++) caps.push_back(i);
        bool found=false; TripOut best; double bestRate=-1; int bestCap=lastCap;
        for(int cap:caps){
            double ce; vector<int> r=build(st,pool,cap,vm,true,ce); if(r.empty()) continue;
            TripOut o; if(refine(st,r,ce,vm,true,o)){
                double rate=o.score/(double)(o.s.t-st.t+1);
                if(o.score>0&&rate>bestRate){bestRate=rate;best=move(o);found=true;bestCap=cap;}
            }
        }
        if(!found){
            ll bs=0;
            for(int cap:caps){
                double ce; vector<int> r=build(st,pool,cap,vm,false,ce); if(r.empty()) continue;
                TripOut o; if(refine(st,r,ce,vm,false,o)){
                    if(o.score>bs){bs=o.score;best=move(o);found=true;bestCap=cap;}
                }
            }
        }
        if(!found) break;
        lastCap=bestCap;
        for(auto&a:best.s.acts) acts.push_back(a);
        for(int g:best.deliv) done[g]=1;
        score+=best.score;
        st.x=best.s.x; st.y=best.s.y; st.t=best.s.t; st.car=best.C-best.s.used; st.nf=best.s.needFloat;
        if(!best.returned) break;
    }
    return {score,acts};
}

int main(){
    START=chrono::steady_clock::now();
    ios::sync_with_stdio(false); cin.tie(nullptr);
    cin>>T>>D>>W>>G;
    L.resize(W); A.resize(W);
    for(int i=0;i<W;i++) cin>>L[i]>>A[i];
    nm.resize(G); sc.resize(G); wt.resize(G); gx.resize(G); gy.resize(G);
    for(int i=0;i<G;i++) cin>>nm[i]>>sc[i]>>wt[i]>>gx[i]>>gy[i];
    D2=(ll)D*D; R0=isq(D2/2);
    vector<pair<double,int>> key;
    for(int g=0; g<G; g++){
        bool near0=within(0,0,gx[g],gy[g]);
        Est e=estMove(0,0,gx[g],gy[g],(double)A[0],1e15);
        if(!near0){
            if(e.t>1.2*T) continue;
            if(wt[g]+1>L[W-1]) continue;
        }
        key.push_back({-(sc[g]/(2*e.t+1)), g});
    }
    sort(key.begin(),key.end());
    for(auto&p:key) activeSorted.push_back(p.second);

    vector<double> vms={1e15,30,15,8,4,2};
    ll bestScore=-1; vector<Act> bestActs;
    double lastDur=0;
    for(size_t i=0;i<vms.size();i++){
        double s0=elapsed();
        if(i>0 && s0+lastDur*1.15>9.0) break;
        auto r=run(vms[i]);
        lastDur=elapsed()-s0;
        if(r.first>bestScore){bestScore=r.first; bestActs=move(r.second);}
    }
    string out; out.reserve(bestActs.size()*24+16);
    out+=to_string(bestActs.size()); out+='\n';
    for(auto&a:bestActs){
        switch(a.type){
            case 'U': out+="AccUp "+to_string(a.val); break;
            case 'D': out+="AccDown "+to_string(a.val); break;
            case 'L': out+="AccLeft "+to_string(a.val); break;
            case 'R': out+="AccRight "+to_string(a.val); break;
            case 'F': out+="Float "+to_string(a.val); break;
            case 'C': out+="LoadCarrots "+to_string(a.val); break;
            case 'G': out+="LoadGift "+nm[a.val]; break;
            case 'X': out+="DeliverGift "+nm[a.val]; break;
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
