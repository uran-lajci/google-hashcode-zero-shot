#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static chrono::steady_clock::time_point START;
static double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-START).count();}

int T,S,C;
vector<ll> s0lat,s0lon,s0v,sw,sd;
vector<int> V; vector<int> collL;
vector<int> rOff; vector<int> rS,rE;
vector<int> locLat,locLon;
vector<int> entOff, entColl;
const int CS=10000; int NR,NCOL;
vector<int> cellOff, cellLoc;

inline bool inRange(int c,int t){
    int a=rOff[c],b=rOff[c+1];
    int i=int(upper_bound(rS.begin()+a,rS.begin()+b,t)-rS.begin())-1;
    if(i<a) return false;
    return rE[i]>=t;
}
inline bool intersects(int c,int x,int y){
    int a=rOff[c],b=rOff[c+1];
    int i=int(lower_bound(rE.begin()+a,rE.begin()+b,x)-rE.begin());
    if(i>=b) return false;
    return rS[i]<=y;
}
inline void stepSat(ll&lat,ll&lon,ll&v){
    ll nl=lat+v;
    if(nl>324000){nl=648000-nl;lon=lon-15-648000;v=-v;}
    else if(nl<-324000){nl=-648000-nl;lon=lon-15-648000;v=-v;}
    else lon-=15;
    while(lon<-648000)lon+=1296000;
    while(lon>647999)lon-=1296000;
    lat=nl;
}
struct Pic{int lat,lon,t,s;};

const int K=8;
vector<int> stampArr; int stampCtr=0;

ll runPass(const vector<char>&alive, vector<Pic>&pics, vector<int>&remOut, bool &aborted, double hardLimit){
    int NL=locLat.size(); int E=entColl.size();
    vector<char> entSat(E,0);
    vector<int> rem(C);
    for(int c=0;c<C;c++) rem[c]=alive[c]?collL[c]:0;
    vector<ll> la(s0lat),lo(s0lon),vv(s0v);
    vector<ll> lastT(S,0),lastA(S,0),lastB(S,0);
    vector<vector<int>> pla(S,vector<int>(K)),plo(S,vector<int>(K));
    vector<vector<int>> cand(S);
    pics.clear(); aborted=false;
    auto queryBox=[&](int s,ll a0,ll a1,ll b0,ll b1,int t0,int t1){
        a0=max(a0,(ll)-306000);a1=min(a1,(ll)306000);
        b0=max(b0,(ll)-648000);b1=min(b1,(ll)647999);
        if(a0>a1||b0>b1)return;
        int r0=(a0+324000)/CS,r1=(a1+324000)/CS,c0=(b0+648000)/CS,c1=(b1+648000)/CS;
        for(int r=r0;r<=r1;r++)for(int cc=c0;cc<=c1;cc++){
            int cell=r*NCOL+cc;
            for(int k=cellOff[cell];k<cellOff[cell+1];k++){
                int L=cellLoc[k];
                if(stampArr[L]==stampCtr)continue;
                if(locLat[L]<a0||locLat[L]>a1||locLon[L]<b0||locLon[L]>b1)continue;
                bool need=false;
                for(int e=entOff[L];e<entOff[L+1];e++){
                    if(entSat[e])continue;int c=entColl[e];
                    if(!alive[c])continue;
                    if(intersects(c,t0,t1-1)){need=true;break;}
                }
                stampArr[L]=stampCtr;
                if(need)cand[s].push_back(L);
            }
        }
    };
    for(int t0=0;t0<T;t0+=K){
        if(elapsed()>hardLimit){aborted=true;break;}
        int t1=min(T,t0+K); int len=t1-t0;
        for(int s=0;s<S;s++){
            ll mina=LLONG_MAX,maxa=LLONG_MIN,minb=LLONG_MAX,maxb=LLONG_MIN;
            for(int i=0;i<len;i++){
                pla[s][i]=la[s];plo[s][i]=lo[s];
                mina=min(mina,la[s]);maxa=max(maxa,la[s]);
                minb=min(minb,lo[s]);maxb=max(maxb,lo[s]);
                stepSat(la[s],lo[s],vv[s]);
            }
            cand[s].clear(); stampCtr++;
            ll d=sd[s];
            if(maxb-minb<=200000){
                queryBox(s,mina-d,maxa+d,minb-d,maxb+d,t0,t1);
            } else {
                for(int i=0;i<len;i++)
                    queryBox(s,pla[s][i]-d,pla[s][i]+d,plo[s][i]-d,plo[s][i]+d,t0,t1);
            }
        }
        for(int t=t0;t<t1;t++){
            int idx=t-t0;
            for(int s=0;s<S;s++){
                if(cand[s].empty())continue;
                ll sa=pla[s][idx],sb=plo[s][idx],d=sd[s];
                ll mv=sw[s]*(t-lastT[s]);
                if(!pics.empty()||true){}
                int best=-1;double bg=0;ll brot=0;ll ba=0,bb=0;
                for(int L:cand[s]){
                    ll da=locLat[L]-sa; if(da<-d||da>d)continue;
                    ll db=locLon[L]-sb; if(db<-d||db>d)continue;
                    ll ra=llabs(da-lastA[s]),rb=llabs(db-lastB[s]);
                    if(ra>mv||rb>mv)continue;
                    double g=0;
                    for(int e=entOff[L];e<entOff[L+1];e++){
                        if(entSat[e])continue;int c=entColl[e];
                        if(!alive[c]||rem[c]<=0)continue;
                        if(!inRange(c,t))continue;
                        g+=(double)V[c]/rem[c];
                    }
                    if(g<=0)continue;
                    ll rot=max(ra,rb);
                    if(best<0||g>bg+1e-12||(fabs(g-bg)<=1e-12&&rot<brot)){
                        best=L;bg=g;brot=rot;ba=da;bb=db;
                    }
                }
                if(best>=0){
                    int L=best;
                    for(int e=entOff[L];e<entOff[L+1];e++){
                        if(entSat[e])continue;int c=entColl[e];
                        if(!alive[c]||rem[c]<=0)continue;
                        if(!inRange(c,t))continue;
                        entSat[e]=1;rem[c]--;
                    }
                    pics.push_back({locLat[L],locLon[L],t,s});
                    lastT[s]=t;lastA[s]=ba;lastB[s]=bb;
                }
            }
        }
    }
    ll score=0;
    for(int c=0;c<C;c++) if(alive[c]&&rem[c]==0) score+=V[c];
    remOut=rem;
    return score;
}

