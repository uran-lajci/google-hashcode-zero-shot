#include <bits/stdc++.h>
using namespace std;
struct Proj{char t;int h,w,val,orig;vector<pair<int,int>> cells,dil;};
int H,W,D,B;
vector<Proj> P;
static uint64_t rs=88172645463325252ULL;
inline uint64_t rnd(){rs^=rs<<7;rs^=rs>>9;return rs;}
inline double rndd(){return (rnd()>>11)*(1.0/9007199254740992.0);}
chrono::steady_clock::time_point T0clk;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0clk).count();}

struct Solver{
    int Ph,Pw,perR,perC,OFF;
    vector<int> rmap,cmap,owner;
    vector<int> bp,br,bc; vector<char> alive; vector<vector<pair<int,int>>> cnt; vector<int> freeIds;
    vector<unsigned> stamp; unsigned cur=0;
    long long score=0,work=0;
    vector<int> nb,ov;
    vector<int> Rl,Ul; int Ktop=1; double avgcap=1;
    long long bestScore=-1; vector<array<int,3>> best;
    bool init(int ph,int pr,int pw,int pc){
        Ph=ph;perR=pr;Pw=pw;perC=pc;OFF=D+2;
        rmap.assign(Ph+60+2*D+4,-1);cmap.assign(Pw+60+2*D+4,-1);
        for(int i=0;i<(int)rmap.size();i++){int v=i-OFF; if(perR) rmap[i]=((v%Ph)+Ph)%Ph; else rmap[i]=(v>=0&&v<Ph)?v:-1;}
        for(int i=0;i<(int)cmap.size();i++){int v=i-OFF; if(perC) cmap[i]=((v%Pw)+Pw)%Pw; else cmap[i]=(v>=0&&v<Pw)?v:-1;}
        owner.assign((size_t)Ph*Pw,-1);
        for(int i=0;i<(int)P.size();i++){ if(P[i].h<=Ph&&P[i].w<=Pw){ if(P[i].t=='R')Rl.push_back(i); else Ul.push_back(i);} }
        if(Rl.empty()||Ul.empty())return false;
        sort(Rl.begin(),Rl.end(),[&](int a,int b){double x=(double)P[a].val/P[a].cells.size(),y=(double)P[b].val/P[b].cells.size(); if(x!=y)return x>y; return a<b;});
        Ktop=min<int>(20,Rl.size()); double s=0; for(int i=0;i<Ktop;i++)s+=P[Rl[i]].val; avgcap=s/Ktop;
        return true;
    }
    int newId(){ if(!freeIds.empty()){int id=freeIds.back();freeIds.pop_back();return id;}
        bp.push_back(0);br.push_back(0);bc.push_back(0);alive.push_back(0);cnt.emplace_back();stamp.push_back(0);return bp.size()-1;}
    void gather(int self,int p,int r,int c){
        nb.clear();++cur; const auto&dl=P[p].dil; work+=dl.size();
        for(auto&o:dl){int x=rmap[r+o.first+OFF]; if(x<0)continue; int y=cmap[c+o.second+OFF]; if(y<0)continue;
            int ow=owner[x*Pw+y]; if(ow>=0&&ow!=self&&stamp[ow]!=cur){stamp[ow]=cur;nb.push_back(ow);}}
    }
    int add(int p,int r,int c){
        int id=newId(); bp[id]=p;br[id]=r;bc[id]=c;alive[id]=1;
        const Proj&pr=P[p];
        for(auto&ce:pr.cells){owner[rmap[r+ce.first+OFF]*Pw+cmap[c+ce.second+OFF]]=id;}
        work+=pr.cells.size();
        gather(id,p,r,c);
        if(pr.t=='R'){ auto&v=cnt[id]; v.clear();
            for(int u:nb){ if(P[bp[u]].t!='U')continue; int t=P[bp[u]].val; bool f=false; for(auto&q:v) if(q.first==t){q.second++;f=true;break;} if(!f)v.push_back({t,1}); }
            score+=(long long)pr.val*v.size();
        } else { int t=pr.val;
            for(int rr:nb){ if(P[bp[rr]].t!='R')continue; auto&v=cnt[rr]; bool f=false; for(auto&q:v) if(q.first==t){q.second++;f=true;break;} if(!f){v.push_back({t,1});score+=P[bp[rr]].val;} }
        }
        return id;
    }
    void remove(int id){
        int p=bp[id],r=br[id],c=bc[id]; const Proj&pr=P[p];
        if(pr.t=='R'){ score-=(long long)pr.val*cnt[id].size(); cnt[id].clear(); }
        else { gather(id,p,r,c); int t=pr.val;
            for(int rr:nb){ if(P[bp[rr]].t!='R')continue; auto&v=cnt[rr];
                for(size_t i=0;i<v.size();i++) if(v[i].first==t){ if(--v[i].second==0){v[i]=v.back();v.pop_back();score-=P[bp[rr]].val;} break; } }
        }
        for(auto&ce:pr.cells){owner[rmap[r+ce.first+OFF]*Pw+cmap[c+ce.second+OFF]]=-1;}
        work+=pr.cells.size();
        alive[id]=0; freeIds.push_back(id);
    }
    bool pos(const Proj&pr,int r,int c,int&ro,int&co){
        if(perR) r=((r%Ph)+Ph)%Ph; else if(r<0||r+pr.h>Ph) return false;
        if(perC) c=((c%Pw)+Pw)%Pw; else if(c<0||c+pr.w>Pw) return false;
        ro=r;co=c;return true;
    }
    void collectOv(int p,int r,int c){ ov.clear();++cur;
        for(auto&ce:P[p].cells){int o=owner[rmap[r+ce.first+OFF]*Pw+cmap[c+ce.second+OFF]]; if(o>=0&&stamp[o]!=cur){stamp[o]=cur;ov.push_back(o);}}
        work+=P[p].cells.size()+10;
    }
    bool freeFit(int p,int r,int c){ work+=4;
        for(auto&ce:P[p].cells){ if(owner[rmap[r+ce.first+OFF]*Pw+cmap[c+ce.second+OFF]]>=0) return false; } return true; }
    int pickProj(){ if(rnd()%100<55){ if(rnd()%100<80) return Rl[rnd()%Ktop]; return Rl[rnd()%Rl.size()]; } return Ul[rnd()%Ul.size()]; }
    void snapshot(){ best.clear(); for(size_t i=0;i<bp.size();i++) if(alive[i]) best.push_back({bp[i],br[i],bc[i]}); bestScore=score; }
    void restoreBest(){
        for(size_t i=0;i<bp.size();i++) if(alive[i]) remove(i);
        score=0; for(auto&b:best) add(b[0],b[1],b[2]);
    }
    void run(long long budget,double Ts,double Te){
        long long start=work; vector<array<int,3>> removed; vector<int> added; long long it=0;
        while(true){
            long long used=work-start; if(used>=budget)break;
            if((++it&255)==0){ if(score>bestScore) snapshot(); if(elapsed()>9.2) break; }
            double T=Ts*pow(Te/Ts,(double)used/budget);
            int p=pickProj(); const Proj&pr=P[p];
            int x=rnd()%Ph,y=rnd()%Pw; auto k=pr.cells[rnd()%pr.cells.size()];
            int r,c; if(!pos(pr,x-k.first,y-k.second,r,c)){work+=5;continue;}
            collectOv(p,r,c);
            if(ov.size()>3)continue;
            long long old=score; removed.clear(); added.clear();
            for(int id:ov){removed.push_back({bp[id],br[id],bc[id]}); remove(id);}
            added.push_back(add(p,r,c));
            if(!removed.empty()){
                for(int t=0;t<6;t++){
                    auto rb=removed[rnd()%removed.size()]; const Proj&q0=P[rb[0]];
                    auto cc=q0.cells[rnd()%q0.cells.size()];
                    int xx=rmap[rb[1]+cc.first+OFF],yy=cmap[rb[2]+cc.second+OFF];
                    if(owner[xx*Pw+yy]>=0)continue;
                    int q=pickProj(); const Proj&Q=P[q]; auto kk=Q.cells[rnd()%Q.cells.size()];
                    int r2,c2; if(!pos(Q,xx-kk.first,yy-kk.second,r2,c2))continue;
                    if(!freeFit(q,r2,c2))continue;
                    added.push_back(add(q,r2,c2));
                }
            }
            long long delta=score-old;
            if(delta>=0||rndd()<exp((double)delta/T))continue;
            for(int i=(int)added.size()-1;i>=0;i--) remove(added[i]);
            for(auto&rb:removed) add(rb[0],rb[1],rb[2]);
        }
        if(score>bestScore) snapshot();
    }
};

