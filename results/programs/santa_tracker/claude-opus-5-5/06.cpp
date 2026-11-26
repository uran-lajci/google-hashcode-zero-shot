#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T; ll D; int W, G;
vector<ll> Lc, Ac;
vector<string> nm; vector<ll> sc, wt, gx, gy;
vector<vector<int>> knn;
chrono::steady_clock::time_point t0;
double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-t0).count(); }

ll maxAcc(ll w){ for(int i=0;i<W;i++) if(w<=Lc[i]) return Ac[i]; return 0; }
ll isq(ll v){ if(v<=0) return 0; ll r=(ll)sqrtl((long double)v); while(r>0 && r*r>v) r--; while((r+1)*(r+1)<=v) r++; return r; }

ll time1D(ll d, ll a){
    if(d==0) return 0; if(a<=0) return (ll)4e18/4;
    ll k=isq(d/a); if(k==0) return 1;
    ll R=d-a*k*k; ll e=R/(k*a); R%=k*a; ll j=R/a; R%=a;
    return 2*k-1+e+(j>0)+(R>0);
}
ll acc1D(ll d, ll a){
    if(d==0) return 0; if(a<=0) return (ll)1e12;
    ll k=isq(d/a); if(k==0) return 2;
    ll R=d-a*k*k; R%=k*a; R%=a;
    return 2*k+(R>0);
}
double estLeg(ll x,ll y,ll tx,ll ty,ll a){
    double dx=max(0.0,fabs((double)(tx-x))-0.7*D), dy=max(0.0,fabs((double)(ty-y))-0.7*D);
    ll ix=(ll)dx, iy=(ll)dy;
    return (double)time1D(ix,a)+(double)time1D(iy,a)+((ix>0&&iy>0)?1:0)+1;
}
ll accLeg(ll x,ll y,ll tx,ll ty,ll a){
    double dx=max(0.0,fabs((double)(tx-x))-0.7*D), dy=max(0.0,fabs((double)(ty-y))-0.7*D);
    return acc1D((ll)dx,a)+acc1D((ll)dy,a);
}

struct St{ ll x,y,vx,vy; ll t; ll car; ll gw; bool lastAcc; };
struct Act{ char ty; ll arg; };
vector<Act> acts;
vector<char> usedG;

static ll clampll(ll v, ll lo, ll hi){ return v<lo?lo:(v>hi?hi:v); }

pair<ll,ll> bestStop(ll x,ll y,ll tx,ll ty,ll a){
    vector<pair<ll,ll>> c;
    {ll ox=clampll(x-tx,-D,D); ll r=isq(D*D-ox*ox); ll oy=clampll(y-ty,-r,r); c.push_back({tx+ox,ty+oy});}
    {ll oy=clampll(y-ty,-D,D); ll r=isq(D*D-oy*oy); ll ox=clampll(x-tx,-r,r); c.push_back({tx+ox,ty+oy});}
    {ll s=isq(D*D/2); ll ox=clampll(x-tx,-s,s), oy=clampll(y-ty,-s,s); c.push_back({tx+ox,ty+oy});}
    ll aa=max(a,1LL);
    pair<ll,ll> best=c[0]; ll bt=LLONG_MAX;
    for(auto&p:c){
        ll dx=llabs(p.first-x), dy=llabs(p.second-y);
        ll t=time1D(dx,aa)+time1D(dy,aa)+((dx>0&&dy>0)?1:0);
        if(t<bt){bt=t;best=p;}
    }
    return best;
}

int move1D(St&s,int axis,ll d,vector<Act>*out){
    if(d==0) return 0;
    ll a=maxAcc(s.car+s.gw);
    if(s.car<=0) return 1;
    if(a<=0) return 3;
    ll ad=llabs(d); ll sg=d>0?1:-1;
    ll tt=time1D(ad,a);
    if(s.t+tt+(s.lastAcc?1:0)>T) return 2;
    vector<pair<ll,ll>> seg;
    ll k=isq(ad/a);
    if(k==0) seg.push_back({ad,1});
    else{
        ll R=ad-a*k*k; ll e=R/(k*a); R%=k*a; ll jj=R/a; R%=a;
        for(ll j=1;j<k;j++) seg.push_back({j*a,1});
        seg.push_back({k*a,1+e});
        for(ll j=k-1;j>=1;j--) seg.push_back({j*a,1+(j==jj?1:0)});
        if(R>0) seg.push_back({R,1});
    }
    ll cur=0;
    auto accTo=[&](ll v)->int{
        ll diff=v-cur; if(diff==0) return 0;
        if(s.lastAcc){
            if(s.t+1>T) return 2;
            s.t+=1; s.x+=s.vx; s.y+=s.vy; s.lastAcc=false;
            if(out) out->push_back({4,1});
        }
        if(s.car<=0) return 1;
        ll w=s.car+s.gw;
        if(llabs(diff)>maxAcc(w)) return 3;
        char ty;
        if(axis==0) ty = diff>0?3:2; else ty = diff>0?0:1;
        if(out) out->push_back({ty,llabs(diff)});
        s.car--; cur=v;
        if(axis==0) s.vx=v; else s.vy=v;
        s.lastAcc=true;
        return 0;
    };
    for(auto&p:seg){
        int c=accTo(sg*p.first); if(c) return c;
        if(s.t+p.second>T) return 2;
        s.t+=p.second; s.x+=s.vx*p.second; s.y+=s.vy*p.second; s.lastAcc=false;
        if(out) out->push_back({4,p.second});
    }
    int c=accTo(0); if(c) return c;
    return 0;
}

