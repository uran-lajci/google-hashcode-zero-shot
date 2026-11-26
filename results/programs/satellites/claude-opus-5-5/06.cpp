#include <bits/stdc++.h>
using namespace std;

static long long floorDiv(long long x,long long y){long long q=x/y; if((x%y!=0)&&((x<0)!=(y<0)))q--; return q;}
static long long ceilDiv(long long x,long long y){long long q=x/y; if((x%y!=0)&&((x<0)==(y<0)))q++; return q;}
// range of integer k with lo <= a + v*k <= hi
static void solveRange(long long a,long long v,long long lo,long long hi,long long&kmin,long long&kmax){
    if(v>0){kmin=ceilDiv(lo-a,v);kmax=floorDiv(hi-a,v);}
    else {kmax=floorDiv(lo-a,v);kmin=ceilDiv(hi-a,v);}
}

struct Sat{int lat,lon,v,w,d;};
struct Win{int s,a,b;};
struct Shot{int ox,oy,p;};

int T,S,C;
vector<Sat> sats;
vector<int> PL,PLo;
vector<vector<pair<int,int>>> pUnion;
vector<vector<Win>> pw;
vector<int> posLat,posLon;

void emitW(int p,int s,int a,int b){
    auto &U=pUnion[p];
    int lo=0,hi=U.size();
    while(lo<hi){int m=(lo+hi)/2; if(U[m].second<a) lo=m+1; else hi=m;}
    for(int i=lo;i<(int)U.size()&&U[i].first<=b;i++){
        int x=max(a,U[i].first),y=min(b,U[i].second);
        if(x>y) continue;
        auto &W=pw[p];
        if(!W.empty()&&W.back().s==s&&W.back().b>=x-1){W.back().b=max(W.back().b,y);}
        else W.push_back({s,x,y});
    }
}

