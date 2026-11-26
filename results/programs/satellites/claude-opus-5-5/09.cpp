#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline bool readInt(long long &x){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return false; c=gc(); } bool neg=false; if(c=='-'){neg=true;c=gc();} x=0; while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); } if(neg)x=-x; return true; }
inline long long rd(){ long long x=0; readInt(x); return x; }

struct Sat{ int lat,lon,v,w,d; };
int T,S,C;
vector<Sat> sats;
vector<int> V;
vector<vector<int>> colPts;
vector<vector<pair<int,int>>> colRanges;
vector<int> pLat,pLon;
vector<int> pcStart, pcList; // point -> collections CSR

const int CELL=10000;
const int LATOFF=324000, LONOFF=648000;
int NR, NC;

struct Photo{ int lat,lon,t,s; };

inline bool inRange(int c,int t){
    const auto &r=colRanges[c];
    // r sorted by start
    int lo=0,hi=(int)r.size()-1,idx=-1;
    while(lo<=hi){ int m=(lo+hi)/2; if(r[m].first<=t){ idx=m; lo=m+1;} else hi=m-1; }
    if(idx<0) return false;
    return t<=r[idx].second;
}

long long opsCount;

long long runPass(const vector<char>& active, vector<Photo>& photos, vector<double>& frac){
    int P=pLat.size();
    vector<int> eStart(P+1,0);
    vector<int> eCol; eCol.reserve(pcList.size());
    for(int p=0;p<P;p++){
        eStart[p]=eCol.size();
        for(int k=pcStart[p];k<pcStart[p+1];k++){ int c=pcList[k]; if(active[c]) eCol.push_back(c); }
    }
    eStart[P]=eCol.size();
    vector<char> eDone(eCol.size(),0);
    vector<int> rem(C,0);
    for(int c=0;c<C;c++) if(active[c]) rem[c]=colPts[c].size();
    vector<int> alive(P,0);
    vector<vector<int>> cells(NR*NC);
    vector<int> posIn(P,-1);
    for(int p=0;p<P;p++){
        alive[p]=eStart[p+1]-eStart[p];
        if(alive[p]>0){
            int r=(pLat[p]+LATOFF)/CELL, cc=(pLon[p]+LONOFF)/CELL;
            int id=r*NC+cc; posIn[p]=cells[id].size(); cells[id].push_back(p);
        }
    }
    photos.clear();
    long long scans=0;
    for(int s=0;s<S;s++){
        int lat=sats[s].lat, lon=sats[s].lon, v=sats[s].v;
        long long w=sats[s].w; int d=sats[s].d;
        int lastT=0; int oLat=0,oLon=0;
        for(int t=0;t<T;t++){
            int a0=lat-d, a1=lat+d;
            if(a0<-306000) a0=-306000;
            if(a1>306000) a1=306000;
            if(a0<=a1){
                int b0=lon-d,b1=lon+d;
                if(b0<-648000) b0=-648000;
                if(b1>647999) b1=647999;
                int r0=(a0+LATOFF)/CELL, r1=(a1+LATOFF)/CELL;
                int c0=(b0+LONOFF)/CELL, c1=(b1+LONOFF)/CELL;
                long long reach=w*(long long)(t-lastT);
                int best=-1; double bestSc=0; long long bestMove=0;
                for(int r=r0;r<=r1;r++) for(int cc=c0;cc<=c1;cc++){
                    const vector<int>& cell=cells[r*NC+cc];
                    for(int p:cell){
                        scans++;
                        int dl=pLat[p]-lat; if(dl<-d||dl>d) continue;
                        int dn=pLon[p]-lon; if(dn<-d||dn>d) continue;
                        long long m1=llabs((long long)dl-oLat), m2=llabs((long long)dn-oLon);
                        if(m1>reach||m2>reach) continue;
                        double sc=0;
                        for(int e=eStart[p];e<eStart[p+1];e++){
                            if(eDone[e]) continue;
                            int c=eCol[e];
                            if(inRange(c,t)) sc+=(double)V[c]/rem[c];
                        }
                        if(sc<=0) continue;
                        long long mv=max(m1,m2);
                        if(best<0||sc>bestSc+1e-12||(fabs(sc-bestSc)<=1e-12&&mv<bestMove)){
                            best=p; bestSc=sc; bestMove=mv;
                        }
                    }
                }
                if(best>=0){
                    int p=best;
                    for(int e=eStart[p];e<eStart[p+1];e++){
                        if(eDone[e]) continue;
                        int c=eCol[e];
                        if(inRange(c,t)){ eDone[e]=1; rem[c]--; alive[p]--; }
                    }
                    photos.push_back({pLat[p],pLon[p],t,s});
                    oLat=pLat[p]-lat; oLon=pLon[p]-lon; lastT=t;
                    if(alive[p]==0){
                        int r=(pLat[p]+LATOFF)/CELL, cc=(pLon[p]+LONOFF)/CELL;
                        vector<int>& cell=cells[r*NC+cc];
                        int pos=posIn[p]; int lastp=cell.back();
                        cell[pos]=lastp; posIn[lastp]=pos; cell.pop_back(); posIn[p]=-1;
                    }
                }
            }
            // step
            int nl=lat+v;
            if(nl>324000){ nl=648000-nl; lon=-648000+(lon-15); v=-v; }
            else if(nl<-324000){ nl=-648000-nl; lon=-648000+(lon-15); v=-v; }
            else lon-=15;
            while(lon<-648000) lon+=1296000;
            while(lon>647999) lon-=1296000;
            lat=nl;
        }
    }
    opsCount=(long long)S*T+scans;
    long long score=0;
    frac.assign(C,0);
    for(int c=0;c<C;c++) if(active[c]){
        int tot=colPts[c].size();
        frac[c]=tot?(double)(tot-rem[c])/tot:1.0;
        if(rem[c]==0) score+=V[c];
    }
    return score;
}

