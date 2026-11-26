#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static inline ll floorDiv(ll a, ll b){ ll q=a/b; if((a%b!=0)&&((a<0)!=(b<0))) q--; return q;}
static inline ll ceilDiv(ll a, ll b){ return -floorDiv(-a,b);}

static char* buf; static size_t bufLen, bufPos=0;
static bool readInt(ll &x){
    while(bufPos<bufLen && buf[bufPos]!='-' && (buf[bufPos]<'0'||buf[bufPos]>'9')) bufPos++;
    if(bufPos>=bufLen) return false;
    bool neg=false; if(buf[bufPos]=='-'){neg=true;bufPos++;}
    ll v=0; while(bufPos<bufLen && buf[bufPos]>='0'&&buf[bufPos]<='9'){v=v*10+(buf[bufPos]-'0');bufPos++;}
    x=neg?-v:v; return true;
}

struct Win{int loc,a,b,nphi,nlam,v;};
struct Sat{int phi,lam,v,w,d;};
struct Photo{int loc,t,s;};

int main(){
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    {
        vector<char> in; char tmp[1<<16]; size_t r;
        while((r=fread(tmp,1,sizeof(tmp),stdin))>0) in.insert(in.end(),tmp,tmp+r);
        bufLen=in.size(); buf=(char*)malloc(bufLen+1); if(bufLen) memcpy(buf,in.data(),bufLen);
    }
    ll x; readInt(x); int T=(int)x; readInt(x); int S=(int)x;
    vector<Sat> sats(S);
    int maxD=0;
    for(int i=0;i<S;i++){ll a,b,c,d,e;readInt(a);readInt(b);readInt(c);readInt(d);readInt(e);
        sats[i]={(int)a,(int)b,(int)c,(int)d,(int)e}; maxD=max(maxD,(int)e);}
    ll Cl=0; readInt(Cl); int C=(int)Cl;
    vector<int> locPhi, locLam; unordered_map<ll,int> locMap;
    vector<int> V(C), Leff(C);
    vector<vector<int>> colLocs(C);
    vector<vector<pair<int,int>>> colR(C);
    for(int c=0;c<C;c++){
        ll v,L,R; readInt(v);readInt(L);readInt(R); V[c]=(int)v;
        vector<int> ids;
        for(int i=0;i<L;i++){ll p,l;readInt(p);readInt(l);
            ll key=(p+400000)*2000000LL+(l+700000);
            auto it=locMap.find(key); int id;
            if(it==locMap.end()){id=locPhi.size();locMap[key]=id;locPhi.push_back((int)p);locLam.push_back((int)l);}
            else id=it->second;
            ids.push_back(id);}
        sort(ids.begin(),ids.end()); ids.erase(unique(ids.begin(),ids.end()),ids.end());
        colLocs[c]=ids; Leff[c]=ids.size();
        for(int i=0;i<R;i++){ll a,b;readInt(a);readInt(b); colR[c].push_back({(int)a,(int)b});}
        sort(colR[c].begin(),colR[c].end());
    }
    int nLoc=locPhi.size();
    vector<char> dead(C,0);
    for(int c=0;c<C;c++) if(V[c]<=0) dead[c]=1;
    // entries
    vector<vector<pair<int,int>>> locEnt(nLoc);
    int nEnt=0;
    vector<int> entStart(C+1,0);
    for(int c=0;c<C;c++){ entStart[c]=nEnt; for(int l:colLocs[c]){locEnt[l].push_back({c,nEnt});nEnt++;} }
    entStart[C]=nEnt;
    // union ranges per loc
    vector<vector<pair<int,int>>> locU(nLoc);
    for(int l=0;l<nLoc;l++){
        vector<pair<int,int>> all;
        for(auto &pe:locEnt[l]) if(!dead[pe.first]) for(auto &r:colR[pe.first]) all.push_back(r);
        sort(all.begin(),all.end());
        vector<pair<int,int>> m;
        for(auto &r:all){ if(!m.empty() && r.first<=m.back().second+1) m.back().second=max(m.back().second,r.second); else m.push_back(r);}
        locU[l]=m;
    }
    // grid
    int G=max(maxD,3600);
    int nLat=max(1,648000/G), nLon=max(1,1296000/G);
    auto cLat=[&](int p){int i=(p+324000)/G; if(i>=nLat)i=nLat-1; if(i<0)i=0; return i;};
    auto cLon=[&](int l){int i=(l+648000)/G; if(i>=nLon)i=nLon-1; if(i<0)i=0; return i;};
    vector<int> cellStart(nLat*nLon+1,0), cellItems(nLoc);
    for(int l=0;l<nLoc;l++) if(!locU[l].empty()) cellStart[cLat(locPhi[l])*nLon+cLon(locLam[l])+1]++;
    for(int i=0;i<nLat*nLon;i++) cellStart[i+1]+=cellStart[i];
    {vector<int> pos(cellStart.begin(),cellStart.end()-1);
     for(int l=0;l<nLoc;l++) if(!locU[l].empty()) cellItems[pos[cLat(locPhi[l])*nLon+cLon(locLam[l])]++]=l;}
    // windows
    vector<vector<Win>> wins(S);
    vector<int> stamp(nLoc,-1), lastEnd(nLoc,0), winIdx(nLoc,0);
    vector<Win> raw;
    for(int s=0;s<S;s++){
        raw.clear();
        int d=sats[s].d;
        int la=sats[s].phi, lo=sats[s].lam, ve=sats[s].v;
        int t=0;
        while(t<T){
            int g0=t, ga=la, gl=lo, gv=ve; int ci=cLat(la), cj=cLon(lo);
            int t1=t;
            while(true){
                if(t1+1>=T){t=T;break;}
                int nl=la+ve; bool fl=false;
                if(nl>324000){la=648000-nl; lo=lo-15-648000; ve=-ve; fl=true;}
                else if(nl<-324000){la=-648000-nl; lo=lo-15-648000; ve=-ve; fl=true;}
                else {la=nl; lo-=15;}
                while(lo<-648000) lo+=1296000; while(lo>647999) lo-=1296000;
                if(fl||cLat(la)!=ci||cLon(lo)!=cj){t=t1+1;break;}
                t1++;
            }
            int len=t1-g0;
            int nbL[3],nbn=0;
            for(int dj=-1;dj<=1;dj++){int j=((cj+dj)%nLon+nLon)%nLon; bool dup=false; for(int q=0;q<nbn;q++) if(nbL[q]==j) dup=true; if(!dup) nbL[nbn++]=j;}
            for(int di=-1;di<=1;di++){int i=ci+di; if(i<0||i>=nLat) continue;
                for(int q=0;q<nbn;q++){int cell=i*nLon+nbL[q];
                    for(int k=cellStart[cell];k<cellStart[cell+1];k++){
                        int L=cellItems[k];
                        ll pL=locPhi[L], lL=locLam[L];
                        ll lo1=pL-d-ga, hi1=pL+d-ga; ll klo,khi;
                        if(gv>0){klo=ceilDiv(lo1,gv);khi=floorDiv(hi1,gv);}
                        else {ll u=-gv; klo=ceilDiv(-hi1,u); khi=floorDiv(-lo1,u);}
                        ll del=lL-gl; while(del<-648000)del+=1296000; while(del>=648000)del-=1296000;
                        klo=max(klo,ceilDiv(-d-del,15)); khi=min(khi,floorDiv(d-del,15));
                        klo=max(klo,0LL); khi=min(khi,(ll)len);
                        if(klo>khi) continue;
                        int a=g0+klo, b=g0+khi;
                        if(stamp[L]==s && lastEnd[L]==a-1){raw[winIdx[L]].b=b; lastEnd[L]=b;}
                        else{
                            Win w; w.loc=L; w.a=a; w.b=b; w.nphi=(int)(pL-(ga+(ll)gv*klo)); w.nlam=(int)(del+15*klo); w.v=gv;
                            stamp[L]=s; lastEnd[L]=b; winIdx[L]=raw.size(); raw.push_back(w);
                        }
                    }
                }
            }
        }
        for(auto &w:raw){
            auto &U=locU[w.loc];
            int lo2=0,hi2=U.size();
            while(lo2<hi2){int m=(lo2+hi2)/2; if(U[m].second<w.a) lo2=m+1; else hi2=m;}
            for(int i=lo2;i<(int)U.size() && U[i].first<=w.b;i++){
                int a=max(w.a,U[i].first), b=min(w.b,U[i].second);
                if(a>b) continue;
                Win p=w; p.a=a; p.b=b; ll off=a-w.a;
                p.nphi=(int)(w.nphi-(ll)w.v*off); p.nlam=(int)(w.nlam+15*off);
                wins[s].push_back(p);
            }
        }
        sort(wins[s].begin(),wins[s].end(),[](const Win&A,const Win&B){if(A.a!=B.a)return A.a<B.a;return A.loc<B.loc;});
    }
    // precheck dead
    {
        vector<vector<pair<int,int>>> lw(nLoc);
        for(int s=0;s<S;s++) for(auto &w:wins[s]) lw[w.loc].push_back({w.a,w.b});
        vector<char> ok(nEnt,0);
        for(int l=0;l<nLoc;l++){
            auto &v=lw[l]; if(v.empty()) continue;
            sort(v.begin(),v.end());
            vector<pair<int,int>> m;
            for(auto &r:v){ if(!m.empty()&&r.first<=m.back().second+1) m.back().second=max(m.back().second,r.second); else m.push_back(r);}
            for(auto &pe:locEnt[l]){ int c=pe.first; if(dead[c]) continue;
                auto &R=colR[c]; size_t i=0,j=0; bool f=false;
                while(i<m.size()&&j<R.size()){
                    if(max(m[i].first,R[j].first)<=min(m[i].second,R[j].second)){f=true;break;}
                    if(m[i].second<R[j].second) i++; else j++;
                }
                if(f) ok[pe.second]=1;
            }
        }
        for(int c=0;c<C;c++) if(!dead[c]){ for(int e=entStart[c];e<entStart[c+1];e++) if(!ok[e]){dead[c]=1;break;} }
    }
    auto inRange=[&](int c,int t)->bool{
        auto &R=colR[c];
        int lo2=0,hi2=R.size();
        while(lo2<hi2){int m=(lo2+hi2)/2; if(R[m].first<=t) lo2=m+1; else hi2=m;}
        if(lo2==0) return false; return R[lo2-1].second>=t;
    };
    vector<double> mult(C,1.0);
    vector<char> covered(nEnt);
    vector<int> remain(C), locRemain(nLoc);
    vector<Photo> best; ll bestScore=-1;
    const int H=400; const double K0=20;
    struct St{int t,cur,ophi,olam; size_t ptr; vector<int> active; };
    double lastPass=0; int pass=0;
    while(true){
        double st0=elapsed();
        if(pass>0 && st0+lastPass*1.15>9.3) break;
        if(pass>=30) break;
        fill(covered.begin(),covered.end(),0);
        for(int c=0;c<C;c++) remain[c]=Leff[c];
        for(int l=0;l<nLoc;l++){int k=0; for(auto&pe:locEnt[l]) if(!dead[pe.first]) k++; locRemain[l]=k;}
        vector<Photo> photos;
        vector<St> ss(S);
        priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
        for(int s=0;s<S;s++){ss[s].t=0;ss[s].cur=0;ss[s].ophi=0;ss[s].olam=0;ss[s].ptr=0; pq.push({0,s});}
        bool aborted=false; ll iter=0;
        while(!pq.empty()){
            if(((++iter)&1023)==0 && elapsed()>9.5){aborted=true;break;}
            int s=pq.top().second; pq.pop();
            St &S_=ss[s]; auto &W=wins[s]; int w=sats[s].w;
            if(S_.cur>=T-1) continue;
            ll hEnd=(ll)S_.cur+H;
            if(S_.active.empty()){
                if(S_.ptr>=W.size()) continue;
                if(W[S_.ptr].a>hEnd){S_.cur=W[S_.ptr].a-1; hEnd=(ll)S_.cur+H;}
            }
            while(S_.ptr<W.size() && W[S_.ptr].a<=hEnd) S_.active.push_back(S_.ptr++);
            if(hEnd>T-1) hEnd=T-1;
            double bestSc=-1; int bestI=-1, bestT=0;
            auto &act=S_.active;
            for(size_t i=0;i<act.size();){
                const Win &wn=W[act[i]];
                if(wn.b<=S_.cur || locRemain[wn.loc]==0){act[i]=act.back();act.pop_back();continue;}
                ll lo2=max((ll)wn.a,(ll)S_.cur+1), hi2=min((ll)wn.b,hEnd);
                if(lo2<=hi2){
                    ll kl=lo2-S_.t, kh=hi2-S_.t; bool okk=true;
                    ll A1=wn.nphi-(ll)wn.v*(S_.t-wn.a)-S_.ophi, B1=-wn.v;
                    ll A2=wn.nlam+15LL*(S_.t-wn.a)-S_.olam, B2=15;
                    auto ap=[&](ll c,ll e){ if(c>0) kl=max(kl,ceilDiv(e,c)); else if(c==0){if(e>0) okk=false;} else kh=min(kh,floorDiv(-e,-c)); };
                    ap(w-B1,A1); ap(w+B1,-A1); ap(w-B2,A2); ap(w+B2,-A2);
                    if(okk && kl<=kh){
                        int tp=S_.t+kl;
                        double wt=0;
                        for(auto &pe:locEnt[wn.loc]){int c=pe.first; if(dead[c]||covered[pe.second]||mult[c]<=0) continue;
                            if(inRange(c,tp)) wt+=V[c]*mult[c]/remain[c];}
                        if(wt>0){
                            double sc=wt/(tp-S_.cur+K0);
                            if(sc>bestSc){bestSc=sc;bestI=act[i];bestT=tp;}
                        }
                    }
                }
                i++;
            }
            if(bestI>=0){
                const Win &wn=W[bestI];
                S_.ophi=(int)(wn.nphi-(ll)wn.v*(bestT-wn.a));
                S_.olam=(int)(wn.nlam+15LL*(bestT-wn.a));
                S_.t=S_.cur=bestT;
                for(auto &pe:locEnt[wn.loc]){int c=pe.first; if(dead[c]||covered[pe.second]) continue;
                    if(inRange(c,bestT)){covered[pe.second]=1; remain[c]--; locRemain[wn.loc]--;}}
                photos.push_back({wn.loc,bestT,s});
            } else {
                S_.cur=(int)hEnd;
            }
            if(S_.cur<T-1 && !(S_.active.empty() && S_.ptr>=W.size())) pq.push({S_.cur,s});
        }
        ll score=0;
        for(int c=0;c<C;c++) if(!dead[c]&&remain[c]==0) score+=V[c];
        if(!aborted || pass==0){
            if(score>bestScore){bestScore=score; best=photos;}
        }
        lastPass=elapsed()-st0;
        if(aborted) break;
        for(int c=0;c<C;c++) if(!dead[c]){
            if(remain[c]==0) mult[c]=min(mult[c]*1.1,4.0);
            else { double f=1.0-(double)remain[c]/Leff[c]; mult[c]*=(f<0.5?0.3:0.7); }
        }
        pass++;
    }
    string out; out.reserve(best.size()*32+16);
    out+=to_string(best.size()); out+='\n';
    for(auto &p:best){ out+=to_string(locPhi[p.loc]); out+=' '; out+=to_string(locLam[p.loc]); out+=' '; out+=to_string(p.t); out+=' '; out+=to_string(p.s); out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
