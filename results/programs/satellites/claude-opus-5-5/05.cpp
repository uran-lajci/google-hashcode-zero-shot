#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline long long readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } bool neg=false; if(c=='-'){neg=true;c=gc();} long long x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return neg?-x:x; }

int T,S,C;
vector<int> sLat,sLon,sV,sW,sD;
vector<int> cV,cL; vector<int> rStart,rOff; vector<int> rs,re;
int NL; vector<int> lLat,lLon; vector<int> pOff,pCol; // pairs per location CSR
vector<int> lTmin,lTmax;
int G,nLat,nLon; vector<int> cellOff,cellLoc;
vector<char> dropped;
vector<int> remC; vector<char> taken; vector<int> locActive;
struct Photo{int lat,lon,t,s;};

inline bool inRange(int c,int t){
    int a=rOff[c],b=rOff[c+1];
    // last range with start<=t
    int lo=a,hi=b-1,res=-1;
    while(lo<=hi){int m=(lo+hi)>>1; if(rs[m]<=t){res=m;lo=m+1;} else hi=m-1;}
    return res>=0 && re[res]>=t;
}
inline int wrapLon(int x){ while(x<-648000)x+=1296000; while(x>=648000)x-=1296000; return x; }

vector<Photo> runPass(){
    vector<Photo> out;
    for(int c=0;c<C;c++) remC[c]=cL[c];
    fill(taken.begin(),taken.end(),0);
    long long totalActive=0;
    for(int u=0;u<NL;u++){ int cnt=0; for(int p=pOff[u];p<pOff[u+1];p++) if(!dropped[pCol[p]]) cnt++; locActive[u]=cnt; totalActive+=cnt; }
    for(int s=0;s<S && totalActive>0;s++){
        long long lat=sLat[s],lon=sLon[s],v=sV[s]; int w=sW[s],d=sD[s];
        long long a0=0,b0=0; long long t0=0;
        for(int t=0;t<T;t++){
            int latLo=(int)lat-d,latHi=(int)lat+d;
            int ci1=max(0,latLo+324000)/G; int ci2=latHi+324000; if(ci2<0) ci2=-1; else ci2=min(nLat-1,ci2/G);
            if(ci1<=ci2){
                int wl1=wrapLon((int)lon-d),wl2=wrapLon((int)lon+d);
                int cj1=(wl1+648000)/G,cj2=(wl2+648000)/G;
                long long maxMove=(long long)w*(t-t0);
                int bestU=-1; double bestSc=0; int bestA=0,bestB=0;
                for(int i=ci1;i<=ci2;i++){
                    int j=cj1;
                    while(true){
                        int cell=i*nLon+j;
                        for(int k=cellOff[cell];k<cellOff[cell+1];k++){
                            int u=cellLoc[k];
                            if(locActive[u]<=0) continue;
                            if(t<lTmin[u]||t>lTmax[u]) continue;
                            int a=lLat[u]-(int)lat; if(a<-d||a>d) continue;
                            int b=lLon[u]-(int)lon; if(b<-648000)b+=1296000; else if(b>=648000)b-=1296000;
                            if(b<-d||b>d) continue;
                            long long da=llabs(a-a0),db=llabs(b-b0);
                            if(da>maxMove||db>maxMove) continue;
                            double ws=0;
                            for(int p=pOff[u];p<pOff[u+1];p++){
                                if(taken[p]) continue; int c=pCol[p]; if(dropped[c]) continue;
                                if(inRange(c,t)) ws+=(double)cV[c]/remC[c];
                            }
                            if(ws<=0) continue;
                            double mv=(double)max(da,db);
                            double sc=ws*(1.0-0.2*mv/max(1.0,2.0*d));
                            if(sc>bestSc){bestSc=sc;bestU=u;bestA=a;bestB=b;}
                        }
                        if(j==cj2) break;
                        j=(j+1)%nLon;
                    }
                }
                if(bestU>=0){
                    int u=bestU;
                    out.push_back({lLat[u],lLon[u],t,s});
                    for(int p=pOff[u];p<pOff[u+1];p++){
                        if(taken[p]) continue; int c=pCol[p]; if(dropped[c]) continue;
                        if(inRange(c,t)){taken[p]=1;remC[c]--;locActive[u]--;totalActive--;}
                    }
                    a0=bestA;b0=bestB;t0=t;
                }
            }
            // step
            long long nl=lat+v; long long nlo=lon-15; bool flip=false;
            if(nl>324000){ nl=648000-nl; flip=true; }
            else if(nl<-324000){ nl=-648000-nl; flip=true; }
            if(flip){ nlo-=648000; v=-v; }
            while(nlo<-648000) nlo+=1296000; while(nlo>=648000) nlo-=1296000;
            lat=nl;lon=nlo;
        }
    }
    return out;
}

