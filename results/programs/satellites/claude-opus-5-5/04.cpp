#include <bits/stdc++.h>
using namespace std;

struct Sat{int la,lo,v,w,d;};
struct Coll{int V; vector<int> locs; vector<pair<int,int>> ranges;};
struct Pass{int s,t0,t1;};
struct Pic{int dp,dl,loc;};

static inline int normL(int x){
    while(x<-648000)x+=1296000;
    while(x>647999)x-=1296000;
    return x;
}
static inline int dlon(int a,int b){ // a-b normalized to [-648000,648000)
    int dl=a-b;
    if(dl<-648000)dl+=1296000;
    if(dl>=648000)dl-=1296000;
    return dl;
}
static inline long long fdiv(long long a,long long b){ long long q=a/b; if((a%b!=0)&&((a<0)!=(b<0)))q--; return q;}

int main(){
    auto start=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-start).count();};
    int T,S;
    if(scanf("%d",&T)!=1)return 0;
    scanf("%d",&S);
    vector<Sat> sats(S);
    for(auto&s:sats)scanf("%d %d %d %d %d",&s.la,&s.lo,&s.v,&s.w,&s.d);
    int C; scanf("%d",&C);
    vector<Coll> colls(C);
    unordered_map<long long,int> locId;
    vector<int> LLa,LLo;
    for(int i=0;i<C;i++){
        int V,L,R; scanf("%d %d %d",&V,&L,&R);
        colls[i].V=V;
        for(int j=0;j<L;j++){
            int a,b; scanf("%d %d",&a,&b);
            long long key=(long long)a*2000000LL+b;
            auto it=locId.find(key);
            int id;
            if(it==locId.end()){id=LLa.size();locId[key]=id;LLa.push_back(a);LLo.push_back(b);}
            else id=it->second;
            colls[i].locs.push_back(id);
        }
        for(int j=0;j<R;j++){int a,b;scanf("%d %d",&a,&b);colls[i].ranges.push_back({a,b});}
        sort(colls[i].ranges.begin(),colls[i].ranges.end());
        // dedupe locs
        auto &v=colls[i].locs; sort(v.begin(),v.end()); v.erase(unique(v.begin(),v.end()),v.end());
    }
    int NLOC=LLa.size();
    if(NLOC==0){printf("0\n");return 0;}
    vector<int> minT(NLOC,INT_MAX),maxT(NLOC,-1);
    for(auto&c:colls)for(int l:c.locs)for(auto&r:c.ranges){minT[l]=min(minT[l],r.first);maxT[l]=max(maxT[l],r.second);}

    // simulate
    vector<int> plat((size_t)S*T),plon((size_t)S*T);
    for(int s=0;s<S;s++){
        int la=sats[s].la,lo=sats[s].lo,v=sats[s].v;
        size_t base=(size_t)s*T;
        for(int t=0;t<T;t++){
            plat[base+t]=la;plon[base+t]=lo;
            int n=la+v;
            if(n>324000){la=648000-n;lo=lo-15-648000;v=-v;}
            else if(n<-324000){la=-648000-n;lo=lo-15-648000;v=-v;}
            else{la=n;lo-=15;}
            lo=normL(lo);
        }
    }
    // grid
    const int G=8000;
    const int NR=648000/G+1, NC=1296000/G;
    vector<int> cellCnt(NR*NC+1,0);
    auto cellOf=[&](int la,int lo){int r=(la+324000)/G;int c=(lo+648000)/G; if(c>=NC)c=NC-1; return r*NC+c;};
    for(int i=0;i<NLOC;i++)cellCnt[cellOf(LLa[i],LLo[i])+1]++;
    for(int i=0;i<NR*NC;i++)cellCnt[i+1]+=cellCnt[i];
    vector<int> cellLoc(NLOC),fillp(cellCnt.begin(),cellCnt.end()-1);
    for(int i=0;i<NLOC;i++)cellLoc[fillp[cellOf(LLa[i],LLo[i])]++]=i;
    vector<int> cLa(NLOC),cLo(NLOC),cMin(NLOC),cMax(NLOC);
    for(int k=0;k<NLOC;k++){int i=cellLoc[k];cLa[k]=LLa[i];cLo[k]=LLo[i];cMin[k]=minT[i];cMax[k]=maxT[i];}
    vector<int> rowCnt(NR,0);
    for(int i=0;i<NLOC;i++)rowCnt[(LLa[i]+324000)/G]++;

    vector<vector<Pass>> passes(NLOC);
    vector<int> lastSat(NLOC,-1),lastT(NLOC,-5);
    for(int s=0;s<S;s++){
        int d=sats[s].d;
        size_t base=(size_t)s*T;
        for(int t=0;t<T;t++){
            int la=plat[base+t],lo=plon[base+t];
            int r0=(la-d+324000);int r1=(la+d+324000);
            if(r0<0)r0=0; if(r1>648000)r1=648000;
            r0/=G;r1/=G;
            long long c0=fdiv(lo-d+648000,G),c1=fdiv(lo+d+648000,G);
            if(c1-c0+1>NC)c1=c0+NC-1;
            for(int r=r0;r<=r1;r++){
                if(!rowCnt[r])continue;
                for(long long cc=c0;cc<=c1;cc++){
                    int c=(int)(((cc%NC)+NC)%NC);
                    int cell=r*NC+c;
                    int b=cellCnt[cell],e=cellCnt[cell+1];
                    for(int k=b;k<e;k++){
                        if(t<cMin[k]||t>cMax[k])continue;
                        int dp=cLa[k]-la; if(dp<-d||dp>d)continue;
                        int dl=dlon(cLo[k],lo); if(dl<-d||dl>d)continue;
                        int id=cellLoc[k];
                        if(lastSat[id]==s&&lastT[id]==t-1){passes[id].back().t1=t;}
                        else passes[id].push_back({s,t,t});
                        lastSat[id]=s;lastT[id]=t;
                    }
                }
            }
        }
    }

    // greedy
    vector<map<int,Pic>> sched(S);
    vector<vector<int>> taken(NLOC);
    vector<int> order(C);
    iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){
        double ka=(double)colls[a].V/colls[a].locs.size(),kb=(double)colls[b].V/colls[b].locs.size();
        if(ka!=kb)return ka>kb; return a<b;});

    auto tryPlace=[&](int s,int loc,int a,int b)->int{
        auto &m=sched[s];
        int w=sats[s].w;
        size_t base=(size_t)s*T;
        auto it=m.lower_bound(a);
        int tp=0,pp=0,pl=0;
        if(it!=m.begin()){auto pv=prev(it);tp=pv->first;pp=pv->second.dp;pl=pv->second.dl;}
        for(int t=a;t<=b;t++){
            while(it!=m.end()&&it->first<t){tp=it->first;pp=it->second.dp;pl=it->second.dl;++it;}
            if(it!=m.end()&&it->first==t)continue;
            int op=LLa[loc]-plat[base+t];
            int ol=dlon(LLo[loc],plon[base+t]);
            long long lim=(long long)w*(t-tp);
            if(llabs((long long)op-pp)>lim||llabs((long long)ol-pl)>lim)continue;
            if(it!=m.end()){
                long long lim2=(long long)w*(it->first-t);
                if(llabs((long long)it->second.dp-op)>lim2||llabs((long long)it->second.dl-ol)>lim2)continue;
            }
            m[t]={op,ol,loc};
            return t;
        }
        return -1;
    };

    for(int ci:order){
        if(elapsed()>8.6)break;
        Coll &c=colls[ci];
        if(c.V==0)continue;
        vector<int> ls=c.locs;
        sort(ls.begin(),ls.end(),[&](int a,int b){return passes[a].size()<passes[b].size();});
        vector<tuple<int,int,int>> added;
        bool ok=true;
        for(int loc:ls){
            bool cov=false;
            for(int t:taken[loc]){
                for(auto&r:c.ranges)if(t>=r.first&&t<=r.second){cov=true;break;}
                if(cov)break;
            }
            if(cov)continue;
            bool placed=false;
            for(auto &p:passes[loc]){
                for(auto&r:c.ranges){
                    int a=max(p.t0,r.first),b=min(p.t1,r.second);
                    if(a>b)continue;
                    int t=tryPlace(p.s,loc,a,b);
                    if(t>=0){added.emplace_back(p.s,t,loc);taken[loc].push_back(t);placed=true;break;}
                }
                if(placed)break;
            }
            if(!placed){ok=false;break;}
        }
        if(!ok){
            for(auto&[s,t,loc]:added){
                sched[s].erase(t);
                auto &tv=taken[loc];
                for(size_t i=0;i<tv.size();i++)if(tv[i]==t){tv.erase(tv.begin()+i);break;}
            }
        }
    }

    size_t P=0;for(auto&m:sched)P+=m.size();
    string out;out.reserve(P*32+16);
    out+=to_string(P);out+='\n';
    char buf[64];
    for(int s=0;s<S;s++)for(auto&[t,pic]:sched[s]){
        int n=snprintf(buf,sizeof(buf),"%d %d %d %d\n",LLa[pic.loc],LLo[pic.loc],t,s);
        out.append(buf,n);
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
