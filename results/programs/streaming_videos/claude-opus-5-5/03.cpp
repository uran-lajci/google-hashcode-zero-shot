#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc();} int s=1; if(c=='-'){s=-1;c=gc();} int x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return x*s; }

int main(){
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    int V=readInt(),E=readInt(),R=readInt(),C=readInt(),X=readInt();
    vector<int> S(V); for(auto&s:S) s=readInt();
    vector<int> LD(E);
    vector<vector<pair<int,int>>> epc(E);
    vector<vector<pair<int,int>>> cacheEps(C);
    vector<int> latEC((size_t)E*C,0);
    for(int e=0;e<E;e++){
        LD[e]=readInt(); int K=readInt();
        for(int k=0;k<K;k++){ int c=readInt(),l=readInt(); if(c<0||c>=C) continue;
            if(latEC[(size_t)e*C+c]) continue;
            latEC[(size_t)e*C+c]=l; epc[e].push_back({c,l}); cacheEps[c].push_back({e,l}); }
    }
    vector<int> rv(R),re(R),rn(R);
    for(int i=0;i<R;i++){ rv[i]=readInt(); re[i]=readInt(); rn[i]=readInt(); }
    vector<vector<int>> reqByVideo(V), reqByEp(E);
    for(int i=0;i<R;i++){ reqByVideo[rv[i]].push_back(i); reqByEp[re[i]].push_back(i); }

    vector<ll> G((size_t)C*V,0);
    vector<char> in((size_t)C*V,0);
    vector<ll> used(C,0);
    vector<int> cur(R);
    for(int r=0;r<R;r++){ int e=re[r]; cur[r]=LD[e];
        for(auto&p:epc[e]) G[(size_t)p.first*V+rv[r]]+=(ll)rn[r]*(LD[e]-p.second); }
    struct Ent{ double ratio; ll g; int c,v; bool operator<(const Ent&o)const{ if(ratio!=o.ratio) return ratio<o.ratio; if(c!=o.c) return c>o.c; return v>o.v;} };
    vector<Ent> heap;
    for(int c=0;c<C;c++) for(int v=0;v<V;v++){ ll g=G[(size_t)c*V+v]; if(g>0&&S[v]<=X) heap.push_back({(double)g/S[v],g,c,v}); }
    make_heap(heap.begin(),heap.end());
    while(!heap.empty()){
        pop_heap(heap.begin(),heap.end()); Ent t=heap.back(); heap.pop_back();
        int c=t.c,v=t.v; size_t idx=(size_t)c*V+v;
        if(in[idx]) continue;
        if(used[c]+S[v]>X) continue;
        ll g=G[idx];
        if(g!=t.g){ if(g>0){ heap.push_back({(double)g/S[v],g,c,v}); push_heap(heap.begin(),heap.end()); } continue; }
        if(g<=0) continue;
        in[idx]=1; used[c]+=S[v];
        for(int r:reqByVideo[v]){ int e=re[r]; int l=latEC[(size_t)e*C+c]; if(!l||l>=cur[r]) continue;
            int old=cur[r];
            for(auto&p:epc[e]){ ll oc=max(0,old-p.second), nc=max(0,l-p.second);
                G[(size_t)p.first*V+v]-=(ll)rn[r]*(oc-nc); }
            cur[r]=l; }
    }
    { vector<ll>().swap(G); }

    // Phase 2
    vector<int> b1(R),b1c(R),b2(R);
    auto recompute=[&](int r){ int e=re[r],v=rv[r]; int a=LD[e],ac=-1,b=LD[e];
        for(auto&p:epc[e]){ if(!in[(size_t)p.first*V+v]) continue; int l=p.second;
            if(l<a){ b=a; a=l; ac=p.first; } else if(l<b) b=l; }
        b1[r]=a;b1c[r]=ac;b2[r]=b; };
    for(int r=0;r<R;r++) recompute(r);
    vector<ll> g(V,0); vector<char> touched(V,0), changed(V,0); vector<int> tl;
    vector<ll> dp; vector<unsigned char> keep;
    const double TL=8.8;
    bool improved=true; bool timeout=false;
    while(improved && !timeout){
        improved=false;
        for(int c=0;c<C;c++){
            if(elapsed()>TL){ timeout=true; break; }
            if(cacheEps[c].empty()) continue;
            tl.clear();
            for(auto&pe:cacheEps[c]){ int e=pe.first,l=pe.second;
                for(int r:reqByEp[e]){ int b=(b1c[r]==c)?b2[r]:b1[r]; if(b>l){ int v=rv[r]; if(!touched[v]){touched[v]=1;tl.push_back(v);} g[v]+=(ll)rn[r]*(b-l);} } }
            vector<int> cand; ll curVal=0; ll tot=0;
            for(int v:tl){ if(g[v]>0&&S[v]<=X){ cand.push_back(v); tot+=S[v]; } if(in[(size_t)c*V+v]) curVal+=g[v]; }
            vector<int> chosen; ll newVal=0;
            if(tot<=X){ chosen=cand; for(int v:cand) newVal+=g[v]; }
            else{
                sort(cand.begin(),cand.end(),[&](int a,int b){ double ra=(double)g[a]/S[a], rb=(double)g[b]/S[b]; if(ra!=rb) return ra>rb; return a<b;});
                // greedy
                { ll u=0,val=0; vector<int> ch; for(int v:cand) if(u+S[v]<=X){u+=S[v];val+=g[v];ch.push_back(v);} chosen=ch; newVal=val; }
                const ll LIM=40000000;
                size_t n=cand.size(); if((ll)n*(X+1)>LIM) n=max<ll>(1,LIM/(X+1));
                n=min(n,cand.size());
                dp.assign(X+1,0); keep.assign(n*(size_t)(X+1),0);
                for(size_t i=0;i<n;i++){ int s=S[cand[i]]; ll val=g[cand[i]]; unsigned char* kp=&keep[i*(size_t)(X+1)];
                    for(int w=X;w>=s;w--){ ll nv=dp[w-s]+val; if(nv>dp[w]){dp[w]=nv;kp[w]=1;} } }
                // fill remaining with greedy from rest
                vector<int> ch; int w=X; ll val=dp[X];
                for(int i=(int)n-1;i>=0;i--){ if(keep[i*(size_t)(X+1)+w]){ ch.push_back(cand[i]); w-=S[cand[i]]; } }
                ll u=0; for(int v:ch) u+=S[v];
                for(size_t i=n;i<cand.size();i++){ int v=cand[i]; if(u+S[v]<=X){u+=S[v];val+=g[v];ch.push_back(v);} }
                if(val>newVal){ newVal=val; chosen=ch; }
            }
            if(newVal>curVal){
                improved=true;
                vector<int> oldList;
                for(int v:tl) if(in[(size_t)c*V+v]) oldList.push_back(v);
                // also videos in cache not touched (zero gain) -> remove them too
                // collect all in-cache videos
                vector<int> allIn; for(int v=0;v<V;v++) if(in[(size_t)c*V+v]) allIn.push_back(v);
                for(int v:allIn){ in[(size_t)c*V+v]=0; changed[v]=1; }
                for(int v:chosen){ size_t id=(size_t)c*V+v; in[id]=1; changed[v]^=1; }
                // fix: changed should be 1 if status differs; chosen videos that were in get 0
                used[c]=0; for(int v:chosen) used[c]+=S[v];
                for(auto&pe:cacheEps[c]) for(int r:reqByEp[pe.first]) if(changed[rv[r]]) recompute(r);
                for(int v:allIn) changed[v]=0; for(int v:chosen) changed[v]=0;
            }
            for(int v:tl){ g[v]=0; touched[v]=0; }
        }
    }
    // output
    string out; int N=0; string body;
    for(int c=0;c<C;c++){
        string line=to_string(c); bool any=false; ll u=0;
        for(int v=0;v<V;v++) if(in[(size_t)c*V+v]){ line+=' '; line+=to_string(v); any=true; u+=S[v]; }
        if(any && u<=X){ N++; body+=line; body+='\n'; }
    }
    out=to_string(N)+"\n"+body;
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
