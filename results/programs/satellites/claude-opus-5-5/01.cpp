#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline ll readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } int s=1; if(c=='-'){s=-1;c=gc();} ll x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return x*s; }

int T,S,C;
struct Sat{ ll phi,lam,v,w,d; };
vector<Sat> sats;
vector<ll> CV; vector<int> CL;
vector<vector<pair<int,int>>> CR;
vector<int> pPhi,pLam;
vector<int> pStart; vector<int> pEnt; // CSR: entries' collection ids
const int G=10000; int NLON, NLAT;
chrono::steady_clock::time_point st;
double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-st).count(); }

inline ll wrapL(ll x){ while(x< -648000) x+=1296000; while(x>647999) x-=1296000; return x; }
inline bool inRange(int c,int t){
    auto &r=CR[c];
    int lo=0,hi=(int)r.size()-1,pos=-1;
    while(lo<=hi){int m=(lo+hi)/2; if(r[m].first<=t){pos=m;lo=m+1;} else hi=m-1;}
    return pos>=0 && r[pos].second>=t;
}

struct Photo{ int phi,lam,t,s; };

// returns score; fills photos, collDone; aborted flag
ll runPass(const vector<char>& active, vector<Photo>& photos, vector<int>& collDone, bool& aborted, double limit){
    int NP=pPhi.size();
    int NE=pEnt.size();
    vector<char> done(NE,0);
    collDone.assign(C,0);
    vector<char> pAct(NP,0);
    vector<vector<int>> grid(NLAT*NLON);
    for(int p=0;p<NP;p++){
        bool a=false;
        for(int e=pStart[p];e<pStart[p+1];e++) if(active[pEnt[e]]){a=true;break;}
        pAct[p]=a;
        if(a){ int la=(pPhi[p]+324000)/G, lo=(pLam[p]+648000)/G; grid[la*NLON+lo].push_back(p);}
    }
    vector<ll> phi(S),lam(S),v(S),lastT(S,0),lDp(S,0),lDl(S,0);
    for(int s=0;s<S;s++){phi[s]=sats[s].phi;lam[s]=sats[s].lam;v[s]=sats[s].v;}
    photos.clear();
    aborted=false;
    for(int t=0;t<T;t++){
        if((t&1023)==0 && elapsed()>limit){aborted=true;break;}
        for(int s=0;s<S;s++){
            ll d=sats[s].d, w=sats[s].w;
            ll sp=phi[s], sl=lam[s];
            ll reach=w*(t-lastT[s]);
            int r0=(int)((max(sp-d,(ll)-324000)+324000)/G);
            int r1=(int)((min(sp+d,(ll)324000)+324000)/G);
            int c0=(int)((wrapL(sl-d)+648000)/G), c1=(int)((wrapL(sl+d)+648000)/G);
            int best=-1; double bestSc=0; ll bdp=0,bdl=0;
            for(int r=r0;r<=r1;r++){
                int c=c0;
                while(true){
                    auto &cell=grid[r*NLON+c];
                    for(size_t i=0;i<cell.size();){
                        int p=cell[i];
                        if(!pAct[p]){cell[i]=cell.back();cell.pop_back();continue;}
                        i++;
                        ll dp=pPhi[p]-sp; if(dp>d||dp<-d) continue;
                        ll dl=pLam[p]-sl; if(dl<-648000) dl+=1296000; else if(dl>=648000) dl-=1296000;
                        if(dl>d||dl<-d) continue;
                        ll m1=llabs(dp-lDp[s]), m2=llabs(dl-lDl[s]);
                        if(m1>reach||m2>reach) continue;
                        double sc=0;
                        for(int e=pStart[p];e<pStart[p+1];e++){
                            if(done[e]) continue; int cc=pEnt[e]; if(!active[cc]) continue;
                            if(!inRange(cc,t)) continue;
                            sc+=(double)CV[cc]/CL[cc]*(1.0+3.0*collDone[cc]/CL[cc])+1e-6;
                        }
                        if(sc<=0) continue;
                        sc-=1e-9*(m1+m2);
                        if(best<0||sc>bestSc){best=p;bestSc=sc;bdp=dp;bdl=dl;}
                    }
                    if(c==c1) break;
                    c++; if(c==NLON) c=0;
                }
            }
            if(best>=0){
                int p=best;
                photos.push_back({pPhi[p],pLam[p],t,s});
                lastT[s]=t; lDp[s]=bdp; lDl[s]=bdl;
                bool still=false;
                for(int e=pStart[p];e<pStart[p+1];e++){
                    if(done[e]) continue; int cc=pEnt[e]; if(!active[cc]) continue;
                    if(inRange(cc,t)){done[e]=1;collDone[cc]++;}
                    else still=true;
                }
                pAct[p]=still;
            }
            // advance
            ll np=phi[s]+v[s];
            if(np>324000){ phi[s]=648000-np; lam[s]=wrapL(lam[s]-15-648000); v[s]=-v[s]; }
            else if(np< -324000){ phi[s]=-648000-np; lam[s]=wrapL(lam[s]-15-648000); v[s]=-v[s]; }
            else { phi[s]=np; lam[s]=wrapL(lam[s]-15); }
        }
    }
    ll score=0;
    for(int c=0;c<C;c++) if(active[c]&&collDone[c]==CL[c]) score+=CV[c];
    return score;
}

