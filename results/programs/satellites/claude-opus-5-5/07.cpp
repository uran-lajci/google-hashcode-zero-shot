#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Sat{ll lat,lon,v,w,d;};
int T,S,C;
vector<Sat> sats;
vector<int> colV,colL;
vector<vector<pair<int,int>>> colR;
vector<vector<int>> colPts;
vector<int> plat,plon,pEnd;
vector<vector<pair<int,int>>> pent; // (col, entryId)
int nEnt=0;
const int G=2500; int nLatC,nLonC;
vector<int> cellStart,cellPts;
chrono::steady_clock::time_point st;
const double DEADLINE=9.0;
double el(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

inline bool active(int c,int t){
    auto&r=colR[c];
    int lo=0,hi=(int)r.size()-1,idx=-1;
    while(lo<=hi){int m=(lo+hi)/2; if(r[m].first<=t){idx=m;lo=m+1;}else hi=m-1;}
    return idx>=0&&r[idx].second>=t;
}
struct Photo{int lat,lon,t,s;};
struct Result{vector<Photo> ph; vector<int> satCnt;};

Result run(const vector<char>&sel){
    Result res; res.satCnt.assign(C,0);
    vector<char> es(nEnt,0); vector<int> rem(colL); vector<int> alive(plat.size(),0);
    for(size_t p=0;p<plat.size();p++) for(auto&e:pent[p]) if(sel[e.first]) alive[p]++;
    struct St{ll lat,lon,v,ol,on,t0;int c0,c1,c2,c3;vector<int> cand;};
    vector<St> ss(S);
    for(int s=0;s<S;s++){ss[s].lat=sats[s].lat;ss[s].lon=sats[s].lon;ss[s].v=sats[s].v;ss[s].ol=0;ss[s].on=0;ss[s].t0=0;ss[s].c0=-5;ss[s].c1=-5;ss[s].c2=-5;ss[s].c3=-5;}
    for(int t=0;t<T;t++){
        if((t&63)==0&&el()>DEADLINE)break;
        for(int s=0;s<S;s++){
            St&x=ss[s]; ll d=sats[s].d,w=sats[s].w;
            ll a=x.lat-d+306000,b=x.lat+d+306000;
            int l0=a<0?0:(int)(a/G), l1=b<0?-1:(int)min<ll>(nLatC-1,b/G);
            ll c=x.lon-d+648000,e=x.lon+d+648000;
            int m0=c<0?0:(int)(c/G), m1=e<0?-1:(int)min<ll>(nLonC-1,e/G);
            if(l0!=x.c0||l1!=x.c1||m0!=x.c2||m1!=x.c3){
                x.c0=l0;x.c1=l1;x.c2=m0;x.c3=m1;x.cand.clear();
                for(int i=l0;i<=l1;i++)for(int j=m0;j<=m1;j++){
                    int cell=i*nLonC+j;
                    for(int k=cellStart[cell];k<cellStart[cell+1];k++){int p=cellPts[k]; if(alive[p]&&pEnd[p]>=t)x.cand.push_back(p);}
                }
            }
            if(!x.cand.empty()){
                ll reach=w*(t-x.t0);
                int best=-1;double bs=0;ll bm=0;
                for(size_t i=0;i<x.cand.size();){
                    int p=x.cand[i];
                    if(!alive[p]||pEnd[p]<t){x.cand[i]=x.cand.back();x.cand.pop_back();continue;}
                    i++;
                    ll dl=plat[p]-x.lat; if(dl>d||dl<-d)continue;
                    ll dn=plon[p]-x.lon; if(dn>d||dn<-d)continue;
                    ll ml=llabs(dl-x.ol), mn=llabs(dn-x.on);
                    if(ml>reach||mn>reach)continue;
                    double sc=0;
                    for(auto&en:pent[p]){ if(!es[en.second]&&sel[en.first]&&active(en.first,t)) sc+=(double)colV[en.first]/rem[en.first]; }
                    if(sc<=0)continue;
                    ll mv=max(ml,mn);
                    if(sc>bs+1e-12||(fabs(sc-bs)<=1e-12&&mv<bm)){bs=sc;best=p;bm=mv;}
                }
                if(best>=0){
                    int p=best;
                    x.ol=plat[p]-x.lat; x.on=plon[p]-x.lon; x.t0=t;
                    res.ph.push_back({plat[p],plon[p],t,s});
                    for(auto&en:pent[p]){ if(!es[en.second]&&sel[en.first]&&active(en.first,t)){es[en.second]=1;rem[en.first]--;alive[p]--;res.satCnt[en.first]++;} }
                }
            }
            // step
            ll nl=x.lat+x.v, nn=x.lon-15;
            if(nl>324000){nl=648000-nl;nn-=648000;x.v=-x.v;}
            else if(nl<-324000){nl=-648000-nl;nn-=648000;x.v=-x.v;}
            while(nn<-648000)nn+=1296000; while(nn>647999)nn-=1296000;
            x.lat=nl;x.lon=nn;
        }
    }
    return res;
}

ll trueScore(const Result&r){
    vector<vector<int>> times(plat.size());
    map<pair<int,int>,int> idx;
    for(size_t p=0;p<plat.size();p++) idx[{plat[p],plon[p]}]=p;
    for(auto&ph:r.ph) times[idx[{ph.lat,ph.lon}]].push_back(ph.t);
    for(auto&v:times) sort(v.begin(),v.end());
    ll sc=0;
    for(int c=0;c<C;c++){
        bool ok=true;
        for(int p:colPts[c]){
            bool f=false;
            for(auto&rg:colR[c]){auto it=lower_bound(times[p].begin(),times[p].end(),rg.first); if(it!=times[p].end()&&*it<=rg.second){f=true;break;}}
            if(!f){ok=false;break;}
        }
        if(ok)sc+=colV[c];
    }
    return sc;
}

int main(){
    st=chrono::steady_clock::now();
    if(scanf("%d",&T)!=1)return 0;
    scanf("%d",&S); sats.resize(S);
    for(auto&s:sats)scanf("%lld %lld %lld %lld %lld",&s.lat,&s.lon,&s.v,&s.w,&s.d);
    scanf("%d",&C);
    colV.resize(C);colL.resize(C);colR.resize(C);colPts.resize(C);
    map<pair<int,int>,int> pm;
    for(int c=0;c<C;c++){
        int V,L,R;scanf("%d %d %d",&V,&L,&R);colV[c]=V;colL[c]=L;
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            auto it=pm.find({a,b});int p;
            if(it==pm.end()){p=plat.size();pm[{a,b}]=p;plat.push_back(a);plon.push_back(b);pent.emplace_back();}
            else p=it->second;
            pent[p].push_back({c,nEnt++}); colPts[c].push_back(p);
        }
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);colR[c].push_back({a,b});}
        sort(colR[c].begin(),colR[c].end());
    }
    int P=plat.size(); pEnd.assign(P,-1);
    for(int p=0;p<P;p++)for(auto&e:pent[p])pEnd[p]=max(pEnd[p],colR[e.first].back().second);
    nLatC=612000/G+1; nLonC=1296000/G+1;
    int NC=nLatC*nLonC; cellStart.assign(NC+1,0);
    vector<int> pc(P);
    for(int p=0;p<P;p++){int i=(plat[p]+306000)/G,j=(plon[p]+648000)/G; i=max(0,min(nLatC-1,i)); j=max(0,min(nLonC-1,j)); pc[p]=i*nLonC+j; cellStart[pc[p]+1]++;}
    for(int i=0;i<NC;i++)cellStart[i+1]+=cellStart[i];
    cellPts.resize(P); {vector<int> pos(cellStart.begin(),cellStart.end()-1); for(int p=0;p<P;p++)cellPts[pos[pc[p]]++]=p;}

    vector<char> sel(C);
    for(int c=0;c<C;c++)sel[c]=colV[c]>0;
    double t1=el();
    Result best=run(sel); ll bestS=trueScore(best);
    double dur=el()-t1;
    vector<double> thr={0.5,1.0,1.0,1.0};
    Result last=best;
    for(double th:thr){
        if(el()+dur*1.1>DEADLINE)break;
        vector<char> ns(C,0); bool changed=false;
        for(int c=0;c<C;c++){ns[c]=sel[c]&&colL[c]>0&&(double)last.satCnt[c]/colL[c]>=th-1e-9; if(ns[c]!=sel[c])changed=true;}
        if(!changed)continue;
        sel=ns;
        double ts=el();
        Result r=run(sel); dur=max(dur,el()-ts);
        ll sc=trueScore(r);
        if(sc>bestS){bestS=sc;best=r;}
        last=r;
    }
    string out; out.reserve(best.ph.size()*32+16);
    out+=to_string(best.ph.size())+"\n";
    for(auto&p:best.ph){out+=to_string(p.lat);out+=' ';out+=to_string(p.lon);out+=' ';out+=to_string(p.t);out+=' ';out+=to_string(p.s);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
