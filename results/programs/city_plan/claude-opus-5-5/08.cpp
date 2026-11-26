#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int H,W,D,B;
struct Proj{ bool isU; int h,w,val; vector<int> cr,cc; vector<short> hr,hc; int area,anchor,orig; };
vector<Proj> cand;
static ll ops=0;
mt19937 rng(12345);
double rnd(){ return (rng()>>8)*(1.0/16777216.0); }
static int stampC=0;
vector<int> tS;
chrono::steady_clock::time_point T0;
double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-T0).count(); }

struct Board{
    int Ph,Pw; bool wr,wc; static const int OFF=32;
    vector<int> mR,mC,owner,bp,br,bc,bS; vector<char> alive; vector<vector<int>> cov;
    vector<int> allowed; int aliveCnt=0; ll curScore=0; int maxh=1,maxw=1;
    void init(int ph,int pw,bool a,bool b){
        Ph=ph;Pw=pw;wr=a;wc=b;
        mR.assign(Ph+OFF+100,-1); mC.assign(Pw+OFF+100,-1);
        for(int i=0;i<(int)mR.size();i++){int x=i-OFF; mR[i]=wr?((x%Ph)+Ph)%Ph:((x>=0&&x<Ph)?x:-1);}
        for(int i=0;i<(int)mC.size();i++){int x=i-OFF; mC[i]=wc?((x%Pw)+Pw)%Pw:((x>=0&&x<Pw)?x:-1);}
        owner.assign(Ph*Pw,-1);
    }
    bool fits(int p,int r,int c){
        const Proj&P=cand[p];
        if(!wr&&(r<0||r+P.h>Ph))return false;
        if(!wc&&(c<0||c+P.w>Pw))return false;
        int n=P.cr.size();
        for(int k=0;k<n;k++){int R=mR[r+P.cr[k]+OFF],C=mC[c+P.cc[k]+OFF];
            if(owner[R*Pw+C]!=-1){ops+=k+1;return false;}}
        ops+=n;return true;
    }
    ll gainR(int p,int r,int c){
        const Proj&P=cand[p]; ++stampC; int cnt=0; int n=P.hr.size();
        for(int k=0;k<n;k++){int R=mR[r+P.hr[k]+OFF]; if(R<0)continue; int C=mC[c+P.hc[k]+OFF]; if(C<0)continue;
            int o=owner[R*Pw+C]; if(o>=0){const Proj&Q=cand[bp[o]]; if(Q.isU&&tS[Q.val]!=stampC){tS[Q.val]=stampC;cnt++;}}}
        ops+=n; return (ll)cnt*P.val;
    }
    ll gainU(int p,int r,int c){
        const Proj&P=cand[p]; ++stampC; ll s=0; int t=P.val; int n=P.hr.size();
        for(int k=0;k<n;k++){int R=mR[r+P.hr[k]+OFF]; if(R<0)continue; int C=mC[c+P.hc[k]+OFF]; if(C<0)continue;
            int o=owner[R*Pw+C]; if(o>=0&&bS[o]!=stampC){bS[o]=stampC; const Proj&Q=cand[bp[o]];
                if(!Q.isU){ auto&v=cov[o]; if(find(v.begin(),v.end(),t)==v.end()) s+=Q.val; }}}
        ops+=n; return s;
    }
    void recomputeCov(int id){
        const Proj&P=cand[bp[id]]; int r=br[id],c=bc[id]; auto&v=cov[id]; v.clear(); ++stampC; int n=P.hr.size();
        for(int k=0;k<n;k++){int R=mR[r+P.hr[k]+OFF]; if(R<0)continue; int C=mC[c+P.hc[k]+OFF]; if(C<0)continue;
            int o=owner[R*Pw+C]; if(o>=0){const Proj&Q=cand[bp[o]]; if(Q.isU&&tS[Q.val]!=stampC){tS[Q.val]=stampC;v.push_back(Q.val);}}}
        ops+=n;
    }
    void place(int p,int r,int c){
        int id=bp.size(); bp.push_back(p);br.push_back(r);bc.push_back(c);alive.push_back(1);bS.push_back(0);cov.emplace_back();aliveCnt++;
        const Proj&P=cand[p]; int n=P.cr.size();
        for(int k=0;k<n;k++){int R=mR[r+P.cr[k]+OFF],C=mC[c+P.cc[k]+OFF]; owner[R*Pw+C]=id;}
        if(!P.isU){ recomputeCov(id); curScore+=(ll)P.val*cov[id].size(); }
        else{ ++stampC; int t=P.val; int m=P.hr.size();
            for(int k=0;k<m;k++){int R=mR[r+P.hr[k]+OFF]; if(R<0)continue; int C=mC[c+P.hc[k]+OFF]; if(C<0)continue;
                int o=owner[R*Pw+C]; if(o>=0&&bS[o]!=stampC){bS[o]=stampC; const Proj&Q=cand[bp[o]];
                    if(!Q.isU){auto&v=cov[o]; if(find(v.begin(),v.end(),t)==v.end()){v.push_back(t);curScore+=Q.val;}}}}
            ops+=m; }
    }
    void removeB(int id,vector<int>&aff,vector<int>&freed){
        const Proj&P=cand[bp[id]]; int r=br[id],c=bc[id]; int n=P.cr.size();
        for(int k=0;k<n;k++){int R=mR[r+P.cr[k]+OFF],C=mC[c+P.cc[k]+OFF]; owner[R*Pw+C]=-1; freed.push_back(R*Pw+C);}
        alive[id]=0;aliveCnt--; cov[id].clear();
        if(P.isU){int m=P.hr.size();
            for(int k=0;k<m;k++){int R=mR[r+P.hr[k]+OFF]; if(R<0)continue; int C=mC[c+P.hc[k]+OFF]; if(C<0)continue;
                int o=owner[R*Pw+C]; if(o>=0&&!cand[bp[o]].isU) aff.push_back(o);}
            ops+=m;}
    }
    ll score(){ ll s=0; for(size_t i=0;i<bp.size();i++) if(alive[i]&&!cand[bp[i]].isU) s+=(ll)cand[bp[i]].val*cov[i].size(); return s; }
    void compact(){
        vector<int> nid(bp.size(),-1); int k=0;
        for(size_t i=0;i<bp.size();i++) if(alive[i]) nid[i]=k++;
        vector<int> nbp(k),nbr(k),nbc(k); vector<vector<int>> ncov(k);
        for(size_t i=0;i<bp.size();i++) if(alive[i]){int j=nid[i]; nbp[j]=bp[i];nbr[j]=br[i];nbc[j]=bc[i];ncov[j]=move(cov[i]);}
        for(auto&o:owner) if(o>=0) o=nid[o];
        bp=nbp;br=nbr;bc=nbc;cov=move(ncov);alive.assign(k,1);bS.assign(k,0);
    }
    vector<array<int,3>> fR,fU;
    void tryCell(int R,int C,double pz){
        double best=-1; int bpI=-1,bR=0,bC=0;
        fR.clear();fU.clear();
        for(int p:allowed){
            const Proj&P=cand[p]; int r=R,c=C-P.anchor;
            if(wc){ if(c<0)c+=Pw; } else if(c<0) continue;
            if(!fits(p,r,c))continue;
            ll g=P.isU?gainU(p,r,c):gainR(p,r,c);
            if(P.isU)fU.push_back({p,r,c}); else fR.push_back({p,r,c});
            if(g>0){double v=(double)g/P.area*(1+0.2*rnd()); if(v>best){best=v;bpI=p;bR=r;bC=c;}}
        }
        if(bpI>=0){place(bpI,bR,bC);return;}
        if(rnd()<pz){
            vector<array<int,3>>*L=nullptr;
            if(!fR.empty()&&!fU.empty()) L=(rnd()<0.5)?&fR:&fU;
            else if(!fR.empty())L=&fR; else if(!fU.empty())L=&fU;
            if(L){auto&a=(*L)[rng()%L->size()]; place(a[0],a[1],a[2]);}
        }
    }
    void initial(double pz){
        for(int R=0;R<Ph;R++){ if(elapsed()>8.0)break;
            for(int C=0;C<Pw;C++) if(owner[R*Pw+C]==-1) tryCell(R,C,pz);}
        curScore=score();
    }
    // returns true if accepted
    void lnsStep(double pz){
        ll old=curScore;
        Board bk=*this; ops+=owner.size()+bp.size()*4;
        int hh=1+rng()%min(Ph,10), ww=1+rng()%min(Pw,10);
        int r0=wr?rng()%Ph:rng()%(Ph-hh+1), c0=wc?rng()%Pw:rng()%(Pw-ww+1);
        vector<int> aff,freed,cells;
        for(int i=0;i<hh;i++)for(int j=0;j<ww;j++){
            int R=(r0+i)%Ph,C=(c0+j)%Pw; int id=R*Pw+C; int o=owner[id];
            if(o>=0) removeB(o,aff,freed); else cells.push_back(id);
        }
        for(int o:aff) if(alive[o]) recomputeCov(o);
        for(int x:freed)cells.push_back(x);
        int mr=wr?max(0,min(Ph-hh,maxh-1)):0, mc=wc?max(0,min(Pw-ww,maxw-1)):0;
        vector<pair<ll,int>> ks; ks.reserve(cells.size());
        for(int id:cells){int R=id/Pw,C=id%Pw; ll kr=wr?((R-r0+mr)%Ph+Ph)%Ph:R; ll kc=wc?((C-c0+mc)%Pw+Pw)%Pw:C; ks.push_back({kr*Pw+kc,id});}
        sort(ks.begin(),ks.end()); ks.erase(unique(ks.begin(),ks.end()),ks.end());
        for(auto&k:ks){int id=k.second; if(owner[id]==-1) tryCell(id/Pw,id%Pw,pz);}
        curScore=score(); ops+=bp.size();
        if(curScore>=old){ if((int)bp.size()>2*aliveCnt+200) compact(); }
        else { *this=move(bk); }
    }
};

