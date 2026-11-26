#include <bits/stdc++.h>
using namespace std;

struct P{bool res;int h,w,v;vector<string> g;};
struct C{int orig;bool res;int h,w,cap,t,area,anc;vector<int> odr,odc,ddr,ddc;};

int H,W,D,B;
vector<P> allp;
vector<C> cand;
int Lh,Lw;bool perR,perC;int NC,T,Wd;
vector<int> owner;vector<uint16_t> cntv;vector<uint64_t> bits;
long long score=0;long long ops=0;
vector<int> bP,bR,bC;vector<char> alive;vector<uint64_t> hasT;vector<int> stampB;int stampCur=0;
vector<int> freeIds;

inline int occIdx(int r,int c,int dr,int dc){int rr=r+dr;if(rr>=Lh)rr-=Lh;int cc=c+dc;if(cc>=Lw)cc-=Lw;return rr*Lw+cc;}
inline int dilIdx(int r,int c,int dr,int dc){
    int rr=r+dr; if(perR){if(rr>=Lh)rr-=Lh;} else if(rr<0||rr>=Lh) return -1;
    int cc=c+dc; if(perC){if(cc>=Lw)cc-=Lw;} else if(cc<0||cc>=Lw) return -1;
    return rr*Lw+cc;}

int newId(){
    if(!freeIds.empty()){int id=freeIds.back();freeIds.pop_back();return id;}
    int id=bP.size();bP.push_back(0);bR.push_back(0);bC.push_back(0);alive.push_back(0);
    hasT.resize(hasT.size()+Wd,0);stampB.push_back(0);return id;}

bool canPlace(int q,int r,int c){
    C&cd=cand[q];
    for(int k=0;k<cd.area;k++){ops++;if(owner[occIdx(r,c,cd.odr[k],cd.odc[k])]!=-1)return false;}
    return true;}
long long evalRes(int q,int r,int c){
    C&cd=cand[q];uint64_t tmp[64];for(int k=0;k<Wd;k++)tmp[k]=0;
    for(int k=0;k<cd.area;k++){int idx=occIdx(r,c,cd.odr[k],cd.odc[k]);const uint64_t*b=&bits[(size_t)idx*Wd];for(int x=0;x<Wd;x++)tmp[x]|=b[x];}
    ops+=(long long)cd.area*Wd;
    int pc=0;for(int x=0;x<Wd;x++)pc+=__builtin_popcountll(tmp[x]);
    return (long long)pc*cd.cap;}
long long evalUtil(int q,int r,int c){
    C&cd=cand[q];int t=cd.t;++stampCur;long long g=0;
    size_t n=cd.ddr.size();ops+=n;
    for(size_t k=0;k<n;k++){int idx=dilIdx(r,c,cd.ddr[k],cd.ddc[k]);if(idx<0)continue;int o=owner[idx];
        if(o>=0&&stampB[o]!=stampCur){stampB[o]=stampCur;C&oc=cand[bP[o]];if(oc.res){if(!((hasT[(size_t)o*Wd+(t>>6)]>>(t&63))&1))g+=oc.cap;}}}
    return g;}
int addB(int q,int r,int c){
    int id=newId();bP[id]=q;bR[id]=r;bC[id]=c;alive[id]=1;C&cd=cand[q];
    for(int k=0;k<cd.area;k++)owner[occIdx(r,c,cd.odr[k],cd.odc[k])]=id;
    if(cd.res){uint64_t*h=&hasT[(size_t)id*Wd];for(int x=0;x<Wd;x++)h[x]=0;
        for(int k=0;k<cd.area;k++){int idx=occIdx(r,c,cd.odr[k],cd.odc[k]);const uint64_t*b=&bits[(size_t)idx*Wd];for(int x=0;x<Wd;x++)h[x]|=b[x];}
        int pc=0;for(int x=0;x<Wd;x++)pc+=__builtin_popcountll(h[x]);score+=(long long)pc*cd.cap;ops+=(long long)cd.area*Wd;}
    else{int t=cd.t;++stampCur;uint64_t bit=1ull<<(t&63);int wi=t>>6;size_t n=cd.ddr.size();ops+=n;
        for(size_t k=0;k<n;k++){int idx=dilIdx(r,c,cd.ddr[k],cd.ddc[k]);if(idx<0)continue;
            if(cntv[(size_t)idx*T+t]++==0)bits[(size_t)idx*Wd+wi]|=bit;
            int o=owner[idx];if(o>=0&&stampB[o]!=stampCur){stampB[o]=stampCur;C&oc=cand[bP[o]];if(oc.res){uint64_t&w=hasT[(size_t)o*Wd+wi];if(!(w&bit)){w|=bit;score+=oc.cap;}}}}}
    return id;}
