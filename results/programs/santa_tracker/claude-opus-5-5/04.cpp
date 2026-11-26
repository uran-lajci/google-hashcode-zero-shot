#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T,D,W,G;
vector<ll> L; vector<int> A;
vector<string> nm; vector<ll> S, Wt, X, Y;
vector<vector<int>> nb;
vector<vector<int>> RT;
ll evals=0;
const ll HARD=90000000LL;

int maxAcc(ll w){ for(int i=0;i<W;i++) if(w<=L[i]) return A[i]; return 0;}
ll Sn(ll n,ll a){ ll m=n/2; if(n%2==0) return a*m*(m+1); return a*(m+1)*(m+1);}
ll nmin(ll d,ll a){
    if(d<=0) return 0;
    ll n=(ll)(2*sqrt((double)d/a))-3; if(n<1)n=1;
    while(Sn(n,a)<d) n++;
    while(n>1 && Sn(n-1,a)>=d) n--;
    return n;
}
ll isq(ll v){ if(v<=0) return 0; ll r=(ll)sqrtl((long double)v); while(r*r>v) r--; while((r+1)*(r+1)<=v) r++; return r;}
struct Pl{ ll tx,ty; ll time; ll car; bool ok;};
Pl plan(ll px,ll py,ll gx,ll gy,int a){
    ll dx=gx-px, dy=gy-py; ll adx=llabs(dx), ady=llabs(dy);
    Pl best; best.ok=false; best.time=LLONG_MAX; best.car=0; best.tx=px; best.ty=py;
    int sx=dx>0?1:(dx<0?-1:0), sy=dy>0?1:(dy<0?-1:0);
    auto tryOpt=[&](ll rx,ll ry){
        if(a==0 && (rx||ry)) return;
        ll nx=rx?nmin(rx,a):0, ny=ry?nmin(ry,a):0;
        ll tm=(rx?nx+1:0)+(ry?ny+1:0);
        if(tm<best.time){best.time=tm;best.car=tm;best.tx=px+sx*rx;best.ty=py+sy*ry;best.ok=true;}
    };
    ll DD=(ll)D*D;
    ll k=isq(DD/2);
    tryOpt(max(0LL,adx-k),max(0LL,ady-k));
    if(adx<=D) tryOpt(0,max(0LL,ady-isq(DD-adx*adx)));
    if(ady<=D) tryOpt(max(0LL,adx-isq(DD-ady*ady)),0);
    return best;
}

struct Trip{ vector<int> r; ll score=0; ll time=0; ll car=0; };

