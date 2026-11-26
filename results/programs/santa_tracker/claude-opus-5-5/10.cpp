#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T, D, W, G;
vector<ll> Lb; vector<int> Ab;
vector<string> nm; vector<ll> sc, wt, gx, gy;

int accelFor(ll w){ for(int i=0;i<W;i++) if(w<=Lb[i]) return Ab[i]; return 0; }
double t1(ll d,double a){ if(d<=0) return 0; return 2.0*sqrt((double)d/a)+2.0; }
ll isq(ll x){ if(x<=0) return 0; ll r=(ll)sqrtl((long double)x); while(r*r>x) r--; while((r+1)*(r+1)<=x) r++; return r; }
bool within(ll ax,ll ay,ll bx,ll by){ __int128 dx=ax-bx, dy=ay-by; return dx*dx+dy*dy <= (__int128)D*D; }

struct St{ ll px=0,py=0,vx=0,vy=0; ll carrots=0; ll giftW=0; int time=0; bool lastAcc=false; };
St gst;
vector<string> outActs;

struct Sim{
    St s; bool fail=false; ll minCar=0; bool emit=false; vector<string> acts; ll pend=0;
    const vector<int>* route=nullptr; vector<char> del;
    void flush(){ if(pend){ if(emit) acts.push_back("Float "+to_string(pend)); pend=0; } }
    void act(const string& x){ flush(); if(emit) acts.push_back(x); }
    void floatSec(){ s.px+=s.vx; s.py+=s.vy; s.time++; if(s.time>T) fail=true; pend++; s.lastAcc=false; }
    ll weight(){ return s.giftW+max(s.carrots,0LL); }
    void acc(int axis, ll amt){
        if(s.lastAcc){ floatSec(); if(fail) return; }
        s.carrots--; minCar=min(minCar,s.carrots);
        string n;
        if(axis==0) n = amt>0?"AccRight":"AccLeft"; else n = amt>0?"AccUp":"AccDown";
        if(emit) act(n+" "+to_string(llabs(amt))); else flush();
        if(axis==0) s.vx+=amt; else s.vy+=amt;
        s.lastAcc=true;
    }
    void moveAxis(int axis, ll delta){
        if(delta==0||fail) return;
        ll a=accelFor(weight());
        if(a==0){ fail=true; return; }
        int dir=delta>0?1:-1; ll r=llabs(delta); ll u=0;
        auto f=[&](ll x)->ll{ if(x<=0) return 0; ll m=(x+a-1)/a-1; return x + m*x - a*m*(m+1)/2; };
        while(true){
            ll lo=max(0LL,u-a), hi=u+a;
            while(lo<hi){ ll mid=(lo+hi+1)/2; if(f(mid)<=r) lo=mid; else hi=mid-1; }
            ll x=lo;
            if(x!=u){ acc(axis,(ll)dir*(x-u)); if(fail) return; }
            if(r==0){ break; }
            r-=x; u=x; floatSec(); if(fail) return;
        }
    }
    void deliverNear(){
        const vector<int>& R=*route;
        for(size_t i=0;i<R.size();i++){
            if(del[i]) continue; int g=R[i];
            if(within(s.px,s.py,gx[g],gy[g])){
                act("DeliverGift "+nm[g]); s.giftW-=wt[g]; del[i]=1;
            }
        }
    }
    void goTo(ll tx0,ll ty0){
        if(fail) return;
        if(within(s.px,s.py,tx0,ty0)) return;
        int a=accelFor(weight());
        if(a==0){ fail=true; return; }
        ll ex=llabs(tx0-s.px), ey=llabs(ty0-s.py);
        ll bestRx=0,bestRy=0; double bc=1e18;
        ll mx=min(ex,(ll)D);
        for(ll rx=0;rx<=mx;rx++){
            ll ry=min(ey,isq((ll)D*D-rx*rx));
            double c=t1(ex-rx,a)+t1(ey-ry,a);
            if(c<bc){bc=c;bestRx=rx;bestRy=ry;}
        }
        ll tx = tx0 - (tx0>s.px?1:(tx0<s.px?-1:0))*bestRx;
        ll ty = ty0 - (ty0>s.py?1:(ty0<s.py?-1:0))*bestRy;
        moveAxis(0,tx-s.px); if(fail) return;
        deliverNear();
        moveAxis(1,ty-s.py); if(fail) return;
    }
};

