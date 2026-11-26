#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
size_t bl=0,bp=0;
inline int gc(){ if(bp==bl){ bl=fread(buf,1,sizeof(buf),stdin); bp=0; if(bl==0) return -1;} return buf[bp++]; }
inline long long rd(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } long long x=0; while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); } return x; }

struct Ent{ double sc; long long g; int c,v;
    bool operator<(const Ent&o)const{ if(sc!=o.sc) return sc<o.sc; if(c!=o.c) return c>o.c; return v>o.v; } };

int main(){
    int V=rd(),E=rd(),R=rd(),C=rd(); long long X=rd();
    vector<int> S(V); for(auto&s:S) s=rd();
    vector<int> LD(E); vector<vector<pair<int,int>>> conn(E);
    vector<vector<pair<int,int>>> cacheEnds(C);
    for(int e=0;e<E;e++){ LD[e]=rd(); int K=rd(); for(int k=0;k<K;k++){ int c=rd(),L=rd(); conn[e].push_back({c,L}); cacheEnds[c].push_back({e,L}); } }
    vector<tuple<int,int,long long>> raw; raw.reserve(R);
    for(int i=0;i<R;i++){ int v=rd(),e=rd(); long long n=rd(); raw.emplace_back(v,e,n); }
    sort(raw.begin(),raw.end());
    vector<int> rv,re; vector<long long> rn;
    for(auto&t:raw){ int v=get<0>(t),e=get<1>(t); if(!rv.empty()&&rv.back()==v&&re.back()==e) rn.back()+=get<2>(t); else { rv.push_back(v); re.push_back(e); rn.push_back(get<2>(t)); } }
    raw.clear(); raw.shrink_to_fit();
    int NR=rv.size();
    vector<int> best(NR), bestCache(NR,-1);
    vector<vector<int>> reqsByV(V), reqsByE(E);
    for(int r=0;r<NR;r++){ best[r]=LD[re[r]]; reqsByV[rv[r]].push_back(r); reqsByE[re[r]].push_back(r); }
    vector<int> latEC((size_t)E*C,0);
    for(int e=0;e<E;e++) for(auto&p:conn[e]) latEC[(size_t)e*C+p.first]=p.second;
    size_t CV=(size_t)C*V;
    vector<char> has(CV,0);
    vector<long long> used(C,0);
    vector<vector<int>> contents(C);
    {
        vector<long long> gain(CV,0);
        for(int r=0;r<NR;r++){ int e=re[r],v=rv[r]; for(auto&p:conn[e]) gain[(size_t)p.first*V+v]+=rn[r]*(LD[e]-p.second); }
        vector<Ent> init;
        for(int c=0;c<C;c++) for(int v=0;v<V;v++){ long long g=gain[(size_t)c*V+v]; if(g>0&&S[v]<=X) init.push_back({(double)g/S[v],g,c,v}); }
        priority_queue<Ent> pq(less<Ent>(),std::move(init));
        while(!pq.empty()){
            Ent t=pq.top(); pq.pop();
            size_t idx=(size_t)t.c*V+t.v;
            if(has[idx]) continue;
            if(used[t.c]+S[t.v]>X) continue;
            long long g=gain[idx];
            if(g<=0) continue;
            if(g!=t.g){ pq.push({(double)g/S[t.v],g,t.c,t.v}); continue; }
            int c=t.c,v=t.v;
            has[idx]=1; used[c]+=S[v]; contents[c].push_back(v);
            for(int r:reqsByV[v]){ int e=re[r]; int L=latEC[(size_t)e*C+c]; if(L==0||L>=best[r]) continue;
                int ob=best[r],nb=L;
                for(auto&p:conn[e]){ long long d=(long long)max(0,ob-p.second)-max(0,nb-p.second); if(d) gain[(size_t)p.first*V+v]-=rn[r]*d; }
                best[r]=nb; bestCache[r]=c; }
        }
    }
    // local search: per-cache knapsack
    long long work=0; const long long BUDGET=2500000000LL; const long long MAXCELLS=400000000LL;
    vector<long long> val(V,0);
    vector<int> touched; vector<pair<int,int>> exclBuf;
    vector<uint64_t> keep; vector<long long> dp;
    bool changed=true;
    while(changed && work<BUDGET){
        changed=false;
        for(int c=0;c<C && work<BUDGET;c++){
            if(cacheEnds[c].empty()) continue;
            touched.clear(); exclBuf.clear();
            for(auto&pe:cacheEnds[c]){ int e=pe.first,L=pe.second;
                for(int r:reqsByE[e]){ int v=rv[r]; int ex,exc;
                    if(bestCache[r]!=c){ ex=best[r]; exc=bestCache[r]; }
                    else { ex=LD[e]; exc=-1; for(auto&p:conn[e]) if(p.first!=c&&p.second<ex&&has[(size_t)p.first*V+v]){ ex=p.second; exc=p.first; } work+=conn[e].size(); }
                    work++;
                    exclBuf.push_back({ex,exc});
                    if(ex>L){ if(val[v]==0) touched.push_back(v); val[v]+=rn[r]*(ex-L); } } }
            long long curVal=0; for(int v:contents[c]) curVal+=val[v];
            vector<int> items; long long tot=0;
            for(int v:touched) if(S[v]<=X){ items.push_back(v); tot+=S[v]; }
            vector<int> sel; long long newVal=0; bool ok=false;
            if(tot<=X){ sel=items; for(int v:sel) newVal+=val[v]; ok=true; }
            else {
                long long cap=X; size_t n=items.size(); long long cells=(long long)n*(cap+1);
                if(cells<=MAXCELLS && work+cells<=BUDGET){
                    work+=cells;
                    size_t W=(cap+1+63)/64;
                    keep.assign(n*W,0); dp.assign(cap+1,0);
                    for(size_t i=0;i<n;i++){ int w=S[items[i]]; long long vv=val[items[i]]; uint64_t* kr=&keep[i*W];
                        for(long long j=cap;j>=w;j--){ long long cand=dp[j-w]+vv; if(cand>dp[j]){ dp[j]=cand; kr[j>>6]|=1ULL<<(j&63); } } }
                    newVal=dp[cap]; long long j=cap;
                    for(size_t i=n;i-->0;){ if(keep[i*W+(j>>6)]>>(j&63)&1){ sel.push_back(items[i]); j-=S[items[i]]; } }
                    ok=true;
                }
            }
            if(ok && newVal>curVal){
                changed=true;
                for(int v:contents[c]) has[(size_t)c*V+v]=0;
                used[c]=0; contents[c]=sel;
                for(int v:sel){ has[(size_t)c*V+v]=1; used[c]+=S[v]; }
                size_t idx=0;
                for(auto&pe:cacheEnds[c]){ int L=pe.second;
                    for(int r:reqsByE[pe.first]){ auto ex=exclBuf[idx++]; int v=rv[r];
                        if(has[(size_t)c*V+v]&&L<ex.first){ best[r]=L; bestCache[r]=c; }
                        else { best[r]=ex.first; bestCache[r]=ex.second; } } }
            }
            for(int v:touched) val[v]=0;
        }
    }
    string out; int N=0;
    for(int c=0;c<C;c++) if(!contents[c].empty()) N++;
    out+=to_string(N)+"\n";
    for(int c=0;c<C;c++) if(!contents[c].empty()){ out+=to_string(c); for(int v:contents[c]){ out+=' '; out+=to_string(v);} out+='\n'; }
    fwrite(out.data(),1,out.size(),stdout);
}
