#include <bits/stdc++.h>
using namespace std;
typedef long long ll; typedef __int128 LL;

int T; ll D; int W, G;
vector<ll> Lw, Aw;
vector<string> nm; vector<ll> gs, gw, gc, gr;
ll work=0;
vector<char> doneG, deadG;

inline ll maxAcc(ll w){ for(int i=0;i<W;i++) if(w<=Lw[i]) return Aw[i]; return 0; }
inline ll fdiv(ll n, ll d){ ll q=n/d; if((n%d!=0)&&((n<0)!=(d<0))) q--; return q; }
inline ll rdiv(ll r, ll w){ return fdiv(2*r+w, 2*w); }

bool tryK(ll K, ll px,ll py,ll vx,ll vy,ll tx,ll ty,ll giftW,ll C,ll used,
          ll&usedOut,ll&nvx,ll&nvy,ll&npx,ll&npy, vector<pair<int,ll>>*rec){
    work+=K;
    ll rx=tx-px-K*vx, ry=ty-py-K*vy;
    for(ll i=0;i<K;i++){
        ll wt=K-i; ll rem=C-used; ll w=giftW+(rem>0?rem:0); ll a=maxAcc(w);
        int dir=-1; ll cc=0;
        if(a>0){
            ll cx=rdiv(rx,wt); if(cx>a)cx=a; if(cx<-a)cx=-a;
            ll cy=rdiv(ry,wt); if(cy>a)cy=a; if(cy<-a)cy=-a;
            LL gx=(LL)cx*wt*(2*(LL)rx-(LL)cx*wt);
            LL gy=(LL)cy*wt*(2*(LL)ry-(LL)cy*wt);
            if(gx>=gy && gx>0){dir=0;cc=cx;} else if(gy>0){dir=1;cc=cy;}
        }
        if(dir==0){rx-=cc*wt; vx+=cc; used++;}
        else if(dir==1){ry-=cc*wt; vy+=cc; used++;}
        if(rec) rec->push_back({dir,cc});
    }
    if((LL)rx*rx+(LL)ry*ry <= (LL)D*D){usedOut=used;nvx=vx;nvy=vy;npx=tx-rx;npy=ty-ry;return true;}
    return false;
}

ll planLeg(ll px,ll py,ll vx,ll vy,ll tx,ll ty,ll giftW,ll C,ll used,ll Trem,
           ll&uo,ll&nvx,ll&nvy,ll&npx,ll&npy){
    ll dx=tx-px, dy=ty-py;
    if((LL)dx*dx+(LL)dy*dy <= (LL)D*D){uo=used;nvx=vx;nvy=vy;npx=px;npy=py;return 0;}
    if(Trem<=0) return -1;
    ll K=1, lo=0, hi=-1;
    while(true){
        ll k=min(K,Trem);
        if(tryK(k,px,py,vx,vy,tx,ty,giftW,C,used,uo,nvx,nvy,npx,npy,nullptr)){hi=k;break;}
        lo=k; if(k==Trem) return -1; K*=2;
    }
    while(hi-lo>1){
        ll mid=(lo+hi)/2;
        if(tryK(mid,px,py,vx,vy,tx,ty,giftW,C,used,uo,nvx,nvy,npx,npy,nullptr)) hi=mid; else lo=mid;
    }
    tryK(hi,px,py,vx,vy,tx,ty,giftW,C,used,uo,nvx,nvy,npx,npy,nullptr);
    return hi;
}

struct St{ll t,px,py,vx,vy,carr;};
struct Em{ vector<string> L; ll pf=0;
    void flush(){ if(pf){L.push_back("Float "+to_string(pf)); pf=0;} }
    void line(const string&s){flush(); L.push_back(s);} void fl(ll n){pf+=n;} };
struct SimRes{bool valid; int k; ll used; ll score; bool ended; St end;};

