#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int T,D,Wn,G;
vector<ll> L,A;
vector<string> nm;
vector<ll> gs,gw,gx,gy;
vector<char> delivered;
ll work=0; const ll WORKCAP=900000000LL;

ll maxAcc(ll w){ for(int i=0;i<Wn;i++) if(w<=L[i]) return A[i]; return 0; }
ll Qf(ll n){ ll m=n/2; if(n%2==0) return m*(m+1); return (m+1)*(m+1); }
ll minN(ll d, ll a){
    ll x=(d+a-1)/a;
    ll n0=(ll)(2*sqrtl((long double)x))-3; if(n0<0)n0=0;
    while(Qf(n0)<x) n0++;
    return n0;
}
vector<ll> prof;
ll fsum(ll h, ll n, ll a){
    ll m=n/2; bool odd=n&1;
    ll j=min(m,h/a);
    ll res=2*(a*j*(j+1)/2+(m-j)*h);
    if(odd) res+=min(a*(m+1),h);
    return res;
}
void buildProfile(ll d, ll a, ll n){
    prof.assign(n+1,0);
    ll lo=0,hi=a*((n+1)/2);
    while(lo<hi){ ll mid=(lo+hi+1)/2; if(fsum(mid,n,a)<=d) lo=mid; else hi=mid-1; }
    ll h=lo; ll r=d-fsum(h,n,a);
    for(ll i=1;i<=n;i++){
        ll c=a*min(i,n+1-i); ll v=min(c,h);
        if(c>h && r>0){v++;r--;}
        prof[i]=v;
    }
}
bool within(ll ax,ll ay,ll bx,ll by){
    __int128 dx=ax-bx, dy=ay-by;
    return dx*dx+dy*dy <= (__int128)D*D;
}
pair<ll,ll> tgt(ll px,ll py,ll qx,ll qy){
    if(within(px,py,qx,qy)) return {px,py};
    long double dx=px-qx, dy=py-qy;
    long double dist=sqrtl(dx*dx+dy*dy);
    ll ox=(ll)(dx*D/dist), oy=(ll)(dy*D/dist);
    while((__int128)ox*ox+(__int128)oy*oy > (__int128)D*D){
        if(llabs(ox)>=llabs(oy)) ox += (ox>0?-1:1); else oy += (oy>0?-1:1);
    }
    return {qx+ox,qy+oy};
}
ll axT(ll d, ll a){ if(d<=0) return 0; return minN(d,a)+1; }
ll est(ll px,ll py,ll qx,ll qy,ll a){
    long double dx=qx-px, dy=qy-py;
    long double dist=sqrtl(dx*dx+dy*dy);
    if(dist<=D) return 0;
    long double f=(dist-D)/dist;
    ll ex=(ll)ceill(fabsl(dx)*f), ey=(ll)ceill(fabsl(dy)*f);
    return axT(ex,a)+axT(ey,a);
}

struct St{ ll x,y; bool lastAcc; ll carrots; };
struct RunRes{ bool movesOk; ll time,used,score; ll x,y; bool la; };

RunRes run(const vector<int>& route, bool ret, const St& st, ll onboard, vector<string>* out, ll limit){
    ll Wt=onboard; for(int g:route) Wt+=gw[g];
    ll time=0; bool la=st.lastAcc; ll cx=st.x, cy=st.y; ll pend=0; ll used=0, score=0; bool ok=true;
    auto flush=[&](){ if(out && pend>0) out->push_back("Float "+to_string(pend)); pend=0; };
    auto fl=[&](ll k){ time+=k; pend+=k; la=false; };
    auto acc=[&](int axis, ll amt){
        if(la) fl(1);
        flush();
        if(out){
            string s;
            if(axis==0) s = amt>0? "AccRight ":"AccLeft ";
            else s = amt>0? "AccUp ":"AccDown ";
            out->push_back(s+to_string(llabs(amt)));
        }
        Wt--; used++; la=true;
    };
    if(out){
        if(onboard-st.carrots>0) out->push_back("LoadCarrots "+to_string(onboard-st.carrots));
        for(int g:route) out->push_back("LoadGift "+nm[g]);
    }
    auto moveAxis=[&](int axis, ll d)->bool{
        if(d==0) return true;
        ll a=maxAcc(Wt); if(a<=0) return false;
        ll ad=llabs(d); ll n=minN(ad,a);
        if(time+n>limit) return false;
        work+=n;
        buildProfile(ad,a,n);
        ll sg=d>0?1:-1; ll prev=0;
        for(ll i=1;i<=n;i++){
            ll ch=prof[i]-prev;
            if(ch) acc(axis, sg*ch);
            fl(1);
            prev=prof[i];
        }
        if(prev) acc(axis, -sg*prev);
        if(axis==0) cx+=d; else cy+=d;
        return true;
    };
    vector<char> done(route.size(),0);
    for(size_t k=0;k<route.size();k++){
        if(done[k]) continue;
        int g=route[k];
        auto q=tgt(cx,cy,gx[g],gy[g]);
        if(!moveAxis(0,q.first-cx) || !moveAxis(1,q.second-cy)){ ok=false; break; }
        for(size_t j=k;j<route.size();j++){
            if(done[j]) continue;
            int h=route[j];
            if(within(cx,cy,gx[h],gy[h])){
                flush();
                if(out) out->push_back("DeliverGift "+nm[h]);
                Wt-=gw[h]; score+=gs[h]; done[j]=1;
            }
        }
        if(!done[k]){ ok=false; break; }
    }
    if(ok && ret){
        auto q=tgt(cx,cy,0,0);
        if(!moveAxis(0,q.first-cx) || !moveAxis(1,q.second-cy)) ok=false;
    }
    flush();
    work+=route.size();
    RunRes R; R.movesOk=ok; R.time=time; R.used=used; R.score=score; R.x=cx; R.y=cy; R.la=la;
    return R;
}

