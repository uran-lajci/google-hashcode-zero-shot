#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;

struct Proj{char t;int h,w,v,area;vector<string> g;};
struct Cand{int pid;bool res;int h,w,area,firstC,cap,t,otype;vector<int> orr,occ_;vector<int> dr,dc;};

int H,W,D,B;
vector<Cand> cands; int NC,NT;
ull ops=0;
chrono::steady_clock::time_point T0clk;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0clk).count();}
mt19937 rng(12345);
double urand(){return (rng()>>8)*(1.0/16777216.0);}

// tile
int Th,Tw; bool wrR,wrC; const int OFF=80;
vector<int> rowTab,colTab,occ,ip,ir_,ic_,kk,cnt,st,tst,freeL; vector<char> alive;
int stc=0,tsc=0; ll score=0; vector<int> nb;

void gatherNb(int p,int r,int c,int self){
    nb.clear(); ++stc; const Cand&C=cands[p];
    int n=C.dr.size(); ops+=n;
    const int* DR=C.dr.data(); const int* DC=C.dc.data();
    for(int i=0;i<n;i++){
        int rr=rowTab[r+DR[i]+OFF]; if(rr<0)continue;
        int cc=colTab[c+DC[i]+OFF]; if(cc<0)continue;
        int id=occ[rr*Tw+cc];
        if(id>=0&&id!=self&&st[id]!=stc){st[id]=stc;nb.push_back(id);}
    }
}
bool fits(int p,int r,int c){
    const Cand&C=cands[p]; int n=C.orr.size();
    for(int i=0;i<n;i++){
        ops++;
        int rr=rowTab[r+C.orr[i]+OFF]; if(rr<0)return false;
        int cc=colTab[c+C.occ_[i]+OFF]; if(cc<0)return false;
        if(occ[rr*Tw+cc]>=0)return false;
    }
    return true;
}
int placeT(int p,int r,int c){
    int id=freeL.back(); freeL.pop_back();
    const Cand&C=cands[p];
    for(size_t i=0;i<C.orr.size();i++){
        int rr=rowTab[r+C.orr[i]+OFF], cc=colTab[c+C.occ_[i]+OFF];
        occ[rr*Tw+cc]=id;
    }
    ip[id]=p;ir_[id]=r;ic_[id]=c;kk[id]=0;alive[id]=1;
    gatherNb(p,r,c,id);
    if(C.res){
        for(int u:nb){const Cand&U=cands[ip[u]]; if(U.res)continue; if(cnt[id*NT+U.t]++==0)kk[id]++;}
        score+=(ll)C.cap*kk[id];
    } else {
        for(int j:nb){const Cand&R=cands[ip[j]]; if(!R.res)continue; if(cnt[j*NT+C.t]++==0){kk[j]++;score+=R.cap;}}
    }
    return id;
}
void removeT(int id){
    int p=ip[id],r=ir_[id],c=ic_[id]; const Cand&C=cands[p];
    for(size_t i=0;i<C.orr.size();i++){
        int rr=rowTab[r+C.orr[i]+OFF], cc=colTab[c+C.occ_[i]+OFF];
        occ[rr*Tw+cc]=-1;
    }
    if(C.res){
        score-=(ll)C.cap*kk[id];
        memset(&cnt[id*NT],0,sizeof(int)*NT); kk[id]=0;
    } else {
        gatherNb(p,r,c,id);
        for(int j:nb){const Cand&R=cands[ip[j]]; if(!R.res)continue; if(--cnt[j*NT+C.t]==0){kk[j]--;score-=R.cap;}}
    }
    alive[id]=0; freeL.push_back(id);
}
double valueT(int p,int r,int c,double alpha){
    gatherNb(p,r,c,-1);
    const Cand&C=cands[p];
    if(C.res){
        ++tsc; int k=0;
        for(int id:nb){const Cand&U=cands[ip[id]]; if(U.res)continue; if(tst[U.t]!=tsc){tst[U.t]=tsc;k++;}}
        return C.cap*(k+alpha)/C.area;
    } else {
        ll s=0;
        for(int id:nb){const Cand&R=cands[ip[id]]; if(R.res&&cnt[id*NT+C.t]==0)s+=R.cap;}
        return (double)s/C.area;
    }
}
void fillCells(const vector<pair<int,int>>&cells,double alpha,double noise,vector<int>*added){
    for(auto&pr:cells){
        int r=pr.first,c=pr.second;
        if(occ[r*Tw+c]>=0)continue;
        int bp=-1,br=0,bc=0; double bv=0;
        for(int p=0;p<NC;p++){
            const Cand&C=cands[p];
            int c0=colTab[c-C.firstC+OFF]; if(c0<0)continue;
            if(!wrR&&r+C.h>Th)continue;
            if(!wrC&&c0+C.w>Tw)continue;
            if(!fits(p,r,c0))continue;
            double v=valueT(p,r,c0,alpha);
            if(noise>0)v*=1+noise*urand();
            if(v>bv){bv=v;bp=p;br=r;bc=c0;}
        }
        if(bp>=0){int id=placeT(bp,br,bc); if(added)added->push_back(id);}
    }
}