SimRes sim(const vector<int>& route, ll C, St s, Em* em){
    SimRes R; ll giftW=0; for(int g:route) giftW+=gw[g];
    ll used=0; R.k=0; R.score=0; R.ended=false;
    vector<pair<int,ll>> rec;
    auto doLeg=[&](ll tx,ll ty)->bool{
        ll Trem=T-s.t; ll uo,nvx,nvy,npx,npy;
        ll K=planLeg(s.px,s.py,s.vx,s.vy,tx,ty,giftW,C,used,Trem,uo,nvx,nvy,npx,npy);
        if(K<0) return false;
        if(em && K>0){
            rec.clear(); ll a1,a2,a3,a4,a5;
            tryK(K,s.px,s.py,s.vx,s.vy,tx,ty,giftW,C,used,a1,a2,a3,a4,a5,&rec);
            for(auto&p:rec){
                if(p.first>=0){
                    string dn;
                    if(p.first==0) dn = p.second>0?"AccRight":"AccLeft";
                    else dn = p.second>0?"AccUp":"AccDown";
                    em->line(dn+" "+to_string(llabs(p.second)));
                }
                em->fl(1);
            }
        }
        s.px=npx; s.py=npy; s.vx=nvx; s.vy=nvy; s.t+=K; used=uo; return true;
    };
    for(int g:route){
        if(!doLeg(gc[g],gr[g])){R.ended=true;break;}
        if(em) em->line("DeliverGift "+nm[g]);
        giftW-=gw[g]; R.score+=gs[g]; R.k++;
    }
    if(!R.ended){ if(!doLeg(0,0)) R.ended=true; }
    R.used=used; R.valid=used<=C; s.carr=C-used; R.end=s; return R;
}

struct Eval{bool ok=false; vector<int> route; ll C=0; SimRes r; double rate=0;};

Eval evalRoute(vector<int> route, const St& s, double estT){
    Eval best;
    ll C=max(s.carr,(ll)estT+5);
    for(int it=0;it<10 && !route.empty();it++){
        SimRes r=sim(route,C,s,nullptr);
        if(!r.valid){
            if(best.ok) break;
            C=max(C+1, r.used+r.used/10+5); continue;
        }
        if(r.k<(int)route.size()){
            if(best.ok) break;
            if(r.k==0) return best;
            route.resize(r.k); continue;
        }
        double tm = r.ended ? (double)(T-s.t) : (double)(r.end.t-s.t);
        best.ok=true; best.route=route; best.C=C; best.r=r; best.rate=(double)r.score/max(tm,0.5);
        ll C2=max(s.carr,r.used); if(C2>=C) break; C=C2;
    }
    return best;
}

const double INF=1e18;
double legEst(double d,double a){ if(d<=D) return 0; return 2*sqrt((d-D)/a)+1; }
double dist(ll ax,ll ay,ll bx,ll by){ double dx=(double)(ax-bx), dy=(double)(ay-by); return sqrt(dx*dx+dy*dy); }

double estRoute(const vector<int>& r, ll carr, double& aOut, double& deliv){
    ll sumW=0; for(int g:r) sumW+=gw[g];
    double ce=0, tt=0;
    for(int it=0;it<3;it++){
        ll a=maxAcc(sumW+max((ll)ce,carr)+1);
        if(a==0) return INF;
        aOut=a; tt=0; ll px=0,py=0;
        for(int g:r){ tt+=legEst(dist(px,py,gc[g],gr[g]),a); px=gc[g]; py=gr[g]; }
        deliv=tt; tt+=legEst(dist(px,py,0,0),a); ce=tt;
    }
    return tt;
}

vector<int> buildRoute(int seed, const St& s, int NC, double& estOut, const vector<int>& alive){
    vector<pair<ll,int>> cd;
    for(int g:alive){ if(g==seed) continue; ll dx=gc[g]-gc[seed], dy=gr[g]-gr[seed]; cd.push_back({dx*dx+dy*dy,g}); }
    work+=alive.size();
    if((int)cd.size()>NC){ nth_element(cd.begin(),cd.begin()+NC,cd.end()); cd.resize(NC); }
    sort(cd.begin(),cd.end());
    vector<int> route{seed};
    double a,dl; double tt=estRoute(route,s.carr,a,dl);
    estOut=tt;
    if(tt>=INF) return route;
    double S=gs[seed]; ll Trem=T-s.t;
    vector<char> usedC(cd.size(),0);
    while(route.size()<60){
        double bestRate=S/max(tt,1.0); int bc=-1,bp=-1;
        for(size_t j=0;j<cd.size();j++){
            if(usedC[j]) continue; int g=cd[j].second;
            double bd=INF; int bpos=-1;
            ll px=0,py=0;
            for(size_t p=0;p<=route.size();p++){
                ll nx = p<route.size()?gc[route[p]]:0, ny = p<route.size()?gr[route[p]]:0;
                double dl2=legEst(dist(px,py,gc[g],gr[g]),a)+legEst(dist(gc[g],gr[g],nx,ny),a)-legEst(dist(px,py,nx,ny),a);
                if(dl2<bd){bd=dl2;bpos=p;}
                px=nx;py=ny;
            }
            work+=route.size()+1;
            double nr=(S+gs[g])/max(tt+bd,1.0);
            if(nr>bestRate){bestRate=nr;bc=j;bp=bpos;}
        }
        if(bc<0) break;
        int g=cd[bc].second; usedC[bc]=1;
        route.insert(route.begin()+bp,g);
        double a2,dl2; double t2=estRoute(route,s.carr,a2,dl2);
        if(t2>=INF || dl2>Trem || (S+gs[g])/max(t2,1.0) <= S/max(tt,1.0)){ route.erase(route.begin()+bp); continue; }
        S+=gs[g]; tt=t2; a=a2;
    }
    estOut=tt;
    return route;
}

