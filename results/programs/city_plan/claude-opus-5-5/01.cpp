#include <bits/stdc++.h>
using namespace std;

struct Proj{bool res;int h,w,val,fr,fc,area;vector<pair<int,int>> cells,reach;};
int H,W,D,B,maxType=0;
vector<Proj> P;
long long work=0;
auto T0=chrono::steady_clock::now();
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}
struct XR{uint64_t s=88172645463325252ULL;
 uint64_t next(){s^=s<<13;s^=s>>7;s^=s<<17;return s;}
 double rnd(){return (next()>>11)*(1.0/9007199254740992.0);}
 int ri(int n){return (int)(next()%(uint64_t)n);}};
const int OFF=200;

struct Board{
 int h,w; bool torus; int MA;
 vector<int> owner,mr,mc;
 vector<int> bp,br,bc,bval; vector<char> alive,act,bres;
 vector<vector<pair<int,int>>> cnt;
 vector<int> freeIds,stampB; int stamp=0;
 vector<int> stampT; int tstamp=0;
 long long score=0;
 void init(int h_,int w_,bool t,int ma){
  h=h_;w=w_;torus=t;MA=ma;owner.assign((size_t)h*w,-1);
  if(torus){mr.resize(h+2*OFF);mc.resize(w+2*OFF);
   for(int x=-OFF;x<h+OFF;x++)mr[x+OFF]=((x%h)+h)%h;
   for(int x=-OFF;x<w+OFF;x++)mc[x+OFF]=((x%w)+w)%w;}
  stampT.assign(maxType+2,0);
 }
 inline int idx(int r,int c)const{
  if(torus)return mr[r+OFF]*w+mc[c+OFF];
  if(r<0||r>=h||c<0||c>=w)return -1;
  return r*w+c;
 }
 bool nearBorder(int p,int r,int c,int m){
  if(MA<0)return true;const Proj&q=P[p];
  return r<=m||c<=m||r+q.h-1>=h-1-m||c+q.w-1>=w-1-m;
 }
 int newId(){
  if(!freeIds.empty()){int id=freeIds.back();freeIds.pop_back();return id;}
  int id=bp.size();bp.push_back(0);br.push_back(0);bc.push_back(0);bval.push_back(0);
  alive.push_back(0);act.push_back(0);bres.push_back(0);cnt.emplace_back();stampB.push_back(0);return id;
 }
 bool fits(int p,int r,int c){
  const Proj&q=P[p];
  for(auto&e:q.cells){work++;int k=idx(r+e.first,c+e.second);if(k<0||owner[k]!=-1)return false;}
  return true;
 }
 int place(int p,int r,int c){
  int id=newId();const Proj&q=P[p];
  bp[id]=p;br[id]=r;bc[id]=c;alive[id]=1;bres[id]=q.res;bval[id]=q.val;
  for(auto&e:q.cells)owner[idx(r+e.first,c+e.second)]=id;
  work+=q.cells.size();
  if(q.res){
   act[id]=nearBorder(p,r,c,MA);
   if(act[id]){
    ++stamp;auto&v=cnt[id];v.clear();work+=q.reach.size();
    for(auto&e:q.reach){int k=idx(r+e.first,c+e.second);if(k<0)continue;int o=owner[k];
     if(o<0||bres[o]||stampB[o]==stamp)continue;stampB[o]=stamp;int t=bval[o];bool f=false;
     for(auto&pr:v)if(pr.first==t){pr.second++;f=true;break;}
     if(!f)v.push_back({t,1});}
    score+=(long long)q.val*v.size();
   }
  }else{
   act[id]=nearBorder(p,r,c,MA+D+50);
   if(act[id]){
    ++stamp;int t=q.val;work+=q.reach.size();
    for(auto&e:q.reach){int k=idx(r+e.first,c+e.second);if(k<0)continue;int o=owner[k];
     if(o<0||!bres[o]||!act[o]||stampB[o]==stamp)continue;stampB[o]=stamp;
     auto&v=cnt[o];bool f=false;
     for(auto&pr:v)if(pr.first==t){pr.second++;f=true;break;}
     if(!f){v.push_back({t,1});score+=bval[o];}}
   }
  }
  return id;
 }
 void remove(int id){
  const Proj&q=P[bp[id]];int r=br[id],c=bc[id];
  if(q.res){if(act[id]){score-=(long long)q.val*cnt[id].size();cnt[id].clear();}}
  else if(act[id]){
   ++stamp;int t=q.val;work+=q.reach.size();
   for(auto&e:q.reach){int k=idx(r+e.first,c+e.second);if(k<0)continue;int o=owner[k];
    if(o<0||!bres[o]||!act[o]||stampB[o]==stamp)continue;stampB[o]=stamp;
    auto&v=cnt[o];
    for(size_t i=0;i<v.size();i++)if(v[i].first==t){
     if(--v[i].second==0){v[i]=v.back();v.pop_back();score-=bval[o];}break;}
   }
  }
  for(auto&e:q.cells)owner[idx(r+e.first,c+e.second)]=-1;
  alive[id]=0;freeIds.push_back(id);
 }
 int evalRes(int p,int r,int c){
  const Proj&q=P[p];++tstamp;int n=0;work+=q.reach.size();
  for(auto&e:q.reach){int k=idx(r+e.first,c+e.second);if(k<0)continue;int o=owner[k];
   if(o<0||bres[o])continue;int t=bval[o];if(stampT[t]!=tstamp){stampT[t]=tstamp;n++;}}
  return n;
 }
 long long evalUtil(int p,int r,int c){
  const Proj&q=P[p];int t=q.val;++stamp;long long g=0;work+=q.reach.size();
  for(auto&e:q.reach){int k=idx(r+e.first,c+e.second);if(k<0)continue;int o=owner[k];
   if(o<0||!bres[o]||!act[o]||stampB[o]==stamp)continue;stampB[o]=stamp;
   bool f=false;for(auto&pr:cnt[o])if(pr.first==t){f=true;break;}
   if(!f)g+=bval[o];}
  return g;
 }
};