struct Sim{
    ll t=0,px=0,py=0,vx=0,vy=0,carrots=0,weight=0;
    vector<char> st; vector<int> loaded, lpos, mark; int stamp=0;
    bool lastAcc=false,bad=false,stranded=false;
    vector<pair<char,int>> acts; ll score=0;
    Sim(){ st.assign(G,0); lpos.assign(G,-1); mark.assign(G,0);}
    void deliver(int g){
        acts.push_back({7,g}); st[g]=2; weight-=Wt[g]; score+=S[g];
        int p=lpos[g]; int last=loaded.back(); loaded[p]=last; lpos[last]=p; loaded.pop_back(); lpos[g]=-1;
    }
    bool inRange(int g){ ll dx=X[g]-px, dy=Y[g]-py; return dx*dx+dy*dy<=(ll)D*D; }
    void deliverNear(){
        if(loaded.size()>200) return;
        vector<int> cp=loaded;
        for(int g:cp) if(st[g]==1 && inRange(g)) deliver(g);
    }
    void floatSec(){
        if(t>=T){bad=true;return;}
        t++; px+=vx; py+=vy; lastAcc=false; acts.push_back({4,1}); deliverNear();
    }
    void accel(int axis,ll diff){
        if(diff==0||bad) return;
        if(lastAcc) floatSec();
        if(bad) return;
        if(carrots<=0 || llabs(diff)>maxAcc(weight)){bad=true;return;}
        char tp; if(axis==0) tp=diff>0?3:2; else tp=diff>0?0:1;
        acts.push_back({tp,(int)llabs(diff)});
        carrots--; weight--; if(axis==0) vx+=diff; else vy+=diff; lastAcc=true;
    }
    void move1D(int axis,ll d,int a){
        if(d==0||bad) return;
        ll ad=llabs(d); int sg=d>0?1:-1;
        ll n=nmin(ad,a);
        vector<ll> b(n+1);
        ll mb=0;
        for(ll i=1;i<=n;i++){ b[i]=min((ll)a*i,(ll)a*(n+1-i)); mb=max(mb,b[i]);}
        auto f=[&](ll c){ ll s=0; for(ll i=1;i<=n;i++) s+=min(b[i],c); return s;};
        ll lo=0,hi=mb;
        while(lo<hi){ ll m=(lo+hi+1)/2; if(f(m)<=ad) lo=m; else hi=m-1;}
        ll c=lo; ll r=ad-f(c);
        vector<ll> v(n+1);
        for(ll i=1;i<=n;i++){ v[i]=min(b[i],c); if(b[i]>c && r>0){v[i]=c+1;r--;} }
        ll prev=0;
        for(ll i=1;i<=n && !bad;i++){ accel(axis,sg*(v[i]-prev)); if(bad)return; floatSec(); prev=v[i]; }
        if(!bad) accel(axis,-sg*prev);
    }
    void execMove(const Pl&p,int a){
        move1D(0,p.tx-px,a);
        move1D(1,p.ty-py,a);
    }
    Trip build(int lv,bool ret,int pw){
        Trip tr; int Ac=A[lv]; ll Lc=L[lv];
        if(weight>Lc) return tr;
        stamp++;
        ll cx=px,cy=py; int cg=-1; ll tau=0,legCar=0,addW=0,lastRt=0;
        while(true){
            if(evals>HARD) break;
            int best=-1; double bv=-1; Pl bp;
            auto ev=[&](int g){
                if(st[g]==2||mark[g]==stamp) return;
                evals++;
                ll rt=ret?RT[lv][g]:0;
                if(rt>=INT_MAX/2) return;
                Pl p=plan(cx,cy,X[g],Y[g],Ac); if(!p.ok) return;
                if(t+tau+p.time+rt>T) return;
                ll tc=legCar+p.car+rt;
                ll ex=max(0LL,tc-carrots);
                ll wn=weight+addW+(st[g]==0?Wt[g]:0)+ex;
                if(wn>Lc) return;
                double den=p.time+1.0; if(pw==2) den*=den;
                double v=S[g]/den;
                if(v>bv){bv=v;best=g;bp=p;}
            };
            if(cg>=0) for(int g:nb[cg]) ev(g);
            if(best<0) for(int g=0;g<G;g++) ev(g);
            if(best<0) break;
            mark[best]=stamp; tr.r.push_back(best); tr.score+=S[best];
            tau+=bp.time; legCar+=bp.car; if(st[best]==0) addW+=Wt[best];
            cx=bp.tx; cy=bp.ty; cg=best; lastRt=ret?RT[lv][best]:0;
        }
        tr.time=tau+lastRt; tr.car=legCar+lastRt;
        return tr;
    }
    int execTrip(const Trip&tr,bool ret){
        ll extra=max(0LL,tr.car-carrots);
        if(extra>0){ acts.push_back({5,(int)extra}); carrots+=extra; weight+=extra; }
        for(int g:tr.r) if(st[g]==0){ acts.push_back({6,g}); st[g]=1; weight+=Wt[g]; lpos[g]=loaded.size(); loaded.push_back(g);}
        int del=0;
        for(int g:tr.r){
            if(bad) return del;
            if(st[g]!=1) continue;
            int a=maxAcc(weight);
            Pl p1=plan(px,py,X[g],Y[g],a); if(!p1.ok) continue;
            if(ret){
                int a2=maxAcc(weight-Wt[g]);
                Pl p2=plan(p1.tx,p1.ty,0,0,a2); if(!p2.ok) continue;
                if(t+p1.time+p2.time>T || carrots<p1.car+p2.car) continue;
            } else {
                if(t+p1.time>T || carrots<p1.car) continue;
            }
            ll sc0=score;
            execMove(p1,a);
            if(bad) return del;
            if(st[g]==1){ if(inRange(g)) deliver(g); else {bad=true;return del;} }
            deliverNear();
            if(score>sc0) del++;
        }
        if(ret && !bad){
            int a=maxAcc(weight);
            Pl p=plan(px,py,0,0,a);
            if(!p.ok || t+p.time>T || carrots<p.car){ stranded=true; return del;}
            execMove(p,a);
            if(px*px+py*py>(ll)D*D) bad=true;
        }
        return del;
    }
    Trip bestTrip(int mode,bool ret,int pw){
        if(mode>=0) return build(mode,ret,pw);
        Trip best; double br=-1;
        for(int lv=0;lv<W;lv++){
            Trip tr=build(lv,ret,pw);
            if(tr.r.empty()) continue;
            double r= ret? (double)tr.score/max(1LL,tr.time) : (double)tr.score;
            if(r>br){br=r;best=tr;}
        }
        return best;
    }
    void run(int mode,double F,int pw){
        while(!bad && !stranded && t<T){
            Trip R=bestTrip(mode,true,pw);
            bool fin=false; Trip O; bool ob=false;
            if(R.r.empty()) fin=true;
            else if(T-t<=3*R.time){
                O=bestTrip(mode,false,pw); ob=true;
                if((double)O.score>=F*R.score*(double)(T-t)/max(1LL,R.time)) fin=true;
            }
            if(fin){
                if(!ob) O=bestTrip(mode,false,pw);
                if(!O.r.empty()) execTrip(O,false);
                break;
            }
            int d=execTrip(R,true);
            if(d==0 && !bad && !stranded){
                O=bestTrip(mode,false,pw);
                if(!O.r.empty()) execTrip(O,false);
                break;
            }
        }
    }
};

