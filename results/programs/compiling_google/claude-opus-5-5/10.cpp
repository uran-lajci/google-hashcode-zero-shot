#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF=(ll)4e18;

int C,T,S;
vector<ll> ct,rt,lb;
vector<vector<int>> deps;
vector<ll> tdl,tg;
vector<int> targets;

struct Sim{
    vector<ll> fr,glob;
    vector<vector<pair<int,ll>>> loc;
    vector<pair<int,int>> steps;
    // undo log: type 0 free(s,old),1 glob(f,old),2 loc pop(f),3 step pop
    struct L{int t;int a;ll v;};
    vector<L> lg;
    vector<int> stamp; int st=0;
    vector<int> vis; int vst=0;
    ll ops=0;
    void init(){
        fr.assign(S,0);glob.assign(C,INF);loc.assign(C,{});steps.clear();lg.clear();
        stamp.assign(S,0);st=0;vis.assign(C,0);vst=0;ops=0;
    }
    ll localOn(int f,int s){
        ll b=INF;for(auto&p:loc[f]) if(p.first==s) b=min(b,p.second);return b;
    }
    ll sched(int f){
        ll maxG=0;
        st++;
        vector<int> cand;
        for(int d:deps[f]){
            maxG=max(maxG,glob[d]);
            for(auto&p:loc[d]) if(stamp[p.first]!=st){stamp[p.first]=st;cand.push_back(p.first);}
        }
        ll best=INF;int bs=0;
        for(int s=0;s<S;s++){
            if(stamp[s]==st) continue;
            ll fin=max(fr[s],maxG)+ct[f];
            if(fin<best){best=fin;bs=s;}
        }
        ops+=S;
        for(int s:cand){
            ll r=0;
            for(int d:deps[f]){ r=max(r,min(glob[d],localOn(d,s))); }
            ops+=deps[f].size()*2;
            ll fin=max(fr[s],r)+ct[f];
            if(fin<best||(fin==best&&s<bs)){best=fin;bs=s;}
        }
        lg.push_back({0,bs,fr[bs]}); fr[bs]=best;
        lg.push_back({1,f,glob[f]}); glob[f]=min(glob[f],best+rt[f]);
        lg.push_back({2,f,0}); loc[f].push_back({bs,best});
        lg.push_back({3,0,0}); steps.push_back({f,bs});
        return best;
    }
    void undo(size_t cp){
        while(lg.size()>cp){
            L l=lg.back();lg.pop_back();
            if(l.t==0) fr[l.a]=l.v;
            else if(l.t==1) glob[l.a]=l.v;
            else if(l.t==2) loc[l.a].pop_back();
            else steps.pop_back();
        }
    }
    ll run(const vector<int>& order, ll budget){
        init();
        vector<int> stk,col;
        for(int t:order){
            if(ops>budget) break;
            if(!loc[t].empty()) continue;
            if(lb[t]>tdl[t]) continue;
            vst++; col.clear(); stk.clear();
            stk.push_back(t); vis[t]=vst;
            while(!stk.empty()){
                int u=stk.back();stk.pop_back();col.push_back(u);
                for(int d:deps[u]) if(vis[d]!=vst && loc[d].empty()){vis[d]=vst;stk.push_back(d);}
            }
            ops+=col.size();
            sort(col.begin(),col.end());
            size_t cp=lg.size();
            ll fin=0;
            for(int u:col){ ll x=sched(u); if(u==t) fin=x; }
            if(fin>tdl[t]) undo(cp);
            lg.clear(); // commit
        }
        ll sc=0;
        for(int t:targets){
            ll b=INF;for(auto&p:loc[t]) b=min(b,p.second);
            if(b<=tdl[t]) sc+=tdl[t]-b+tg[t];
        }
        return sc;
    }
};

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    unordered_map<string,int> id; id.reserve(C*2);
    vector<string> names(C);
    ct.resize(C);rt.resize(C);deps.resize(C);lb.assign(C,0);
    for(int i=0;i<C;i++){
        cin>>names[i]>>ct[i]>>rt[i]; id[names[i]]=i;
        int n;cin>>n;deps[i].resize(n);
        for(int j=0;j<n;j++){string s;cin>>s;deps[i][j]=id[s];}
        ll m=0;for(int d:deps[i]) m=max(m,lb[d]);
        lb[i]=m+ct[i];
    }
    tdl.assign(C,0);tg.assign(C,0);
    for(int i=0;i<T;i++){string s;ll d,g;cin>>s>>d>>g;int f=id[s];tdl[f]=d;tg[f]=g;targets.push_back(f);}
    vector<vector<int>> orders;
    {
        vector<int> o=targets;
        stable_sort(o.begin(),o.end(),[&](int a,int b){
            double ka=(double)(tg[a]+max(0LL,tdl[a]-lb[a]))/lb[a];
            double kb=(double)(tg[b]+max(0LL,tdl[b]-lb[b]))/lb[b];
            if(ka!=kb) return ka>kb; return a<b;});
        orders.push_back(o);
    }
    {
        vector<int> o=targets;
        stable_sort(o.begin(),o.end(),[&](int a,int b){if(tdl[a]!=tdl[b])return tdl[a]<tdl[b];return a<b;});
        orders.push_back(o);
    }
    {
        vector<int> o=targets;
        stable_sort(o.begin(),o.end(),[&](int a,int b){if(tg[a]!=tg[b])return tg[a]>tg[b];return a<b;});
        orders.push_back(o);
    }
    {
        vector<int> o=targets;
        stable_sort(o.begin(),o.end(),[&](int a,int b){
            ll sa=tdl[a]-lb[a], sb=tdl[b]-lb[b];
            if(sa!=sb) return sa<sb; return a<b;});
        orders.push_back(o);
    }
    Sim sim;
    ll bestSc=-1; vector<pair<int,int>> bestSteps;
    for(auto&o:orders){
        ll sc=sim.run(o,(ll)1.5e8);
        if(sc>bestSc){bestSc=sc;bestSteps=sim.steps;}
    }
    if(bestSteps.empty()){
        int f=0; for(int i=0;i<C;i++) if(deps[i].empty()){f=i;break;}
        bestSteps.push_back({f,0});
    }
    string out;
    out+=to_string(bestSteps.size())+"\n";
    for(auto&p:bestSteps){out+=names[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
