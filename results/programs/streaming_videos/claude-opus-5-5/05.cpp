#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline ll readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } bool neg=false; if(c=='-'){neg=true;c=gc();} ll x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return neg?-x:x; }

int main(){
    int V=readInt(),E=readInt(),R=readInt(),C=readInt(); ll X=readInt();
    vector<int> sz(V); for(auto&s:sz) s=readInt();
    vector<int> LD(E); vector<vector<pair<int,int>>> conn(E);
    vector<uint16_t> lat((size_t)E*C,0);
    vector<vector<pair<int,int>>> epsOfCache(C);
    for(int e=0;e<E;e++){
        LD[e]=readInt(); int K=readInt();
        for(int k=0;k<K;k++){int c=readInt(),l=readInt();
            if(c<0||c>=C) continue;
            if(lat[(size_t)e*C+c]) continue;
            conn[e].push_back({c,l}); lat[(size_t)e*C+c]=l; epsOfCache[c].push_back({e,l});}
    }
    unordered_map<ll,int> mp; mp.reserve(R*2+10);
    vector<int> rv,re; vector<ll> rn;
    for(int i=0;i<R;i++){
        int v=readInt(),e=readInt(); ll n=readInt();
        ll key=(ll)v*E+e; auto it=mp.find(key);
        if(it==mp.end()){mp[key]=rv.size(); rv.push_back(v); re.push_back(e); rn.push_back(n);}
        else rn[it->second]+=n;
    }
    int NR=rv.size();
    vector<vector<int>> byVid(V), byEp(E);
    for(int r=0;r<NR;r++){byVid[rv[r]].push_back(r); byEp[re[r]].push_back(r);}
    vector<char> has((size_t)C*V,0);
    vector<ll> rem(C,X);
    // greedy
    {
        vector<int> cur(NR); for(int r=0;r<NR;r++) cur[r]=LD[re[r]];
        vector<ll> G((size_t)C*V,0);
        for(int r=0;r<NR;r++){int e=re[r],v=rv[r]; for(auto&p:conn[e]) G[(size_t)p.first*V+v]+=rn[r]*(LD[e]-p.second);}
        vector<tuple<double,int,int>> init;
        for(int c=0;c<C;c++) for(int v=0;v<V;v++){ ll g=G[(size_t)c*V+v]; if(g>0 && sz[v]<=X) init.emplace_back((double)g/sz[v],-c,-v);}
        G.clear(); G.shrink_to_fit();
        priority_queue<tuple<double,int,int>> pq(less<tuple<double,int,int>>(), move(init));
        while(!pq.empty()){
            auto [k,nc,nv]=pq.top(); pq.pop(); int c=-nc,v=-nv;
            if(sz[v]>rem[c]) continue;
            ll g=0;
            for(int r:byVid[v]){int l=lat[(size_t)re[r]*C+c]; if(l&&cur[r]>l) g+=rn[r]*(cur[r]-l);}
            if(g<=0) continue;
            double key=(double)g/sz[v];
            if(pq.empty()||key>=get<0>(pq.top())-1e-9){
                rem[c]-=sz[v]; has[(size_t)c*V+v]=1;
                for(int r:byVid[v]){int l=lat[(size_t)re[r]*C+c]; if(l&&cur[r]>l) cur[r]=l;}
            } else pq.emplace(key,nc,nv);
        }
    }
    // best/second
    vector<int> b1(NR),b1c(NR),b2(NR);
    auto recompute=[&](int r){
        int e=re[r],v=rv[r]; int a=LD[e],ac=-1,b=LD[e];
        for(auto&p:conn[e]) if(has[(size_t)p.first*V+v]){
            int l=p.second; if(l<a){b=a;a=l;ac=p.first;} else if(l<b) b=l;}
        b1[r]=a;b1c[r]=ac;b2[r]=b;
    };
    for(int r=0;r<NR;r++) recompute(r);
    ll work=0; const ll BUDGET=2200000000LL;
    vector<ll> mg(V,0); vector<char> touchedF(V,0); vector<int> touched;
    vector<ll> dp; vector<char> take;
    bool improved=true;
    while(improved && work<BUDGET){
        improved=false;
        for(int c=0;c<C && work<BUDGET;c++){
            touched.clear();
            for(auto&pe:epsOfCache[c]){int e=pe.first,l=pe.second;
                for(int r:byEp[e]){ work++;
                    int ex=(b1c[r]==c)?b2[r]:b1[r];
                    if(ex>l){int v=rv[r]; if(!touchedF[v]){touchedF[v]=1;touched.push_back(v);} mg[v]+=rn[r]*(ex-l);}
                }}
            ll cv=0; vector<int> cand;
            for(int v:touched){ if(has[(size_t)c*V+v]) cv+=mg[v]; if(mg[v]>0&&sz[v]<=X) cand.push_back(v);}
            // check whether current set has members not touched (gain 0) - fine
            sort(cand.begin(),cand.end(),[&](int a,int b){ double ka=(double)mg[a]/sz[a],kb=(double)mg[b]/sz[b]; if(ka!=kb) return ka>kb; return a<b;});
            ll cum=0; size_t cut=0;
            while(cut<cand.size() && cum<2*X+1000){cum+=sz[cand[cut]];cut++;}
            cand.resize(cut);
            vector<int> S;
            ll nvv=0;
            if(cum<=X){ S=cand; for(int v:S) nvv+=mg[v]; }
            else if(!cand.empty()){
                int n=cand.size();
                ll W=min<ll>(X,max<ll>(1,20000000LL/n));
                ll u=(X+W-1)/W; ll cap=X/u;
                dp.assign(cap+1,0); take.assign((size_t)n*(cap+1),0);
                vector<ll> w(n);
                for(int i=0;i<n;i++){ w[i]=(sz[cand[i]]+u-1)/u; ll g=mg[cand[i]];
                    char* tk=&take[(size_t)i*(cap+1)];
                    for(ll j=cap;j>=w[i];j--){ ll t=dp[j-w[i]]+g; if(t>dp[j]){dp[j]=t;tk[j]=1;} }
                    work+=cap; }
                nvv=dp[cap]; ll j=cap;
                for(int i=n-1;i>=0;i--) if(take[(size_t)i*(cap+1)+j]){S.push_back(cand[i]); j-=w[i];}
            }
            for(int v:touched){mg[v]=0;touchedF[v]=0;}
            if(nvv>cv){
                improved=true;
                vector<char> inS; // mark
                vector<int> changed;
                // current members
                vector<int> curM;
                for(int v=0;v<V;v++) if(has[(size_t)c*V+v]) curM.push_back(v);
                work+=V;
                for(int v:S) touchedF[v]=1;
                for(int v:curM) if(!touchedF[v]){has[(size_t)c*V+v]=0;changed.push_back(v);}
                for(int v:S) touchedF[v]=0;
                ll used=0;
                for(int v:S){ if(!has[(size_t)c*V+v]){has[(size_t)c*V+v]=1;changed.push_back(v);} used+=sz[v];}
                rem[c]=X-used;
                for(int v:changed) for(int r:byVid[v]) if(lat[(size_t)re[r]*C+c]){recompute(r); work+=conn[re[r]].size();}
            }
        }
    }
    // output
    string out; int N=0; string body;
    for(int c=0;c<C;c++){
        string line; bool any=false; ll tot=0;
        for(int v=0;v<V;v++) if(has[(size_t)c*V+v]){ tot+=sz[v]; line+=' '; line+=to_string(v); any=true;}
        if(any && tot<=X){ N++; body+=to_string(c)+line+"\n"; }
    }
    out=to_string(N)+"\n"+body;
    fwrite(out.data(),1,out.size(),stdout);
}
