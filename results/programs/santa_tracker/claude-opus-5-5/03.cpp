#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T,D,W,G; ll D2;
vector<ll> LIM,ACC; vector<string> NM; vector<ll> SC,WT,GX,GY;

static inline int maxAcc(ll w){ for(int i=0;i<W;i++) if(w<=LIM[i]) return (int)ACC[i]; return 0;}
static inline bool inR(ll ax,ll ay,ll bx,ll by){ll dx=ax-bx,dy=ay-by; return dx*dx+dy*dy<=D2;}
struct Prof{ll n,c;};
static inline ll kOf(ll h,ll a){ ll k=(ll)sqrt((double)h/(double)a); while(k>0&&a*k*k>h)k--; while(a*(k+1)*(k+1)<=h)k++; return k;}
static inline Prof prof(ll h,ll a){ if(!h) return {0,0}; ll k=kOf(h,a); if(!k) return {1,2}; ll R=h-a*k*k,q=R/a,r=R%a; return {2*k-1+q/k+(q%k>0)+(r>0), 2*k-1+(r>0?2:1)}; }
vector<ll> profVel(ll h,ll a){ vector<ll> v; if(!h) return v; ll k=kOf(h,a); if(!k){v.push_back(h);return v;}
    ll R=h-a*k*k,q=R/a,r=R%a; for(ll i=1;i<=k;i++)v.push_back(i*a); for(ll j=0;j<q/k;j++)v.push_back(k*a);
    ll e=q%k; for(ll l=k-1;l>=1;l--){v.push_back(l*a); if(l==e)v.push_back(l*a);} if(r)v.push_back(r); return v;}
struct MC{ll L,c; int mode; ll tx,ty;};
static inline MC seqCost(ll dx,ll dy,ll a){Prof x=prof(llabs(dx),a),y=prof(llabs(dy),a); MC m; m.L=(x.n&&y.n)?x.n+1+y.n:max(x.n,y.n); m.c=x.c+y.c; m.mode=1; m.tx=m.ty=0; return m;}
static inline MC dilCost(ll dx,ll dy,ll a){Prof x=prof(llabs(dx)/2,a),y=prof(llabs(dy)/2,a); ll nl=max(x.n,y.n),ns=min(x.n,y.n); MC m; m.L=ns==0?2*nl:max(2*nl,2*ns+1); m.c=x.c+y.c; m.mode=2; m.tx=m.ty=0; return m;}
static inline void approach(ll px,ll py,ll gx,ll gy,ll&sx,ll&sy){
    if(D==0){sx=gx;sy=gy;return;}
    double ux=(double)(px-gx),uy=(double)(py-gy),L=sqrt(ux*ux+uy*uy); double s=(D-0.75)/L; if(s<0)s=0;
    sx=gx+llround(ux*s); sy=gy+llround(uy*s); if(!inR(sx,sy,gx,gy)){sx=gx;sy=gy;}
}
static inline bool parAdj(ll px,ll py,ll gx,ll gy,ll sx,ll sy,ll&ox,ll&oy){
    bool ex=((sx-px)&1), ey=((sy-py)&1);
    if(!ex&&!ey){ox=sx;oy=sy;return true;}
    ll cx[2],cy[2]; int nx=1,ny=1;
    if(ex){ll d=(gx>sx)?1:-1; cx[0]=d;cx[1]=-d;nx=2;} else cx[0]=0;
    if(ey){ll d=(gy>sy)?1:-1; cy[0]=d;cy[1]=-d;ny=2;} else cy[0]=0;
    for(int i=0;i<nx;i++)for(int j=0;j<ny;j++) if(inR(sx+cx[i],sy+cy[j],gx,gy)){ox=sx+cx[i];oy=sy+cy[j];return true;}
    return false;
}
static inline MC bestMove(ll px,ll py,ll gx,ll gy,ll a){
    if(inR(px,py,gx,gy)) return {0,0,0,px,py};
    ll sx,sy; approach(px,py,gx,gy,sx,sy);
    MC m=seqCost(sx-px,sy-py,a); m.tx=sx; m.ty=sy;
    ll ox,oy; if(parAdj(px,py,gx,gy,sx,sy,ox,oy)){ MC m2=dilCost(ox-px,oy-py,a); if(m2.L<m.L||(m2.L==m.L&&m2.c<m.c)){m=m2;m.tx=ox;m.ty=oy;} }
    return m;
}
static inline MC costFixed(ll px,ll py,ll tx,ll ty,ll a){
    ll dx=tx-px,dy=ty-py; if(!dx&&!dy) return {0,0,0,tx,ty};
    MC m=seqCost(dx,dy,a);
    if(!(dx&1)&&!(dy&1)){MC m2=dilCost(dx,dy,a); if(m2.L<m.L||(m2.L==m.L&&m2.c<m.c)) m=m2;}
    m.tx=tx;m.ty=ty; return m;
}
static inline MC costByMode(ll px,ll py,ll tx,ll ty,ll a,int mode){
    ll dx=tx-px,dy=ty-py; if(!dx&&!dy) return {0,0,0,tx,ty};
    MC m = mode==2? dilCost(dx,dy,a):seqCost(dx,dy,a); m.tx=tx;m.ty=ty; return m;
}
struct Move{ll tx,ty; int a,mode; ll L,c; vector<int> gifts;};
struct St{ll t,x,y,vx,vy,carrots,gw; bool nf;};
struct Act{int type; ll arg;};
struct Cfg{ll Ccap,Bw; bool contN;};
struct Cand{double r; int g; MC mc;};

