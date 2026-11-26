#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static ll floorDiv(ll a, ll b){ ll q=a/b; if((a%b!=0)&&((a<0)!=(b<0))) q--; return q; }
int main(){
    int T,S;
    scanf("%d",&T);
    scanf("%d",&S);
    vector<array<ll,5>> sat(S);
    for(auto&s:sat) for(int k=0;k<5;k++) scanf("%lld",&s[k]);
    int C; scanf("%d",&C);
    vector<int> V(C), rem(C);
    vector<vector<int>> st(C), en(C);
    struct E{ int phi,lam,c; };
    vector<E> es;
    for(int c=0;c<C;c++){
        int L,R; scanf("%d %d %d",&V[c],&L,&R);
        rem[c]=L;
        for(int i=0;i<L;i++){ int a,b; scanf("%d %d",&a,&b); es.push_back({a,b,c}); }
        vector<pair<int,int>> r(R);
        for(auto&p:r) scanf("%d %d",&p.first,&p.second);
        sort(r.begin(),r.end());
        for(auto&p:r){ st[c].push_back(p.first); en[c].push_back(p.second); }
    }
    int N=es.size();
    vector<int> ord(N);
    for(int i=0;i<N;i++) ord[i]=i;
    sort(ord.begin(),ord.end(),[&](int a,int b){
        if(es[a].phi!=es[b].phi) return es[a].phi<es[b].phi;
        return es[a].lam<es[b].lam; });
    vector<int> locPhi, locLam, locStart; // unique locs
    vector<int> locEntries; // entries sorted
    for(int k=0;k<N;k++){
        int i=ord[k];
        if(k==0||es[i].phi!=locPhi.back()||es[i].lam!=locLam.back()){
            locPhi.push_back(es[i].phi); locLam.push_back(es[i].lam); locStart.push_back(k);
        }
        locEntries.push_back(i);
    }
    int U=locPhi.size();
    locStart.push_back(N);
    vector<char> done(N,0);
    const int CS=2000, NLAT=307, NLON=648;
    vector<int> cellStart(NLAT*NLON+1,0), cellItems(U);
    auto cellOf=[&](int u){
        int a=(locPhi[u]+306000)/CS;
        int b=(locLam[u]+648000)/CS;
        return a*NLON+b; };
    for(int u=0;u<U;u++) cellStart[cellOf(u)+1]++;
    for(int i=0;i<NLAT*NLON;i++) cellStart[i+1]+=cellStart[i];
    {
        vector<int> pos(cellStart.begin(),cellStart.end()-1);
        for(int u=0;u<U;u++) cellItems[pos[cellOf(u)]++]=u;
    }
    auto inRange=[&](int c,int t){
        auto&s=st[c];
        int k=upper_bound(s.begin(),s.end(),t)-s.begin()-1;
        return k>=0&&en[c][k]>=t;
    };
    struct P{int phi,lam,t,s;};
    vector<P> out;
    ll ops=0; const ll BUDGET=700000000LL;
    for(int si=0;si<S&&ops<BUDGET;si++){
        ll phi=sat[si][0], lam=sat[si][1], v=sat[si][2], w=sat[si][3], d=sat[si][4];
        ll oP=0,oL=0; int tl=0;
        for(int t=0;t<T;t++){
            if(t>0){
                ll np=phi+v;
                ll nl=lam-15;
                if(np>324000){ np=648000-np; nl-=648000; v=-v; }
                else if(np<-324000){ np=-648000-np; nl-=648000; v=-v; }
                nl=((nl+648000)%1296000+1296000)%1296000-648000;
                phi=np; lam=nl;
            }
            if(ops>=BUDGET) break;
            ll lo=max(phi-d,(ll)-306000), hi=min(phi+d,(ll)306000);
            if(lo>hi) continue;
            ll reach=w*(t-tl);
            int a0=(lo+306000)/CS, a1=(hi+306000)/CS;
            ll l0=lam-d+648000, l1=lam+d+648000;
            ll c0=floorDiv(l0,CS), c1=floorDiv(l1,CS);
            if(c1-c0>=NLON) c1=c0+NLON-1;
            double best=0; int bu=-1; ll bdp=0,bdl=0;
            for(int a=a0;a<=a1;a++){
                for(ll cc=c0;cc<=c1;cc++){
                    int b=((cc%NLON)+NLON)%NLON;
                    int ci=a*NLON+b;
                    ops+=1+cellStart[ci+1]-cellStart[ci];
                    for(int k=cellStart[ci];k<cellStart[ci+1];k++){
                        int u=cellItems[k];
                        ll dp=locPhi[u]-phi;
                        if(dp<-d||dp>d) continue;
                        ll dl=locLam[u]-lam;
                        dl=((dl+648000)%1296000+1296000)%1296000-648000;
                        if(dl<-d||dl>d) continue;
                        if(llabs(dp-oP)>reach||llabs(dl-oL)>reach) continue;
                        double val=0;
                        for(int q=locStart[u];q<locStart[u+1];q++){
                            int e=locEntries[q];
                            if(done[e]) continue;
                            int c=es[e].c;
                            if(inRange(c,t)) val+=(double)V[c]/rem[c];
                        }
                        if(val>best){best=val;bu=u;bdp=dp;bdl=dl;}
                    }
                }
            }
            if(bu>=0){
                for(int q=locStart[bu];q<locStart[bu+1];q++){
                    int e=locEntries[q];
                    if(done[e]) continue;
                    int c=es[e].c;
                    if(inRange(c,t)){ done[e]=1; rem[c]--; }
                }
                out.push_back({locPhi[bu],locLam[bu],t,si});
                oP=bdp;oL=bdl;tl=t;
            }
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&p:out) printf("%d %d %d %d\n",p.phi,p.lam,p.t,p.s);
}