vector<int> tmpNear;
void removeB(int id){
    int q=bP[id],r=bR[id],c=bC[id];C&cd=cand[q];
    for(int k=0;k<cd.area;k++)owner[occIdx(r,c,cd.odr[k],cd.odc[k])]=-1;
    if(cd.res){uint64_t*h=&hasT[(size_t)id*Wd];int pc=0;for(int x=0;x<Wd;x++)pc+=__builtin_popcountll(h[x]);score-=(long long)pc*cd.cap;}
    else{int t=cd.t;++stampCur;uint64_t bit=1ull<<(t&63);int wi=t>>6;size_t n=cd.ddr.size();ops+=n;tmpNear.clear();
        for(size_t k=0;k<n;k++){int idx=dilIdx(r,c,cd.ddr[k],cd.ddc[k]);if(idx<0)continue;
            if(--cntv[(size_t)idx*T+t]==0)bits[(size_t)idx*Wd+wi]&=~bit;
            int o=owner[idx];if(o>=0&&stampB[o]!=stampCur){stampB[o]=stampCur;if(cand[bP[o]].res)tmpNear.push_back(o);}}
        for(int o:tmpNear){uint64_t&w=hasT[(size_t)o*Wd+wi];if(!(w&bit))continue;C&oc=cand[bP[o]];bool f=false;
            for(int k=0;k<oc.area&&!f;k++){int idx=occIdx(bR[o],bC[o],oc.odr[k],oc.odc[k]);if(bits[(size_t)idx*Wd+wi]&bit)f=true;}
            ops+=oc.area;if(!f){w&=~bit;score-=oc.cap;}}}
    alive[id]=0;freeIds.push_back(id);}

void recreate(const vector<int>&cells,double beta,vector<int>*added){
    int nq=cand.size();
    for(int idx:cells){if(owner[idx]!=-1)continue;int ar=idx/Lw,ac=idx%Lw;
        double bestV=0;int bq=-1,br=0,bc=0;
        for(int q=0;q<nq;q++){C&cd=cand[q];int r=ar,c=ac-cd.anc;
            if(perC){if(c<0)c+=Lw;}else{if(c<0||c+cd.w>Lw)continue;}
            if(!perR&&r+cd.h>Lh)continue;
            if(cd.res){double ub=cd.cap*(T+beta)/cd.area;if(ub<=bestV)continue;}
            if(!canPlace(q,r,c))continue;
            double v;if(cd.res)v=(evalRes(q,r,c)+beta*cd.cap)/cd.area;else v=evalUtil(q,r,c)/(double)cd.area;
            if(v>bestV+1e-12){bestV=v;bq=q;br=r;bc=c;}}
        if(bq>=0){int id=addB(bq,br,bc);if(added)added->push_back(id);}}}