int main(){
    scanf("%d %lld %d %d",&T,&D,&W,&G);
    Lw.resize(W); Aw.resize(W);
    for(int i=0;i<W;i++) scanf("%lld %lld",&Lw[i],&Aw[i]);
    nm.resize(G); gs.resize(G); gw.resize(G); gc.resize(G); gr.resize(G);
    char buf[128];
    for(int i=0;i<G;i++){ scanf("%s %lld %lld %lld %lld",buf,&gs[i],&gw[i],&gc[i],&gr[i]); nm[i]=buf; }
    doneG.assign(G,0); deadG.assign(G,0);
    for(int i=0;i<G;i++){
        ll dx=gc[i],dy=gr[i];
        bool nearO = (LL)dx*dx+(LL)dy*dy <= (LL)D*D;
        if(!nearO && maxAcc(gw[i]+1)==0) deadG[i]=1;
    }
    St s{0,0,0,0,0,0};
    Em em;
    const ll HARD=550000000LL;
    while(s.t<=T){
        if(work>HARD) break;
        int NS = work<200000000LL?8:(work<350000000LL?3:1);
        int NC = work<200000000LL?30:12;
        vector<int> alive;
        for(int g=0;g<G;g++) if(!doneG[g]&&!deadG[g]) alive.push_back(g);
        if(alive.empty()) break;
        vector<pair<double,int>> cand;
        for(int g:alive){
            double d=dist(0,0,gc[g],gr[g]);
            double ce=0, tt=0; bool bad=false;
            for(int it=0;it<3;it++){
                ll a=maxAcc(gw[g]+(ll)ce+1); if(a==0){bad=true;break;}
                tt=2*legEst(d,a); ce=tt;
            }
            if(bad){ continue; }
            cand.push_back({-(double)gs[g]/(tt+1),g});
        }
        work+=alive.size();
        if(cand.empty()) break;
        int ns=min((int)cand.size(),NS);
        partial_sort(cand.begin(),cand.begin()+ns,cand.end());
        Eval best;
        for(int i=0;i<ns;i++){
            int seed=cand[i].second;
            double est;
            vector<int> route=buildRoute(seed,s,NC,est,alive);
            if(est>=INF){ deadG[seed]=1; continue; }
            Eval e=evalRoute(route,s,est);
            if(!e.ok){
                vector<int> r1{seed}; double a,dl; double e1=estRoute(r1,s.carr,a,dl);
                if(e1>=INF){deadG[seed]=1; continue;}
                e=evalRoute(r1,s,e1);
                if(!e.ok){deadG[seed]=1; continue;}
            }
            if(!best.ok || e.rate>best.rate) best=e;
        }
        if(!best.ok) continue;
        if(best.C>s.carr) em.line("LoadCarrots "+to_string(best.C-s.carr));
        for(int g:best.route) em.line("LoadGift "+nm[g]);
        SimRes r=sim(best.route,best.C,s,&em);
        for(int i=0;i<r.k;i++) doneG[best.route[i]]=1;
        s=r.end;
        if(r.ended) break;
        if(s.t>=T) break;
    }
    em.flush();
    string out=to_string(em.L.size())+"\n";
    for(auto&l:em.L){ out+=l; out+='\n'; }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
