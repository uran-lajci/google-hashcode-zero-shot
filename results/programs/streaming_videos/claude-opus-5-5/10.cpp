#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline ll readInt(){
    int c=gc();while(c!=-1&&(c<'0'||c>'9'))c=gc();
    ll x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}
    return x;
}

int main(){
    int V=readInt(),E=readInt(),R=readInt(),C=readInt();ll X=readInt();
    vector<int> sz(V);for(auto&s:sz)s=readInt();
    vector<int> LD(E);vector<vector<pair<int,int>>> conn(E);
    vector<int> lat((size_t)E*C,-1);
    vector<vector<pair<int,int>>> cacheEps(C);
    for(int e=0;e<E;e++){
        LD[e]=readInt();int K=readInt();
        for(int k=0;k<K;k++){int c=readInt(),l=readInt();
            if(lat[(size_t)e*C+c]==-1){conn[e].push_back({c,l});lat[(size_t)e*C+c]=l;}
            else if(l<lat[(size_t)e*C+c]){lat[(size_t)e*C+c]=l;for(auto&p:conn[e])if(p.first==c)p.second=l;}
        }
    }
    for(int e=0;e<E;e++)for(auto&p:conn[e])cacheEps[p.first].push_back({e,p.second});
    vector<pair<ll,ll>> raw;raw.reserve(R);
    for(int i=0;i<R;i++){ll v=readInt(),e=readInt(),n=readInt();raw.push_back({v*E+e,n});}
    sort(raw.begin(),raw.end());
    vector<int> rv,re;vector<ll> rn;
    for(size_t i=0;i<raw.size();){
        size_t j=i;ll s=0;while(j<raw.size()&&raw[j].first==raw[i].first){s+=raw[j].second;j++;}
        int v=raw[i].first/E,e=raw[i].first%E;
        if(sz[v]<=X&&!conn[e].empty()){rv.push_back(v);re.push_back(e);rn.push_back(s);}
        i=j;
    }
    int NR=rv.size();
    vector<vector<int>> byV(V),byE(E);
    for(int r=0;r<NR;r++){byV[rv[r]].push_back(r);byE[re[r]].push_back(r);}
    vector<int> cur(NR);for(int r=0;r<NR;r++)cur[r]=LD[re[r]];
    vector<ll> G((size_t)C*V,0);
    for(int r=0;r<NR;r++){int e=re[r],v=rv[r];for(auto&p:conn[e])G[(size_t)p.first*V+v]+=rn[r]*(LD[e]-p.second);}
    struct Ent{double ratio;ll gain;int c,v;};
    auto cmp=[](const Ent&a,const Ent&b){
        if(a.ratio!=b.ratio)return a.ratio<b.ratio;
        if(a.c!=b.c)return a.c>b.c;return a.v>b.v;};
    vector<Ent> init;
    for(int c=0;c<C;c++)for(int v=0;v<V;v++){ll g=G[(size_t)c*V+v];if(g>0)init.push_back({(double)g/sz[v],g,c,v});}
    vector<ll>().swap(G);
    priority_queue<Ent,vector<Ent>,decltype(cmp)> pq(cmp,std::move(init));
    vector<ll> rem(C,X);
    vector<char> has((size_t)C*V,0);
    while(!pq.empty()){
        Ent t=pq.top();pq.pop();
        int c=t.c,v=t.v;
        if(sz[v]>rem[c])continue;
        ll g=0;
        for(int r:byV[v]){int l=lat[(size_t)re[r]*C+c];if(l>=0&&cur[r]>l)g+=rn[r]*(cur[r]-l);}
        if(g<=0)continue;
        if(g!=t.gain){pq.push({(double)g/sz[v],g,c,v});continue;}
        has[(size_t)c*V+v]=1;rem[c]-=sz[v];
        for(int r:byV[v]){int l=lat[(size_t)re[r]*C+c];if(l>=0&&cur[r]>l)cur[r]=l;}
    }
    { decltype(pq) tmp(cmp); pq.swap(tmp); }
    // Local search
    vector<int> b1l(NR),b1c(NR),b2l(NR);
    auto compB=[&](int r){
        int e=re[r],v=rv[r];int a=LD[e],ac=-1,b=LD[e];
        for(auto&p:conn[e])if(has[(size_t)p.first*V+v]){
            if(p.second<a){b=a;a=p.second;ac=p.first;}else if(p.second<b)b=p.second;}
        b1l[r]=a;b1c[r]=ac;b2l[r]=b;
    };
    for(int r=0;r<NR;r++)compB(r);
    vector<ll> gv(V,0);vector<char> touchedF(V,0),changed(V,0);
    vector<int> touched;
    double budget=2.0e9,used=0;
    const double perCache=2.0e7;
    vector<ll> dp;vector<uint64_t> bits;
    for(int pass=0;pass<50&&used<budget;pass++){
        bool improved=false;
        for(int c=0;c<C&&used<budget;c++){
            touched.clear();
            for(auto&pe:cacheEps[c]){int e=pe.first,l=pe.second;
                for(int r:byE[e]){
                    int be=(b1c[r]==c)?b2l[r]:b1l[r];
                    int v=rv[r];
                    if(!touchedF[v]){touchedF[v]=1;touched.push_back(v);gv[v]=0;}
                    if(be>l)gv[v]+=rn[r]*(be-l);
                }
                used+=byE[e].size();
            }
            ll curVal=0;
            vector<int> cand;
            for(int v:touched){
                if(has[(size_t)c*V+v])curVal+=gv[v];
                if(gv[v]>0&&sz[v]<=X)cand.push_back(v);
            }
            sort(cand.begin(),cand.end());
            int n=cand.size();
            if(n>0){
                double cells=(double)n*(X+1);
                ll s=1;if(cells>perCache)s=(ll)ceil(cells/perCache);
                ll cap=X/s;
                vector<int> w(n);for(int i=0;i<n;i++)w[i]=(sz[cand[i]]+s-1)/s;
                size_t W=cap+1;size_t words=(W+63)/64;
                dp.assign(W,0);bits.assign((size_t)n*words,0);
                for(int i=0;i<n;i++){
                    ll g=gv[cand[i]];int wi=w[i];uint64_t*bb=&bits[(size_t)i*words];
                    for(ll x=cap;x>=wi;x--){ll nv=dp[x-wi]+g;if(nv>dp[x]){dp[x]=nv;bb[x>>6]|=1ULL<<(x&63);}}
                }
                used+=(double)n*W;
                if(dp[cap]>curVal){
                    vector<char> newIn(n,0);ll x=cap;
                    for(int i=n-1;i>=0;i--){if((bits[(size_t)i*words+(x>>6)]>>(x&63))&1ULL){newIn[i]=1;x-=w[i];}}
                    // new set: chosen cand
                    vector<int> newSet;for(int i=0;i<n;i++)if(newIn[i])newSet.push_back(cand[i]);
                    vector<int> chg;
                    // remove all current
                    vector<int> oldSet;
                    for(int v:touched)if(has[(size_t)c*V+v])oldSet.push_back(v);
                    // also videos in cache not touched (no requests) -> remove too, harmless
                    for(int v=0;v<V;v++)if(has[(size_t)c*V+v]&&!touchedF[v]){has[(size_t)c*V+v]=0;}
                    for(int v:oldSet){has[(size_t)c*V+v]=0;changed[v]^=1;}
                    for(int v:newSet){has[(size_t)c*V+v]=1;changed[v]^=1;}
                    ll tot=0;for(int v:newSet)tot+=sz[v];rem[c]=X-tot;
                    for(auto&pe:cacheEps[c])for(int r:byE[pe.first])if(changed[rv[r]])compB(r);
                    for(int v:oldSet)changed[v]=0;for(int v:newSet)changed[v]=0;
                    improved=true;
                }
            }
            for(int v:touched)touchedF[v]=0;
        }
        if(!improved)break;
    }
    // output
    string out;vector<string> lines;
    int cnt=0;
    string body;
    for(int c=0;c<C;c++){
        string line=to_string(c);bool any=false;ll tot=0;
        for(int v=0;v<V;v++)if(has[(size_t)c*V+v]){line+=' ';line+=to_string(v);any=true;tot+=sz[v];}
        if(any&&tot<=X){body+=line;body+='\n';cnt++;}
    }
    out=to_string(cnt)+"\n"+body;
    fwrite(out.data(),1,out.size(),stdout);
}