int main(){
    T=rd(); S=rd();
    sats.resize(S);
    for(int i=0;i<S;i++){ sats[i].lat=rd(); sats[i].lon=rd(); sats[i].v=rd(); sats[i].w=rd(); sats[i].d=rd(); }
    C=rd();
    V.resize(C); colPts.resize(C); colRanges.resize(C);
    unordered_map<long long,int> pid; pid.reserve(1<<20);
    for(int c=0;c<C;c++){
        V[c]=rd(); int L=rd(), R=rd();
        for(int i=0;i<L;i++){
            int a=rd(), b=rd();
            long long key=(long long)(a+400000)*2000000LL+(b+700000);
            auto it=pid.find(key); int id;
            if(it==pid.end()){ id=pLat.size(); pid[key]=id; pLat.push_back(a); pLon.push_back(b); }
            else id=it->second;
            colPts[c].push_back(id);
        }
        sort(colPts[c].begin(),colPts[c].end());
        colPts[c].erase(unique(colPts[c].begin(),colPts[c].end()),colPts[c].end());
        for(int i=0;i<R;i++){ int a=rd(), b=rd(); colRanges[c].push_back({a,b}); }
        sort(colRanges[c].begin(),colRanges[c].end());
    }
    int P=pLat.size();
    pcStart.assign(P+1,0);
    for(int c=0;c<C;c++) for(int p:colPts[c]) pcStart[p+1]++;
    for(int p=0;p<P;p++) pcStart[p+1]+=pcStart[p];
    pcList.resize(pcStart[P]);
    { vector<int> fill(pcStart.begin(),pcStart.end()-1);
      for(int c=0;c<C;c++) for(int p:colPts[c]) pcList[fill[p]++]=c; }
    NR=(648000/CELL)+2; NC=(1296000/CELL)+2;

    vector<char> active(C,0);
    for(int c=0;c<C;c++) active[c]=(V[c]>0 && !colPts[c].empty());
    vector<Photo> bestPhotos, photos;
    long long bestScore=-1;
    vector<double> frac;
    long long sc=runPass(active,photos,frac);
    bestScore=sc; bestPhotos=photos;
    long long ops1=max(1LL,opsCount);
    long long budget=600000000LL;
    int maxPasses=(int)min<long long>(10,budget/ops1);
    for(int pass=1;pass<maxPasses;pass++){
        vector<pair<double,int>> inc;
        for(int c=0;c<C;c++) if(active[c] && frac[c]<1.0) inc.push_back({frac[c],c});
        if(inc.empty()) break;
        sort(inc.begin(),inc.end());
        size_t drop=(inc.size()+1)/2;
        for(size_t i=0;i<drop;i++) active[inc[i].second]=0;
        sc=runPass(active,photos,frac);
        if(sc>bestScore){ bestScore=sc; bestPhotos=photos; }
    }
    string out;
    out.reserve(bestPhotos.size()*32+16);
    out+=to_string(bestPhotos.size()); out+='\n';
    for(auto &ph:bestPhotos){
        out+=to_string(ph.lat); out+=' ';
        out+=to_string(ph.lon); out+=' ';
        out+=to_string(ph.t); out+=' ';
        out+=to_string(ph.s); out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