int moveTo(St&s,ll tx,ll ty,vector<Act>*out){
    int c=move1D(s,0,tx-s.x,out); if(c) return c;
    c=move1D(s,1,ty-s.y,out); if(c) return c;
    return 0;
}

int runTrip(St&s,const vector<int>&ord,ll C,bool ret,vector<Act>*out,vector<int>*dv){
    if(C>s.car){
        if(s.x*s.x+s.y*s.y>D*D) return 4;
        if(out) out->push_back({5,C-s.car});
        s.car=C;
    }
    for(int g:ord){
        if(s.x*s.x+s.y*s.y>D*D) return 4;
        if(out) out->push_back({6,(ll)g});
        s.gw+=wt[g];
    }
    vector<char> dl(ord.size(),0);
    auto deliverNear=[&](){
        for(size_t i=0;i<ord.size();i++) if(!dl[i]){
            int g=ord[i]; ll dx=gx[g]-s.x, dy=gy[g]-s.y;
            if(llabs(dx)<=D && llabs(dy)<=D && dx*dx+dy*dy<=D*D){
                dl[i]=1; s.gw-=wt[g];
                if(out) out->push_back({7,(ll)g});
                if(dv) dv->push_back(g);
            }
        }
    };
    deliverNear();
    for(size_t i=0;i<ord.size();i++){
        if(dl[i]) continue;
        int g=ord[i];
        ll a=maxAcc(s.car+s.gw);
        auto p=bestStop(s.x,s.y,gx[g],gy[g],a);
        int c=moveTo(s,p.first,p.second,out); if(c) return c;
        deliverNear();
    }
    if(ret){
        ll a=maxAcc(s.car+s.gw);
        auto p=bestStop(s.x,s.y,0,0,a);
        int c=moveTo(s,p.first,p.second,out); if(c) return c;
    }
    return 0;
}

struct Plan{ vector<int> ord; double time=0; ll score=0; ll cEst=0; };
vector<int> stampA; int stampC=0;

Plan build(int ti,bool ret,const St&s){
    Plan p; ll a=Ac[ti], cap=Lc[ti]; double rem=T-s.t; ll have=s.car;
    stampC++;
    int bg=-1; double br=-1;
    for(int g=0;g<G;g++){
        if(usedG[g]) continue;
        double t=estLeg(s.x,s.y,gx[g],gy[g],a)+(ret?estLeg(gx[g],gy[g],0,0,a):0);
        if(t>rem) continue;
        ll c=accLeg(s.x,s.y,gx[g],gy[g],a)+(ret?accLeg(gx[g],gy[g],0,0,a):0)+2;
        if(wt[g]+max(have,c)>cap) continue;
        double r=sc[g]/max(t,0.5);
        if(r>br){br=r;bg=g;}
    }
    if(bg<0) return p;
    int cur=bg; stampA[bg]=stampC; p.ord.push_back(bg);
    double tGo=estLeg(s.x,s.y,gx[bg],gy[bg],a); ll cGo=accLeg(s.x,s.y,gx[bg],gy[bg],a);
    ll gw=wt[bg]; ll score=sc[bg];
    while(true){
        double back=ret?estLeg(gx[cur],gy[cur],0,0,a):0;
        double curTot=tGo+back;
        int bh=-1; double bm=-1;
        auto consider=[&](int h){
            if(usedG[h]||stampA[h]==stampC) return;
            double l=estLeg(gx[cur],gy[cur],gx[h],gy[h],a);
            double hb=ret?estLeg(gx[h],gy[h],0,0,a):0;
            if(tGo+l+hb>rem) return;
            ll c=cGo+accLeg(gx[cur],gy[cur],gx[h],gy[h],a)+(ret?accLeg(gx[h],gy[h],0,0,a):0)+2;
            if(gw+wt[h]+max(have,c)>cap) return;
            double m=sc[h]/max(0.5,l+hb-back);
            if(m>bm){bm=m;bh=h;}
        };
        for(int h:knn[cur]) consider(h);
        if(bh<0 && elapsed()<5.0) for(int h=0;h<G;h++) consider(h);
        if(bh<0) break;
        if(bm<0.3*score/max(curTot,1.0)) break;
        tGo+=estLeg(gx[cur],gy[cur],gx[bh],gy[bh],a);
        cGo+=accLeg(gx[cur],gy[cur],gx[bh],gy[bh],a);
        gw+=wt[bh]; score+=sc[bh]; stampA[bh]=stampC; p.ord.push_back(bh); cur=bh;
    }
    p.time=tGo+(ret?estLeg(gx[cur],gy[cur],0,0,a):0);
    p.score=score;
    p.cEst=cGo+(ret?accLeg(gx[cur],gy[cur],0,0,a):0)+2;
    return p;
}

