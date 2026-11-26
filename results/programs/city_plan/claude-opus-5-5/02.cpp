#include <bits/stdc++.h>
using namespace std;

int H,W,D,B;
struct Proj{char t;int h,w,v;vector<string> g;vector<pair<int,int>> occ,nb;int fc;int area;};
vector<Proj> pr;
int P,Q; bool wrapR,wrapC;
vector<int> rowMap,colMap; const int OFF=128;
inline int RM(int r){return rowMap[r+OFF];}
inline int CM(int c){return colMap[c+OFF];}
vector<int> owner;
vector<int> bp,br,bc,bval; vector<char> bres,alive;
vector<vector<pair<int,int>>> rt;
vector<int> freeIds;
vector<int> bst,tst; int stampC=0;
long long score=0; long long work=0;

int allocId(){
    if(!freeIds.empty()){int x=freeIds.back();freeIds.pop_back();return x;}
    int id=bp.size();
    bp.push_back(0);br.push_back(0);bc.push_back(0);bval.push_back(0);bres.push_back(0);alive.push_back(0);
    rt.emplace_back();bst.push_back(0);
    return id;
}
bool canPlace(int p,int r,int c){
    for(auto&d:pr[p].occ){work++;int rr=RM(r+d.first),cc=CM(c+d.second);
        if(rr<0||cc<0)return false; if(owner[rr*Q+cc]!=-1)return false;}
    return true;
}
long long gain(int p,int r,int c){
    const Proj&pp=pr[p]; ++stampC; work+=pp.nb.size();
    if(pp.t=='R'){
        int cnt=0;
        for(auto&d:pp.nb){int rr=RM(r+d.first),cc=CM(c+d.second); if(rr<0||cc<0)continue;
            int o=owner[rr*Q+cc]; if(o<0||bres[o])continue; int t=bval[o];
            if(tst[t]!=stampC){tst[t]=stampC;cnt++;}}
        return (long long)cnt*pp.v;
    } else {
        long long g=0;int t=pp.v;
        for(auto&d:pp.nb){int rr=RM(r+d.first),cc=CM(c+d.second); if(rr<0||cc<0)continue;
            int o=owner[rr*Q+cc]; if(o<0||!bres[o])continue; if(bst[o]==stampC)continue; bst[o]=stampC;
            bool f=false; for(auto&e:rt[o]) if(e.first==t){f=true;break;}
            if(!f) g+=bval[o];}
        return g;
    }
}
int place(int p,int r,int c){
    int id=allocId(); const Proj&pp=pr[p];
    bp[id]=p;br[id]=r;bc[id]=c;bval[id]=pp.v;bres[id]=(pp.t=='R');alive[id]=1;
    for(auto&d:pp.occ){int rr=RM(r+d.first),cc=CM(c+d.second);owner[rr*Q+cc]=id;}
    work+=pp.occ.size()+pp.nb.size();
    ++stampC;
    if(pp.t=='R'){
        rt[id].clear();
        for(auto&d:pp.nb){int rr=RM(r+d.first),cc=CM(c+d.second); if(rr<0||cc<0)continue;
            int o=owner[rr*Q+cc]; if(o<0||o==id||bres[o])continue; if(bst[o]==stampC)continue; bst[o]=stampC;
            int t=bval[o]; bool f=false; for(auto&e:rt[id]) if(e.first==t){e.second++;f=true;break;}
            if(!f) rt[id].push_back({t,1});}
        score+=(long long)pp.v*rt[id].size();
    } else {
        int t=pp.v;
        for(auto&d:pp.nb){int rr=RM(r+d.first),cc=CM(c+d.second); if(rr<0||cc<0)continue;
            int o=owner[rr*Q+cc]; if(o<0||o==id||!bres[o])continue; if(bst[o]==stampC)continue; bst[o]=stampC;
            bool f=false; for(auto&e:rt[o]) if(e.first==t){e.second++;f=true;break;}
            if(!f){rt[o].push_back({t,1});score+=bval[o];}}
    }
    return id;
}
void removeB(int id){
    int p=bp[id],r=br[id],c=bc[id]; const Proj&pp=pr[p];
    for(auto&d:pp.occ){int rr=RM(r+d.first),cc=CM(c+d.second);owner[rr*Q+cc]=-1;}
    work+=pp.occ.size()+pp.nb.size();
    if(pp.t=='R'){ score-=(long long)pp.v*rt[id].size(); rt[id].clear(); }
    else {
        ++stampC; int t=pp.v;
        for(auto&d:pp.nb){int rr=RM(r+d.first),cc=CM(c+d.second); if(rr<0||cc<0)continue;
            int o=owner[rr*Q+cc]; if(o<0||o==id||!bres[o])continue; if(bst[o]==stampC)continue; bst[o]=stampC;
            auto&v=rt[o];
            for(size_t k=0;k<v.size();k++) if(v[k].first==t){ if(--v[k].second==0){v[k]=v.back();v.pop_back();score-=bval[o];} break;}
        }
    }
    alive[id]=0; freeIds.push_back(id);
}