ll evals=0; bool canAbort=false; ll HARD=60000000;
vector<char> asg; vector<int> active; vector<Act> acts;

bool replay(const St& st,ll C0,const vector<Move>& mv,bool hasRet,const Move& rs,bool refine,vector<Move>& out,Move& rOut,ll& tEnd){
    ll carrots=C0, undel=st.gw; for(auto&m:mv) for(int g:m.gifts) undel+=WT[g];
    ll t=st.t; bool nf=st.nf; ll px=st.x,py=st.y; out.clear();
    auto doMove=[&](const Move& m,Move& m2,bool isRet)->bool{
        ll w=carrots+undel; int amax=maxAcc(w); m2=m;
        MC mc;
        if(isRet&&refine){ if(inR(px,py,0,0)) mc={0,0,0,px,py}; else { if(!amax) return false; mc=bestMove(px,py,0,0,amax);} m2.tx=mc.tx;m2.ty=mc.ty; m2.a=amax; }
        else if(m.tx==px&&m.ty==py) mc={0,0,0,px,py};
        else if(refine){ if(!amax) return false; mc=costFixed(px,py,m.tx,m.ty,amax); m2.a=amax; }
        else { if(m.a>amax||m.a<=0) return false; mc=costByMode(px,py,m.tx,m.ty,m.a,m.mode); }
        m2.mode=mc.mode; m2.L=mc.L; m2.c=mc.c;
        if(mc.L>0){ ll tS=t+(nf?1:0); t=tS+mc.L; nf=true; if(t>T) return false; carrots-=mc.c; if(carrots<0) return false; }
        for(int g:m.gifts) undel-=WT[g];
        px=m2.tx; py=m2.ty; return true;
    };
    for(auto&m:mv){ Move m2; if(!doMove(m,m2,false)) return false; out.push_back(m2);}
    if(hasRet){ if(!doMove(rs,rOut,true)) return false; }
    tEnd=t; return true;
}

void emitMove(St& s,const Move& m){
    if(m.tx==s.x&&m.ty==s.y) return;
    if(s.nf){acts.push_back({4,1}); s.t+=1; s.x+=s.vx; s.y+=s.vy; s.nf=false;}
    ll dx=m.tx-s.x, dy=m.ty-s.y; ll sx=dx<0?-1:1, sy=dy<0?-1:1;
    struct Ev{ll slot;int ax;ll v;}; vector<Ev> ev;
    if(m.mode==1){
        auto vx=profVel(llabs(dx),m.a); ll n=vx.size();
        for(ll i=0;i<n;i++) ev.push_back({i,0,sx*vx[i]}); if(n) ev.push_back({n,0,0});
        ll off=n?n+1:0; auto vy=profVel(llabs(dy),m.a); ll ny=vy.size();
        for(ll i=0;i<ny;i++) ev.push_back({off+i,1,sy*vy[i]}); if(ny) ev.push_back({off+ny,1,0});
    } else {
        auto vx=profVel(llabs(dx)/2,m.a); auto vy=profVel(llabs(dy)/2,m.a);
        bool xl=vx.size()>=vy.size();
        vector<ll>& Lv= xl?vx:vy; vector<ll>& Sv= xl?vy:vx; int axL=xl?0:1, axS=xl?1:0; ll sgL= xl?sx:sy, sgS= xl?sy:sx;
        ll nL=Lv.size(), nS=Sv.size();
        for(ll i=0;i<nL;i++) ev.push_back({2*i,axL,sgL*Lv[i]}); if(nL) ev.push_back({2*nL,axL,0});
        for(ll i=0;i<nS;i++) ev.push_back({2*i+1,axS,sgS*Sv[i]}); if(nS) ev.push_back({2*nS+1,axS,0});
    }
    sort(ev.begin(),ev.end(),[](const Ev&a,const Ev&b){return a.slot<b.slot;});
    ll cur=0;
    for(auto&e:ev){
        if(e.slot>cur){ ll dt=e.slot-cur; acts.push_back({4,dt}); s.t+=dt; s.x+=s.vx*dt; s.y+=s.vy*dt; cur=e.slot; }
        ll& v= e.ax? s.vy: s.vx; ll d=e.v-v; int type;
        if(e.ax==0) type= d>0?3:2; else type= d>0?0:1;
        acts.push_back({type,llabs(d)}); v=e.v; s.carrots--;
    }
    s.nf=true;
}

