#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T,D,W,G;
vector<ll> Lw,Aw;
struct Gift{string name; ll s,w,x,y;};
vector<Gift> gifts;

ll accMax(ll w){ for(int i=0;i<W;i++) if(w<=Lw[i]) return Aw[i]; return 0; }

struct Op{int type; ll v;}; // 0 Up 1 Down 2 Left 3 Right 4 Float 5 LoadCarrots 6 LoadGift 7 Deliver
struct State{ll t,px,py,vx,vy,carrots,weight;};

static inline ll Sfun(ll u,ll a){ if(u<=a) return 0; ll m=(u-1)/a; return m*u - a*m*(m+1)/2; }

void closestWithin(ll px,ll py,ll gx,ll gy,ll &tx,ll &ty){
    ll ux=px-gx, uy=py-gy; ll D2=(ll)D*D;
    if(ux*ux+uy*uy<=D2){tx=px;ty=py;return;}
    double L=sqrt((double)ux*ux+(double)uy*uy);
    ll ox=(ll)trunc(ux*(double)D/L), oy=(ll)trunc(uy*(double)D/L);
    while(ox*ox+oy*oy>D2){ if(llabs(ox)>=llabs(oy)) ox-= (ox>0?1:-1); else oy-=(oy>0?1:-1); }
    tx=gx+ox; ty=gy+oy;
}

struct SimRes{bool ok=false,heavy=false,trunc=false; State st; vector<Op> ops; vector<int> deliv; ll score=0; ll used=0;};

SimRes sim(State st,const vector<int>&ord,bool ret,ll Ctot){
    SimRes r; ll D2=(ll)D*D;
    if(Ctot>st.carrots){ r.ops.push_back({5,Ctot-st.carrots}); st.carrots=Ctot; }
    for(int g:ord){ r.ops.push_back({6,g}); st.weight+=gifts[g].w; }
    if(accMax(st.weight+st.carrots)==0){ r.heavy=true; return r; }
    int n=ord.size(); vector<char> dl(n,0);
    int ti=0; bool hasT=false; ll tx=0,ty=0;
    while(true){
        if(st.t>=T){ r.trunc=true; r.ok=true; break; }
        for(int i=0;i<n;i++) if(!dl[i]){
            const Gift&g=gifts[ord[i]]; ll dx=g.x-st.px, dy=g.y-st.py;
            if(llabs(dx)<=D && llabs(dy)<=D && dx*dx+dy*dy<=D2){
                dl[i]=1; r.ops.push_back({7,ord[i]}); st.weight-=g.w; r.score+=g.s; r.deliv.push_back(ord[i]);
            }
        }
        bool finished=false;
        while(true){
            if(ti<n){
                if(dl[ti]){ti++;hasT=false;continue;}
                if(!hasT){ closestWithin(st.px,st.py,gifts[ord[ti]].x,gifts[ord[ti]].y,tx,ty); hasT=true; }
                if(st.px==tx&&st.py==ty&&st.vx==0&&st.vy==0){ti++;hasT=false;continue;}
                break;
            } else {
                if(!ret){finished=true;break;}
                if(!hasT){ closestWithin(st.px,st.py,0,0,tx,ty); hasT=true; }
                if(st.px==tx&&st.py==ty&&st.vx==0&&st.vy==0) finished=true;
                break;
            }
        }
        if(finished){ r.ok=true; break; }
        // movement
        ll dx=tx-st.px, dy=ty-st.py;
        bool axisX = (dx!=0 || st.vx!=0);
        ll R = axisX?dx:dy, v = axisX?st.vx:st.vy;
        if(R!=0 || v!=0){
            ll a = st.carrots>0 ? accMax(st.weight+st.carrots) : 0;
            ll sgn = R>0?1:(R<0?-1:(v>0?1:-1));
            ll Rp=R*sgn, u=v*sgn;
            if(u<0){ return r; }
            ll lo,hi;
            if(a==0){ lo=hi=u; } else { lo=max(0LL,u-a); hi=u+a; }
            if(lo+(a?Sfun(lo,a):0)>Rp){ return r; } // fail
            if(a==0){
                if(u==0) return r; // stuck
            } else {
                while(lo<hi){ ll m=(lo+hi+1)/2; if(m+Sfun(m,a)<=Rp) lo=m; else hi=m-1; }
            }
            ll nv=lo*sgn;
            if(nv!=v){
                ll delta=nv-v;
                if(axisX) r.ops.push_back({delta>0?3:2,llabs(delta)});
                else r.ops.push_back({delta>0?0:1,llabs(delta)});
                st.carrots--; r.used++;
                if(axisX) st.vx=nv; else st.vy=nv;
            }
        }
        r.ops.push_back({4,1});
        st.px+=st.vx; st.py+=st.vy; st.t++;
    }
    r.st=st; return r;
}

double T1(double d,double a){ return d<0.5?0:2*sqrt(d/a)+1; }
double legEst(double ax,double ay,double bx,double by,double a){
    double dx=fabs(bx-ax), dy=fabs(by-ay); double L=hypot(dx,dy);
    if(L<=D) return 0; double f=(L-D)/L; return T1(dx*f,a)+T1(dy*f,a);
}