int main(){
    st=chrono::steady_clock::now();
    T=readInt(); S=readInt();
    sats.resize(S);
    for(int i=0;i<S;i++){sats[i].phi=readInt();sats[i].lam=readInt();sats[i].v=readInt();sats[i].w=readInt();sats[i].d=readInt();}
    C=readInt();
    CV.resize(C);CL.resize(C);CR.resize(C);
    map<pair<int,int>,int> pid;
    vector<vector<int>> pe;
    for(int c=0;c<C;c++){
        CV[c]=readInt(); int L=readInt(); int R=readInt(); CL[c]=L;
        for(int i=0;i<L;i++){
            int a=readInt(), b=readInt();
            auto it=pid.find({a,b}); int id;
            if(it==pid.end()){id=pid.size();pid[{a,b}]=id;pPhi.push_back(a);pLam.push_back(b);pe.emplace_back();}
            else id=it->second;
            pe[id].push_back(c);
        }
        for(int i=0;i<R;i++){int a=readInt(),b=readInt();CR[c].push_back({a,b});}
        sort(CR[c].begin(),CR[c].end());
    }
    int NP=pPhi.size();
    pStart.assign(NP+1,0);
    for(int p=0;p<NP;p++){pStart[p+1]=pStart[p]+pe[p].size(); for(int c:pe[p]) pEnt.push_back(c);}
    pe.clear(); pe.shrink_to_fit();
    NLON=(1296000+G-1)/G; NLAT=648000/G+1;

    vector<char> active(C,1);
    vector<Photo> bestPh, ph; ll bestScore=-1;
    vector<int> collDone;
    double thrs[]={0.3,0.5,0.7,0.8,0.9,0.95,0.95,0.95};
    const double HARD=9.3;
    for(int pass=0;pass<8;pass++){
        double t0=elapsed();
        bool ab;
        ll sc=runPass(active,ph,collDone,ab,HARD);
        double dt=elapsed()-t0;
        if(ab){ if(bestScore<0){bestPh=ph;bestScore=0;} break; }
        if(sc>bestScore){bestScore=sc;bestPh=ph;}
        double thr=thrs[pass];
        bool changed=false;
        for(int c=0;c<C;c++) if(active[c]){
            double f=(double)collDone[c]/CL[c];
            if(collDone[c]<CL[c] && f<thr){active[c]=0;changed=true;}
        }
        if(!changed && pass>=3) break;
        if(elapsed()+dt*1.15>HARD) break;
    }
    string out; out.reserve(bestPh.size()*32+16);
    out+=to_string(bestPh.size()); out+='\n';
    for(auto &p:bestPh){
        out+=to_string(p.phi);out+=' ';out+=to_string(p.lam);out+=' ';out+=to_string(p.t);out+=' ';out+=to_string(p.s);out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
}