int main(){
    scanf("%d %d %d %d",&T,&D,&W,&G);
    L.resize(W);A.resize(W);
    for(int i=0;i<W;i++){ scanf("%lld %d",&L[i],&A[i]); }
    nm.resize(G);S.resize(G);Wt.resize(G);X.resize(G);Y.resize(G);
    char buf[64];
    for(int i=0;i<G;i++){ scanf("%s %lld %lld %lld %lld",buf,&S[i],&Wt[i],&X[i],&Y[i]); nm[i]=buf; }
    int K=min(40,G-1);
    nb.assign(G,{});
    if(K>0){
        vector<pair<double,int>> tmp(G);
        for(int i=0;i<G;i++){
            int c=0;
            for(int j=0;j<G;j++){ if(j==i) continue; double dx=(double)(X[i]-X[j]),dy=(double)(Y[i]-Y[j]); tmp[c++]={dx*dx+dy*dy,j}; }
            nth_element(tmp.begin(),tmp.begin()+K-1,tmp.begin()+c);
            sort(tmp.begin(),tmp.begin()+K);
            for(int k=0;k<K;k++) nb[i].push_back(tmp[k].second);
        }
    }
    RT.assign(W,vector<int>(G));
    for(int lv=0;lv<W;lv++) for(int g=0;g<G;g++){
        Pl p=plan(X[g],Y[g],0,0,A[lv]);
        RT[lv][g]= p.ok ? (int)min<ll>(p.time,INT_MAX/2) : INT_MAX/2;
    }
    struct Cfg{int mode; double F; int pw;};
    vector<Cfg> cfgs={{-1,1.0,1},{-1,0.7,1},{-1,1.0,2}};
    for(int lv=0;lv<W;lv++) cfgs.push_back({lv,1.0,1});
    cfgs.push_back({-1,1.5,1});
    ll bestScore=-1; vector<pair<char,int>> bestActs;
    const ll BUDGET=40000000LL;
    for(size_t ci=0;ci<cfgs.size();ci++){
        if(ci>0 && evals>BUDGET) break;
        Sim s; s.run(cfgs[ci].mode,cfgs[ci].F,cfgs[ci].pw);
        if(s.bad) continue;
        if(s.score>bestScore){bestScore=s.score;bestActs=s.acts;}
    }
    // merge floats
    vector<pair<char,int>> out;
    for(auto&a:bestActs){
        if(a.first==4 && !out.empty() && out.back().first==4) out.back().second+=a.second;
        else out.push_back(a);
    }
    string res; res.reserve(out.size()*20+20);
    res+=to_string(out.size()); res+='\n';
    static const char* names[]={"AccUp ","AccDown ","AccLeft ","AccRight ","Float ","LoadCarrots ","LoadGift ","DeliverGift "};
    for(auto&a:out){
        res+=names[(int)a.first];
        if(a.first==6||a.first==7) res+=nm[a.second];
        else res+=to_string(a.second);
        res+='\n';
    }
    fwrite(res.data(),1,res.size(),stdout);
    return 0;
}
