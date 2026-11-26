#include <bits/stdc++.h>
using namespace std;
typedef unsigned __int128 u128;

int H,W,D,B;
struct Proj{int h,w;bool res;int val;int orig;vector<pair<int,int>> occ;vector<pair<int,int>> reg;int anchor;int cells;int tidx;};
vector<Proj> cand;
static inline int pc128(u128 x){return __builtin_popcountll((unsigned long long)x)+__builtin_popcountll((unsigned long long)(x>>64));}
static inline u128 bitOf(int t){return ((u128)1)<<t;}

long long ops=0;
chrono::steady_clock::time_point T0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}

struct TB{int p,r,c;};
struct Result{double est; vector<TB> b; bool torR,torC; int Th,Tw;};
double rho=1;
const int OFF=130;

Result runTile(int Th,int Tw,bool torR,bool torC,double aR,double aU,double beta,double gam,long long opCap,bool &aborted){
    aborted=false;
    Result res; res.est=0; res.torR=torR; res.torC=torC; res.Th=Th; res.Tw=Tw;
    vector<int> mapR(Th+2*OFF),mapC(Tw+2*OFF);
    for(int i=0;i<Th+2*OFF;i++){int rr=i-OFF; if(torR) mapR[i]=((rr%Th)+Th)%Th; else mapR[i]=(rr>=0&&rr<Th)?rr:-1;}
    for(int i=0;i<Tw+2*OFF;i++){int rr=i-OFF; if(torC) mapC[i]=((rr%Tw)+Tw)%Tw; else mapC[i]=(rr>=0&&rr<Tw)?rr:-1;}
    vector<int> own((size_t)Th*Tw,-1);
    vector<int> bp; vector<u128> bmask; vector<int> stamp; int curSt=0;
    vector<int> usable; vector<double> potU(cand.size(),0);
    for(int i=0;i<(int)cand.size();i++){ if(cand[i].h<=Th&&cand[i].w<=Tw){usable.push_back(i); if(!cand[i].res) potU[i]=aU*rho*pow((double)cand[i].cells,gam);} }
    long long startOps=ops;
    for(int r=0;r<Th;r++){
        if(ops-startOps>opCap || elapsed()>9.0){aborted=true;return res;}
        for(int c=0;c<Tw;c++){
            if(own[(size_t)r*Tw+c]>=0) continue;
            int best=-1; double bestV=-1e18; u128 bestMask=0;
            for(int p:usable){
                const Proj&P=cand[p];
                int tr=r, tc=c-P.anchor;
                bool ok=true;
                for(auto&o:P.occ){int R=mapR[tr+o.first+OFF],C=mapC[tc+o.second+OFF]; if(R<0||C<0||own[(size_t)R*Tw+C]>=0){ok=false;break;}}
                ops+=4;
                if(!ok) continue;
                ops+=P.occ.size()+P.reg.size();
                u128 um=0; long long gain=0; curSt++;
                u128 ub = P.res?0:bitOf(P.tidx);
                for(auto&o:P.reg){int R=mapR[tr+o.first+OFF],C=mapC[tc+o.second+OFF]; if(R<0||C<0) continue; int b=own[(size_t)R*Tw+C]; if(b<0) continue;
                    const Proj&Q=cand[bp[b]];
                    if(!Q.res) um|=bitOf(Q.tidx);
                    else if(!P.res){ if(stamp[b]!=curSt){stamp[b]=curSt; if(!(bmask[b]&ub)) gain+=Q.val;} }
                }
                double v;
                if(P.res){ v=(P.val*(pc128(um)+aR))/P.cells; }
                else { bool novel=!(um&ub); v=(gain+potU[p]*(novel?1.0:beta))/P.cells; }
                if(v>bestV){bestV=v;best=p;bestMask=um;}
            }
            if(best<0) continue;
            const Proj&P=cand[best];
            int tr=r, tc=c-P.anchor;
            int id=bp.size(); bp.push_back(best); bmask.push_back(P.res?bestMask:0); stamp.push_back(0);
            for(auto&o:P.occ){int R=mapR[tr+o.first+OFF],C=mapC[tc+o.second+OFF]; own[(size_t)R*Tw+C]=id;}
            if(!P.res){
                u128 ub=bitOf(P.tidx);
                for(auto&o:P.reg){int R=mapR[tr+o.first+OFF],C=mapC[tc+o.second+OFF]; if(R<0||C<0) continue; int b=own[(size_t)R*Tw+C]; if(b<0) continue;
                    if(cand[bp[b]].res) bmask[b]|=ub;}
            }
            res.b.push_back({best,mapR[tr+OFF],mapC[tc+OFF]});
        }
    }
    double est=0;
    for(size_t i=0;i<res.b.size();i++){
        const Proj&P=cand[res.b[i].p];
        if(!P.res) continue;
        double s=(double)P.val*pc128(bmask[i]);
        long long nr,nc; int tr=res.b[i].r,tc=res.b[i].c;
        if(tr+P.h>H) nr=0; else nr=(H-P.h-tr)/Th+1;
        if(tc+P.w>W) nc=0; else nc=(W-P.w-tc)/Tw+1;
        est+=s*nr*nc;
    }
    res.est=est;
    return res;
}

