#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int V,E,R,C,X;
vector<int> S, LD;
vector<vector<pair<int,int>>> conn;
vector<int> lat; // E*C
vector<int> rv, re, rn, best;
vector<vector<int>> reqByV, reqByE;
vector<char> inCache; // C*V
vector<int> remCap;

static inline int L(int e,int c){return lat[(size_t)e*C+c];}

int main(){
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    if(scanf("%d %d %d %d %d",&V,&E,&R,&C,&X)!=5) return 0;
    S.resize(V); for(auto&s:S) scanf("%d",&s);
    LD.resize(E); conn.resize(E); lat.assign((size_t)E*C,0);
    for(int e=0;e<E;e++){int k;scanf("%d %d",&LD[e],&k);
        for(int i=0;i<k;i++){int c,l;scanf("%d %d",&c,&l);
            if(lat[(size_t)e*C+c]==0){conn[e].push_back({c,l});lat[(size_t)e*C+c]=l;}
            else if(l<lat[(size_t)e*C+c]){lat[(size_t)e*C+c]=l;for(auto&p:conn[e])if(p.first==c)p.second=l;}
        }}
    rv.resize(R);re.resize(R);rn.resize(R);
    reqByV.resize(V);reqByE.resize(E);
    for(int i=0;i<R;i++){scanf("%d %d %d",&rv[i],&re[i],&rn[i]);reqByV[rv[i]].push_back(i);reqByE[re[i]].push_back(i);}
    best.resize(R); for(int i=0;i<R;i++) best[i]=LD[re[i]];
    inCache.assign((size_t)C*V,0);
    remCap.assign(C,X);

    // Greedy
    {
        vector<ll> g((size_t)C*V,0);
        for(int i=0;i<R;i++){int e=re[i];for(auto&p:conn[e]) g[(size_t)p.first*V+rv[i]]+=(ll)rn[i]*(LD[e]-p.second);}
        priority_queue<tuple<double,int,int>> pq;
        for(int c=0;c<C;c++)for(int v=0;v<V;v++){ll x=g[(size_t)c*V+v];if(x>0&&S[v]<=X)pq.push({(double)x/S[v],c,v});}
        vector<ll>().swap(g);
        while(!pq.empty()){
            auto [sc,c,v]=pq.top();pq.pop();
            if(S[v]>remCap[c])continue;
            ll gg=0;
            for(int r:reqByV[v]){int l=L(re[r],c);if(l>0&&best[r]>l)gg+=(ll)rn[r]*(best[r]-l);}
            if(gg<=0)continue;
            double ns=(double)gg/S[v];
            if(!pq.empty()&&ns<get<0>(pq.top())-1e-12){pq.push({ns,c,v});continue;}
            inCache[(size_t)c*V+v]=1;remCap[c]-=S[v];
            for(int r:reqByV[v]){int l=L(re[r],c);if(l>0&&best[r]>l)best[r]=l;}
        }
    }

    // Knapsack refinement
    vector<ll> gain(V,0);
    vector<char> touched(V,0);
    vector<vector<int>> contents(C);
    for(int c=0;c<C;c++)for(int v=0;v<V;v++)if(inCache[(size_t)c*V+v])contents[c].push_back(v);
    // endpoints connected to cache
    vector<vector<int>> cacheEps(C);
    for(int e=0;e<E;e++)for(auto&p:conn[e])cacheEps[p.first].push_back(e);
    const double TL=8.3;
    const ll OPLIM=40000000;
    bool stop=false;
    for(int pass=0;pass<50&&!stop;pass++){
        bool improved=false;
        for(int c=0;c<C;c++){
            if(elapsed()>TL){stop=true;break;}
            vector<int> orig=contents[c];
            for(int v:orig){inCache[(size_t)c*V+v]=0;}
            for(int v:orig)for(int r:reqByV[v]){int e=re[r];if(L(e,c)==0)continue;
                int b=LD[e];for(auto&p:conn[e])if(p.second<b&&inCache[(size_t)p.first*V+v])b=p.second;best[r]=b;}
            vector<int> tl;
            for(int e:cacheEps[c]){int l=L(e,c);
                for(int r:reqByE[e]){if(best[r]>l){int v=rv[r];if(!touched[v]){touched[v]=1;tl.push_back(v);}gain[v]+=(ll)rn[r]*(best[r]-l);}}}
            ll origVal=0;for(int v:orig)origVal+=gain[v];
            vector<int> cand;
            for(int v:tl)if(gain[v]>0&&S[v]<=X)cand.push_back(v);
            sort(cand.begin(),cand.end(),[&](int a,int b){
                __int128 x=(__int128)gain[a]*S[b],y=(__int128)gain[b]*S[a];
                if(x!=y)return x>y; return a<b;});
            ll sumS=0;for(int v:cand)sumS+=S[v];
            int cap=(int)min<ll>(X,sumS);
            size_t n=cand.size();
            while(n>0&&(ll)n*(cap+1)>OPLIM){n--;}
            // recompute cap for truncated
            sumS=0;for(size_t i=0;i<n;i++)sumS+=S[cand[i]];
            cap=(int)min<ll>(X,sumS);
            vector<int> chosen; ll newVal=-1;
            if(n>0){
                size_t W=(size_t)(cap+64)/64;
                vector<uint64_t> keep(n*W,0);
                vector<ll> dp(cap+1,0);
                for(size_t i=0;i<n;i++){int v=cand[i];int s=S[v];ll g=gain[v];uint64_t*kr=&keep[i*W];
                    for(int w=cap;w>=s;w--){ll t=dp[w-s]+g;if(t>dp[w]){dp[w]=t;kr[w>>6]|=1ULL<<(w&63);}}}
                newVal=dp[cap];int w=cap;
                for(size_t i=n;i-->0;){if(keep[i*W+(w>>6)]>>(w&63)&1){chosen.push_back(cand[i]);w-=S[cand[i]];}}
            }
            vector<int>* use=&orig;
            if(newVal>origVal){use=&chosen;improved=true;}
            for(int v:tl){gain[v]=0;touched[v]=0;}
            contents[c]=*use;
            int used=0;
            for(int v:*use){inCache[(size_t)c*V+v]=1;used+=S[v];
                for(int r:reqByV[v]){int l=L(re[r],c);if(l>0&&best[r]>l)best[r]=l;}}
            remCap[c]=X-used;
        }
        if(!improved)break;
    }

    int N=0;for(int c=0;c<C;c++)if(!contents[c].empty())N++;
    string out;out.reserve(1<<20);
    out+=to_string(N)+"\n";
    for(int c=0;c<C;c++){if(contents[c].empty())continue;
        out+=to_string(c);for(int v:contents[c]){out+=' ';out+=to_string(v);}out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