int main(){
    T0clk=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>H>>W>>D>>B;
    map<string,int> seen;
    for(int i=0;i<B;i++){
        string ts;int h,w,v;cin>>ts>>h>>w>>v; vector<string> g(h); for(auto&s:g)cin>>s;
        string key=ts+"|"+(ts=="U"?to_string(v):string(""))+"|"+to_string(h)+"|"+to_string(w)+"|";
        for(auto&s:g)key+=s;
        auto itr=seen.find(key);
        if(itr!=seen.end()){ if(ts=="R"&&v>P[itr->second].val){P[itr->second].val=v;P[itr->second].orig=i;} continue; }
        Proj pr; pr.t=ts[0];pr.h=h;pr.w=w;pr.val=v;pr.orig=i;
        for(int r=0;r<h;r++)for(int c=0;c<w;c++) if(g[r][c]=='#') pr.cells.push_back({r,c});
        int GH=h+2*D,GW=w+2*D; vector<int> dist(GH*GW,INT_MAX); deque<int> q;
        for(auto&ce:pr.cells){int id=(ce.first+D)*GW+ce.second+D; dist[id]=0;q.push_back(id);}
        while(!q.empty()){int id=q.front();q.pop_front(); int r=id/GW,c=id%GW; int dd=dist[id]; if(dd>=D)continue;
            const int dr[4]={1,-1,0,0},dc[4]={0,0,1,-1};
            for(int k=0;k<4;k++){int nr=r+dr[k],nc=c+dc[k]; if(nr<0||nc<0||nr>=GH||nc>=GW)continue; int ni=nr*GW+nc; if(dist[ni]>dd+1){dist[ni]=dd+1;q.push_back(ni);}}}
        for(int r=0;r<GH;r++)for(int c=0;c<GW;c++){int dd=dist[r*GW+c]; if(dd>=1&&dd<=D) pr.dil.push_back({r-D,c-D});}
        seen[key]=P.size(); P.push_back(pr);
    }
    // configs
    set<array<int,4>> cfgset; vector<array<int,4>> cfgs;
    auto pick=[&](int L,int s,int&Pl,int&per){ if(L<=3*s){Pl=L;per=0;} else { per=1; int bp=s,bw=INT_MAX; for(int x=s;x<=s+s/4;x++){int wst=L%x; if(wst<bw){bw=wst;bp=x;}} Pl=bp; } };
    for(int s:{16,24,32,48,64,96}){ int a,b,c,d; pick(H,s,a,b); pick(W,s,c,d); array<int,4> k={a,b,c,d}; if(!cfgset.count(k)){cfgset.insert(k);cfgs.push_back(k);} }
    vector<unique_ptr<Solver>> sols;
    for(auto&k:cfgs){ auto s=make_unique<Solver>(); if(s->init(k[0],k[1],k[2],k[3])) sols.push_back(move(s)); }
    if(sols.empty()){ printf("0\n"); return 0; }
    const long long TOTAL=2000000000LL;
    long long b1=(long long)(TOTAL*0.4)/sols.size();
    int bi=0; double bd=-1;
    for(size_t i=0;i<sols.size();i++){ auto&s=*sols[i]; s.run(b1,0.5*s.avgcap,0.005*s.avgcap);
        double dens=(double)s.bestScore/((double)s.Ph*s.Pw); if(dens>bd){bd=dens;bi=i;} }
    {
        auto&s=*sols[bi]; s.restoreBest(); s.bestScore=s.score;
        s.run((long long)(TOTAL*0.6),0.15*s.avgcap,0.003*s.avgcap);
    }
    auto&s=*sols[bi];
    vector<array<int,3>> out;
    for(auto&b:s.best){
        const Proj&pr=P[b[0]];
        vector<int> rows,cols;
        if(s.perR){ for(int r=b[1];r+pr.h<=H;r+=s.Ph) rows.push_back(r);} else rows.push_back(b[1]);
        if(s.perC){ for(int c=b[2];c+pr.w<=W;c+=s.Pw) cols.push_back(c);} else cols.push_back(b[2]);
        for(int r:rows)for(int c:cols) out.push_back({pr.orig,r,c});
    }
    string o; o.reserve(out.size()*16+16);
    o+=to_string(out.size()); o+='\n';
    for(auto&x:out){ o+=to_string(x[0]); o+=' '; o+=to_string(x[1]); o+=' '; o+=to_string(x[2]); o+='\n'; }
    fwrite(o.data(),1,o.size(),stdout);
    return 0;
}
