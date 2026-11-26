#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static uint64_t rs=88172645463325252ULL;
inline uint64_t rnd(){ rs^=rs<<7; rs^=rs>>9; return rs;}
inline int rndi(int n){return (int)(rnd()%(uint64_t)n);}
inline double rndd(){return (rnd()>>11)*(1.0/9007199254740992.0);}

int H,W,D,B;
struct Proj{ bool res; int h,w,val; vector<pair<int,int>> occ, reach; int anchor, area;};
vector<Proj> P;
ll work=0;
const int OFF=128;
double avgDens=1, avgCap=1;
vector<int> cand;

struct Opt{
    int Hg,Wg; bool wr,wc;
    vector<int> rowMap,colMap,owner,cellSt;
    vector<int> bp,br,bc; vector<char> alive; vector<int> freeIds;
    vector<vector<pair<int,int>>> cnt; vector<int> cov,stB;
    vector<int> stT, projSt; int cur=0, cs=0, ps=0;
    ll score=0;
    ll bestScore=0; bool saved=false; vector<array<int,3>> snap;
    void init(int h,int w,bool a,bool b){
        Hg=h;Wg=w;wr=a;wc=b;
        rowMap.resize(Hg+2*OFF); for(int x=-OFF;x<Hg+OFF;x++){int v; if(wr) v=((x%Hg)+Hg)%Hg; else v=(x>=0&&x<Hg)?x:-1; rowMap[x+OFF]=v;}
        colMap.resize(Wg+2*OFF); for(int x=-OFF;x<Wg+OFF;x++){int v; if(wc) v=((x%Wg)+Wg)%Wg; else v=(x>=0&&x<Wg)?x:-1; colMap[x+OFF]=v;}
        owner.assign((size_t)Hg*Wg,-1); cellSt.assign((size_t)Hg*Wg,0); stT.assign(1002,0); projSt.assign(B,0);
    }
    template<class F> inline void forNb(int p,int r,int c,F f){
        const Proj&q=P[p]; cur++; work+=q.reach.size();
        for(auto&o:q.reach){
            int rr=rowMap[r+o.first+OFF]; if(rr<0)continue;
            int cc=colMap[c+o.second+OFF]; if(cc<0)continue;
            int id=owner[rr*Wg+cc]; if(id<0||stB[id]==cur)continue; stB[id]=cur; f(id);
        }
    }
    bool fits(int p,int r,int c){
        const Proj&q=P[p]; work++;
        for(auto&o:q.occ){
            int rr=rowMap[r+o.first+OFF]; if(rr<0)return false;
            int cc=colMap[c+o.second+OFF]; if(cc<0)return false;
            if(owner[rr*Wg+cc]>=0) return false; work++;
        }
        return true;
    }
    ll gain(int p,int r,int c){
        const Proj&q=P[p];
        if(q.res){
            int k=0;
            forNb(p,r,c,[&](int id){ const Proj&u=P[bp[id]]; if(u.res)return; if(stT[u.val]!=cur){stT[u.val]=cur;k++;} });
            return (ll)q.val*k;
        } else {
            ll g=0; int t=q.val;
            forNb(p,r,c,[&](int id){ const Proj&u=P[bp[id]]; if(!u.res)return; bool f=false; for(auto&e:cnt[id]) if(e.first==t){f=true;break;} if(!f) g+=u.val; });
            return g;
        }
    }
    int add(int p,int r,int c){
        const Proj&q=P[p];
        r=rowMap[r+OFF]; c=colMap[c+OFF];
        int id;
        if(!freeIds.empty()){id=freeIds.back();freeIds.pop_back();}
        else{ id=bp.size(); bp.push_back(0);br.push_back(0);bc.push_back(0);alive.push_back(0);cnt.emplace_back();cov.push_back(0);stB.push_back(0);}
        bp[id]=p;br[id]=r;bc[id]=c;alive[id]=1;
        for(auto&o:q.occ){ owner[rowMap[r+o.first+OFF]*Wg+colMap[c+o.second+OFF]]=id; }
        work+=q.occ.size();
        if(q.res){
            cnt[id].clear(); int k=0;
            forNb(p,r,c,[&](int id2){ if(id2==id)return; const Proj&u=P[bp[id2]]; if(u.res)return; int t=u.val; auto&v=cnt[id]; bool f=false; for(auto&e:v) if(e.first==t){e.second++;f=true;break;} if(!f){v.push_back({t,1});k++;} });
            cov[id]=k; score+=(ll)q.val*k;
        } else {
            int t=q.val;
            forNb(p,r,c,[&](int id2){ if(id2==id)return; const Proj&u=P[bp[id2]]; if(!u.res)return; auto&v=cnt[id2]; bool f=false; for(auto&e:v) if(e.first==t){e.second++;f=true;break;} if(!f){v.push_back({t,1});cov[id2]++;score+=u.val;} });
        }
        return id;
    }
    void remove(int id){
        int p=bp[id]; const Proj&q=P[p]; int r=br[id],c=bc[id];
        if(q.res){ score-=(ll)q.val*cov[id]; cnt[id].clear(); cov[id]=0; }
        else{
            int t=q.val;
            forNb(p,r,c,[&](int id2){ if(id2==id)return; const Proj&u=P[bp[id2]]; if(!u.res)return; auto&v=cnt[id2];
                for(size_t k=0;k<v.size();k++) if(v[k].first==t){ if(--v[k].second==0){ v[k]=v.back(); v.pop_back(); cov[id2]--; score-=u.val;} break;} });
        }
        for(auto&o:q.occ){ owner[rowMap[r+o.first+OFF]*Wg+colMap[c+o.second+OFF]]=-1; }
        work+=q.occ.size();
        alive[id]=0; freeIds.push_back(id);
    }
    vector<int> recreate(const vector<pair<int,int>>& cells,const vector<int>& sub,double beta){
        vector<int> added;
        for(auto&ce:cells){
            int gr=ce.first,gc=ce.second;
            if(owner[gr*Wg+gc]>=0) continue;
            int bestP=-1; double bv=-1; int bcc=0;
            for(int p:sub){
                const Proj&q=P[p]; int c=gc-q.anchor;
                if(!fits(p,gr,c)) continue;
                ll g=gain(p,gr,c);
                double v= q.res ? (g+beta*q.val)/(double)q.area : g/(double)q.area+beta*avgDens*0.5;
                if(v>bv){bv=v;bestP=p;bcc=c;}
            }
            if(bestP>=0) added.push_back(add(bestP,gr,bcc));
        }
        return added;
    }
    void doMove(int r0,int c0,int wh,int ww,double temp,double beta,bool useSnap){
        work+=100;
        int st=++cur;
        vector<int> rem;
        cs++;
        vector<array<int,4>> scan;
        auto pushCell=[&](int vr,int vc){
            int rr=rowMap[vr+OFF], cc=colMap[vc+OFF]; if(rr<0||cc<0) return;
            int idx=rr*Wg+cc; if(cellSt[idx]==cs) return; cellSt[idx]=cs;
            int kr=vr-r0, kc=vc-c0;
            if(wr){kr=((kr%Hg)+Hg)%Hg; if(kr>Hg/2) kr-=Hg;}
            if(wc){kc=((kc%Wg)+Wg)%Wg; if(kc>Wg/2) kc-=Wg;}
            scan.push_back({kr,kc,rr,cc});
        };
        for(int i=0;i<wh;i++)for(int j=0;j<ww;j++){
            int rr=rowMap[r0+i+OFF], cc=colMap[c0+j+OFF]; if(rr<0||cc<0)continue;
            int id=owner[rr*Wg+cc]; if(id>=0&&stB[id]!=st){stB[id]=st;rem.push_back(id);}
            pushCell(r0+i,c0+j);
        }
        vector<array<int,3>> remInfo;
        for(int id:rem){
            remInfo.push_back({bp[id],br[id],bc[id]});
            for(auto&o:P[bp[id]].occ) pushCell(br[id]+o.first,bc[id]+o.second);
        }
        for(int id:rem) remove(id);
        ps++; vector<int> sub;
        for(auto&x:remInfo) if(projSt[x[0]]!=ps){projSt[x[0]]=ps;sub.push_back(x[0]);}
        int extra=min((int)cand.size(),10);
        for(int k=0;k<extra;k++){int p=cand[rndi(cand.size())]; if(projSt[p]!=ps){projSt[p]=ps;sub.push_back(p);}}
        for(int k=(int)sub.size()-1;k>0;k--) swap(sub[k],sub[rndi(k+1)]);
        sort(scan.begin(),scan.end());
        vector<pair<int,int>> cells; cells.reserve(scan.size());
        for(auto&s:scan) cells.push_back({s[2],s[3]});
        ll old=score;
        vector<int> added=recreate(cells,sub,beta);
        ll delta=score-old;
        bool acc = delta>=0 || (temp>0 && rndd()<exp((double)delta/temp));
        if(acc){
            if(delta<0 && useSnap && !saved && old==bestScore){
                int mk=++cur;
                for(int id:added) stB[id]=mk;
                snap.clear();
                for(size_t id=0;id<bp.size();id++) if(alive[id]&&stB[id]!=mk) snap.push_back({bp[id],br[id],bc[id]});
                for(auto&x:remInfo) snap.push_back(x);
                saved=true;
            }
            if(score>bestScore){bestScore=score;saved=false;}
        } else {
            for(int k=(int)added.size()-1;k>=0;k--) remove(added[k]);
            for(auto&x:remInfo) add(x[0],x[1],x[2]);
        }
    }
    vector<array<int,3>> current(){
        vector<array<int,3>> v;
        for(size_t id=0;id<bp.size();id++) if(alive[id]) v.push_back({bp[id],br[id],bc[id]});
        return v;
    }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    auto t0=chrono::steady_clock::now();
    auto el=[&]{return chrono::duration<double>(chrono::steady_clock::now()-t0).count();};
    cin>>H>>W>>D>>B;
    P.resize(B);
    vector<string> keys(B);
    for(int i=0;i<B;i++){
        char t; int h,w,v; cin>>t>>h>>w>>v;
        Proj&q=P[i]; q.res=(t=='R'); q.h=h;q.w=w;q.val=v; q.anchor=-1;
        string key=to_string(h)+" "+to_string(w)+" ";
        for(int r=0;r<h;r++){string s;cin>>s; key+=s; for(int c=0;c<w&&c<(int)s.size();c++) if(s[c]=='#'){q.occ.push_back({r,c}); if(r==0&&q.anchor<0)q.anchor=c;}}
        if(q.anchor<0) q.anchor=0;
        q.area=max(1,(int)q.occ.size()); keys[i]=key;
        int GH=h+2*D,GW=w+2*D; vector<int> dist(GH*GW,-1); vector<int> qu; qu.reserve(GH*GW);
        for(auto&o:q.occ){int id=(o.first+D)*GW+o.second+D; dist[id]=0; qu.push_back(id);}
        for(size_t k=0;k<qu.size();k++){
            int id=qu[k]; int d=dist[id]; if(d>=D) continue; int r=id/GW,c=id%GW;
            const int dr[4]={1,-1,0,0},dc[4]={0,0,1,-1};
            for(int z=0;z<4;z++){int nr=r+dr[z],nc=c+dc[z]; if(nr<0||nc<0||nr>=GH||nc>=GW)continue; int nid=nr*GW+nc; if(dist[nid]<0){dist[nid]=d+1;qu.push_back(nid);}}
        }
        for(int id=0;id<GH*GW;id++) if(dist[id]>0) q.reach.push_back({id/GW-D,id%GW-D});
    }
    // candidates
    map<string,int> rb; map<pair<int,string>,int> ub;
    for(int i=0;i<B;i++){
        if(P[i].occ.empty()) continue;
        if(P[i].res){ auto it=rb.find(keys[i]); if(it==rb.end()||P[it->second].val<P[i].val) rb[keys[i]]=i; }
        else { auto k=make_pair(P[i].val,keys[i]); if(!ub.count(k)) ub[k]=i; }
    }
    map<int,vector<int>> byType;
    for(auto&e:ub) byType[e.first.first].push_back(e.second);
    vector<int> resC;
    for(auto&e:rb){cand.push_back(e.second); resC.push_back(e.second);}
    vector<int> utilFirst;
    for(auto&e:byType){ auto v=e.second; sort(v.begin(),v.end(),[&](int a,int b){return P[a].area!=P[b].area?P[a].area<P[b].area:a<b;}); if(v.size()>5)v.resize(5); for(int x:v)cand.push_back(x); utilFirst.push_back(v[0]); }
    sort(cand.begin(),cand.end());
    {
        double s1=0,s2=0; for(int x:resC){s1+=P[x].val/(double)P[x].area; s2+=P[x].val;}
        if(!resC.empty()){avgDens=s1/resC.size(); avgCap=s2/resC.size();}
    }
    int maxDim=1; double avgDim=0;
    for(int x:cand){maxDim=max({maxDim,P[x].h,P[x].w}); avgDim+=(P[x].h+P[x].w)/2.0;}
    avgDim/=max<size_t>(1,cand.size());
    int target=min(100,max({30,2*maxDim,(int)(2*D+2*avgDim)}));
    auto chooseT=[&](int L,int&T,bool&wrap){
        if(L<=2*target){T=L;wrap=false;return;}
        wrap=true; int best=target,bw=INT_MAX;
        for(int t=target;t<=min(L,target+target/3);t++){int w=L%t; if(w<bw){bw=w;best=t;}}
        T=best;
    };
    int Th,Tw; bool wr,wc;
    chooseT(H,Th,wr); chooseT(W,Tw,wc);
    {
        vector<int> c2; for(int x:cand) if(P[x].h<=Th&&P[x].w<=Tw) c2.push_back(x); cand=c2;
    }
    vector<array<int,3>> finalList;
    if(!cand.empty()){
        Opt T; T.init(Th,Tw,wr,wc);
        // initial subset
        vector<int> init;
        {
            vector<int> rc; for(int x:cand) if(P[x].res) rc.push_back(x);
            sort(rc.begin(),rc.end(),[&](int a,int b){double da=P[a].val/(double)P[a].area,db=P[b].val/(double)P[b].area; return da!=db?da>db:a<b;});
            for(int k=0;k<(int)rc.size()&&k<10;k++) init.push_back(rc[k]);
            vector<int> uf; for(int x:utilFirst) if(P[x].h<=Th&&P[x].w<=Tw) uf.push_back(x);
            for(int k=(int)uf.size()-1;k>0;k--) swap(uf[k],uf[rndi(k+1)]);
            for(int k=0;k<(int)uf.size()&&k<20;k++) init.push_back(uf[k]);
        }
        vector<pair<int,int>> cells;
        for(int r=0;r<Th;r++)for(int c=0;c<Tw;c++) cells.push_back({r,c});
        T.recreate(cells,init,0.5);
        T.bestScore=T.score; T.saved=false;
        double T0=avgCap*0.3, T1=avgCap*0.005;
        ll budget1=1200000000LL; ll ws=work;
        int wmax=max(4,min(30,(int)(2*avgDim)+3));
        long it=0;
        while(true){
            if((it&15)==0 && el()>6.0) break;
            if(work-ws>=budget1) break;
            it++;
            double frac=(double)(work-ws)/budget1;
            double temp=T0*pow(T1/T0,frac);
            int wh=1+rndi(min(Th,wmax)), ww=1+rndi(min(Tw,wmax));
            int r0=wr?rndi(Th):rndi(Th-wh+1);
            int c0=wc?rndi(Tw):rndi(Tw-ww+1);
            T.doMove(r0,c0,wh,ww,temp,rndd(),true);
        }
        vector<array<int,3>> tl = (T.score>=T.bestScore)? T.current() : T.snap;
        // tile
        vector<array<int,3>> copies;
        ll cost=0;
        for(auto&b:tl){
            const Proj&q=P[b[0]];
            vector<int> rows,cols;
            if(wr){for(int R=b[1];R+q.h<=H;R+=Th) rows.push_back(R);} else if(b[1]+q.h<=H) rows.push_back(b[1]);
            if(wc){for(int C=b[2];C+q.w<=W;C+=Tw) cols.push_back(C);} else if(b[2]+q.w<=W) cols.push_back(b[2]);
            for(int R:rows)for(int C:cols){copies.push_back({b[0],R,C}); cost+=q.reach.size()+q.occ.size();}
        }
        finalList=copies;
        if((wr||wc) && cost<300000000LL && el()<6.5){
            Opt C; C.init(H,W,false,false);
            for(auto&b:copies) C.add(b[0],b[1],b[2]);
            int lossR = wr? max(0,(H/Th)*Th-maxDim) : H;
            int lossC = wc? max(0,(W/Tw)*Tw-maxDim) : W;
            ll budget2=250000000LL; ws=work; it=0;
            while(true){
                if((it&15)==0 && el()>9.0) break;
                if(work-ws>=budget2) break;
                it++;
                int wh=1+rndi(min(H,wmax)), ww=1+rndi(min(W,wmax));
                int r0,c0;
                double pr=rndd();
                bool hasR=lossR<H, hasC=lossC<W;
                if(pr<0.7 && (hasR||hasC)){
                    bool bottom = hasR && (!hasC || rndi(2)==0);
                    if(bottom){ int lo=min(lossR,H-wh); r0=lo+rndi(H-wh-lo+1); c0=rndi(W-ww+1); }
                    else { int lo=min(lossC,W-ww); c0=lo+rndi(W-ww-lo+1); r0=rndi(H-wh+1); }
                } else { r0=rndi(H-wh+1); c0=rndi(W-ww+1); }
                C.doMove(r0,c0,wh,ww,0.0,rndd(),false);
            }
            finalList=C.current();
        }
    }
    // validate
    vector<char> g((size_t)H*W,0);
    vector<array<int,3>> outL;
    for(auto&b:finalList){
        const Proj&q=P[b[0]];
        if(b[1]<0||b[2]<0||b[1]+q.h>H||b[2]+q.w>W) continue;
        bool ok=true;
        for(auto&o:q.occ) if(g[(size_t)(b[1]+o.first)*W+b[2]+o.second]){ok=false;break;}
        if(!ok) continue;
        for(auto&o:q.occ) g[(size_t)(b[1]+o.first)*W+b[2]+o.second]=1;
        outL.push_back(b);
    }
    string out; out.reserve(outL.size()*16+16);
    out+=to_string(outL.size()); out+='\n';
    for(auto&b:outL){ out+=to_string(b[0]); out+=' '; out+=to_string(b[1]); out+=' '; out+=to_string(b[2]); out+='\n'; }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