int chooseL(int S){int best=50,bw=INT_MAX;for(int L=50;L<=70;L++){int w=S%L;if(w<bw){bw=w;best=L;}}return best;}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d",&H,&W,&D,&B)!=4)return 0;
    allp.resize(B);char buf[64];
    for(int i=0;i<B;i++){char tp[4];scanf("%s %d %d %d",tp,&allp[i].h,&allp[i].w,&allp[i].v);allp[i].res=tp[0]=='R';
        allp[i].g.resize(allp[i].h);static char line[128];for(int r=0;r<allp[i].h;r++){scanf("%s",line);allp[i].g[r]=line;}}
    (void)buf;
    auto areaOf=[&](int i){int a=0;for(auto&s:allp[i].g)for(char ch:s)if(ch=='#')a++;return a;};
    // dedupe
    map<pair<vector<string>,int>,int> resMap; map<pair<vector<string>,int>,int> utMap;
    for(int i=0;i<B;i++){if(allp[i].res){auto key=make_pair(allp[i].g,0);auto it=resMap.find(key);if(it==resMap.end()||allp[it->second].v<allp[i].v)resMap[key]=i;}
        else{auto key=make_pair(allp[i].g,allp[i].v);if(!utMap.count(key))utMap[key]=i;}}
    vector<int> resL,utL;for(auto&kv:resMap)resL.push_back(kv.second);for(auto&kv:utMap)utL.push_back(kv.second);
    vector<int> area(B);for(int i=0;i<B;i++)area[i]=areaOf(i);
    sort(resL.begin(),resL.end(),[&](int a,int b){double da=(double)allp[a].v/area[a],db=(double)allp[b].v/area[b];if(da!=db)return da>db;return a<b;});
    vector<int> resK;set<int> rs;for(int i=0;i<(int)resL.size()&&i<30;i++){resK.push_back(resL[i]);rs.insert(resL[i]);}
    {vector<int> s2=resL;stable_sort(s2.begin(),s2.end(),[&](int a,int b){return area[a]<area[b];});int cnt=0;
        for(int x:s2){if(cnt>=10)break;if(!rs.count(x)){resK.push_back(x);rs.insert(x);cnt++;}}}
    sort(resK.begin(),resK.end(),[&](int a,int b){double da=(double)allp[a].v/area[a],db=(double)allp[b].v/area[b];if(da!=db)return da>db;return a<b;});
    map<int,vector<int>> byType;for(int x:utL)byType[allp[x].v].push_back(x);
    vector<int> utK;for(auto&kv:byType){auto v=kv.second;stable_sort(v.begin(),v.end(),[&](int a,int b){return area[a]<area[b];});for(int i=0;i<(int)v.size()&&i<2;i++)utK.push_back(v[i]);}
    if(utK.size()>60){stable_sort(utK.begin(),utK.end(),[&](int a,int b){return area[a]<area[b];});utK.resize(60);}
    map<int,int> tmap;for(int x:utK)if(!tmap.count(allp[x].v)){int k=tmap.size();tmap[allp[x].v]=k;}
    T=max(1,(int)tmap.size());Wd=(T+63)/64;
    perR=H>100;perC=W>100;Lh=perR?chooseL(H):H;Lw=perC?chooseL(W):W;NC=Lh*Lw;
    auto build=[&](int i){C cd;cd.orig=i;cd.res=allp[i].res;cd.h=allp[i].h;cd.w=allp[i].w;cd.cap=cd.res?allp[i].v:0;cd.t=cd.res?0:tmap[allp[i].v];
        cd.anc=-1;for(int r=0;r<cd.h;r++)for(int c=0;c<cd.w;c++)if(allp[i].g[r][c]=='#'){cd.odr.push_back(r);cd.odc.push_back(c);if(r==0&&cd.anc<0)cd.anc=c;}
        cd.area=cd.odr.size();
        if(!cd.res){int GH=cd.h+2*D,GW=cd.w+2*D;vector<int> dist(GH*GW,INT_MAX);deque<int> dq;
            for(int k=0;k<cd.area;k++){int id=(cd.odr[k]+D)*GW+cd.odc[k]+D;dist[id]=0;dq.push_back(id);}
            while(!dq.empty()){int u=dq.front();dq.pop_front();if(dist[u]>=D)continue;int ur=u/GW,uc=u%GW;int dr4[4]={-1,1,0,0},dc4[4]={0,0,-1,1};
                for(int d=0;d<4;d++){int nr=ur+dr4[d],nc=uc+dc4[d];if(nr<0||nc<0||nr>=GH||nc>=GW)continue;int v=nr*GW+nc;if(dist[v]>dist[u]+1){dist[v]=dist[u]+1;dq.push_back(v);}}}
            vector<long long> keys;
            for(int a=0;a<GH;a++)for(int b=0;b<GW;b++)if(dist[a*GW+b]<=D){int dr=a-D,dc=b-D;if(perR)dr=((dr%Lh)+Lh)%Lh;if(perC)dc=((dc%Lw)+Lw)%Lw;
                keys.push_back((long long)(dr+100000)*400000+(dc+100000));}
            sort(keys.begin(),keys.end());keys.erase(unique(keys.begin(),keys.end()),keys.end());
            for(long long k:keys){cd.ddr.push_back((int)(k/400000)-100000);cd.ddc.push_back((int)(k%400000)-100000);}}
        cand.push_back(cd);};
    for(int x:resK)build(x);for(int x:utK)build(x);
    int maxH=0,maxW=0;for(auto&c:cand){maxH=max(maxH,c.h);maxW=max(maxW,c.w);}
    owner.assign(NC,-1);cntv.assign((size_t)NC*T,0);bits.assign((size_t)NC*Wd,0);
    // initial
    {vector<int> cells(NC);iota(cells.begin(),cells.end(),0);recreate(cells,0.5,nullptr);}
    auto snapshot=[&](){vector<array<int,3>> s;for(size_t i=0;i<bP.size();i++)if(alive[i])s.push_back({bP[i],bR[i],bC[i]});return s;};
    long long bestScore=score;auto best=snapshot();
    double avgCap=0;int nr=0;for(auto&c:cand)if(c.res){avgCap+=c.cap;nr++;}avgCap=nr?avgCap/nr:1;
    const long long BUDGET=3000000000LL;
    mt19937 rng(12345);
    vector<int> cellStamp(NC,0);int csCur=0;vector<int> cells;vector<array<int,3>> removed;vector<int> added;
    long long iter=0;
    while(true){
        iter++;
        if((iter&31)==0){double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();if(el>9.3)break;}
        if(ops>BUDGET)break;
        double prog=(double)ops/BUDGET;double Temp=avgCap*1.0*(1-prog)*(1-prog);
        int hR=1+rng()%min(Lh,10),wR=1+rng()%min(Lw,10);int r0=rng()%Lh,c0=rng()%Lw;
        ++csCur;cells.clear();removed.clear();added.clear();
        long long old=score;int upR=0,upC=0;
        for(int i=0;i<hR;i++){int rr=r0+i;if(perR){if(rr>=Lh)rr-=Lh;}else if(rr>=Lh)break;
            for(int j=0;j<wR;j++){int cc=c0+j;if(perC){if(cc>=Lw)cc-=Lw;}else if(cc>=Lw)break;
                int idx=rr*Lw+cc;if(cellStamp[idx]!=csCur){cellStamp[idx]=csCur;cells.push_back(idx);}
                int o=owner[idx];if(o>=0){C&cd=cand[bP[o]];removed.push_back({bP[o],bR[o],bC[o]});
                    int d=((r0-bR[o])%Lh+Lh)%Lh;if(d<cd.h)upR=max(upR,d);
                    int e=((c0-bC[o])%Lw+Lw)%Lw;if(e<cd.w)upC=max(upC,e);
                    for(int k=0;k<cd.area;k++){int ci=occIdx(bR[o],bC[o],cd.odr[k],cd.odc[k]);if(cellStamp[ci]!=csCur){cellStamp[ci]=csCur;cells.push_back(ci);}}
                    removeB(o);}}}
        int baseR=perR?(((r0-upR)%Lh)+Lh)%Lh:0,baseC=perC?(((c0-upC)%Lw)+Lw)%Lw:0;
        sort(cells.begin(),cells.end(),[&](int a,int b){int ar=a/Lw,ac=a%Lw,br=b/Lw,bc=b%Lw;
            int ka=(perR?(ar-baseR+Lh)%Lh:ar),kb=(perR?(br-baseR+Lh)%Lh:br);if(ka!=kb)return ka<kb;
            int la=(perC?(ac-baseC+Lw)%Lw:ac),lb=(perC?(bc-baseC+Lw)%Lw:bc);return la<lb;});
        double beta=(rng()%1000)/1000.0;
        recreate(cells,beta,&added);
        long long delta=score-old;bool acc=delta>=0;
        if(!acc&&Temp>1e-9){double u=(rng()%1000000+0.5)/1000000.0;if(u<exp(delta/Temp))acc=true;}
        if(acc){if(score>bestScore){bestScore=score;best=snapshot();}}
        else{for(int i=(int)added.size()-1;i>=0;i--)removeB(added[i]);for(auto&x:removed)addB(x[0],x[1],x[2]);}
    }
    if(score>bestScore){bestScore=score;best=snapshot();}
    // output tiling
    int nTr=perR?(H+Lh-1)/Lh:1,nTc=perC?(W+Lw-1)/Lw:1;
    vector<array<int,3>> out;
    for(int tr=0;tr<nTr;tr++)for(int tc=0;tc<nTc;tc++)for(auto&b:best){C&cd=cand[b[0]];int R=tr*Lh+b[1],Cc=tc*Lw+b[2];
        if(R+cd.h<=H&&Cc+cd.w<=W)out.push_back({cd.orig,R,Cc});}
    string s;s.reserve(out.size()*16+16);s+=to_string(out.size());s+='\n';
    for(auto&o:out){s+=to_string(o[0]);s+=' ';s+=to_string(o[1]);s+=' ';s+=to_string(o[2]);s+='\n';}
    fwrite(s.data(),1,s.size(),stdout);
    return 0;
}