int main(){
    START=chrono::steady_clock::now();
    if(scanf("%d",&T)!=1)return 0;
    scanf("%d",&S);
    s0lat.resize(S);s0lon.resize(S);s0v.resize(S);sw.resize(S);sd.resize(S);
    for(int i=0;i<S;i++)scanf("%lld %lld %lld %lld %lld",&s0lat[i],&s0lon[i],&s0v[i],&sw[i],&sd[i]);
    scanf("%d",&C);
    V.resize(C);collL.assign(C,0);rOff.assign(C+1,0);
    unordered_map<ll,int> mp; mp.reserve(1<<20);
    vector<pair<int,int>> ents; // loc, coll
    for(int c=0;c<C;c++){
        int L,R;scanf("%d %d %d",&V[c],&L,&R);
        collL[c]=L;
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            ll key=(ll)(a+400000)*2000000LL+(b+700000);
            auto it=mp.find(key);int id;
            if(it==mp.end()){id=locLat.size();mp[key]=id;locLat.push_back(a);locLon.push_back(b);}
            else id=it->second;
            ents.push_back({id,c});
        }
        vector<pair<int,int>> rg(R);
        for(int i=0;i<R;i++)scanf("%d %d",&rg[i].first,&rg[i].second);
        sort(rg.begin(),rg.end());
        for(auto&p:rg){rS.push_back(p.first);rE.push_back(p.second);}
        rOff[c+1]=rS.size();
    }
    int NL=locLat.size();
    entOff.assign(NL+1,0);
    for(auto&p:ents)entOff[p.first+1]++;
    for(int i=0;i<NL;i++)entOff[i+1]+=entOff[i];
    entColl.resize(ents.size());
    {vector<int> pos(entOff.begin(),entOff.end()-1);
     for(auto&p:ents)entColl[pos[p.first]++]=p.second;}
    NR=648000/CS+2;NCOL=1296000/CS+2;
    cellOff.assign(NR*NCOL+1,0);
    vector<int> cellOf(NL);
    for(int i=0;i<NL;i++){int r=(locLat[i]+324000)/CS,cc=(locLon[i]+648000)/CS;cellOf[i]=r*NCOL+cc;cellOff[cellOf[i]+1]++;}
    for(int i=0;i<NR*NCOL;i++)cellOff[i+1]+=cellOff[i];
    cellLoc.resize(NL);
    {vector<int> pos(cellOff.begin(),cellOff.end()-1);
     for(int i=0;i<NL;i++)cellLoc[pos[cellOf[i]]++]=i;}
    stampArr.assign(NL,0);

    vector<char> alive(C,1);
    for(int c=0;c<C;c++)if(V[c]<=0)alive[c]=0;
    vector<Pic> best; ll bestScore=-1;
    vector<double> th={0.5,0.75,0.9,1.0,1.0,1.0,1.0,1.0};
    double limit=8.7;
    int pass=0;
    while(true){
        double st=elapsed();
        vector<Pic> pics; vector<int> rem; bool ab=false;
        ll sc=runPass(alive,pics,rem,ab,limit);
        if(!ab||bestScore<0){
            if(sc>bestScore||bestScore<0){bestScore=sc;best=pics;}
        }
        if(ab)break;
        double dur=elapsed()-st;
        if(pass>=(int)th.size())break;
        if(elapsed()+dur*1.15>limit)break;
        double thr=th[pass];
        bool changed=false;
        for(int c=0;c<C;c++){
            if(!alive[c]||rem[c]==0)continue;
            double frac=1.0-(double)rem[c]/collL[c];
            if(frac<thr){alive[c]=0;changed=true;}
        }
        pass++;
        if(!changed){ if(thr>=1.0)break; else continue;}
    }
    string out; out.reserve(best.size()*32+16);
    out+=to_string(best.size());out+='\n';
    char buf[64];
    for(auto&p:best){int n=sprintf(buf,"%d %d %d %d\n",p.lat,p.lon,p.t,p.s);out.append(buf,n);}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