long long scoreOf(const vector<Photo>& ph, unordered_map<long long,int>& idmap){
    vector<char> cov(pCol.size(),0);
    for(auto&q:ph){
        long long key=(long long)q.lat*2000000LL+q.lon;
        auto it=idmap.find(key); if(it==idmap.end()) continue; int u=it->second;
        for(int p=pOff[u];p<pOff[u+1];p++) if(!cov[p]&&inRange(pCol[p],q.t)) cov[p]=1;
    }
    vector<int> cnt(C,0);
    for(int u=0;u<NL;u++) for(int p=pOff[u];p<pOff[u+1];p++) if(cov[p]) cnt[pCol[p]]++;
    long long sc=0; for(int c=0;c<C;c++) if(cnt[c]==cL[c]) sc+=cV[c];
    return sc;
}

int main(){
    auto st=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-st).count();};
    T=readInt(); S=readInt();
    sLat.resize(S);sLon.resize(S);sV.resize(S);sW.resize(S);sD.resize(S);
    int dmax=0;
    for(int i=0;i<S;i++){sLat[i]=readInt();sLon[i]=readInt();sV[i]=readInt();sW[i]=readInt();sD[i]=readInt();dmax=max(dmax,sD[i]);}
    C=readInt();
    cV.resize(C);cL.assign(C,0);rOff.assign(C+1,0);
    unordered_map<long long,int> idmap; idmap.reserve(1<<20);
    vector<vector<int>> locCols;
    for(int c=0;c<C;c++){
        cV[c]=readInt(); int L=readInt(),R=readInt();
        for(int i=0;i<L;i++){
            int la=readInt(),lo=readInt();
            long long key=(long long)la*2000000LL+lo;
            auto it=idmap.find(key); int u;
            if(it==idmap.end()){u=lLat.size();idmap[key]=u;lLat.push_back(la);lLon.push_back(lo);locCols.emplace_back();}
            else u=it->second;
            if(!locCols[u].empty()&&locCols[u].back()==c) continue;
            locCols[u].push_back(c); cL[c]++;
        }
        vector<pair<int,int>> rr(R);
        for(int i=0;i<R;i++){rr[i].first=readInt();rr[i].second=readInt();}
        sort(rr.begin(),rr.end());
        for(auto&x:rr){rs.push_back(x.first);re.push_back(x.second);}
        rOff[c+1]=rs.size();
    }
    NL=lLat.size();
    pOff.assign(NL+1,0);
    for(int u=0;u<NL;u++){pOff[u+1]=pOff[u]+locCols[u].size(); for(int c:locCols[u]) pCol.push_back(c);}
    lTmin.assign(NL,INT_MAX);lTmax.assign(NL,-1);
    for(int u=0;u<NL;u++) for(int c:locCols[u]){ if(rOff[c+1]>rOff[c]){ lTmin[u]=min(lTmin[u],rs[rOff[c]]); for(int k=rOff[c];k<rOff[c+1];k++) lTmax[u]=max(lTmax[u],re[k]); } }
    locCols.clear(); locCols.shrink_to_fit();
    G=max(dmax,500);
    nLat=648000/G+1; nLon=(1296000+G-1)/G;
    cellOff.assign(nLat*nLon+1,0);
    vector<int> cellOf(NL);
    for(int u=0;u<NL;u++){int i=(lLat[u]+324000)/G; int j=(lLon[u]+648000)/G; cellOf[u]=i*nLon+j; cellOff[cellOf[u]+1]++;}
    for(int k=0;k<nLat*nLon;k++) cellOff[k+1]+=cellOff[k];
    cellLoc.resize(NL); { vector<int> pos(cellOff.begin(),cellOff.end()-1); for(int u=0;u<NL;u++) cellLoc[pos[cellOf[u]]++]=u; }
    dropped.assign(C,0); for(int c=0;c<C;c++) if(cV[c]==0||cL[c]==0) dropped[c]=1;
    remC.assign(C,0); taken.assign(pCol.size(),0); locActive.assign(NL,0);

    vector<Photo> best; long long bestScore=-1;
    double thrs[]={0.3,0.5,0.7,0.85,1.0,1.0,1.0,1.0,1.0,1.0};
    int pass=0; double lastPassTime=0;
    while(true){
        double t1=el();
        vector<Photo> ph=runPass();
        long long sc=scoreOf(ph,idmap);
        if(sc>bestScore){bestScore=sc;best=ph;}
        lastPassTime=el()-t1;
        // drop
        double thr=thrs[min(pass,9)];
        bool any=false;
        for(int c=0;c<C;c++) if(!dropped[c]&&remC[c]>0){
            double f=(double)(cL[c]-remC[c])/cL[c];
            if(f<thr){dropped[c]=1;any=true;}
        }
        pass++;
        if(!any){
            bool inc=false;
            for(int c=0;c<C;c++) if(!dropped[c]&&remC[c]>0){dropped[c]=1;inc=true;}
            if(!inc) break;
        }
        if(el()+lastPassTime*1.3+0.3>8.0) break;
    }
    string o; o.reserve(best.size()*32+16);
    o+=to_string(best.size()); o+='\n';
    for(auto&q:best){o+=to_string(q.lat);o+=' ';o+=to_string(q.lon);o+=' ';o+=to_string(q.t);o+=' ';o+=to_string(q.s);o+='\n';}
    fwrite(o.data(),1,o.size(),stdout);
    return 0;
}
