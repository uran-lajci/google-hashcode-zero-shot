#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int C,T,S;
vector<ll> cc, rr;
vector<vector<int>> deps;
vector<int> tIdx; vector<ll> tD, tG;
vector<ll> cp;

struct Result{ ll score; vector<array<ll,3>> steps; };

Result run(const vector<int>& order, ll budget){
    vector<int> srv(C,-1); vector<ll> fin(C,0);
    vector<ll> freeT(S,0);
    vector<int> stamp(C,0); int st=0;
    vector<array<ll,3>> steps;
    ll work=0;
    vector<int> clos, stk, cand;
    for(int ti: order){
        if(work>budget) break;
        int t=tIdx[ti]; ll d=tD[ti];
        if(cp[t]>d) continue;
        if(srv[t]!=-1) continue;
        st++; clos.clear(); stk.clear();
        stk.push_back(t); stamp[t]=st;
        while(!stk.empty()){
            int f=stk.back(); stk.pop_back(); clos.push_back(f); work++;
            for(int x: deps[f]){ work++; if(srv[x]==-1 && stamp[x]!=st){stamp[x]=st; stk.push_back(x);} }
        }
        sort(clos.begin(),clos.end());
        vector<ll> saved=freeT;
        size_t stepBase=steps.size();
        bool ok=true; size_t done=0;
        for(int f: clos){
            ll R=0; cand.clear();
            for(int x: deps[f]){ R=max(R,fin[x]+rr[x]); cand.push_back(srv[x]); }
            work+=deps[f].size()+S;
            int bs=-1; ll bf=LLONG_MAX;
            for(int s=0;s<S;s++){
                ll fn=max(freeT[s],R)+cc[f];
                if(fn<bf){bf=fn;bs=s;}
            }
            for(int s: cand){
                ll rd=0;
                for(int x: deps[f]){ ll a=(srv[x]==s)?fin[x]:fin[x]+rr[x]; rd=max(rd,a);}
                work+=deps[f].size();
                ll fn=max(freeT[s],rd)+cc[f];
                if(fn<bf){bf=fn;bs=s;}
            }
            srv[f]=bs; fin[f]=bf; freeT[bs]=bf;
            steps.push_back({bf-cc[f],(ll)f,(ll)bs});
            done++;
            ll lim = d - (f!=t?cc[t]:0);
            if(bf>lim){ok=false;break;}
        }
        if(!ok){
            for(size_t i=0;i<done;i++){ srv[clos[i]]=-1; fin[clos[i]]=0; }
            freeT=saved; steps.resize(stepBase);
        }
    }
    ll score=0;
    for(int i=0;i<T;i++){
        int t=tIdx[i];
        if(srv[t]!=-1 && fin[t]<=tD[i]) score+=tD[i]-fin[t]+tG[i];
    }
    return {score,steps};
}

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    cin>>C>>T>>S;
    cc.resize(C); rr.resize(C); deps.resize(C);
    vector<string> names(C);
    unordered_map<string,int> id; id.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>names[i]>>cc[i]>>rr[i]; id[names[i]]=i;
        int n; cin>>n; deps[i].resize(n);
        for(int j=0;j<n;j++){string s; cin>>s; deps[i][j]=id[s];}
    }
    tIdx.resize(T); tD.resize(T); tG.resize(T);
    for(int i=0;i<T;i++){string s; cin>>s>>tD[i]>>tG[i]; tIdx[i]=id[s];}
    cp.assign(C,0);
    for(int i=0;i<C;i++){ ll m=0; for(int x:deps[i]) m=max(m,cp[x]); cp[i]=m+cc[i]; }
    // work estimates
    vector<double> wk(T);
    {
        vector<int> stamp(C,0); int st=0; ll visits=0; const ll CAP=30000000;
        vector<int> stk;
        for(int i=0;i<T;i++){
            if(visits>CAP){ wk[i]=(double)cp[tIdx[i]]; continue; }
            st++; stk.clear(); stk.push_back(tIdx[i]); stamp[tIdx[i]]=st; double sum=0;
            while(!stk.empty()){
                int f=stk.back(); stk.pop_back(); sum+=cc[f]; visits++;
                for(int x:deps[f]){ visits++; if(stamp[x]!=st){stamp[x]=st; stk.push_back(x);} }
            }
            wk[i]=sum;
        }
    }
    vector<vector<int>> orders;
    vector<int> base(T); iota(base.begin(),base.end(),0);
    {
        vector<int> o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){
            double va=(double)(tG[a]+max(0LL,tD[a]-cp[tIdx[a]]))/max(1.0,wk[a]);
            double vb=(double)(tG[b]+max(0LL,tD[b]-cp[tIdx[b]]))/max(1.0,wk[b]);
            return va>vb;});
        orders.push_back(o);
    }
    {
        vector<int> o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){return tD[a]<tD[b];});
        orders.push_back(o);
    }
    {
        vector<int> o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){
            return tG[a]+max(0LL,tD[a]-cp[tIdx[a]]) > tG[b]+max(0LL,tD[b]-cp[tIdx[b]]);});
        orders.push_back(o);
    }
    Result best{-1,{}};
    for(auto& o: orders){
        Result r=run(o,150000000LL);
        if(r.score>best.score) best=move(r);
    }
    auto& steps=best.steps;
    if(steps.empty()){
        int f=0; for(int i=0;i<C;i++) if(deps[i].empty()){f=i;break;}
        cout<<1<<"\n"<<names[f]<<" 0\n";
        return 0;
    }
    stable_sort(steps.begin(),steps.end(),[](const array<ll,3>&a,const array<ll,3>&b){return a[0]<b[0];});
    string out; out.reserve(steps.size()*16);
    out+=to_string(steps.size()); out+='\n';
    for(auto& s: steps){ out+=names[s[1]]; out+=' '; out+=to_string(s[2]); out+='\n'; }
    cout<<out;
    return 0;
}