bool runTrip(const vector<int>& route, ll C, bool doRet, bool emit, Sim& sim){
    sim.s=gst; sim.emit=emit; sim.route=&route; sim.del.assign(route.size(),0);
    if(C>sim.s.carrots){ sim.act("LoadCarrots "+to_string(C-sim.s.carrots)); sim.s.carrots=C; }
    for(int g:route){ sim.act("LoadGift "+nm[g]); sim.s.giftW+=wt[g]; }
    sim.minCar=0;
    for(size_t i=0;i<route.size();i++){
        if(sim.del[i]) continue;
        sim.goTo(gx[route[i]],gy[route[i]]);
        if(sim.fail) return false;
        sim.deliverNear();
        if(!sim.del[i]){ sim.fail=true; return false; }
    }
    if(doRet){
        sim.goTo(0,0);
        if(sim.fail) return false;
        if(!within(sim.s.px,sim.s.py,0,0)){ sim.fail=true; return false; }
    }
    sim.flush();
    return !sim.fail;
}

bool tryTrip(const vector<int>& route, bool doRet, ll& C){
    C=gst.carrots;
    for(int it=0;it<40;it++){
        Sim sim;
        if(!runTrip(route,C,doRet,false,sim)) return false;
        if(sim.minCar<0){ C+=-sim.minCar; if(C>1000000) return false; continue; }
        return true;
    }
    return false;
}

void commit(const vector<int>& route, ll C, bool doRet){
    Sim sim; runTrip(route,C,doRet,true,sim);
    for(auto&x:sim.acts) outActs.push_back(x);
    gst=sim.s;
}

vector<int> alive; vector<char> inR;
vector<vector<double>> retE;
double sq2;
ll work=0; const ll WORKCAP=250000000LL; int lastBest=0;

double legEst(ll ax,ll ay,ll bx,ll by,double a){
    ll s=(ll)(D/sq2);
    return t1(llabs(bx-ax)-s,a)+t1(llabs(by-ay)-s,a);
}

struct Built{ vector<int> route; double est; ll score; int b; };
Built build(int b,bool doRet){
    double Trem=T-gst.time; double a=Ab[b]; ll cap=Lb[b];
    ll cx=gst.px, cy=gst.py; double est=0, retCur=0; ll Wt=0, score=0;
    double c0=gst.carrots;
    vector<int> route;
    while(true){
        int best=-1; double bestR=-1,bd1=0,bd2=0;
        for(int g:alive){
            if(inR[g]) continue; if(Wt+wt[g]>cap) continue;
            work++;
            double d1=legEst(cx,cy,gx[g],gy[g],a); double d2=doRet?retE[b][g]:0;
            double tot=est+d1+d2; if(tot>Trem) continue;
            if(Wt+wt[g]+max(c0,0.8*tot)>cap) continue;
            double delta=d1+d2-retCur; double r=sc[g]/max(delta,0.5);
            if(r>bestR){bestR=r;best=g;bd1=d1;bd2=d2;}
        }
        if(best<0) break;
        inR[best]=1; route.push_back(best); est+=bd1; retCur=bd2; Wt+=wt[best]; score+=sc[best];
        cx=gx[best]; cy=gy[best];
    }
    for(int g:route) inR[g]=0;
    return {route,est+retCur,score,b};
}