bool planTrip(St& st,const Cfg& cf,bool startN,bool& finalTrip,bool& aborted){
    ll C0=max(st.carrots,cf.Ccap), Bw=cf.Bw;
    bool modeN=startN, switched=startN, fin=false;
    ll t=st.t; bool nf=st.nf; ll px=st.x,py=st.y, used=0,deliv=0,routeW=0;
    vector<Move> mv; Move lastRet; bool haveRet=false;
    vector<Cand> cands; cands.reserve(active.size());
    auto cmp=[](const Cand&a,const Cand&b){ if(a.r!=b.r) return a.r<b.r; return a.g>b.g;};
    while(true){
        if(canAbort&&evals>HARD){aborted=true;break;}
        ll w=C0-used+Bw-deliv; int a=maxAcc(w); if(!a) break;
        ll tS=t+(nf?1:0);
        cands.clear(); size_t j=0;
        for(size_t i=0;i<active.size();i++){ int g=active[i]; if(asg[g]) continue; active[j++]=g;
            if(routeW+WT[g]>Bw) continue;
            MC mc=bestMove(px,py,GX[g],GY[g],a); evals++;
            ll te= mc.L>0? tS+mc.L : t; if(te>T) continue; if(used+mc.c>C0) continue;
            cands.push_back({(double)SC[g]/(double)(mc.L+1),g,mc});
        }
        active.resize(j);
        int chosen=-1; MC cm{}; Move retM;
        make_heap(cands.begin(),cands.end(),cmp);
        while(!cands.empty()){
            pop_heap(cands.begin(),cands.end(),cmp); Cand c=cands.back(); cands.pop_back();
            if(modeN){chosen=c.g;cm=c.mc;break;}
            ll te=c.mc.L>0?tS+c.mc.L:t; bool nf2=c.mc.L>0?true:nf;
            ll w2=C0-used-c.mc.c+Bw-deliv-WT[c.g]; int a2=maxAcc(w2);
            MC r; if(inR(c.mc.tx,c.mc.ty,0,0)) r={0,0,0,c.mc.tx,c.mc.ty}; else { if(!a2) continue; r=bestMove(c.mc.tx,c.mc.ty,0,0,a2); }
            ll tr= r.L>0? te+(nf2?1:0)+r.L : te; if(tr>T) continue; if(used+c.mc.c+r.c>C0) continue;
            chosen=c.g; cm=c.mc; retM=Move{r.tx,r.ty,a2,r.mode,r.L,r.c,{}}; break;
        }
        if(chosen<0){ if(!switched&&cf.contN){switched=true;modeN=true;continue;} break; }
        Move m{cm.tx,cm.ty,a,cm.mode,cm.L,cm.c,{chosen}}; asg[chosen]=1; routeW+=WT[chosen]; ll dw=WT[chosen];
        for(int h:active) if(!asg[h]&&routeW+WT[h]<=Bw&&inR(cm.tx,cm.ty,GX[h],GY[h])){asg[h]=1;routeW+=WT[h];dw+=WT[h];m.gifts.push_back(h);}
        deliv+=dw; used+=cm.c; if(cm.L>0){t=tS+cm.L;nf=true;} px=cm.tx;py=cm.ty;
        mv.push_back(m);
        if(modeN) fin=true; else {lastRet=retM;haveRet=true;}
    }
    auto revert=[&]{for(auto&m:mv)for(int g:m.gifts)asg[g]=0;};
    if(aborted){revert();return false;}
    if(mv.empty()) return false;
    bool hasRet=!fin;
    if(hasRet&&!haveRet){revert();return false;}
    vector<Move> o1,o2; Move r1,r2; ll te1=0,te2=0;
    bool ok1=replay(st,C0,mv,hasRet,lastRet,false,o1,r1,te1);
    if(!ok1){revert();return false;}
    bool ok2=replay(st,C0,mv,hasRet,lastRet,true,o2,r2,te2);
    vector<Move>& U = (ok2&&te2<=te1)? o2:o1; Move& R = (ok2&&te2<=te1)? r2:r1;
    if(C0>st.carrots){acts.push_back({5,C0-st.carrots}); st.carrots=C0;}
    for(auto&m:U) for(int g:m.gifts){acts.push_back({6,g}); st.gw+=WT[g];}
    for(auto&m:U){ emitMove(st,m); for(int g:m.gifts){acts.push_back({7,g}); st.gw-=WT[g];} }
    if(hasRet) emitMove(st,R);
    finalTrip=fin;
    return true;
}