void fillB(Board&b,const vector<int>&order,const vector<int>&cands,double lam,double noise,XR&rng,vector<int>*placed,double avgCap){
 for(size_t i=0;i<order.size();i++){
  if((i&255)==0&&elapsed()>9.2)return;
  int code=order[i];if(b.owner[code]!=-1)continue;
  int r=code/b.w,c=code%b.w;
  int best=-1,bR=0,bC=0;double bv=-1;
  for(int p:cands){const Proj&q=P[p];int r0=r-q.fr,c0=c-q.fc;
   if(b.torus){r0=b.mr[r0+OFF];c0=b.mc[c0+OFF];}
   else if(r0<0||c0<0||r0+q.h>b.h||c0+q.w>b.w)continue;
   if(!b.fits(p,r0,c0))continue;
   double v;
   if(q.res){int n=b.evalRes(p,r0,c0);v=q.val*(n+lam)/q.area;}
   else{long long g=b.evalUtil(p,r0,c0);v=(g+0.01*avgCap)/q.area;}
   if(noise>0)v*=1+noise*rng.rnd();
   if(v>bv){bv=v;best=p;bR=r0;bC=c0;}
  }
  if(best>=0){int id=b.place(best,bR,bC);if(placed)placed->push_back(id);}
 }
}

struct Cands{vector<int> c;bool ok=false;double avgCap=1;};
Cands getCands(int mh,int mw){
 vector<int> rs,us;
 for(int i=0;i<B;i++)if(P[i].h<=mh&&P[i].w<=mw)(P[i].res?rs:us).push_back(i);
 Cands C;C.ok=!rs.empty()&&!us.empty();if(!C.ok)return C;
 vector<int> out;
 auto add=[&](int i){if(find(out.begin(),out.end(),i)==out.end())out.push_back(i);};
 auto a=rs;
 sort(a.begin(),a.end(),[](int x,int y){long long l=(long long)P[x].val*P[y].area,r=(long long)P[y].val*P[x].area;if(l!=r)return l>r;return x<y;});
 for(int i=0;i<(int)a.size()&&i<8;i++)add(a[i]);
 a=rs;
 sort(a.begin(),a.end(),[](int x,int y){long long l=(long long)P[x].val*P[y].h*P[y].w,r=(long long)P[y].val*P[x].h*P[x].w;if(l!=r)return l>r;return x<y;});
 for(int i=0;i<(int)a.size()&&i<2;i++)add(a[i]);
 a=rs;
 sort(a.begin(),a.end(),[](int x,int y){if(P[x].area!=P[y].area)return P[x].area<P[y].area;if(P[x].val!=P[y].val)return P[x].val>P[y].val;return x<y;});
 for(int i=0;i<(int)a.size()&&i<3;i++)add(a[i]);
 double s=0;for(int i:out)s+=P[i].val;C.avgCap=s/out.size();
 map<int,int> bt;
 for(int u:us){int t=P[u].val;auto it=bt.find(t);
  if(it==bt.end())bt[t]=u;
  else{int o=it->second;if(P[u].area<P[o].area||(P[u].area==P[o].area&&P[u].h*P[u].w<P[o].h*P[o].w))it->second=u;}}
 vector<int> ub;for(auto&kv:bt)ub.push_back(kv.second);
 sort(ub.begin(),ub.end(),[](int x,int y){if(P[x].area!=P[y].area)return P[x].area<P[y].area;if(P[x].h*P[x].w!=P[y].h*P[y].w)return P[x].h*P[x].w<P[y].h*P[y].w;return x<y;});
 for(int i=0;i<(int)ub.size()&&i<25;i++)add(ub[i]);
 C.c=out;return C;
}