int main(){
    T0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d",&H,&W,&D,&B)!=4){printf("0\n");return 0;}
    tS.assign(1100,0);
    struct Raw{bool isU;int h,w,val;vector<string> g;};
    vector<Raw> raws(B);
    char buf[64];
    for(int i=0;i<B;i++){
        int h,w,v; scanf("%s %d %d %d",buf,&h,&w,&v);
        raws[i].isU=(buf[0]=='U');raws[i].h=h;raws[i].w=w;raws[i].val=v;
        raws[i].g.resize(h); static char line[128];
        for(int r=0;r<h;r++){scanf("%s",line);raws[i].g[r]=line;}
    }
    // dedupe
    map<string,int> mp; vector<int> uniq;
    for(int i=0;i<B;i++){
        string key=string(raws[i].isU?"U":"R")+to_string(raws[i].h)+","+to_string(raws[i].w)+",";
        if(raws[i].isU) key+=to_string(raws[i].val)+",";
        for(auto&s:raws[i].g) key+=s+"|";
        auto it=mp.find(key);
        if(it==mp.end()){mp[key]=i;uniq.push_back(i);}
        else if(!raws[i].isU && raws[i].val>raws[it->second].val){
            int old=it->second; it->second=i; for(auto&u:uniq) if(u==old) u=i;
        }
    }
    auto areaOf=[&](int i){int a=0;for(auto&s:raws[i].g)for(char ch:s)if(ch=='#')a++;return a;};
    vector<int> resU,utlU;
    for(int i:uniq)(raws[i].isU?utlU:resU).push_back(i);
    set<int> chosen;
    {
        vector<int> a=resU;
        sort(a.begin(),a.end(),[&](int x,int y){double dx=(double)raws[x].val/areaOf(x),dy=(double)raws[y].val/areaOf(y); if(dx!=dy)return dx>dy; return x<y;});
        for(int i=0;i<(int)a.size()&&i<25;i++)chosen.insert(a[i]);
        sort(a.begin(),a.end(),[&](int x,int y){int ax=areaOf(x),ay=areaOf(y); if(ax!=ay)return ax<ay; if(raws[x].val!=raws[y].val)return raws[x].val>raws[y].val; return x<y;});
        for(int i=0;i<(int)a.size()&&i<5;i++)chosen.insert(a[i]);
    }
    {
        map<int,vector<int>> byT;
        for(int i:utlU) byT[raws[i].val].push_back(i);
        vector<pair<int,int>> tl;
        for(auto&kv:byT){auto&v=kv.second; sort(v.begin(),v.end(),[&](int x,int y){int ax=areaOf(x),ay=areaOf(y); if(ax!=ay)return ax<ay; int bx=raws[x].h*raws[x].w,by=raws[y].h*raws[y].w; if(bx!=by)return bx<by; return x<y;}); tl.push_back({areaOf(v[0]),kv.first});}
        sort(tl.begin(),tl.end());
        int cnt=0;
        for(auto&t:tl){ if(cnt>=60)break; auto&v=byT[t.second]; for(int k=0;k<(int)v.size()&&k<2&&cnt<60;k++){chosen.insert(v[k]);cnt++;} }
    }
    for(int i:chosen){
        Proj P; auto&R=raws[i]; P.isU=R.isU;P.h=R.h;P.w=R.w;P.val=R.val;P.orig=i;
        for(int r=0;r<R.h;r++)for(int c=0;c<R.w;c++) if(R.g[r][c]=='#'){P.cr.push_back(r);P.cc.push_back(c);}
        P.area=P.cr.size(); P.anchor=0; for(int c=0;c<R.w;c++) if(R.g[0][c]=='#'){P.anchor=c;break;}
        int GH=R.h+2*D,GW=R.w+2*D; vector<int> dist(GH*GW,-1); deque<int> q;
        for(size_t k=0;k<P.cr.size();k++){int id=(P.cr[k]+D)*GW+P.cc[k]+D; dist[id]=0;q.push_back(id);}
        while(!q.empty()){int id=q.front();q.pop_front(); int d=dist[id]; if(d>=D)continue; int r=id/GW,c=id%GW;
            const int dr[4]={1,-1,0,0},dc[4]={0,0,1,-1};
            for(int z=0;z<4;z++){int nr=r+dr[z],nc=c+dc[z]; if(nr<0||nc<0||nr>=GH||nc>=GW)continue; int nid=nr*GW+nc; if(dist[nid]<0){dist[nid]=d+1;q.push_back(nid);}}}
        for(int r=0;r<GH;r++)for(int c=0;c<GW;c++){int d=dist[r*GW+c]; if(d>=1){P.hr.push_back(r-D);P.hc.push_back(c-D);}}
        cand.push_back(P);
    }
    // configs
    vector<array<int,4>> cfgs;
    int Ps[]={8,12,16,24,32,48,64};
    for(int P:Ps){ int ph=P<H?P:H, pw=P<W?P:W; array<int,4> a={ph,pw,P<H?1:0,P<W?1:0};
        if(find(cfgs.begin(),cfgs.end(),a)==cfgs.end()) cfgs.push_back(a);}
    if((ll)H*W<=4096){array<int,4> a={H,W,0,0}; if(find(cfgs.begin(),cfgs.end(),a)==cfgs.end()) cfgs.push_back(a);}
    vector<Board> boards; vector<double> ests;
    const ll PH1=1200000000LL, TOTAL=3000000000LL;
    for(auto&cf:cfgs){
        Board bd; bd.init(cf[0],cf[1],cf[2],cf[3]);
        bool hasR=false,hasU=false;
        for(int p=0;p<(int)cand.size();p++) if(cand[p].h<=bd.Ph&&cand[p].w<=bd.Pw){bd.allowed.push_back(p); (cand[p].isU?hasU:hasR)=true; bd.maxh=max(bd.maxh,cand[p].h); bd.maxw=max(bd.maxw,cand[p].w);}
        if(!hasR||!hasU) continue;
        boards.push_back(bd); ests.push_back(0);
    }
    int nb=boards.size(); int best=-1;
    if(nb>0){
        ll per=PH1/nb;
        for(int i=0;i<nb;i++){
            ll start=ops;
            boards[i].initial(0.7);
            while(ops-start<per && elapsed()<8.0) boards[i].lnsStep(0.5);
            ests[i]=(double)boards[i].curScore*H*W/((double)boards[i].Ph*boards[i].Pw);
            if(best<0||ests[i]>ests[best]) best=i;
        }
        Board&bd=boards[best];
        while(ops<TOTAL && elapsed()<8.0) bd.lnsStep(0.5);
    }
    string out; vector<array<int,3>> res;
    if(best>=0){
        Board&bd=boards[best];
        for(size_t i=0;i<bd.bp.size();i++){ if(!bd.alive[i])continue;
            const Proj&P=cand[bd.bp[i]]; int r=bd.br[i],c=bd.bc[i];
            int sr=bd.wr?bd.Ph:H+1, sc=bd.wc?bd.Pw:W+1;
            for(int rr=r;rr+P.h<=H;rr+=sr) for(int cc=c;cc+P.w<=W;cc+=sc) res.push_back({P.orig,rr,cc});
        }
    }
    out.reserve(res.size()*16+16);
    out+=to_string(res.size()); out+='\n';
    for(auto&a:res){out+=to_string(a[0]);out+=' ';out+=to_string(a[1]);out+=' ';out+=to_string(a[2]);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