ll validate(const vector<Act>& A){
    if(A.size()>1000000) return -1;
    ll t=0,x=0,y=0,vx=0,vy=0,car=0,gw=0; bool lastAcc=false; ll score=0;
    vector<char> ld(G,0),dl(G,0);
    for(auto&a:A){
        if(a.type<=3){
            if(lastAcc||car<=0) return -1; if(a.arg<0||a.arg>maxAcc(car+gw)) return -1;
            if(a.type==0)vy+=a.arg; else if(a.type==1)vy-=a.arg; else if(a.type==2)vx-=a.arg; else vx+=a.arg;
            car--; lastAcc=true;
        } else if(a.type==4){
            if(a.arg<1||t+a.arg>T) return -1; x+=vx*a.arg; y+=vy*a.arg; t+=a.arg; lastAcc=false;
        } else if(a.type==5){
            if(a.arg<1||a.arg>1000000||!inR(x,y,0,0)) return -1; car+=a.arg;
        } else if(a.type==6){
            int g=(int)a.arg; if(ld[g]||!inR(x,y,0,0)) return -1; ld[g]=1; gw+=WT[g];
        } else {
            int g=(int)a.arg; if(!ld[g]||dl[g]||!inR(x,y,GX[g],GY[g])) return -1; dl[g]=1; gw-=WT[g]; score+=SC[g];
        }
    }
    return score;
}

bool runCfg(const Cfg& cf){
    asg.assign(G,0); active.resize(G); for(int i=0;i<G;i++)active[i]=i; acts.clear();
    St st{0,0,0,0,0,0,0,false}; bool aborted=false;
    while(true){
        for(int g:active) if(!asg[g]&&inR(st.x,st.y,GX[g],GY[g])){acts.push_back({6,g});acts.push_back({7,g});asg[g]=1;}
        bool fin=false,ab=false;
        if(planTrip(st,cf,false,fin,ab)){ if(fin)break; continue; }
        if(ab){aborted=true;break;}
        if(planTrip(st,cf,true,fin,ab)) break;
        if(ab) aborted=true;
        break;
    }
    return !aborted;
}

int main(){
    if(scanf("%d %d %d %d",&T,&D,&W,&G)!=4) {printf("0\n");return 0;}
    D2=(ll)D*D; LIM.resize(W);ACC.resize(W);
    for(int i=0;i<W;i++) scanf("%lld %lld",&LIM[i],&ACC[i]);
    NM.resize(G);SC.resize(G);WT.resize(G);GX.resize(G);GY.resize(G);
    char buf[64]; ll minW=LLONG_MAX;
    for(int i=0;i<G;i++){ scanf("%s %lld %lld %lld %lld",buf,&SC[i],&WT[i],&GX[i],&GY[i]); NM[i]=buf; minW=min(minW,WT[i]); }
    vector<Cfg> cfgs; set<tuple<ll,ll,int>> seen;
    vector<int> lv; int mid=W/2; lv.push_back(mid); for(int d=1;d<W;d++){ if(mid-d>=0)lv.push_back(mid-d); if(mid+d<W)lv.push_back(mid+d); }
    auto add=[&](ll Ccap,ll L,bool cn){ Ccap=max(1LL,min(Ccap,(ll)T+5)); ll Bw=L-Ccap; if(Bw<minW) return; auto k=make_tuple(Ccap,Bw,(int)cn); if(seen.count(k))return; seen.insert(k); cfgs.push_back({Ccap,Bw,cn}); };
    double fr[]={0.5,0.25,0.75,0.1,0.9,0.05,0.35,0.6};
    for(double f:fr) for(int i:lv) for(int cn=1;cn>=0;cn--) add(llround(f*LIM[i]),LIM[i],cn==1);
    ll abso[]={10,30,100,300,1000};
    for(ll c:abso) for(int i:lv) for(int cn=1;cn>=0;cn--) add(c,LIM[i],cn==1);
    vector<Act> best; ll bestS=-1;
    const ll SOFT=40000000;
    for(size_t ci=0;ci<cfgs.size();ci++){
        if(ci>0&&evals>SOFT) break;
        canAbort=(ci>0);
        bool ok=runCfg(cfgs[ci]);
        if(!ok) continue;
        ll s=validate(acts);
        if(s>bestS){bestS=s;best=acts;}
    }
    if(bestS<0) best.clear();
    string out; out.reserve(best.size()*20+16);
    out+=to_string(best.size()); out+='\n';
    static const char* nmA[]={"AccUp ","AccDown ","AccLeft ","AccRight ","Float ","LoadCarrots ","LoadGift ","DeliverGift "};
    for(auto&a:best){
        out+=nmA[a.type];
        if(a.type==6||a.type==7) out+=NM[a.arg]; else out+=to_string(a.arg);
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