int main(){
    auto t0=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();};
    scanf("%d %d %d %d",&T,&D,&W,&G);
    Lw.resize(W);Aw.resize(W);
    for(int i=0;i<W;i++) scanf("%lld %lld",&Lw[i],&Aw[i]);
    gifts.resize(G);
    char buf[64];
    for(int i=0;i<G;i++){ scanf("%s %lld %lld %lld %lld",buf,&gifts[i].s,&gifts[i].w,&gifts[i].x,&gifts[i].y); gifts[i].name=buf; }
    State st{0,0,0,0,0,0,0};
    vector<Op> all; vector<char> done(G,0), pk(G,0);
    int cap=50; ll lmax=Lw[W-1];
    while(st.t<T && el()<8.0){
        vector<int> rem; for(int i=0;i<G;i++) if(!done[i]) rem.push_back(i);
        if(rem.empty()) break;
        vector<int> picks; double cx=st.px,cy=st.py; ll Wg=st.weight; double Cest=st.carrots;
        for(int step=0;step<cap;step++){
            int best=-1; double bv=-1, bleg=0;
            for(int g:rem){ if(pk[g]) continue;
                ll w=Wg+gifts[g].w; if(w>lmax) continue;
                ll a=accMax(w+(ll)Cest+10); if(a==0) continue;
                double leg=legEst(cx,cy,gifts[g].x,gifts[g].y,a);
                double val=gifts[g].s/(leg+1.0);
                if(val>bv){bv=val;best=g;bleg=leg;}
            }
            if(best<0) break;
            pk[best]=1; picks.push_back(best); Wg+=gifts[best].w; Cest+=bleg+1; cx=gifts[best].x; cy=gifts[best].y;
        }
        for(int g:picks) pk[g]=0;
        int n=picks.size(); if(n==0) break;
        vector<double> tR(n+1,1e18),tO(n+1,1e18); vector<ll> sc(n+1,0);
        ll wsum=0,ssum=0;
        for(int k=1;k<=n;k++){
            wsum+=gifts[picks[k-1]].w; ssum+=gifts[picks[k-1]].s; sc[k]=ssum;
            double C=0,tr=0,to=0; bool okk=true;
            for(int it=0;it<3;it++){
                ll a=accMax(wsum+max((ll)C,st.carrots)+st.weight); if(a==0){okk=false;break;}
                double px=st.px,py=st.py,s=0;
                for(int i=0;i<k;i++){ s+=legEst(px,py,gifts[picks[i]].x,gifts[picks[i]].y,a)+1; px=gifts[picks[i]].x; py=gifts[picks[i]].y; }
                to=s; tr=s+legEst(px,py,0,0,a)+1; C=tr;
            }
            if(okk){tR[k]=tr;tO[k]=to;}
        }
        double remT=T-st.t;
        int bk=-1; double br=-1;
        for(int k=1;k<=n;k++) if(tR[k]<=remT){ double r=sc[k]/(tR[k]+1); if(r>br){br=r;bk=k;} }
        int bo=-1;
        for(int k=1;k<=n;k++) if(tO[k]<=remT){ if(bo<0||sc[k]>sc[bo]) bo=k; }
        bool fin=false; int k;
        if(bk<0){ if(bo<0){ if(tO[1]<1e17) bo=1; else break; } fin=true; k=bo; }
        else{
            double expct=sc[bk]+(remT-tR[bk])*br*0.8;
            if(bo>=0 && sc[bo]>expct){fin=true;k=bo;} else k=bk;
        }
        if(!fin){ if(k==n && n==cap) cap=min(cap*2,400); else if(k<cap/4) cap=max(8,cap/2); }
        bool committed=false; SimRes chosen;
        while(k>0 && !committed){
            vector<int> ord(picks.begin(),picks.begin()+k);
            ll have=st.carrots; double est=fin?tO[k]:tR[k]; if(est>1e17) est=100;
            ll C=max(have,(ll)ceil(est)+5);
            bool hasBest=false, shrunk=false; SimRes best;
            for(int it=0;it<25;it++){
                if(C>1000000) C=1000000;
                SimRes r=sim(st,ord,!fin,C);
                if(r.heavy) break;
                if(r.ok){
                    if(!hasBest || r.score>best.score || (r.score==best.score && r.st.t<best.st.t)){best=r;hasBest=true;}
                    if(!shrunk && max(have,r.used)<C){ shrunk=true; C=max(have,r.used); continue; }
                    break;
                } else {
                    if(hasBest) break;
                    if(C>=1000000) break;
                    C=C+max(5LL,C/2);
                }
            }
            if(hasBest){committed=true;chosen=best;}
            else k--;
        }
        if(!committed) break;
        for(auto&o:chosen.ops) all.push_back(o);
        st=chosen.st;
        for(int g:chosen.deliv) done[g]=1;
        if(fin||chosen.trunc) break;
        if(chosen.deliv.empty()) break;
    }
    // compress floats and output
    vector<Op> out;
    for(auto&o:all){
        if(o.type==4 && !out.empty() && out.back().type==4) out.back().v+=o.v;
        else out.push_back(o);
    }
    string s; s.reserve(out.size()*20+20);
    s+=to_string(out.size()); s+='\n';
    static const char* nm[]={"AccUp","AccDown","AccLeft","AccRight","Float","LoadCarrots","LoadGift","DeliverGift"};
    for(auto&o:out){
        s+=nm[o.type]; s+=' ';
        if(o.type==6||o.type==7) s+=gifts[o.v].name; else s+=to_string(o.v);
        s+='\n';
    }
    fwrite(s.data(),1,s.size(),stdout);
    return 0;
}
