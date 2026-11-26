#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF = LLONG_MAX/4;

int C,T,S;
vector<ll> cc, rr;
vector<vector<int>> deps;
vector<int> tf; vector<ll> td, tg;
vector<ll> cp;

vector<ll> ff; vector<int> sf; vector<ll> freeT;
vector<pair<int,int>> steps;
vector<int> stamp; int curStamp=0;
vector<ll> mL, mG; vector<char> touchedFlag;
ll work=0;
chrono::steady_clock::time_point t0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();}

void placeFile(int f){
    vector<int> touched;
    for(int d:deps[f]){
        int s=sf[d];
        if(!touchedFlag[s]){touchedFlag[s]=1;touched.push_back(s);mL[s]=0;mG[s]=0;}
        mL[s]=max(mL[s],ff[d]);
        mG[s]=max(mG[s],ff[d]+rr[d]);
    }
    ll b1=0,b2=0;int b1s=-1;
    for(int s:touched){
        ll v=mG[s];
        if(v>b1){b2=b1;b1=v;b1s=s;}
        else if(v>b2)b2=v;
    }
    ll best=INF;int bs=0;
    for(int s=0;s<S;s++){
        ll other=(s==b1s)?b2:b1;
        ll loc=touchedFlag[s]?mL[s]:0;
        ll ready=max(other,max(loc,freeT[s]));
        ll fin=ready+cc[f];
        if(fin<best){best=fin;bs=s;}
    }
    for(int s:touched)touchedFlag[s]=0;
    work+=deps[f].size()+S;
    ff[f]=best;sf[f]=bs;freeT[bs]=best;steps.push_back({f,bs});
}

// returns score or -1 if aborted
ll runPass(const vector<int>& order, ll budget, bool mustComplete){
    ff.assign(C,INF);sf.assign(C,0);freeT.assign(S,0);steps.clear();
    ll startWork=work;
    vector<int> stk,lst;
    for(int ti:order){
        if(work-startWork>budget){ if(!mustComplete) return -1; }
        if(mustComplete && elapsed()>9.0) break;
        int t=tf[ti];
        if(ff[t]<INF)continue;
        if(cp[t]>td[ti])continue;
        curStamp++;
        stk.clear();lst.clear();
        stk.push_back(t);stamp[t]=curStamp;
        while(!stk.empty()){
            int f=stk.back();stk.pop_back();lst.push_back(f);
            for(int d:deps[f]){
                work++;
                if(ff[d]<INF||stamp[d]==curStamp)continue;
                stamp[d]=curStamp;stk.push_back(d);
            }
        }
        sort(lst.begin(),lst.end());
        vector<ll> saveFree=freeT;size_t ss=steps.size();
        for(int f:lst)placeFile(f);
        if(ff[t]>td[ti]){
            for(int f:lst)ff[f]=INF;
            freeT=saveFree;steps.resize(ss);
        }
    }
    ll sc=0;
    for(int i=0;i<T;i++){int t=tf[i];if(ff[t]<=td[i])sc+=td[i]-ff[t]+tg[i];}
    return sc;
}

int main(){
    t0=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    unordered_map<string,int> id;id.reserve(C*2);
    vector<string> names(C);cc.resize(C);rr.resize(C);deps.resize(C);
    for(int i=0;i<C;i++){
        cin>>names[i]>>cc[i]>>rr[i];id[names[i]]=i;
        int n;cin>>n;deps[i].resize(n);
        for(int j=0;j<n;j++){string s;cin>>s;deps[i][j]=id[s];}
    }
    tf.resize(T);td.resize(T);tg.resize(T);
    for(int i=0;i<T;i++){string s;cin>>s>>td[i]>>tg[i];tf[i]=id[s];}
    cp.assign(C,0);
    for(int i=0;i<C;i++){ll m=0;for(int d:deps[i])m=max(m,cp[d]);cp[i]=m+cc[i];}
    stamp.assign(C,0);mL.assign(S,0);mG.assign(S,0);touchedFlag.assign(S,0);

    vector<vector<int>> orders;
    vector<int> base(T);iota(base.begin(),base.end(),0);
    {auto o=base;stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]<td[b];});orders.push_back(o);}
    {auto o=base;stable_sort(o.begin(),o.end(),[&](int a,int b){return tg[a]>tg[b];});orders.push_back(o);}
    {auto o=base;stable_sort(o.begin(),o.end(),[&](int a,int b){return td[a]-cp[tf[a]]<td[b]-cp[tf[b]];});orders.push_back(o);}
    {auto o=base;stable_sort(o.begin(),o.end(),[&](int a,int b){
        return (double)(tg[a]+td[a]-cp[tf[a]])/cp[tf[a]] > (double)(tg[b]+td[b]-cp[tf[b]])/cp[tf[b]];});orders.push_back(o);}

    const ll TOTAL=400000000LL;
    ll bestScore=-1;vector<pair<int,int>> bestSteps;vector<int> bestOrder;
    for(size_t k=0;k<orders.size();k++){
        bool first=(k==0);
        ll remaining=TOTAL-work;
        if(!first&&remaining<=0)break;
        ll sc=runPass(orders[k],first?INF:remaining,first);
        if(sc>bestScore){bestScore=sc;bestSteps=steps;bestOrder=orders[k];}
        if(elapsed()>8.0)break;
    }
    mt19937 rng(12345);
    if(T>1){
        while(work<TOTAL && elapsed()<8.0){
            vector<int> o=bestOrder;
            int a=rng()%T,b=rng()%T;
            if(a==b)continue;
            if(rng()%2) swap(o[a],o[b]);
            else { int x=o[a]; o.erase(o.begin()+a); o.insert(o.begin()+b,x); }
            ll sc=runPass(o,TOTAL-work,false);
            if(sc<0)break;
            if(sc>=bestScore){
                if(sc>bestScore||true){bestScore=sc;bestSteps=steps;bestOrder=o;}
            }
        }
    }
    if(bestSteps.empty()){
        bestSteps.push_back({0,0});
    }
    string out;
    out+=to_string(bestSteps.size())+"\n";
    for(auto&p:bestSteps){out+=names[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
