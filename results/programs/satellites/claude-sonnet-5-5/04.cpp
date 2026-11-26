#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
struct Entry{int c,loc;bool done;};
int main(){
    int T,S;
    scanf("%d %d",&T,&S);
    vector<ll> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++)scanf("%lld %lld %lld %lld %lld",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    int C;scanf("%d",&C);
    vector<double> val(C);
    vector<int> rem(C,0);
    vector<vector<pair<int,int>>> rg(C);
    vector<Entry> ent;
    map<pair<int,int>,int> lid;
    vector<pair<int,int>> locs;
    vector<vector<int>> locEnt;
    for(int c=0;c<C;c++){
        int V,L,R;scanf("%d %d %d",&V,&L,&R);
        val[c]=V;
        set<int> seen;
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            auto k=make_pair(a,b);
            auto it=lid.find(k);
            int id;
            if(it==lid.end()){id=locs.size();lid[k]=id;locs.push_back(k);locEnt.push_back({});}
            else id=it->second;
            if(seen.count(id))continue;
            seen.insert(id);
            locEnt[id].push_back(ent.size());
            ent.push_back({c,id,false});
            rem[c]++;
        }
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);rg[c].push_back({a,b});}
        sort(rg[c].begin(),rg[c].end());
    }
    const int CS=3000,NC=1296000/CS,NR=612000/CS+1;
    vector<vector<int>> cells((size_t)NC*NR);
    for(size_t i=0;i<locs.size();i++){
        int r=(locs[i].first+306000)/CS;
        int cc=(locs[i].second+648000)/CS;
        cells[(size_t)r*NC+cc].push_back(i);
    }
    vector<int> alive(locs.size());
    for(size_t i=0;i<locs.size();i++)alive[i]=locEnt[i].size();
    auto active=[&](int c,int t){
        auto&v=rg[c];
        auto it=upper_bound(v.begin(),v.end(),make_pair(t,INT_MAX));
        if(it==v.begin())return false;
        --it;
        return it->second>=t;
    };
    vector<ll> ldp(S,0),ldl(S,0),lt(S,0);
    vector<ll> P=sp,L=sl,V=sv;
    vector<array<ll,4>> out;
    ll ops=0;const ll LIM=500000000LL;
    auto wrap=[&](ll x){
        x=((x+648000)%1296000+1296000)%1296000-648000;
        return x;
    };
    for(int t=0;t<T;t++){
        if(ops<LIM){
            for(int s=0;s<S;s++){
                ll d=sd[s];
                ll lo=max(-306000LL,P[s]-d),hi=min(306000LL,P[s]+d);
                if(lo>hi)continue;
                int r0=(lo+306000)/CS,r1=(hi+306000)/CS;
                ll lamlo=L[s]-d;
                ll lamhi=L[s]+d;
                // column range
                ll c0=(lamlo+648000);
                ll c1=(lamhi+648000);
                ll k0=(c0>=0?c0/CS:-((-c0+CS-1)/CS));
                ll k1=(c1>=0?c1/CS:-((-c1+CS-1)/CS));
                if(k1-k0+1>NC){k0=0;k1=NC-1;}
                double best=0;int bl=-1;ll bdp=0,bdl=0;
                ll maxch=sw[s]*(t-lt[s]);
                for(int r=r0;r<=r1;r++){
                    for(ll k=k0;k<=k1;k++){
                        int cc=((k%NC)+NC)%NC;
                        auto&cell=cells[(size_t)r*NC+cc];
                        for(int id:cell){
                            ops++;
                            if(!alive[id])continue;
                            ll dp=locs[id].first-P[s];
                            if(dp<-d||dp>d)continue;
                            ll dl=wrap(locs[id].second-L[s]);
                            if(dl<-d||dl>d)continue;
                            if(llabs(dp-ldp[s])>maxch||llabs(dl-ldl[s])>maxch)continue;
                            double w=0;
                            for(int e:locEnt[id]){
                                if(ent[e].done)continue;
                                int c=ent[e].c;
                                if(!active(c,t))continue;
                                w+=val[c]/rem[c];
                            }
                            if(w>best){best=w;bl=id;bdp=dp;bdl=dl;}
                        }
                    }
                }
                if(bl>=0){
                    for(int e:locEnt[bl]){
                        if(ent[e].done)continue;
                        int c=ent[e].c;
                        if(!active(c,t))continue;
                        ent[e].done=true;rem[c]--;alive[bl]--;
                    }
                    out.push_back({locs[bl].first,locs[bl].second,t,s});
                    ldp[s]=bdp;ldl[s]=bdl;lt[s]=t;
                }
            }
        }
        // advance
        for(int s=0;s<S;s++){
            ll np=P[s]+V[s];
            ll nl=L[s]-15;
            if(np>324000){np=648000-np;nl+=648000;V[s]=-V[s];}
            else if(np<-324000){np=-648000-np;nl+=648000;V[s]=-V[s];}
            P[s]=np;L[s]=wrap(nl);
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out)printf("%lld %lld %lld %lld\n",o[0],o[1],o[2],o[3]);
}