template<class F>
void lns(Board&b,const Cands&C,double lam,long long workLimit,double timeLimit,XR&rng,int maxWin,F sampler){
 vector<int> ids,placed;vector<array<int,3>> rem;vector<pair<long long,int>> cl;vector<int> order;
 int it=0;
 while(work<workLimit&&elapsed()<timeLimit){
  it++;
  int sh=2+rng.ri(max(1,maxWin-1)),sw=2+rng.ri(max(1,maxWin-1));
  if(b.torus){sh=min(sh,b.h);sw=min(sw,b.w);}
  int r0,c0;sampler(r0,c0,sh,sw);
  ids.clear();cl.clear();
  int shift=b.torus?min(50,b.h/3):0,shiftc=b.torus?min(50,b.w/3):0;
  auto keyOf=[&](int k)->long long{
   if(!b.torus)return k;int r=k/b.w,c=k%b.w;
   return (long long)b.mr[r-r0+shift+OFF]*b.w+b.mc[c-c0+shiftc+OFF];};
  for(int i=0;i<sh;i++)for(int j=0;j<sw;j++){
   int k=b.idx(r0+i,c0+j);if(k<0)continue;
   cl.push_back({keyOf(k),k});
   int o=b.owner[k];if(o<0||!b.act[o])continue;
   if(find(ids.begin(),ids.end(),o)==ids.end())ids.push_back(o);}
  long long old=b.score;
  rem.clear();
  for(int id:ids){const Proj&q=P[b.bp[id]];
   for(auto&e:q.cells){int k=b.idx(b.br[id]+e.first,b.bc[id]+e.second);cl.push_back({keyOf(k),k});}
   rem.push_back({b.bp[id],b.br[id],b.bc[id]});b.remove(id);}
  sort(cl.begin(),cl.end());cl.erase(unique(cl.begin(),cl.end()),cl.end());
  order.clear();for(auto&x:cl)order.push_back(x.second);
  placed.clear();
  double lamI=lam*exp(rng.rnd()-0.5);
  fillB(b,order,C.c,lamI,0.2,rng,&placed,C.avgCap);
  if(b.score>=old)continue;
  for(int id:placed)b.remove(id);
  for(auto&x:rem)b.place(x[0],x[1],x[2]);
 }
}

