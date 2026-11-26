#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF=LLONG_MAX/4;

int C,T,S;
vector<string> nm; vector<ll> cc,rr; vector<vector<int>> deps;
vector<int> tf; vector<ll> td,tg; vector<ll> CP;

struct LogE{int f,s; ll pf,pm; int ps;};
struct State{
    vector<ll> freeT; vector<vector<pair<int,ll>>> loc; vector<ll> fmin; vector<int> fminS;
    vector<pair<int,int>> steps; vector<LogE> lg; ll ops=0;
    void init(){freeT.assign(S,0);loc.assign(C,{});fmin.assign(C,INF);fminS.assign(C,-1);steps.clear();lg.clear();ops=0;}
    bool hasLocal(int f,int s){for(auto&p:loc[f]) if(p.first==s) return true; return false;}
    ll avail(int f,int s){
        if(fmin[f]==INF) return INF;
        if(fminS[f]==s) return fmin[f];
        ll a=fmin[f]+rr[f];
        for(auto&p:loc[f]) if(p.first==s) return min(a,p.second);
        return a;
    }
    ll startTime(int f,int s){
        ll st=freeT[s]; ops+=deps[f].size()+1;
        for(int d:deps[f]) st=max(st,avail(d,s));
        return st;
    }
    ll apply(int f,int s){
        ll st=startTime(f,s); ll fin=st+cc[f];
        lg.push_back({f,s,freeT[s],fmin[f],fminS[f]});
        loc[f].push_back({s,fin}); freeT[s]=fin;
        if(fin<fmin[f]){fmin[f]=fin;fminS[f]=s;}
        steps.push_back({f,s}); return fin;
    }
    void undo(size_t m){
        while(lg.size()>m){
            LogE e=lg.back(); lg.pop_back();
            loc[e.f].pop_back(); freeT[e.s]=e.pf; fmin[e.f]=e.pm; fminS[e.f]=e.ps; steps.pop_back();
        }
    }
};

State st;
vector<int> stampV; int curStamp=0;
vector<int> inU; int uStamp=0;

ll stratA(int s,const vector<int>&U,ll dl){
    for(int f:U){
        for(int d:deps[f]){
            if(inU[d]==uStamp) continue;
            if(st.hasLocal(d,s)) continue;
            ll a=st.avail(d,s);
            ll rf=st.startTime(d,s)+cc[d];
            if(rf<a){ if(rf>dl) return INF; st.apply(d,s);}
        }
        ll fin=st.apply(f,s);
        if(fin>dl) return INF;
    }
    return st.fmin[U.back()];
}
ll stratB(const vector<int>&cands,const vector<int>&U,ll dl){
    for(int f:U){
        ll best=INF;int bs=cands[0];
        for(int s:cands){ll fin=st.startTime(f,s)+cc[f]; if(fin<best){best=fin;bs=s;}}
        if(best>dl) return INF;
        st.apply(f,bs);
    }
    return st.fmin[U.back()];
}

pair<ll,vector<pair<int,int>>> runPass(const vector<int>&order,ll budget){
    st.init();
    int n=order.size();
    vector<int> U; vector<int> stk;
    for(int i=0;i<n;i++){
        if(st.ops>budget*3/2) break;
        int t=order[i]; int f=tf[t]; ll dl=td[t];
        if(st.fmin[f]!=INF) continue;
        if(CP[f]>dl) continue;
        curStamp++; uStamp++;
        U.clear(); stk.clear(); stk.push_back(f); stampV[f]=curStamp;
        ll W=0;
        while(!stk.empty()){
            int x=stk.back();stk.pop_back(); U.push_back(x); inU[x]=uStamp;
            W+=deps[x].size()+1;
            for(int d:deps[x]) if(stampV[d]!=curStamp){stampV[d]=curStamp; if(st.fmin[d]==INF) stk.push_back(d);}
        }
        st.ops+=W;
        sort(U.begin(),U.end());
        ll rem=budget-st.ops; ll per=rem>0?rem/(n-i):0;
        ll K=per/(3*W+1); if(K<1)K=1; if(K>S)K=S;
        vector<int> sv(S); iota(sv.begin(),sv.end(),0);
        stable_sort(sv.begin(),sv.end(),[&](int a,int b){return st.freeT[a]<st.freeT[b];});
        vector<int> cands(sv.begin(),sv.begin()+K);
        size_t m=st.lg.size();
        ll bestF=INF; int bestKind=-1,bestS=-1;
        for(int s:cands){
            ll r=stratA(s,U,dl); st.undo(m);
            if(r<bestF){bestF=r;bestKind=0;bestS=s;}
        }
        {
            ll r=stratB(cands,U,dl); st.undo(m);
            if(r<bestF){bestF=r;bestKind=1;}
        }
        if(bestF<=dl){
            if(bestKind==0) stratA(bestS,U,dl); else stratB(cands,U,dl);
        }
    }
    ll score=0;
    for(int t=0;t<T;t++){ll x=st.fmin[tf[t]]; if(x<=td[t]) score+=td[t]-x+tg[t];}
    return {score,st.steps};
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    nm.resize(C);cc.resize(C);rr.resize(C);deps.resize(C);
    unordered_map<string,int> id; id.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>nm[i]>>cc[i]>>rr[i]; id[nm[i]]=i;
        int k;cin>>k; deps[i].resize(k);
        for(int j=0;j<k;j++){string s;cin>>s;deps[i][j]=id[s];}
    }
    tf.resize(T);td.resize(T);tg.resize(T);
    for(int i=0;i<T;i++){string s;cin>>s>>td[i]>>tg[i];tf[i]=id[s];}
    CP.assign(C,0);
    for(int i=0;i<C;i++){ll m=0;for(int d:deps[i])m=max(m,CP[d]);CP[i]=m+cc[i];}
    stampV.assign(C,0); inU.assign(C,0);

    vector<vector<int>> orders;
    vector<int> base(T); iota(base.begin(),base.end(),0);
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]<td[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]-CP[tf[a]]<td[b]-CP[tf[b]];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return tg[a]>tg[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return (double)tg[a]/CP[tf[a]]>(double)tg[b]/CP[tf[b]];}); orders.push_back(o);}
    vector<ll> budgets={300000000LL,130000000LL,130000000LL,130000000LL};
    ll bestScore=-1; vector<pair<int,int>> bestSteps;
    for(size_t i=0;i<orders.size();i++){
        auto res=runPass(orders[i],budgets[i]);
        if(res.first>bestScore){bestScore=res.first;bestSteps=res.second;}
    }
    if(bestSteps.empty()) bestSteps.push_back({0,0});
    string out; out.reserve(bestSteps.size()*16);
    out+=to_string(bestSteps.size()); out+='\n';
    for(auto&p:bestSteps){out+=nm[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
