#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF=LLONG_MAX/4;

int C,T,S;
vector<string> nm;
vector<ll> cc,rr;
vector<vector<int>> deps;
vector<int> tid; vector<ll> td,tg;

struct Sim{
    vector<ll> avail;
    vector<vector<pair<int,ll>>> loc;
    vector<ll> G;
    vector<pair<int,int>> steps;
    // undo log: (type, idx, old)
    // type0: avail[idx]=old ; type1: loc[idx].pop_back, G[idx]=old ; steps pop
    vector<tuple<int,int,ll>> lg;
    vector<int> mark; vector<ll> dr;
    void init(){
        avail.assign(S,0); loc.assign(C,{}); G.assign(C,INF); steps.clear(); lg.clear();
        mark.assign(S,0); dr.assign(S,0);
    }
    ll lookLoc(int d,int s){
        for(auto&p:loc[d]) if(p.first==s) return p.second;
        return INF;
    }
    ll schedule(int f){
        ll maxG=0;
        vector<int> touched;
        for(int d:deps[f]){
            maxG=max(maxG,G[d]);
            for(auto&p:loc[d]) if(!mark[p.first]){mark[p.first]=1;touched.push_back(p.first);}
        }
        for(int s:touched){
            ll v=0;
            for(int d:deps[f]) v=max(v,min(G[d],lookLoc(d,s)));
            dr[s]=v;
        }
        int bs=-1; ll bf=INF;
        for(int s=0;s<S;s++){
            ll r=mark[s]?dr[s]:maxG;
            ll fin=max(avail[s],r)+cc[f];
            if(fin<bf){bf=fin;bs=s;}
        }
        for(int s:touched) mark[s]=0;
        lg.emplace_back(0,bs,avail[bs]);
        avail[bs]=bf;
        lg.emplace_back(1,f,G[f]);
        loc[f].push_back({bs,bf});
        G[f]=min(G[f],bf+rr[f]);
        steps.push_back({f,bs});
        return bf;
    }
    void undo(size_t sz){
        while(lg.size()>sz){
            auto [t,i,o]=lg.back(); lg.pop_back();
            if(t==0) avail[i]=o;
            else { loc[i].pop_back(); G[i]=o; steps.pop_back(); }
        }
    }
};

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    auto T0=chrono::steady_clock::now();
    cin>>C>>T>>S;
    nm.resize(C);cc.resize(C);rr.resize(C);deps.resize(C);
    unordered_map<string,int> id; id.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>nm[i]>>cc[i]>>rr[i]; id[nm[i]]=i;
        int n;cin>>n; deps[i].resize(n);
        for(int j=0;j<n;j++){string x;cin>>x;deps[i][j]=id[x];}
    }
    tid.resize(T);td.resize(T);tg.resize(T);
    for(int i=0;i<T;i++){string x;cin>>x>>td[i]>>tg[i];tid[i]=id[x];}
    vector<ll> cp(C);
    for(int i=0;i<C;i++){ll m=0;for(int d:deps[i])m=max(m,cp[d]);cp[i]=m+cc[i];}

    vector<vector<int>> orders;
    vector<int> base(T); iota(base.begin(),base.end(),0);
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]<td[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return tg[a]>tg[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]-cp[tid[a]]<td[b]-cp[tid[b]];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){
        return (double)tg[a]/cp[tid[a]]>(double)tg[b]/cp[tid[b]];}); orders.push_back(o);}

    double budget=2.1;
    ll bestScore=-1; vector<pair<int,int>> bestSteps;
    Sim sim;
    vector<int> vis(C,0); int stamp=0;
    for(size_t oi=0;oi<orders.size();oi++){
        auto st=chrono::steady_clock::now();
        sim.init();
        for(int t:orders[oi]){
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>budget) break;
            int f=tid[t];
            if(!sim.loc[f].empty()) continue;
            // collect uncompiled ancestors
            stamp++;
            vector<int> need, stk{f}; vis[f]=stamp;
            while(!stk.empty()){
                int x=stk.back();stk.pop_back();need.push_back(x);
                for(int d:deps[x]) if(vis[d]!=stamp && sim.loc[d].empty()){vis[d]=stamp;stk.push_back(d);}
            }
            sort(need.begin(),need.end());
            size_t sz=sim.lg.size();
            ll fin=0;
            for(int x:need) fin=sim.schedule(x);
            if(fin>td[t]) sim.undo(sz);
            else sim.lg.clear();
        }
        ll sc=0;
        for(int t=0;t<T;t++){
            ll x=INF; for(auto&p:sim.loc[tid[t]]) x=min(x,p.second);
            if(x<=td[t]) sc+=td[t]-x+tg[t];
        }
        if(sc>bestScore){bestScore=sc;bestSteps=sim.steps;}
        double tot=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
        if(tot>8.5) break;
    }
    if(bestSteps.empty()) bestSteps.push_back({0,0});
    string out; out.reserve(bestSteps.size()*16);
    out+=to_string(bestSteps.size())+"\n";
    for(auto&p:bestSteps){out+=nm[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
