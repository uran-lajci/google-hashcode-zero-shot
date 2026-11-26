#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char*buf;size_t bpos=0,blen=0;
static inline int readInt(){
    while(bpos<blen&&(buf[bpos]<'0'||buf[bpos]>'9'))bpos++;
    int x=0;while(bpos<blen&&buf[bpos]>='0'&&buf[bpos]<='9'){x=x*10+(buf[bpos]-'0');bpos++;}
    return x;
}
struct HE{double r;int c,v;bool operator<(const HE&o)const{if(r!=o.r)return r<o.r;if(c!=o.c)return c>o.c;return v>o.v;}};

int main(){
    // read all
    {size_t cap=1<<24;buf=(char*)malloc(cap);size_t n;
     while((n=fread(buf+blen,1,cap-blen,stdin))>0){blen+=n;if(blen==cap){cap*=2;buf=(char*)realloc(buf,cap);}}}
    int V=readInt(),E=readInt(),R=readInt(),C=readInt(),X=readInt();
    vector<int>S(V);for(auto&s:S)s=readInt();
    vector<int>LD(E);vector<vector<pair<int,int>>>epC(E);
    vector<int>lat((size_t)E*C,0);
    vector<vector<pair<int,int>>>cacheEps(C);
    for(int e=0;e<E;e++){LD[e]=readInt();int K=readInt();
        for(int k=0;k<K;k++){int c=readInt(),l=readInt();
            if(l<LD[e]){ if(lat[(size_t)e*C+c]==0||l<lat[(size_t)e*C+c]) lat[(size_t)e*C+c]=l;}
        }
    }
    for(int e=0;e<E;e++)for(int c=0;c<C;c++){int l=lat[(size_t)e*C+c];if(l){epC[e].push_back({c,l});cacheEps[c].push_back({e,l});}}
    vector<pair<ll,int>>rr(R);
    for(int i=0;i<R;i++){int v=readInt(),e=readInt(),n=readInt();rr[i]={(ll)v*E+e,n};}
    sort(rr.begin(),rr.end());
    vector<int>rv,re;vector<ll>rn;
    for(int i=0;i<R;){int j=i;ll s=0;while(j<R&&rr[j].first==rr[i].first){s+=rr[j].second;j++;}
        rv.push_back(rr[i].first/E);re.push_back(rr[i].first%E);rn.push_back(s);i=j;}
    rr.clear();rr.shrink_to_fit();
    int M=rv.size();
    vector<int>best(M);for(int i=0;i<M;i++)best[i]=LD[re[i]];
    vector<int>vs(V+1,0);for(int i=0;i<M;i++)vs[rv[i]+1]++;for(int v=0;v<V;v++)vs[v+1]+=vs[v];
    vector<vector<int>>epReq(E);for(int i=0;i<M;i++)epReq[re[i]].push_back(i);
    vector<char>has((size_t)C*V,0);
    vector<ll>used(C,0);
    ll work=0;const ll BUDGET=2500000000LL;

    // initial gains
    vector<HE>init;
    {
        vector<ll>acc(C,0);vector<int>touch;
        for(int v=0;v<V;v++){
            if(S[v]>X)continue;
            for(int i=vs[v];i<vs[v+1];i++){int e=re[i];
                for(auto&p:epC[e]){if(acc[p.first]==0)touch.push_back(p.first);acc[p.first]+=rn[i]*(LD[e]-p.second);}
                work+=epC[e].size();
            }
            for(int c:touch){init.push_back({(double)acc[c]/S[v],c,v});acc[c]=0;}
            touch.clear();
        }
    }
    priority_queue<HE>pq(less<HE>(),move(init));
    while(!pq.empty()){
        HE h=pq.top();pq.pop();
        int c=h.c,v=h.v;
        if(has[(size_t)c*V+v]||used[c]+S[v]>X)continue;
        ll g=0;
        for(int i=vs[v];i<vs[v+1];i++){int l=lat[(size_t)re[i]*C+c];if(l&&l<best[i])g+=rn[i]*(best[i]-l);}
        work+=vs[v+1]-vs[v];
        if(g<=0)continue;
        double r=(double)g/S[v];
        if(pq.empty()||r>=pq.top().r){
            has[(size_t)c*V+v]=1;used[c]+=S[v];
            for(int i=vs[v];i<vs[v+1];i++){int l=lat[(size_t)re[i]*C+c];if(l&&l<best[i])best[i]=l;}
        }else pq.push({r,c,v});
    }
    {priority_queue<HE>tmp;swap(pq,tmp);}

    // refinement
    vector<ll>val(V,0);vector<char>tch(V,0);vector<int>bex(M);
    const ll LIMIT=20000000LL;
    vector<uint8_t>keep;vector<ll>dp;
    bool improved=true;
    while(improved&&work<BUDGET){
        improved=false;
        for(int c=0;c<C&&work<BUDGET;c++){
            if(cacheEps[c].empty())continue;
            vector<int>touched;
            for(auto&pe:cacheEps[c]){int e=pe.first,l=pe.second;
                for(int idx:epReq[e]){int v=rv[idx];int b;
                    if(has[(size_t)c*V+v]){b=LD[e];for(auto&p:epC[e])if(p.first!=c&&p.second<b&&has[(size_t)p.first*V+v])b=p.second;work+=epC[e].size();}
                    else b=best[idx];
                    bex[idx]=b;
                    if(b>l){val[v]+=rn[idx]*(b-l);if(!tch[v]){tch[v]=1;touched.push_back(v);}}
                }
                work+=epReq[e].size();
            }
            ll curVal=0;vector<int>mem,oth;
            for(int v:touched){if(val[v]<=0||S[v]>X)continue;
                if(has[(size_t)c*V+v]){mem.push_back(v);curVal+=val[v];}else oth.push_back(v);}
            ll g=X;for(int v:mem)g=__gcd(g,(ll)S[v]);for(int v:oth)g=__gcd(g,(ll)S[v]);
            ll W=X/g;
            bool ok=(ll)(mem.size()+1)*(W+1)<=LIMIT;
            if(ok){
                sort(oth.begin(),oth.end(),[&](int a,int b){
                    __int128 x=(__int128)val[a]*S[b],y=(__int128)val[b]*S[a];if(x!=y)return x>y;return a<b;});
                vector<int>items=mem;
                for(int v:oth){if((ll)(items.size()+1)*(W+1)>LIMIT)break;items.push_back(v);}
                int n=items.size();
                dp.assign(W+1,0);keep.assign((size_t)n*(W+1),0);
                for(int i=0;i<n;i++){int s=S[items[i]]/g;ll vl=val[items[i]];uint8_t*kp=&keep[(size_t)i*(W+1)];
                    for(ll w=W;w>=s;w--){ll t=dp[w-s]+vl;if(t>dp[w]){dp[w]=t;kp[w]=1;}}}
                work+=(ll)n*(W+1);
                ll w=0;for(ll x=0;x<=W;x++)if(dp[x]>dp[w])w=x;
                if(dp[w]>curVal){
                    vector<char>sel(n,0);
                    for(int i=n-1;i>=0;i--)if(keep[(size_t)i*(W+1)+w]){sel[i]=1;w-=S[items[i]]/g;}
                    // clear all current members of cache c
                    for(int v:touched)has[(size_t)c*V+v]=0;
                    // also any members not touched (zero value) - keep them? remove all for correctness of used
                    ll u=0;
                    for(int v=0;v<V;v++)if(has[(size_t)c*V+v]){has[(size_t)c*V+v]=0;}
                    work+=V;
                    for(int i=0;i<n;i++)if(sel[i]){has[(size_t)c*V+items[i]]=1;u+=S[items[i]];}
                    used[c]=u;
                    for(auto&pe:cacheEps[c]){int l=pe.second;
                        for(int idx:epReq[pe.first]){int v=rv[idx];
                            best[idx]=has[(size_t)c*V+v]?min(bex[idx],l):bex[idx];}}
                    improved=true;
                }
            }
            for(int v:touched){val[v]=0;tch[v]=0;}
        }
    }

    // output
    string out;
    vector<int>cs;
    for(int c=0;c<C;c++)if(used[c]>0)cs.push_back(c);
    out+=to_string(cs.size())+"\n";
    for(int c:cs){
        out+=to_string(c);
        for(int v=0;v<V;v++)if(has[(size_t)c*V+v]){out+=' ';out+=to_string(v);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