int main(){
    scanf("%d",&T);
    scanf("%d",&S);
    sats.resize(S);
    for(auto&s:sats) scanf("%d %d %d %d %d",&s.lat,&s.lon,&s.v,&s.w,&s.d);
    scanf("%d",&C);
    vector<int> cV(C);
    vector<vector<int>> cP(C);
    vector<vector<pair<int,int>>> cR(C);
    unordered_map<long long,int> pid;
    for(int c=0;c<C;c++){
        int V,L,R; scanf("%d %d %d",&V,&L,&R);
        cV[c]=V;
        for(int i=0;i<L;i++){
            int a,b; scanf("%d %d",&a,&b);
            long long key=(long long)(a+400000)*2000000LL+(b+700000);
            auto it=pid.find(key); int id;
            if(it==pid.end()){id=PL.size();pid[key]=id;PL.push_back(a);PLo.push_back(b);}
            else id=it->second;
            cP[c].push_back(id);
        }
        sort(cP[c].begin(),cP[c].end());
        cP[c].erase(unique(cP[c].begin(),cP[c].end()),cP[c].end());
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);cR[c].push_back({a,b});}
        sort(cR[c].begin(),cR[c].end());
    }
    int NP=PL.size();
    pUnion.assign(NP,{});
    for(int c=0;c<C;c++) for(int p:cP[c]) for(auto&r:cR[c]) pUnion[p].push_back(r);
    for(int p=0;p<NP;p++){
        auto &U=pUnion[p]; sort(U.begin(),U.end());
        vector<pair<int,int>> M;
        for(auto&r:U){ if(!M.empty()&&r.first<=M.back().second+1) M.back().second=max(M.back().second,r.second); else M.push_back(r);}
        U.swap(M);
    }
    pw.assign(NP,{});
    // grid
    const int CELL=10000;
    const int ROWS=648001/CELL+1, COLS=1296000/CELL+1;
    vector<int> cellStart(ROWS*COLS+1,0), cellItems(NP);
    auto cellOf=[&](int lat,int lon){int r=(lat+324000)/CELL,c=(lon+648000)/CELL;return r*COLS+c;};
    for(int p=0;p<NP;p++) cellStart[cellOf(PL[p],PLo[p])+1]++;
    for(int i=0;i<ROWS*COLS;i++) cellStart[i+1]+=cellStart[i];
    {vector<int> fill(cellStart.begin(),cellStart.end()-1);
     for(int p=0;p<NP;p++) cellItems[fill[cellOf(PL[p],PLo[p])]++]=p;}
    auto rowIdx=[&](long long lat){long long r=(lat+324000);if(r<0)r=0; r/=CELL; if(r>ROWS-1)r=ROWS-1;return (int)r;};
    auto colIdx=[&](long long lon){long long c=(lon+648000);if(c<0)c=0; c/=CELL; if(c>COLS-1)c=COLS-1;return (int)c;};

    posLat.assign((size_t)S*T,0); posLon.assign((size_t)S*T,0);
    vector<int> vel(T);
    const int K=16;
    for(int s=0;s<S;s++){
        int la=sats[s].lat,lo=sats[s].lon,v=sats[s].v,d=sats[s].d;
        int* PLa=&posLat[(size_t)s*T]; int* PLn=&posLon[(size_t)s*T];
        for(int t=0;t<T;t++){
            PLa[t]=la;PLn[t]=lo;vel[t]=v;
            int nl=la+v;
            if(nl>=-324000&&nl<=324000){la=nl;lo-=15;}
            else if(nl>324000){la=648000-nl;lo=-648000+(lo-15);v=-v;}
            else {la=-648000-nl;lo=-648000+(lo-15);v=-v;}
            while(lo<-648000)lo+=1296000;
            while(lo>647999)lo-=1296000;
        }
        if(NP==0) continue;
        for(int t0=0;t0<T;t0+=K){
            int len=min(K,T-t0);
            int te=t0+len-1;
            bool linear = vel[te]==vel[t0] && PLa[te]==PLa[t0]+(long long)vel[t0]*(len-1) && PLn[te]==PLn[t0]-15*(len-1);
            if(linear){
                long long la0=PLa[t0],lo0=PLn[t0],v0=vel[t0];
                long long latMin=min(la0,(long long)PLa[te])-d, latMax=max(la0,(long long)PLa[te])+d;
                if(latMin>306000||latMax<-306000) continue;
                long long lonMin=PLn[te]-d, lonMax=lo0+d;
                int r0=rowIdx(latMin),r1=rowIdx(latMax),c0=colIdx(lonMin),c1=colIdx(lonMax);
                for(int r=r0;r<=r1;r++)for(int c=c0;c<=c1;c++){
                    int cell=r*COLS+c;
                    for(int i=cellStart[cell];i<cellStart[cell+1];i++){
                        int p=cellItems[i];
                        long long k1,k2,k3,k4;
                        solveRange(la0,v0,(long long)PL[p]-d,(long long)PL[p]+d,k1,k2);
                        long long kmin=max(0LL,k1),kmax=min((long long)len-1,k2);
                        if(kmin>kmax) continue;
                        solveRange((long long)PLo[p]-lo0,15,-d,d,k3,k4);
                        kmin=max(kmin,k3);kmax=min(kmax,k4);
                        if(kmin>kmax) continue;
                        emitW(p,s,t0+(int)kmin,t0+(int)kmax);
                    }
                }
            } else {
                for(int t=t0;t<=te;t++){
                    long long la0=PLa[t],lo0=PLn[t];
                    if(la0-d>306000||la0+d<-306000) continue;
                    int r0=rowIdx(la0-d),r1=rowIdx(la0+d),c0=colIdx(lo0-d),c1=colIdx(lo0+d);
                    for(int r=r0;r<=r1;r++)for(int c=c0;c<=c1;c++){
                        int cell=r*COLS+c;
                        for(int i=cellStart[cell];i<cellStart[cell+1];i++){
                            int p=cellItems[i];
                            if(llabs(PL[p]-la0)<=d&&llabs(PLo[p]-lo0)<=d) emitW(p,s,t,t);
                        }
                    }
                }
            }
        }
    }
    // scheduling
    vector<map<int,Shot>> sched(S);
    vector<vector<int>> pShots(NP);
    vector<int> order;
    for(int c=0;c<C;c++) if(cV[c]>0&&!cP[c].empty()) order.push_back(c);
    sort(order.begin(),order.end(),[&](int a,int b){
        double x=(double)cV[a]/cP[a].size(), y=(double)cV[b]/cP[b].size();
        if(x!=y) return x>y; return a<b;});
    long long budget=1200000000LL, ops=0;
    auto inRanges=[&](const vector<pair<int,int>>&R,int t){
        int lo=0,hi=R.size();
        while(lo<hi){int m=(lo+hi)/2; if(R[m].second<t) lo=m+1; else hi=m;}
        return lo<(int)R.size()&&R[lo].first<=t;
    };
    auto scan=[&](int s,int p,int x,int y)->int{
        auto &M=sched[s]; const Sat&st=sats[s];
        const int* PLa=&posLat[(size_t)s*T]; const int* PLn=&posLon[(size_t)s*T];
        auto it=M.lower_bound(x);
        int pt=0,px=0,py=0;
        if(it!=M.begin()){auto q=prev(it);pt=q->first;px=q->second.ox;py=q->second.oy;}
        for(int t=x;t<=y;t++){
            ops++;
            while(it!=M.end()&&it->first<t){pt=it->first;px=it->second.ox;py=it->second.oy;++it;}
            if(it!=M.end()&&it->first==t) continue;
            int ox=PL[p]-PLa[t], oy=PLo[p]-PLn[t];
            if(abs(ox)>st.d||abs(oy)>st.d) continue;
            long long lim=(long long)st.w*(t-pt);
            if(llabs(ox-px)>lim||llabs(oy-py)>lim) continue;
            if(it!=M.end()){
                long long lim2=(long long)st.w*(it->first-t);
                if(llabs(ox-it->second.ox)>lim2||llabs(oy-it->second.oy)>lim2) continue;
            }
            return t;
        }
        return -1;
    };
    for(int c:order){
        if(ops>budget) break;
        auto &R=cR[c];
        vector<pair<long long,int>> need;
        bool fail=false;
        for(int p:cP[c]){
            bool sat=false;
            for(int t:pShots[p]) if(inRanges(R,t)){sat=true;break;}
            if(sat) continue;
            long long cnt=0;
            for(auto&w:pw[p]){
                for(auto&r:R){int x=max(w.a,r.first),y=min(w.b,r.second); if(x<=y) cnt+=y-x+1;}
            }
            ops+= pw[p].size();
            if(cnt==0){fail=true;break;}
            need.push_back({cnt,p});
        }
        if(fail) continue;
        sort(need.begin(),need.end());
        vector<pair<int,int>> ins; // sat, turn
        for(auto&np:need){
            int p=np.second; bool ok=false;
            for(auto&w:pw[p]){
                for(auto&r:R){
                    int x=max(w.a,r.first),y=min(w.b,r.second);
                    if(x>y) continue;
                    int t=scan(w.s,p,x,y);
                    if(t>=0){
                        int ox=PL[p]-posLat[(size_t)w.s*T+t], oy=PLo[p]-posLon[(size_t)w.s*T+t];
                        sched[w.s][t]={ox,oy,p};
                        pShots[p].push_back(t);
                        ins.push_back({w.s,t});
                        ok=true;break;
                    }
                }
                if(ok) break;
            }
            if(!ok){fail=true;break;}
        }
        if(fail){
            for(auto&q:ins){
                int p=sched[q.first][q.second].p;
                sched[q.first].erase(q.second);
                auto &v=pShots[p];
                for(size_t i=0;i<v.size();i++) if(v[i]==q.second){v.erase(v.begin()+i);break;}
            }
        }
    }
    // output
    size_t P=0; for(auto&m:sched) P+=m.size();
    string out; out.reserve(P*32+16);
    out+=to_string(P); out+='\n';
    for(int s=0;s<S;s++) for(auto&kv:sched[s]){
        int p=kv.second.p;
        out+=to_string(PL[p]);out+=' ';out+=to_string(PLo[p]);out+=' ';
        out+=to_string(kv.first);out+=' ';out+=to_string(s);out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