int main(){
 ios::sync_with_stdio(false);cin.tie(nullptr);
 cin>>H>>W>>D>>B;P.resize(B);
 for(int i=0;i<B;i++){
  char t;int h,w,v;cin>>t>>h>>w>>v;Proj&q=P[i];q.res=(t=='R');q.h=h;q.w=w;q.val=v;
  if(!q.res)maxType=max(maxType,v);
  vector<string> g(h);for(auto&s:g)cin>>s;
  for(int r=0;r<h;r++)for(int c=0;c<w;c++)if(g[r][c]=='#')q.cells.push_back({r,c});
  q.area=q.cells.size();q.fr=q.cells[0].first;q.fc=q.cells[0].second;
  int GH=h+2*D,GW=w+2*D;vector<int> dist(GH*GW,-1);deque<int> dq;
  for(auto&e:q.cells){int k=(e.first+D)*GW+e.second+D;dist[k]=0;dq.push_back(k);}
  while(!dq.empty()){int k=dq.front();dq.pop_front();if(dist[k]>=D)continue;int r=k/GW,c=k%GW;
   const int dr[4]={1,-1,0,0},dc[4]={0,0,1,-1};
   for(int d=0;d<4;d++){int nr=r+dr[d],nc=c+dc[d];if(nr<0||nc<0||nr>=GH||nc>=GW)continue;int nk=nr*GW+nc;
    if(dist[nk]<0){dist[nk]=dist[k]+1;dq.push_back(nk);}}}
  for(int k=0;k<GH*GW;k++)if(dist[k]>=1)q.reach.push_back({k/GW-D,k%GW-D});
 }
 XR rng;
 bool direct=(long long)H*W<=40000;
 Board bestB;double bestDens=-1,bestLam=1;int bestT=-1;Cands bestC;
 vector<double> lams={0.3,1.0,3.0};
 if(!direct){
  vector<int> Ts={4,5,6,8,10,12,14,16,20,24,28,32,40,48,56,64};
  bool stop=false;
  for(int T:Ts){
   if(stop)break;if(T>H||T>W)continue;
   Cands C=getCands(T,T);if(!C.ok)continue;
   for(double lam:lams){
    if(bestT>=0&&(work>300000000LL||elapsed()>2.5)){stop=true;break;}
    Board b;b.init(T,T,true,-1);vector<int> ord(T*T);iota(ord.begin(),ord.end(),0);
    fillB(b,ord,C.c,lam,0,rng,nullptr,C.avgCap);
    double dens=(double)b.score/(T*T);
    if(dens>=bestDens){bestDens=dens;bestB=b;bestLam=lam;bestT=T;bestC=C;}
   }
  }
  if(bestT<0)direct=true;
 }
 Board R;
 if(!direct){
  int T=bestT;
  lns(bestB,bestC,bestLam,1000000000LL,6.0,rng,min(12,T),[&](int&r0,int&c0,int,int){r0=rng.ri(T);c0=rng.ri(T);});
  R.init(H,W,false,2*D+100);
  for(int I=0;I*T<H;I++)for(int J=0;J*T<W;J++){
   if(elapsed()>8.8)break;
   for(size_t id=0;id<bestB.bp.size();id++){if(!bestB.alive[id])continue;
    int p=bestB.bp[id];int r=I*T+bestB.br[id],c=J*T+bestB.bc[id];
    if(r+P[p].h>H||c+P[p].w>W)continue;
    if(R.fits(p,r,c))R.place(p,r,c);}
  }
  Cands G=getCands(H,W);
  vector<int> ord((size_t)H*W);iota(ord.begin(),ord.end(),0);
  fillB(R,ord,G.c,bestLam,0,rng,nullptr,G.avgCap);
  int MW=D+20;
  lns(R,G,bestLam,1600000000LL,8.5,rng,12,[&](int&r0,int&c0,int sh,int sw){
   int side=rng.ri(4),depth=rng.ri(MW);
   if(side==0){r0=depth-rng.ri(sh);c0=rng.ri(W)-rng.ri(sw);}
   else if(side==1){r0=H-1-depth-rng.ri(sh);c0=rng.ri(W)-rng.ri(sw);}
   else if(side==2){c0=depth-rng.ri(sw);r0=rng.ri(H)-rng.ri(sh);}
   else{c0=W-1-depth-rng.ri(sw);r0=rng.ri(H)-rng.ri(sh);}
  });
 }else{
  Cands G=getCands(H,W);
  vector<int> ord((size_t)H*W);iota(ord.begin(),ord.end(),0);
  long long bs=-1;
  for(double lam:lams){
   if(bs>=0&&elapsed()>3.0)break;
   Board b;b.init(H,W,false,-1);fillB(b,ord,G.c,lam,0,rng,nullptr,G.avgCap);
   if(b.score>bs){bs=b.score;R=b;bestLam=lam;}
  }
  lns(R,G,bestLam,1500000000LL,8.5,rng,12,[&](int&r0,int&c0,int sh,int sw){r0=rng.ri(H)-rng.ri(sh);c0=rng.ri(W)-rng.ri(sw);});
 }
 string out;size_t n=0;
 for(size_t id=0;id<R.bp.size();id++)if(R.alive[id])n++;
 out+=to_string(n)+"\n";
 for(size_t id=0;id<R.bp.size();id++)if(R.alive[id]){
  out+=to_string(R.bp[id]);out+=' ';out+=to_string(R.br[id]);out+=' ';out+=to_string(R.bc[id]);out+='\n';}
 fwrite(out.data(),1,out.size(),stdout);
 return 0;
}