St S;
bool execute(Plan p,bool ret){
    vector<int> ord=p.ord;
    while(!ord.empty()){
        if(elapsed()>9.0) return false;
        ll have=S.car;
        ll C=max(have,p.cEst);
        int code=0; St s2;
        for(int it=0;it<40;it++){
            s2=S; code=runTrip(s2,ord,C,ret,nullptr,nullptr);
            if(code==1){C+=max(3LL,C/5);continue;}
            break;
        }
        if(code==0){
            ll usedC=C-s2.car; ll C2=max(have,usedC);
            if(C2<C){ St s3=S; if(runTrip(s3,ord,C2,ret,nullptr,nullptr)==0) C=C2; }
            vector<int> dv; St s4=S;
            int c=runTrip(s4,ord,C,ret,&acts,&dv);
            if(c!=0) return false; // shouldn't happen
            S=s4;
            for(int g:dv) usedG[g]=1;
            return true;
        }
        ord.pop_back();
    }
    return false;
}

int main(){
    t0=chrono::steady_clock::now();
    ios::sync_with_stdio(false); cin.tie(nullptr);
    cin>>T>>D>>W>>G;
    Lc.resize(W); Ac.resize(W);
    for(int i=0;i<W;i++) cin>>Lc[i]>>Ac[i];
    nm.resize(G); sc.resize(G); wt.resize(G); gx.resize(G); gy.resize(G);
    for(int i=0;i<G;i++) cin>>nm[i]>>sc[i]>>wt[i]>>gx[i]>>gy[i];
    usedG.assign(G,0); stampA.assign(G,0);
    // kNN
    int K=min(32,G-1);
    knn.assign(G,{});
    if(K>0){
        vector<pair<double,int>> buf(G);
        for(int i=0;i<G;i++){
            int n=0;
            for(int j=0;j<G;j++) if(j!=i){
                double dx=(double)(gx[i]-gx[j]), dy=(double)(gy[i]-gy[j]);
                buf[n++]={dx*dx+dy*dy,j};
            }
            nth_element(buf.begin(),buf.begin()+K-1,buf.begin()+n);
            sort(buf.begin(),buf.begin()+K);
            for(int t=0;t<K;t++) knn[i].push_back(buf[t].second);
        }
    }
    S={0,0,0,0,0,0,0,false};
    int fails=0;
    while(true){
        if(elapsed()>8.6) break;
        if(S.t>=T) break;
        Plan best; double bestR=-1;
        for(int i=0;i<W;i++){
            Plan p=build(i,true,S);
            if(p.ord.empty()) continue;
            double r=p.score/max(p.time,1.0);
            if(r>bestR){bestR=r;best=p;}
        }
        if(bestR>0){
            if(execute(best,true)){fails=0;continue;}
            usedG[best.ord[0]]=1; // ban
            if(++fails<50) continue;
        }
        // one-way final trip
        Plan b2; double r2=-1;
        for(int i=0;i<W;i++){
            Plan p=build(i,false,S);
            if(p.ord.empty()) continue;
            double r=p.score/max(p.time,1.0);
            if(r>r2){r2=r;b2=p;}
        }
        if(r2>0) execute(b2,false);
        break;
    }
    string o; o.reserve(acts.size()*20+20);
    o+=to_string(acts.size()); o+='\n';
    for(auto&a:acts){
        switch(a.ty){
            case 0: o+="AccUp "+to_string(a.arg); break;
            case 1: o+="AccDown "+to_string(a.arg); break;
            case 2: o+="AccLeft "+to_string(a.arg); break;
            case 3: o+="AccRight "+to_string(a.arg); break;
            case 4: o+="Float "+to_string(a.arg); break;
            case 5: o+="LoadCarrots "+to_string(a.arg); break;
            case 6: o+="LoadGift "+nm[a.arg]; break;
            case 7: o+="DeliverGift "+nm[a.arg]; break;
        }
        o+='\n';
    }
    fwrite(o.data(),1,o.size(),stdout);
    return 0;
}
