#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static inline ll floorDiv(ll a, ll b){ return a>=0 ? a/b : -((-a+b-1)/b); }
static inline ll ceilDiv(ll a, ll b){ return -floorDiv(-a,b); }

struct Win{int sat,a,b;};
struct Pic{int dlat,dlon,pt;};

int main(){
    int T,S;
    if(scanf("%d",&T)!=1) return 0;
    scanf("%d",&S);
    vector<int> sLat(S),sLon(S),sV(S),sW(S),sD(S);
    for(int i=0;i<S;i++) scanf("%d %d %d %d %d",&sLat[i],&sLon[i],&sV[i],&sW[i],&sD[i]);
    int C; scanf("%d",&C);
    vector<int> cV(C);
    vector<vector<int>> cPts(C);
    vector<vector<pair<int,int>>> cR(C);
    unordered_map<ll,int> pmap;
    vector<int> pLat,pLon;
    for(int c=0;c<C;c++){
        int V,L,R; scanf("%d %d %d",&V,&L,&R);
        cV[c]=V;
        for(int i=0;i<L;i++){
            int a,b; scanf("%d %d",&a,&b);
            ll key=(ll)(a+400000)*2000000LL+(b+700000);
            auto it=pmap.find(key);
            int id;
            if(it==pmap.end()){id=pLat.size();pmap[key]=id;pLat.push_back(a);pLon.push_back(b);}
            else id=it->second;
            cPts[c].push_back(id);
        }
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);cR[c].push_back({a,b});}
        sort(cR[c].begin(),cR[c].end());
    }
    int P=pLat.size();
    vector<int> ptMin(P,INT_MAX),ptMax(P,-1);
    for(int c=0;c<C;c++) for(int p:cPts[c]) for(auto&r:cR[c]){ptMin[p]=min(ptMin[p],r.first);ptMax[p]=max(ptMax[p],r.second);}
    const int G=3600,NR=181,NC=360;
    vector<vector<int>> cells(NR*NC);
    for(int p=0;p<P;p++){
        int r=(pLat[p]+324000)/G; r=max(0,min(NR-1,r));
        int cc=(pLon[p]+648000)/G; cc=max(0,min(NC-1,cc));
        cells[r*NC+cc].push_back(p);
    }
    vector<int> latA((size_t)S*T),lonA((size_t)S*T);
    vector<int> vel(T);
    vector<vector<Win>> wins(P);
    ll totalWins=0; const ll WCAP=80000000LL;
    for(int s=0;s<S;s++){
        int* LA=&latA[(size_t)s*T]; int* LO=&lonA[(size_t)s*T];
        int lat=sLat[s],lon=sLon[s],v=sV[s];
        for(int t=0;t<T;t++){
            LA[t]=lat;LO[t]=lon;vel[t]=v;
            int nl=lat+v,nlon;
            if(nl>324000){nl=648000-nl;nlon=lon-15-648000;v=-v;}
            else if(nl<-324000){nl=-648000-nl;nlon=lon-15-648000;v=-v;}
            else nlon=lon-15;
            while(nlon<-648000) nlon+=1296000;
            while(nlon>647999) nlon-=1296000;
            lat=nl;lon=nlon;
        }
        int d=sD[s];
        int av=abs(sV[s]);
        int K=max(1,min(1000,d/av+1));
        int s0=0;
        while(s0<T){
            int e=s0; int vs=vel[s0];
            while(e+1<T && e+1-s0<K && LA[e+1]==LA[e]+vs && LO[e+1]==LO[e]-15) e++;
            if(totalWins<WCAP){
                int latmin=min(LA[s0],LA[e])-d, latmax=max(LA[s0],LA[e])+d;
                int lonmin=LO[e]-d, lonmax=LO[s0]+d;
                lonmin=max(lonmin,-648000); lonmax=min(lonmax,647999);
                latmin=max(latmin,-324000); latmax=min(latmax,324000);
                if(lonmin<=lonmax && latmin<=latmax){
                    int r0=max(0,(latmin+324000)/G), r1=min(NR-1,(latmax+324000)/G);
                    int c0=max(0,(lonmin+648000)/G), c1=min(NC-1,(lonmax+648000)/G);
                    int len=e-s0+1; ll latS=LA[s0], lonS=LO[s0];
                    for(int r=r0;r<=r1;r++) for(int cc=c0;cc<=c1;cc++){
                        for(int p:cells[r*NC+cc]){
                            if(ptMax[p]<0) continue;
                            ll lo=0,hi=len-1;
                            ll A=pLat[p]-d-latS,B=pLat[p]+d-latS; ll vv=vs;
                            if(vv<0){vv=-vv;ll tmp=A;A=-B;B=-tmp;}
                            lo=max(lo,ceilDiv(A,vv)); hi=min(hi,floorDiv(B,vv));
                            if(lo>hi) continue;
                            ll X=lonS-pLon[p];
                            lo=max(lo,ceilDiv(X-d,15)); hi=min(hi,floorDiv(X+d,15));
                            lo=max(lo,(ll)ptMin[p]-s0); hi=min(hi,(ll)ptMax[p]-s0);
                            if(lo>hi) continue;
                            int a=s0+lo,b=s0+hi;
                            auto&W=wins[p];
                            if(!W.empty()&&W.back().sat==s&&W.back().b+1>=a){W.back().b=max(W.back().b,b);}
                            else {W.push_back({s,a,b});totalWins++;}
                        }
                    }
                }
            }
            s0=e+1;
        }
    }
    for(int p=0;p<P;p++) sort(wins[p].begin(),wins[p].end(),[](const Win&x,const Win&y){if(x.a!=y.a)return x.a<y.a;return x.sat<y.sat;});

    vector<map<int,Pic>> sched(S);
    vector<vector<int>> taken(P);
    vector<int> order(C);
    for(int i=0;i<C;i++) order[i]=i;
    sort(order.begin(),order.end(),[&](int a,int b){
        double x=(double)cV[a]/cPts[a].size(), y=(double)cV[b]/cPts[b].size();
        if(x!=y) return x>y; return a<b;});
    ll globalChecks=0; const ll GCAP=400000000LL;
    const ll LCAP=20000;
    for(int ci:order){
        if(globalChecks>GCAP) break;
        if(cV[ci]==0) continue;
        auto&R=cR[ci];
        vector<int> pl=cPts[ci];
        sort(pl.begin(),pl.end(),[&](int a,int b){if(wins[a].size()!=wins[b].size())return wins[a].size()<wins[b].size();return a<b;});
        vector<array<int,3>> added;
        bool ok=true;
        for(int p:pl){
            bool done=false;
            for(int t:taken[p]){
                auto it=upper_bound(R.begin(),R.end(),make_pair(t,INT_MAX));
                if(it!=R.begin()){--it; if(it->second>=t){done=true;break;}}
            }
            if(done) continue;
            ll checks=0; bool found=false;
            for(auto&wd:wins[p]){
                if(found||checks>LCAP) break;
                int idx=lower_bound(R.begin(),R.end(),make_pair(wd.a,INT_MIN))-R.begin();
                if(idx>0 && R[idx-1].second>=wd.a) idx--;
                int s=wd.sat; int* LA=&latA[(size_t)s*T]; int* LO=&lonA[(size_t)s*T];
                ll w=sW[s]; int d=sD[s];
                auto&M=sched[s];
                for(;idx<(int)R.size() && R[idx].first<=wd.b && !found && checks<=LCAP;idx++){
                    int lo=max(wd.a,R[idx].first),hi=min(wd.b,R[idx].second);
                    if(lo>hi) continue;
                    auto it=M.lower_bound(lo);
                    for(int t=lo;t<=hi;t++){
                        checks++;
                        while(it!=M.end()&&it->first<t) ++it;
                        if(it!=M.end()&&it->first==t) continue;
                        ll ola=pLat[p]-LA[t], olo=pLon[p]-LO[t];
                        if(llabs(ola)>d||llabs(olo)>d) continue;
                        ll tp=0,pa=0,po=0;
                        if(it!=M.begin()){auto pv=prev(it);tp=pv->first;pa=pv->second.dlat;po=pv->second.dlon;}
                        ll dt=t-tp;
                        if(llabs(ola-pa)>w*dt||llabs(olo-po)>w*dt) continue;
                        if(it!=M.end()){
                            ll dn=it->first-t;
                            if(llabs(it->second.dlat-ola)>w*dn||llabs(it->second.dlon-olo)>w*dn) continue;
                        }
                        M.emplace(t,Pic{(int)ola,(int)olo,p});
                        taken[p].push_back(t);
                        added.push_back({s,t,p});
                        found=true; break;
                    }
                }
            }
            globalChecks+=checks;
            if(!found){ok=false;break;}
        }
        if(!ok){
            for(auto&a:added){
                sched[a[0]].erase(a[1]);
                auto&tk=taken[a[2]];
                for(size_t i=0;i<tk.size();i++) if(tk[i]==a[1]){tk.erase(tk.begin()+i);break;}
            }
        }
    }
    // final validation (removal of a picture never invalidates neighbours)
    vector<array<int,4>> out;
    for(int s=0;s<S;s++){
        ll tp=0,pa=0,po=0; ll w=sW[s]; ll d=sD[s];
        int* LA=&latA[(size_t)s*T]; int* LO=&lonA[(size_t)s*T];
        for(auto&kv:sched[s]){
            int t=kv.first; int p=kv.second.pt;
            ll ola=pLat[p]-LA[t], olo=pLon[p]-LO[t];
            if(llabs(ola)>d||llabs(olo)>d) continue;
            ll dt=t-tp;
            if(llabs(ola-pa)>w*dt||llabs(olo-po)>w*dt) continue;
            tp=t;pa=ola;po=olo;
            out.push_back({pLat[p],pLon[p],t,s});
        }
    }
    string buf;
    buf.reserve(out.size()*32+16);
    buf+=to_string(out.size()); buf+='\n';
    for(auto&o:out){
        buf+=to_string(o[0]);buf+=' ';buf+=to_string(o[1]);buf+=' ';
        buf+=to_string(o[2]);buf+=' ';buf+=to_string(o[3]);buf+='\n';
    }
    fwrite(buf.data(),1,buf.size(),stdout);
    return 0;
}