bool attempt(const vector<int>& route,bool doRet){
    if(route.empty()) return false;
    ll C;
    if(tryTrip(route,doRet,C)){ commit(route,C,doRet); return true; }
    int lo=0, hi=(int)route.size()-1; ll bestC=0; int bestK=0;
    while(lo<hi){
        int mid=(lo+hi+1)/2;
        vector<int> pr(route.begin(),route.begin()+mid);
        ll c2; if(tryTrip(pr,doRet,c2)){ lo=mid; bestK=mid; bestC=c2; } else hi=mid-1;
    }
    if(bestK>=1){
        vector<int> pr(route.begin(),route.begin()+bestK);
        commit(pr,bestC,doRet); return true;
    }
    return false;
}

int main(){
    sq2=sqrt(2.0);
    if(!(cin>>T>>D>>W>>G)) return 0;
    Lb.resize(W); Ab.resize(W);
    for(int i=0;i<W;i++) cin>>Lb[i]>>Ab[i];
    nm.resize(G); sc.resize(G); wt.resize(G); gx.resize(G); gy.resize(G);
    for(int i=0;i<G;i++) cin>>nm[i]>>sc[i]>>wt[i]>>gx[i]>>gy[i];
    inR.assign(G,0);
    retE.assign(W,vector<double>(G,0));
    for(int b=0;b<W;b++) for(int g=0;g<G;g++) retE[b][g]=legEst(0,0,gx[g],gy[g],Ab[b]);
    for(int g=0;g<G;g++){
        if(wt[g]>Lb[W-1]) continue;
        if(retE[0][g]>T*1.5+5) continue;
        alive.push_back(g);
    }
    vector<char> delivered(G,0);
    while(!alive.empty() && gst.time<T){
        bool done=false;
        {
            vector<Built> bs;
            if(work<WORKCAP){ for(int b=0;b<W;b++) bs.push_back(build(b,true)); }
            else bs.push_back(build(lastBest,true));
            sort(bs.begin(),bs.end(),[](const Built&x,const Built&y){
                double rx=x.score/max(x.est,1.0), ry=y.score/max(y.est,1.0);
                if(rx!=ry) return rx>ry; return x.b<y.b; });
            for(auto& bt:bs){
                if(bt.route.empty()) continue;
                size_t before=outActs.size(); (void)before;
                St old=gst;
                if(attempt(bt.route,true)){
                    lastBest=bt.b; done=true;
                    break;
                }
                gst=old;
            }
        }
        if(done){
            // update delivered: all gifts loaded in committed trip delivered; detect by scanning outActs tail is complex,
            // so recompute: gifts in alive whose LoadGift appears... simpler: track via set of names delivered.
        }
        if(!done){
            vector<Built> bs;
            if(work<WORKCAP){ for(int b=0;b<W;b++) bs.push_back(build(b,false)); }
            else bs.push_back(build(lastBest,false));
            sort(bs.begin(),bs.end(),[](const Built&x,const Built&y){
                if(x.score!=y.score) return x.score>y.score; return x.b<y.b; });
            for(auto& bt:bs){
                if(bt.route.empty()) continue;
                if(attempt(bt.route,false)) break;
            }
            break;
        }
        // mark delivered: parse new DeliverGift actions
        static size_t scanned=0;
        static unordered_map<string,int> idx;
        if(idx.empty()) for(int i=0;i<G;i++) idx[nm[i]]=i;
        for(;scanned<outActs.size();scanned++){
            const string& s=outActs[scanned];
            if(s.compare(0,12,"DeliverGift ")==0){ auto it=idx.find(s.substr(12)); if(it!=idx.end()) delivered[it->second]=1; }
        }
        vector<int> na; for(int g:alive) if(!delivered[g]) na.push_back(g);
        alive.swap(na);
    }
    string out; out.reserve(outActs.size()*16+16);
    out+=to_string(outActs.size()); out+='\n';
    for(auto&s:outActs){ out+=s; out+='\n'; }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
