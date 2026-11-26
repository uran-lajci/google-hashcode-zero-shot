#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static auto T0 = chrono::steady_clock::now();
static double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-T0).count(); }

int V,E,R,C,X;
vector<int> S, LD;
vector<int> lat; // E*C, 0 = none
vector<vector<pair<int,int>>> epC, cEp;
vector<int> rv, re_; vector<ll> rn; vector<int> cur;
vector<vector<int>> byV, byE;
vector<vector<char>> has;
vector<int> used;
vector<vector<int>> contents;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!=-1 && (c<'0'||c>'9')) c=gc(); int x=0; while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); } return x; }

ll gainCV(int c,int v){
    ll g=0;
    for(int r: byV[v]){ int l=lat[(size_t)re_[r]*C+c]; if(l && l<cur[r]) g+=rn[r]*(cur[r]-l); }
    return g;
}

int main(){
    V=readInt();E=readInt();R=readInt();C=readInt();X=readInt();
    S.resize(V); for(auto&s:S) s=readInt();
    LD.resize(E); lat.assign((size_t)E*C,0); epC.resize(E); cEp.resize(C);
    for(int e=0;e<E;e++){
        LD[e]=readInt(); int K=readInt();
        for(int k=0;k<K;k++){ int c=readInt(), l=readInt(); if(l>=LD[e]) continue;
            if(lat[(size_t)e*C+c]==0){ lat[(size_t)e*C+c]=l; epC[e].push_back({c,l}); }
            else if(l<lat[(size_t)e*C+c]) lat[(size_t)e*C+c]=l; }
    }
    for(int e=0;e<E;e++){ for(auto&p:epC[e]) p.second=lat[(size_t)e*C+p.first]; for(auto&p:epC[e]) cEp[p.first].push_back({e,p.second}); }
    vector<array<ll,3>> rq(R);
    for(int i=0;i<R;i++){ ll v=readInt(), e=readInt(), n=readInt(); rq[i]={v,e,n}; }
    sort(rq.begin(),rq.end());
    for(int i=0;i<R;){ int j=i; ll n=0; while(j<R && rq[j][0]==rq[i][0] && rq[j][1]==rq[i][1]){ n+=rq[j][2]; j++; }
        if(!epC[rq[i][1]].empty() && S[rq[i][0]]<=X){ rv.push_back(rq[i][0]); re_.push_back(rq[i][1]); rn.push_back(n); }
        i=j; }
    int M=rv.size(); cur.resize(M); byV.resize(V); byE.resize(E);
    for(int r=0;r<M;r++){ cur[r]=LD[re_[r]]; byV[rv[r]].push_back(r); byE[re_[r]].push_back(r); }
    has.assign(C, vector<char>(V,0)); used.assign(C,0); contents.resize(C);

    // initial greedy
    struct Ent{ double k; int c,v; bool operator<(const Ent&o)const{ if(k!=o.k) return k<o.k; if(c!=o.c) return c>o.c; return v>o.v; } };
    vector<Ent> init;
    {
        vector<ll> acc(C,0); vector<int> touched;
        for(int v=0;v<V;v++){
            touched.clear();
            for(int r: byV[v]){ int e=re_[r]; for(auto&p:epC[e]){ if(acc[p.first]==0) touched.push_back(p.first); acc[p.first]+=rn[r]*(LD[e]-p.second);} }
            for(int c:touched){ if(acc[c]>0) init.push_back({(double)acc[c]/S[v],c,v}); acc[c]=0; }
        }
    }
    priority_queue<Ent> pq(less<Ent>(), move(init));
    while(!pq.empty()){
        Ent t=pq.top(); pq.pop();
        int c=t.c,v=t.v;
        if(used[c]+S[v]>X || has[c][v]) continue;
        ll g=gainCV(c,v); if(g<=0) continue;
        double k=(double)g/S[v];
        if(pq.empty() || k>=pq.top().k-1e-9){
            has[c][v]=1; used[c]+=S[v]; contents[c].push_back(v);
            for(int r: byV[v]){ int l=lat[(size_t)re_[r]*C+c]; if(l && l<cur[r]) cur[r]=l; }
        } else pq.push({k,c,v});
    }

    // rebuild passes
    const double TL=9.3;
    vector<ll> gain(V,0); vector<char> inOld(V,0);
    vector<uint8_t> keep;
    bool timeUp=false;
    for(int pass=0; pass<50 && !timeUp; pass++){
        bool improved=false;
        for(int c=0;c<C;c++){
            if(elapsed()>TL){ timeUp=true; break; }
            if(cEp[c].empty()) continue;
            vector<int> old=contents[c];
            for(int v:old){ has[c][v]=0; inOld[v]=1; }
            used[c]=0; contents[c].clear();
            vector<int> touched;
            for(auto&pe:cEp[c]){ int e=pe.first, l=pe.second;
                for(int r: byE[e]){ int v=rv[r];
                    if(inOld[v]){ int b=LD[e]; for(auto&p:epC[e]) if(has[p.first][v] && p.second<b) b=p.second; cur[r]=b; }
                    if(l<cur[r]){ if(gain[v]==0) touched.push_back(v); gain[v]+=rn[r]*(cur[r]-l); }
                } }
            ll oldVal=0; for(int v:old) oldVal+=gain[v];
            vector<int> items; for(int v:touched) if(gain[v]>0 && S[v]<=X) items.push_back(v);
            sort(items.begin(),items.end());
            vector<int> chosen; ll newVal=0;
            ll tot=0; for(int v:items) tot+=S[v];
            if(tot<=X){ chosen=items; for(int v:items) newVal+=gain[v]; }
            else if((double)items.size()*(X+1) <= 6e7){
                int n=items.size();
                vector<ll> dp(X+1,0); keep.assign((size_t)n*(X+1),0);
                for(int i=0;i<n;i++){ int s=S[items[i]]; ll g=gain[items[i]]; uint8_t* kp=&keep[(size_t)i*(X+1)];
                    for(int w=X;w>=s;w--){ ll nv=dp[w-s]+g; if(nv>dp[w]){ dp[w]=nv; kp[w]=1; } } }
                int w=X; newVal=dp[X];
                for(int i=n-1;i>=0;i--){ if(keep[(size_t)i*(X+1)+w]){ chosen.push_back(items[i]); w-=S[items[i]]; } }
            } else {
                vector<int> ord=items;
                sort(ord.begin(),ord.end(),[&](int a,int b){ double ka=(double)gain[a]/S[a], kb=(double)gain[b]/S[b]; if(ka!=kb) return ka>kb; return a<b; });
                int w=0; for(int v:ord) if(w+S[v]<=X){ chosen.push_back(v); w+=S[v]; newVal+=gain[v]; }
            }
            if(newVal>oldVal){ improved=true; } else chosen=old;
            for(int v:touched) gain[v]=0;
            for(int v:old) inOld[v]=0;
            for(int v:chosen){ has[c][v]=1; used[c]+=S[v]; contents[c].push_back(v); }
            for(auto&pe:cEp[c]){ int l=pe.second; for(int r: byE[pe.first]) if(has[c][rv[r]] && l<cur[r]) cur[r]=l; }
        }
        if(!improved) break;
    }

    string out; int N=0;
    for(int c=0;c<C;c++) if(!contents[c].empty()) N++;
    out+=to_string(N)+"\n";
    for(int c=0;c<C;c++) if(!contents[c].empty()){
        out+=to_string(c);
        for(int v:contents[c]){ out+=' '; out+=to_string(v); }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
}