struct Trip{ vector<int> route; ll onboard,time,score; bool ok; };

Trip evalTrip(const vector<int>& route, bool ret, const St& st, ll limit, ll startOn){
    Trip tr; tr.route=route; tr.ok=false;
    ll on=max(st.carrots,startOn);
    for(int it=0;it<25;it++){
        RunRes r=run(route,ret,st,on,nullptr,limit);
        if(!r.movesOk || r.time>limit) return tr;
        if(r.used<=on){ tr.ok=true; tr.onboard=on; tr.time=r.time; tr.score=r.score; return tr; }
        on=r.used;
    }
    return tr;
}

vector<int> inR;
int stampv=0;

Trip build(const St& st, bool ret, vector<int> route, ll limit){
    stampv++;
    for(int g:route) inR[g]=stampv;
    Trip tr; tr.route=route; tr.onboard=st.carrots; tr.time=0; tr.score=0; tr.ok=true;
    if(!route.empty()){
        Trip e=evalTrip(route,ret,st,limit,st.carrots);
        if(!e.ok){ tr.route.clear(); return tr; }
        tr=e;
    }
    ll giftW=0; for(int g:tr.route) giftW+=gw[g];
    const int K=4;
    while(true){
        if(work>WORKCAP) break;
        ll px,py;
        if(tr.route.empty()){px=st.x;py=st.y;} else {px=gx[tr.route.back()];py=gy[tr.route.back()];}
        vector<pair<double,int>> top;
        for(int g=0;g<G;g++){
            if(delivered[g]||inR[g]==stampv) continue;
            ll wt=giftW+gw[g]+tr.onboard;
            if(wt>L.back()) continue;
            ll a=maxAcc(wt+2); if(a==0) a=maxAcc(wt); if(a==0) continue;
            ll add=est(px,py,gx[g],gy[g],a);
            if(ret) add+=est(gx[g],gy[g],0,0,a)-est(px,py,0,0,a);
            if(tr.time+add>limit) continue;
            double ratio=(double)gs[g]/(double)(max(add,0LL)+1);
            if((int)top.size()<K){ top.push_back({ratio,g}); sort(top.rbegin(),top.rend()); }
            else if(ratio>top.back().first){ top.back()={ratio,g}; sort(top.rbegin(),top.rend()); }
        }
        work+=G;
        bool acc=false;
        for(auto &pr:top){
            int g=pr.second;
            vector<int> r2=tr.route; r2.push_back(g);
            Trip e=evalTrip(r2,ret,st,limit,tr.onboard);
            if(e.ok){ tr=e; inR[g]=stampv; giftW+=gw[g]; acc=true; break; }
        }
        if(!acc) break;
    }
    return tr;
}

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    cin>>T>>D>>Wn>>G;
    L.resize(Wn);A.resize(Wn);
    for(int i=0;i<Wn;i++) cin>>L[i]>>A[i];
    nm.resize(G);gs.resize(G);gw.resize(G);gx.resize(G);gy.resize(G);
    for(int i=0;i<G;i++) cin>>nm[i]>>gs[i]>>gw[i]>>gx[i]>>gy[i];
    delivered.assign(G,0); inR.assign(G,0);
    vector<string> out;
    St st{0,0,false,0}; ll t=0;
    auto execute=[&](const Trip& tr, bool ret, ll R){
        RunRes r=run(tr.route,ret,st,tr.onboard,&out,R);
        st.x=r.x; st.y=r.y; st.lastAcc=r.la; st.carrots=tr.onboard-r.used;
        t+=r.time;
        for(int g:tr.route) delivered[g]=1;
    };
    while(true){
        ll R=T-t; if(R<=0) break;
        if(work>WORKCAP) break;
        Trip A1=build(st,true,{},R);
        if(A1.route.empty()){
            Trip E=build(st,false,{},R);
            if(!E.route.empty()) execute(E,false,R);
            break;
        }
        if(A1.time>0 && (R-A1.time)<3*A1.time){
            Trip E=build(st,false,A1.route,R);
            if(!E.route.empty()){
                double gain=(double)(E.score-A1.score);
                double fut=(double)A1.score/(double)A1.time*(double)(R-A1.time)*0.8;
                if(gain>=fut){ execute(E,false,R); break; }
            }
        }
        execute(A1,true,R);
    }
    string s; s.reserve(out.size()*20+20);
    s+=to_string(out.size()); s+='\n';
    for(auto&x:out){ s+=x; s+='\n'; }
    fwrite(s.data(),1,s.size(),stdout);
    return 0;
}
