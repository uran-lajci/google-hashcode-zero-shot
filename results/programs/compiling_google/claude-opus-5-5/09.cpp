#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int C,T,S;
vector<ll> cc, rr;
vector<vector<int>> deps;
vector<string> names;
vector<int> tid; vector<ll> td, tg;
vector<ll> cp;
chrono::steady_clock::time_point t0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();}
const double LIMIT=9.0;

struct Result{ll score; vector<pair<int,int>> steps;};

Result run(const vector<int>& order){
    vector<ll> fr(S,0);
    vector<vector<pair<int,ll>>> copies(C);
    vector<pair<int,int>> steps;
    vector<int> stamp(C,-1);
    vector<int> need, st;
    int cnt=0;
    for(int idx:order){
        if((cnt++&15)==0 && elapsed()>LIMIT) break;
        int t=tid[idx]; ll d=td[idx];
        if(!copies[t].empty()) continue;
        if(cp[t]>d) continue;
        need.clear(); st.clear(); st.push_back(t); stamp[t]=idx;
        while(!st.empty()){
            int u=st.back(); st.pop_back(); need.push_back(u);
            for(int v:deps[u]) if(copies[v].empty() && stamp[v]!=idx){stamp[v]=idx; st.push_back(v);}
        }
        sort(need.begin(),need.end());
        size_t base=steps.size();
        vector<ll> savedFree=fr;
        bool fail=false;
        for(int u:need){
            ll R=0;
            vector<int> holders;
            for(int v:deps[u]){
                ll mf=LLONG_MAX;
                for(auto&p:copies[v]){mf=min(mf,p.second); holders.push_back(p.first);}
                R=max(R,mf+rr[v]);
            }
            // baseline
            int bs=-1; ll bstart=LLONG_MAX; ll bfree=-1;
            for(int s=0;s<S;s++){
                ll stt=max(fr[s],R);
                if(stt<bstart || (stt==bstart && fr[s]>bfree)){bstart=stt;bs=s;bfree=fr[s];}
            }
            sort(holders.begin(),holders.end());
            holders.erase(unique(holders.begin(),holders.end()),holders.end());
            for(int s:holders){
                ll stt=fr[s];
                for(int v:deps[u]){
                    ll mf=LLONG_MAX, loc=LLONG_MAX;
                    for(auto&p:copies[v]){mf=min(mf,p.second); if(p.first==s) loc=min(loc,p.second);}
                    ll a=min(mf+rr[v],loc);
                    stt=max(stt,a);
                }
                if(stt<=bstart){bstart=stt;bs=s;}
            }
            ll fin=bstart+cc[u];
            if(fin>d){fail=true;break;}
            copies[u].push_back({bs,fin});
            fr[bs]=fin;
            steps.push_back({u,bs});
        }
        if(fail){
            while(steps.size()>base){copies[steps.back().first].pop_back(); steps.pop_back();}
            fr=savedFree;
        }
    }
    ll score=0;
    for(int i=0;i<T;i++){
        int t=tid[i]; ll mf=LLONG_MAX;
        for(auto&p:copies[t]) mf=min(mf,p.second);
        if(mf<=td[i]) score+=td[i]-mf+tg[i];
    }
    return {score,steps};
}

int main(){
    t0=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    cc.resize(C);rr.resize(C);deps.resize(C);names.resize(C);cp.resize(C);
    unordered_map<string,int> mp; mp.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>names[i]>>cc[i]>>rr[i]; mp[names[i]]=i;
        int n;cin>>n; deps[i].resize(n);
        for(int j=0;j<n;j++){string s;cin>>s;deps[i][j]=mp[s];}
        ll m=0; for(int v:deps[i]) m=max(m,cp[v]);
        cp[i]=m+cc[i];
    }
    tid.resize(T);td.resize(T);tg.resize(T);
    for(int i=0;i<T;i++){string s;cin>>s>>td[i]>>tg[i];tid[i]=mp[s];}
    vector<int> base(T); iota(base.begin(),base.end(),0);
    vector<vector<int>> orders;
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]<td[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]-cp[tid[a]]<td[b]-cp[tid[b]];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return tg[a]>tg[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return (double)tg[a]/cp[tid[a]]>(double)tg[b]/cp[tid[b]];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return tg[a]-cp[tid[a]]>tg[b]-cp[tid[b]];}); orders.push_back(o);}
    Result best{-1,{}};
    vector<int> bestOrder;
    for(auto&o:orders){
        if(elapsed()>LIMIT) break;
        Result r=run(o);
        if(r.score>best.score){best=r;bestOrder=o;}
    }
    mt19937 rng(12345);
    double lastDur=0;
    while(!bestOrder.empty()){
        double st=elapsed();
        if(st+lastDur*1.2>LIMIT) break;
        auto o=bestOrder;
        int k=max(1,T/20);
        for(int i=0;i<k;i++){
            int a=rng()%T; int b=max(0,min(T-1,a+(int)(rng()%11)-5));
            swap(o[a],o[b]);
        }
        Result r=run(o);
        lastDur=elapsed()-st;
        if(r.score>=best.score){best=r;bestOrder=o;}
    }
    if(best.steps.empty()){
        best.steps.push_back({0,0});
    }
    string out;
    out+=to_string(best.steps.size())+"\n";
    for(auto&p:best.steps){out+=names[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