int main(){
    T0clk=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>H>>W>>D>>B;
    vector<Proj> P(B);
    for(int i=0;i<B;i++){
        string t;cin>>t>>P[i].h>>P[i].w>>P[i].v;P[i].t=t[0];
        P[i].g.resize(P[i].h);P[i].area=0;
        for(int r=0;r<P[i].h;r++){cin>>P[i].g[r];for(char ch:P[i].g[r])if(ch=='#')P[i].area++;}
    }
    unordered_map<string,int> seen; vector<int> uq;
    for(int i=0;i<B;i++){
        string key; key.push_back(P[i].t);
        if(P[i].t=='U')key+=to_string(P[i].v);
        key+="|"+to_string(P[i].h)+"x"+to_string(P[i].w)+"|";
        for(auto&s:P[i].g)key+=s;
        auto it=seen.find(key);
        if(it==seen.end()){seen[key]=uq.size();uq.push_back(i);}
        else if(P[i].t=='R'&&P[i].v>P[uq[it->second]].v)uq[it->second]=i;
    }
    auto chooseT=[&](int X){
        if(X<=32)return X;
        int best=-1,bm=INT_MAX;
        for(int t=32;t<=min(X,60);t++){int m=X%t;if(m<bm){bm=m;best=t;}}
        return best;
    };
    Th=chooseT(H);Tw=chooseT(W);
    vector<int> Rl,Ul;
    auto buildLists=[&](){
        Rl.clear();Ul.clear();
        for(int i:uq){if(P[i].h>Th||P[i].w>Tw)continue; if(P[i].t=='R')Rl.push_back(i);else Ul.push_back(i);}
    };
    buildLists();
    if(Rl.empty()||Ul.empty()){Th=min(H,60);Tw=min(W,60);buildLists();}
    // select residential
    vector<int> selR;
    {
        vector<int> a=Rl;
        sort(a.begin(),a.end(),[&](int x,int y){
            double rx=(double)P[x].v/P[x].area, ry=(double)P[y].v/P[y].area;
            if(rx!=ry)return rx>ry; if(P[x].area!=P[y].area)return P[x].area<P[y].area; return x<y;});
        set<int> used;
        for(int i=0;i<(int)a.size()&&(int)selR.size()<30;i++){selR.push_back(a[i]);used.insert(a[i]);}
        vector<int> b=Rl;
        sort(b.begin(),b.end(),[&](int x,int y){
            if(P[x].area!=P[y].area)return P[x].area<P[y].area;
            double rx=(double)P[x].v/P[x].area, ry=(double)P[y].v/P[y].area;
            if(rx!=ry)return rx>ry; return x<y;});
        int add=0;
        for(int i=0;i<(int)b.size()&&add<10;i++)if(!used.count(b[i])){selR.push_back(b[i]);used.insert(b[i]);add++;}
    }
    vector<int> selU;
    {
        map<int,vector<int>> bt;
        for(int i:Ul)bt[P[i].v].push_back(i);
        vector<tuple<int,int,int>> L;
        for(auto&kv:bt){
            auto v=kv.second;
            sort(v.begin(),v.end(),[&](int x,int y){if(P[x].area!=P[y].area)return P[x].area<P[y].area;return x<y;});
            for(int k=0;k<(int)v.size()&&k<2;k++)L.emplace_back(k,P[v[k]].area,v[k]);
        }
        sort(L.begin(),L.end());
        for(int i=0;i<(int)L.size()&&i<80;i++)selU.push_back(get<2>(L[i]));
    }
    map<int,int> tmap;
    auto mkCand=[&](int i){
        Cand C; C.pid=i; C.res=P[i].t=='R'; C.h=P[i].h; C.w=P[i].w; C.area=P[i].area;
        C.cap=C.res?P[i].v:0; C.otype=C.res?-1:P[i].v; C.t=0;
        C.firstC=0; while(P[i].g[0][C.firstC]!='#')C.firstC++;
        for(int r=0;r<C.h;r++)for(int c=0;c<C.w;c++)if(P[i].g[r][c]=='#'){C.orr.push_back(r);C.occ_.push_back(c);}
        int GH=C.h+2*D,GW=C.w+2*D;
        vector<int> dist(GH*GW,INT_MAX); vector<int> q;
        for(size_t k=0;k<C.orr.size();k++){int id=(C.orr[k]+D)*GW+C.occ_[k]+D;dist[id]=0;q.push_back(id);}
        for(size_t h=0;h<q.size();h++){
            int id=q[h]; int d=dist[id]; if(d>=D)continue;
            int r=id/GW,c=id%GW;
            int nr[4]={r-1,r+1,r,r},nc[4]={c,c,c-1,c+1};
            for(int k=0;k<4;k++){if(nr[k]<0||nr[k]>=GH||nc[k]<0||nc[k]>=GW)continue;int j=nr[k]*GW+nc[k];if(dist[j]==INT_MAX){dist[j]=d+1;q.push_back(j);}}
        }
        for(int r=0;r<GH;r++)for(int c=0;c<GW;c++){int d=dist[r*GW+c];if(d>0&&d<=D){C.dr.push_back(r-D);C.dc.push_back(c-D);}}
        return C;
    };
    for(int i:selR)cands.push_back(mkCand(i));
    for(int i:selU){Cand C=mkCand(i); if(!tmap.count(C.otype)){int s=tmap.size();tmap[C.otype]=s;} C.t=tmap[C.otype]; cands.push_back(C);}
    NC=cands.size(); NT=max(1,(int)tmap.size());

    // tile init
    wrR=Th<H; wrC=Tw<W;
    rowTab.assign(Th+300,-1); colTab.assign(Tw+300,-1);
    for(int x=0;x<Th+300;x++){int v=x-OFF; rowTab[x]=wrR?((v%Th)+Th)%Th:((v>=0&&v<Th)?v:-1);}
    for(int x=0;x<Tw+300;x++){int v=x-OFF; colTab[x]=wrC?((v%Tw)+Tw)%Tw:((v>=0&&v<Tw)?v:-1);}
    int maxInst=Th*Tw;
    occ.assign(maxInst,-1); ip.assign(maxInst,0); ir_.assign(maxInst,0); ic_.assign(maxInst,0); kk.assign(maxInst,0);
    alive.assign(maxInst,0); cnt.assign((size_t)maxInst*NT,0); st.assign(maxInst,0); tst.assign(NT,0);
    for(int i=maxInst-1;i>=0;i--)freeL.push_back(i);
    {
        vector<pair<int,int>> cells;
        for(int r=0;r<Th;r++)for(int c=0;c<Tw;c++)cells.push_back({r,c});
        fillCells(cells,0.5,0,nullptr);
    }
    auto snapshot=[&](){vector<array<int,3>> s; for(int i=0;i<maxInst;i++)if(alive[i])s.push_back({ip[i],ir_[i],ic_[i]}); return s;};
    ll bestScore=score; vector<array<int,3>> best=snapshot();
    double avgCap=0; {int n=0; for(auto&C:cands)if(C.res){avgCap+=C.cap;n++;} if(n)avgCap/=n;}
    double temp0=0.2*avgCap;
    ull budget=2500000000ULL, startOps=ops;
    long long iter=0;
    while(true){
        if((iter&31)==0&&elapsed()>7.5)break;
        if(ops-startOps>budget)break;
        iter++;
        double frac=(double)(ops-startOps)/budget;
        int wh=1+rng()%min(Th,12), ww=1+rng()%min(Tw,12);
        int r0=wrR?rng()%Th:rng()%(Th-wh+1);
        int c0=wrC?rng()%Tw:rng()%(Tw-ww+1);
        ++stc; vector<int> rem;
        for(int i=0;i<wh;i++)for(int j=0;j<ww;j++){
            int rr=rowTab[r0+i+OFF],cc=colTab[c0+j+OFF];
            int id=occ[rr*Tw+cc]; if(id>=0&&st[id]!=stc){st[id]=stc;rem.push_back(id);}
        }
        vector<pair<int,pair<int,int>>> kc;
        auto keyOf=[&](int rr,int cc){int a=wrR?(rr-r0+Th)%Th:rr; int b=wrC?(cc-c0+Tw)%Tw:cc; return a*Tw+b;};
        vector<array<int,3>> remd;
        for(int id:rem){
            int p=ip[id],r=ir_[id],c=ic_[id]; remd.push_back({p,r,c});
            const Cand&C=cands[p];
            for(size_t k=0;k<C.orr.size();k++){int rr=rowTab[r+C.orr[k]+OFF],cc=colTab[c+C.occ_[k]+OFF]; kc.push_back({keyOf(rr,cc),{rr,cc}});}
            removeT(id);
        }
        for(int i=0;i<wh;i++)for(int j=0;j<ww;j++){int rr=rowTab[r0+i+OFF],cc=colTab[c0+j+OFF];kc.push_back({keyOf(rr,cc),{rr,cc}});}
        sort(kc.begin(),kc.end()); kc.erase(unique(kc.begin(),kc.end()),kc.end());
        vector<pair<int,int>> cells; cells.reserve(kc.size()); for(auto&x:kc)cells.push_back(x.second);
        ll old=score;
        double alpha=urand();
        int nm=rng()%3; double noise=nm==0?0:(nm==1?0.1:0.3);
        vector<int> added;
        fillCells(cells,alpha,noise,&added);
        ll delta=score-old;
        double temp=temp0*(1-frac)+1e-9;
        bool acc=delta>=0||urand()<exp((double)delta/temp);
        if(!acc){
            for(int id:added)removeT(id);
            for(auto&a:remd)placeT(a[0],a[1],a[2]);
        } else if(score>bestScore){bestScore=score;best=snapshot();}
    }

    // city
    vector<int> cocc((size_t)H*W,-1); vector<int> cP,cR,cC;
    vector<vector<int>> rt; vector<char> rcm;
    auto cplace=[&](int p,int r,int c){
        int id=cP.size(); cP.push_back(p);cR.push_back(r);cC.push_back(c); rt.emplace_back(); rcm.push_back(0);
        const Cand&C=cands[p];
        for(size_t k=0;k<C.orr.size();k++)cocc[(size_t)(r+C.orr[k])*W+c+C.occ_[k]]=id;
        return id;
    };
    for(auto&a:best){
        int p=a[0],r=a[1],c=a[2]; const Cand&C=cands[p];
        for(int R=r;R+C.h<=H;R+=Th){
            for(int Cc=c;Cc+C.w<=W;Cc+=Tw){
                cplace(p,R,Cc);
                if(!wrC)break;
            }
            if(!wrR)break;
        }
    }
    if(wrR||wrC){
        vector<int> cst((size_t)H*W+1,0),cst2((size_t)H*W+1,0); int sc1=0,sc2=0;
        vector<int> ts1(1002,0),ts2(1002,0); int tc1=0,tc2=0;
        vector<int> nb1,nb2;
        auto cGather=[&](int p,int r,int c,int self,vector<int>&out,vector<int>&stamp,int&sc){
            out.clear(); ++sc; const Cand&C=cands[p]; int n=C.dr.size(); ops+=n;
            for(int i=0;i<n;i++){
                int rr=r+C.dr[i]; if(rr<0||rr>=H)continue;
                int cc=c+C.dc[i]; if(cc<0||cc>=W)continue;
                int id=cocc[(size_t)rr*W+cc];
                if(id>=0&&id!=self&&stamp[id]!=sc){stamp[id]=sc;out.push_back(id);}
            }
        };
        auto computeRT=[&](int j){
            cGather(cP[j],cR[j],cC[j],j,nb2,cst2,sc2); ++tc2;
            rt[j].clear();
            for(int id:nb2){const Cand&U=cands[cP[id]]; if(U.res)continue; if(ts2[U.otype]!=tc2){ts2[U.otype]=tc2;rt[j].push_back(U.otype);}}
            rcm[j]=1;
        };
        auto has=[&](int j,int t){for(int x:rt[j])if(x==t)return true;return false;};
        auto cfits=[&](int p,int r,int c){
            const Cand&C=cands[p];
            for(size_t k=0;k<C.orr.size();k++){ops++; if(cocc[(size_t)(r+C.orr[k])*W+c+C.occ_[k]]>=0)return false;}
            return true;
        };
        auto cvalue=[&](int p,int r,int c,double alpha){
            const Cand&C=cands[p];
            cGather(p,r,c,-1,nb1,cst,sc1);
            if(C.res){
                ++tc1; int k=0;
                for(int id:nb1){const Cand&U=cands[cP[id]]; if(U.res)continue; if(ts1[U.otype]!=tc1){ts1[U.otype]=tc1;k++;}}
                return C.cap*(k+alpha)/C.area;
            } else {
                vector<int> loc=nb1; ll s=0;
                for(int j:loc){const Cand&R=cands[cP[j]]; if(!R.res)continue; if(!rcm[j])computeRT(j); if(!has(j,C.otype))s+=R.cap;}
                return (double)s/C.area;
            }
        };
        int remH=H%Th, remW=W%Tw;
        auto inStrip=[&](int r,int c){
            if(wrR&&(r<Th||r>=H-Th-remH))return true;
            if(wrC&&(c<Tw||c>=W-Tw-remW))return true;
            return false;
        };
        ull fstart=ops, fbudget=800000000ULL;
        bool stop=false;
        for(int pass=0;pass<2&&!stop;pass++){
            for(int r=0;r<H&&!stop;r++){
                if(elapsed()>9.0||ops-fstart>fbudget){stop=true;break;}
                for(int c=0;c<W;c++){
                    if(!inStrip(r,c))continue;
                    if(cocc[(size_t)r*W+c]>=0)continue;
                    int bp=-1,bc=0; double bv=0;
                    for(int p=0;p<NC;p++){
                        const Cand&C=cands[p];
                        int c0=c-C.firstC; if(c0<0||c0+C.w>W||r+C.h>H)continue;
                        if(!cfits(p,r,c0))continue;
                        double v=cvalue(p,r,c0,0.3);
                        if(v>bv){bv=v;bp=p;bc=c0;}
                    }
                    if(bp>=0){
                        int id=cplace(bp,r,bc);
                        const Cand&C=cands[bp];
                        if(!C.res){
                            cGather(bp,r,bc,id,nb1,cst,sc1);
                            for(int j:nb1){if(!cands[cP[j]].res)continue; if(rcm[j]&&!has(j,C.otype))rt[j].push_back(C.otype);}
                        }
                    }
                }
            }
        }
    }
    string out; out.reserve(cP.size()*16+16);
    out+=to_string(cP.size()); out+='\n';
    char buf[64];
    for(size_t i=0;i<cP.size();i++){
        int n=snprintf(buf,sizeof(buf),"%d %d %d\n",cands[cP[i]].pid,cR[i],cC[i]);
        out.append(buf,n);
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