vector<int> cands; vector<double> biasv;
mt19937 rng(12345);
double urand(){return (rng()>>8)*(1.0/16777216.0);}

void refill(const vector<int>&cells,double alpha,double beta,vector<int>&added){
    vector<double> ap(cands.size());
    for(size_t k=0;k<cands.size();k++) ap[k]=pow((double)pr[cands[k]].area,alpha);
    for(int cell:cells){
        if(owner[cell]!=-1)continue;
        int cr=cell/Q,ccl=cell%Q;
        int best=-1,bestR=0,bestC=0;double bm=-1;
        for(size_t k=0;k<cands.size();k++){
            int p=cands[k];const Proj&pp=pr[p];
            int r=cr,c=ccl-pp.fc;
            if(wrapC){c%=Q;if(c<0)c+=Q;} else if(c<0||c+pp.w>Q)continue;
            if(!wrapR&&r+pp.h>P)continue;
            if(!canPlace(p,r,c))continue;
            long long g=gain(p,r,c);
            double m=((double)g+biasv[k]*beta)/ap[k]*(1.0+0.05*urand());
            if(m>bm){bm=m;best=p;bestR=r;bestC=c;}
        }
        if(best>=0) added.push_back(place(best,bestR,bestC));
    }
}

int main(){
    auto T0=chrono::steady_clock::now();
    scanf("%d %d %d %d",&H,&W,&D,&B);
    pr.resize(B);
    for(int i=0;i<B;i++){
        char tb[4];scanf("%s %d %d %d",tb,&pr[i].h,&pr[i].w,&pr[i].v);pr[i].t=tb[0];
        pr[i].g.resize(pr[i].h); static char buf[64];
        for(int r=0;r<pr[i].h;r++){scanf("%s",buf);pr[i].g[r]=buf;}
        pr[i].area=0;
        for(int r=0;r<pr[i].h;r++)for(int c=0;c<pr[i].w;c++)if(pr[i].g[r][c]=='#'){pr[i].occ.push_back({r,c});pr[i].area++;}
        pr[i].fc=0; for(int c=0;c<pr[i].w;c++)if(pr[i].g[0][c]=='#'){pr[i].fc=c;break;}
    }
    if(H<=100){P=H;wrapR=false;}else{int k=(H+99)/100;P=H/k;wrapR=true;}
    if(W<=100){Q=W;wrapC=false;}else{int k=(W+99)/100;Q=W/k;wrapC=true;}
    // candidates
    vector<int> res,uti;
    set<string> seen;
    for(int i=0;i<B;i++){
        if(pr[i].h>P||pr[i].w>Q)continue;
        string key=string(1,pr[i].t)+to_string(pr[i].v)+"|"+to_string(pr[i].w)+"|";
        for(auto&s:pr[i].g)key+=s;
        if(seen.count(key))continue; seen.insert(key);
        if(pr[i].t=='R')res.push_back(i);else uti.push_back(i);
    }
    sort(res.begin(),res.end(),[&](int a,int b){double ra=(double)pr[a].v/pr[a].area,rb=(double)pr[b].v/pr[b].area; if(ra!=rb)return ra>rb; if(pr[a].area!=pr[b].area) return pr[a].area<pr[b].area; return a<b;});
    set<int> cs;
    for(size_t k=0;k<res.size()&&k<12;k++)cs.insert(res[k]);
    for(int lim:{4,9,16}){for(int x:res)if(pr[x].area<=lim){cs.insert(x);break;}}
    map<int,int> bestU;
    for(int x:uti){auto it=bestU.find(pr[x].v);
        if(it==bestU.end()||pr[x].area<pr[it->second].area||(pr[x].area==pr[it->second].area&&pr[x].h*pr[x].w<pr[it->second].h*pr[it->second].w))bestU[pr[x].v]=x;}
    vector<int> ul;for(auto&kv:bestU)ul.push_back(kv.second);
    sort(ul.begin(),ul.end(),[&](int a,int b){if(pr[a].area!=pr[b].area)return pr[a].area<pr[b].area;return a<b;});
    for(size_t k=0;k<ul.size()&&k<30;k++)cs.insert(ul[k]);
    cands.assign(cs.begin(),cs.end());
    double avgCap=0;int nr=0;for(int x:cands)if(pr[x].t=='R'){avgCap+=pr[x].v;nr++;} if(nr)avgCap/=nr; else avgCap=1;
    for(int x:cands) biasv.push_back(pr[x].t=='R'?(double)pr[x].v:avgCap);
    // neighborhoods
    for(int x:cands){
        Proj&pp=pr[x]; int GH=pp.h+2*D,GW=pp.w+2*D;
        vector<int> dist(GH*GW,INT_MAX); deque<int> dq;
        for(auto&o:pp.occ){int id=(o.first+D)*GW+o.second+D;dist[id]=0;dq.push_back(id);}
        while(!dq.empty()){int u=dq.front();dq.pop_front();int ur=u/GW,uc=u%GW;if(dist[u]>=D)continue;
            const int dr[4]={1,-1,0,0},dc[4]={0,0,1,-1};
            for(int k=0;k<4;k++){int vr=ur+dr[k],vc=uc+dc[k];if(vr<0||vc<0||vr>=GH||vc>=GW)continue;int v=vr*GW+vc;
                if(dist[v]==INT_MAX){dist[v]=dist[u]+1;dq.push_back(v);}}}
        for(int i=0;i<GH;i++)for(int j=0;j<GW;j++){int d=dist[i*GW+j];if(d>0&&d<=D)pp.nb.push_back({i-D,j-D});}
    }
    rowMap.resize(P+2*OFF);colMap.resize(Q+2*OFF);
    for(int x=-OFF;x<P+OFF;x++)rowMap[x+OFF]=wrapR?(((x%P)+P)%P):((x>=0&&x<P)?x:-1);
    for(int x=-OFF;x<Q+OFF;x++)colMap[x+OFF]=wrapC?(((x%Q)+Q)%Q):((x>=0&&x<Q)?x:-1);
    owner.assign(P*Q,-1); tst.assign(1005,0);
    {
        vector<int> cells(P*Q);for(int i=0;i<P*Q;i++)cells[i]=i;
        vector<int> added; refill(cells,1.0,0.3,added);
    }
    const long long BUDGET=2000000000LL;
    int maxDim=0;for(int x:cands)maxDim=max(maxDim,max(pr[x].h,pr[x].w));
    int Lmax=max(4,2*D+5);
    vector<int> wst(P*Q,0);int wstamp=0;
    long long iter=0;
    while(work<BUDGET){
        if((iter&15)==0){double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count(); if(el>8.8)break;}
        iter++;
        int wr=2+rng()%max(1,min(P,Lmax)-1); if(wr>P)wr=P;
        int wc=2+rng()%max(1,min(Q,Lmax)-1); if(wc>Q)wc=Q;
        int r0=wrapR?rng()%P:rng()%(P-wr+1);
        int c0=wrapC?rng()%Q:rng()%(Q-wc+1);
        long long old=score;
        ++stampC; vector<int> rem;
        vector<int> wcells;
        for(int i=0;i<wr;i++)for(int j=0;j<wc;j++){int rr=RM(r0+i),cc=CM(c0+j);int cell=rr*Q+cc;wcells.push_back(cell);
            int o=owner[cell];if(o>=0&&bst[o]!=stampC){bst[o]=stampC;rem.push_back(o);}}
        vector<array<int,3>> remInfo;
        ++wstamp; vector<pair<int,int>> keyed;
        auto addCell=[&](int cell){if(wst[cell]==wstamp)return;wst[cell]=wstamp;int rr=cell/Q,cc=cell%Q;
            int kr=wrapR?(((rr-r0+55)%P+P)%P):rr; int kc=wrapC?(((cc-c0+55)%Q+Q)%Q):cc;
            keyed.push_back({kr*Q+kc,cell});};
        for(int o:rem){remInfo.push_back({bp[o],br[o],bc[o]});
            for(auto&d:pr[bp[o]].occ){int rr=RM(br[o]+d.first),cc=CM(bc[o]+d.second);addCell(rr*Q+cc);}}
        for(int o:rem)removeB(o);
        for(int cell:wcells)addCell(cell);
        sort(keyed.begin(),keyed.end());
        vector<int> cells;for(auto&k:keyed)cells.push_back(k.second);
        double alpha=0.6+0.8*urand(); double beta=0.5*urand();
        vector<int> added; refill(cells,alpha,beta,added);
        if(score<old){
            for(int id:added)removeB(id);
            for(auto&ri:remInfo)place(ri[0],ri[1],ri[2]);
        }
    }
    // output
    string out; long long N=0; string body;
    for(size_t id=0;id<bp.size();id++){
        if(!alive[id])continue;
        int p=bp[id];const Proj&pp=pr[p];
        for(int R=br[id];R+pp.h<=H;R+=P){
            for(int C=bc[id];C+pp.w<=W;C+=Q){
                N++; body+=to_string(p);body+=' ';body+=to_string(R);body+=' ';body+=to_string(C);body+='\n';
                if(!wrapC)break;
            }
            if(!wrapR)break;
        }
    }
    out=to_string(N)+"\n";
    fwrite(out.data(),1,out.size(),stdout);
    fwrite(body.data(),1,body.size(),stdout);
    return 0;
}