int main(){
    T0=chrono::steady_clock::now();
    scanf("%d %d %d %d",&H,&W,&D,&B);
    vector<Proj> all(B);
    vector<string> keys(B);
    for(int i=0;i<B;i++){
        char t[4]; int h,w,v; scanf("%s %d %d %d",t,&h,&w,&v);
        Proj&P=all[i]; P.h=h;P.w=w;P.res=(t[0]=='R');P.val=v;P.orig=i;
        string key=to_string(h)+"x"+to_string(w)+":";
        static char buf[100];
        for(int r=0;r<h;r++){scanf("%s",buf); string s(buf); key+=s;
            for(int c=0;c<w;c++) if(s[c]=='#') P.occ.push_back({r,c});}
        keys[i]=key;
        P.cells=P.occ.size();
        P.anchor=0; for(int c=0;c<w;c++) if(P.occ.size()&&true){ }
        for(auto&o:P.occ) if(o.first==0){P.anchor=o.second;break;}
    }
    // residential dedupe
    map<string,int> rmap;
    vector<int> resIdx;
    for(int i=0;i<B;i++) if(all[i].res){
        auto it=rmap.find(keys[i]);
        if(it==rmap.end()){rmap[keys[i]]=i;resIdx.push_back(i);}
        else if(all[i].val>all[it->second].val){
            for(auto&x:resIdx) if(x==it->second) x=i;
            it->second=i;
        }
    }
    vector<int> rs=resIdx;
    sort(rs.begin(),rs.end(),[&](int a,int b){double ra=(double)all[a].val/all[a].cells, rb=(double)all[b].val/all[b].cells; if(ra!=rb) return ra>rb; return a<b;});
    vector<int> keepR; set<int> ks;
    for(int i=0;i<(int)rs.size()&&i<15;i++){keepR.push_back(rs[i]);ks.insert(rs[i]);}
    vector<int> rs2=resIdx;
    sort(rs2.begin(),rs2.end(),[&](int a,int b){if(all[a].cells!=all[b].cells) return all[a].cells<all[b].cells; double ra=(double)all[a].val/all[a].cells, rb=(double)all[b].val/all[b].cells; if(ra!=rb) return ra>rb; return a<b;});
    int added=0;
    for(int x:rs2){ if(added>=5) break; if(!ks.count(x)){keepR.push_back(x);ks.insert(x);added++;} }
    // utilities
    map<int,vector<int>> ut;
    for(int i=0;i<B;i++) if(!all[i].res) ut[all[i].val].push_back(i);
    vector<pair<int,vector<int>>> tl;
    for(auto&kv:ut){
        auto v=kv.second;
        sort(v.begin(),v.end(),[&](int a,int b){if(all[a].cells!=all[b].cells) return all[a].cells<all[b].cells; if(all[a].h*all[a].w!=all[b].h*all[b].w) return all[a].h*all[a].w<all[b].h*all[b].w; return a<b;});
        vector<int> v2; set<string> seen;
        for(int x:v) if(!seen.count(keys[x])){seen.insert(keys[x]);v2.push_back(x);}
        tl.push_back({kv.first,v2});
    }
    sort(tl.begin(),tl.end(),[&](const pair<int,vector<int>>&a,const pair<int,vector<int>>&b){
        int ca=all[a.second[0]].cells, cb=all[b.second[0]].cells; if(ca!=cb) return ca<cb; return a.first<b.first;});
    int nT=min((int)tl.size(),100);
    int perType=max(1,min(3,50/max(1,nT)));
    for(int t=0;t<nT;t++){
        for(int k=0;k<(int)tl[t].second.size()&&k<perType;k++){
            Proj P=all[tl[t].second[k]]; P.tidx=t; cand.push_back(P);
        }
    }
    for(int x:keepR){Proj P=all[x]; P.tidx=-1; cand.push_back(P);}
    rho=1e-9;
    for(auto&P:cand) if(P.res) rho=max(rho,(double)P.val/P.cells);
    // regions
    for(auto&P:cand){
        int GH=P.h+2*D, GW=P.w+2*D;
        vector<int> dist((size_t)GH*GW,-1);
        deque<int> q;
        for(auto&o:P.occ){int id=(o.first+D)*GW+(o.second+D); dist[id]=0; q.push_back(id);}
        while(!q.empty()){int id=q.front();q.pop_front(); int y=id/GW,x=id%GW; if(dist[id]>=D) continue;
            int dy[4]={1,-1,0,0},dx[4]={0,0,1,-1};
            for(int k=0;k<4;k++){int ny=y+dy[k],nx=x+dx[k]; if(ny<0||nx<0||ny>=GH||nx>=GW) continue; int nid=ny*GW+nx; if(dist[nid]<0){dist[nid]=dist[id]+1;q.push_back(nid);}}
        }
        for(int y=0;y<GH;y++)for(int x=0;x<GW;x++){int d=dist[(size_t)y*GW+x]; if(d>=1&&d<=D) P.reg.push_back({y-D,x-D});}
    }
    // tile sizes
    vector<pair<int,int>> pairs;
    int Ss[]={16,24,32,40,48,56,64,80,96,128};
    for(int s:Ss){pair<int,int> pr={min(s,H),min(s,W)}; if(find(pairs.begin(),pairs.end(),pr)==pairs.end()) pairs.push_back(pr);}
    if((long long)H*W<=40000){pair<int,int> pr={H,W}; if(find(pairs.begin(),pairs.end(),pr)==pairs.end()) pairs.push_back(pr);}
    sort(pairs.begin(),pairs.end(),[](const pair<int,int>&a,const pair<int,int>&b){return (long long)a.first*a.second<(long long)b.first*b.second;});
    const long long BUDGET=1500000000LL;
    vector<long long> cost(pairs.size(),0);
    Result best; best.est=-1;
    mt19937 rng(12345);
    auto uni=[&](double a,double b){return a+(b-a)*(rng()/4294967296.0);};
    auto doTrial=[&](int pi,double aR,double aU,double beta,double gam)->bool{
        long long rem=BUDGET-ops; if(rem<=0) return false;
        int Th=pairs[pi].first,Tw=pairs[pi].second;
        bool aborted; long long before=ops;
        Result r=runTile(Th,Tw,Th<H,Tw<W,aR,aU,beta,gam,rem,aborted);
        cost[pi]=max(cost[pi],ops-before);
        if(aborted){cost[pi]=LLONG_MAX/4; return false;}
        if(r.est>best.est) best=r;
        return true;
    };
    for(int pi=0;pi<(int)pairs.size();pi++){
        if(ops>=BUDGET||elapsed()>8.0) break;
        doTrial(pi,1.0,1.0,0.3,0.5);
    }
    while(ops<BUDGET&&elapsed()<8.0){
        vector<int> ok;
        for(int pi=0;pi<(int)pairs.size();pi++) if(cost[pi]>0&&cost[pi]<=BUDGET-ops) ok.push_back(pi);
        if(ok.empty()) break;
        int pi=ok[rng()%ok.size()];
        double aR=uni(0,2.5), aU=exp(uni(log(0.05),log(50.0))), beta=uni(0,1), gam=uni(0,1);
        doTrial(pi,aR,aU,beta,gam);
    }
    // replicate
    vector<int> own((size_t)H*W,-1);
    vector<int> pb,pr,pcl;
    auto canPlace=[&](int p,int R,int C)->bool{
        const Proj&P=cand[p];
        if(R<0||C<0||R+P.h>H||C+P.w>W) return false;
        for(auto&o:P.occ) if(own[(size_t)(R+o.first)*W+C+o.second]>=0) return false;
        return true;
    };
    auto place=[&](int p,int R,int C){
        int id=pb.size(); pb.push_back(p);pr.push_back(R);pcl.push_back(C);
        for(auto&o:cand[p].occ) own[(size_t)(R+o.first)*W+C+o.second]=id;
    };
    if(best.est>=0){
        for(auto&tb:best.b){
            const Proj&P=cand[tb.p];
            for(int R=tb.r;R+P.h<=H;R+=best.Th)
                for(int C=tb.c;C+P.w<=W;C+=best.Tw)
                    if(canPlace(tb.p,R,C)) place(tb.p,R,C);
        }
    }
    // gap fill border bands with residentials
    {
        int band=55;
        long long gapBudget=300000000LL, gops=0;
        vector<int> resC; for(int i=0;i<(int)cand.size();i++) if(cand[i].res) resC.push_back(i);
        bool stop=false;
        for(int r=0;r<H&&!stop;r++){
            if(gops>gapBudget||elapsed()>9.3) break;
            for(int c=0;c<W;c++){
                if(!(r<band||r>=H-band||c<band||c>=W-band)) continue;
                if(own[(size_t)r*W+c]>=0) continue;
                int bp=-1; double bv=0;
                for(int p:resC){
                    const Proj&P=cand[p];
                    int tr=r,tc=c-P.anchor;
                    gops+=2;
                    if(!canPlace(p,tr,tc)) continue;
                    gops+=P.reg.size()+P.occ.size();
                    u128 um=0;
                    for(auto&o:P.reg){int R=tr+o.first,C=tc+o.second; if(R<0||C<0||R>=H||C>=W) continue; int b=own[(size_t)R*W+C]; if(b<0) continue; const Proj&Q=cand[pb[b]]; if(!Q.res) um|=bitOf(Q.tidx);}
                    double v=(double)P.val*pc128(um)/P.cells;
                    if(v>bv){bv=v;bp=p;}
                }
                if(bp>=0) place(bp,r,c-cand[bp].anchor);
            }
        }
    }
    // output
    string out;
    out+=to_string(pb.size())+"\n";
    for(size_t i=0;i<pb.size();i++){
        out+=to_string(cand[pb[i]].orig)+" "+to_string(pr[i])+" "+to_string(pcl[i])+"\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
