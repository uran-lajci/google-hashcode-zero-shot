#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> B,E,L;
vector<string> nm;
vector<int> pathOff, pathData;
vector<vector<int>> order_;
vector<int> dur, off_, cyc;
vector<char> inS;
vector<int> lastDep;
vector<long long> waitI;
vector<vector<int>> buckets;
vector<int> posC;
const int INF = INT_MAX/2;

static inline int nextGreen(int s,int t){
    if(!inS[s]) return INF;
    int C=cyc[E[s]];
    int r=t%C, o=off_[s], d=dur[s];
    if(r>=o && r<o+d) return t;
    int w=o-r; if(w<0) w+=C;
    return t+w;
}

void recomputeInter(int i){
    int o=0;
    for(int s:order_[i]){ off_[s]=o; o+=dur[s]; }
    cyc[i]=o;
}

long long sim(bool assign, vector<vector<int>>* slots=nullptr, vector<char>* assigned=nullptr){
    fill(lastDep.begin(),lastDep.end(),-1);
    fill(waitI.begin(),waitI.end(),0);
    for(int t=0;t<=D;t++) buckets[t].clear();
    long long score=0;
    for(int c=0;c<V;c++){ posC[c]=pathOff[c]; buckets[0].push_back(c); }
    for(int t=0;t<D;t++){
        auto &bk=buckets[t];
        for(size_t k=0;k<bk.size();k++){
            int c=bk[k];
            int p=posC[c];
            int s=pathData[p];
            if(assign && !(*assigned)[s]){
                int i=E[s]; int C=cyc[i];
                int r=t%C;
                auto &sl=(*slots)[i];
                while(sl[r]!=-1){ r++; if(r==C) r=0; }
                sl[r]=s; off_[s]=r; (*assigned)[s]=1;
            }
            int st=max(t,lastDep[s]+1);
            int dep=nextGreen(s,st);
            if(dep>=INF) continue;
            waitI[E[s]]+=dep-t;
            if(dep>D) continue;
            lastDep[s]=dep;
            p++;
            int nx=pathData[p];
            long long arr=(long long)dep+L[nx];
            if(p==pathOff[c+1]-1){
                if(arr<=D) score+=F+(D-arr);
            } else {
                if(arr<D){ posC[c]=p; buckets[arr].push_back(c); }
            }
        }
    }
    return score;
}

int main(){
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    ios::sync_with_stdio(false); cin.tie(nullptr);
    cin>>D>>I>>S>>V>>F;
    B.resize(S);E.resize(S);L.resize(S);nm.resize(S);
    unordered_map<string,int> mp; mp.reserve(S*2);
    for(int i=0;i<S;i++){ cin>>B[i]>>E[i]>>nm[i]>>L[i]; mp[nm[i]]=i; }
    pathOff.resize(V+1);
    for(int c=0;c<V;c++){
        int P; cin>>P; pathOff[c]=pathData.size();
        string x;
        for(int j=0;j<P;j++){ cin>>x; pathData.push_back(mp[x]); }
    }
    pathOff[V]=pathData.size();
    vector<int> cnt(S,0);
    for(int c=0;c<V;c++) for(int p=pathOff[c];p<pathOff[c+1]-1;p++) cnt[pathData[p]]++;
    order_.assign(I,{}); dur.assign(S,1); off_.assign(S,0); cyc.assign(I,0); inS.assign(S,0);
    lastDep.assign(S,-1); waitI.assign(I,0); buckets.assign(D+1,{}); posC.assign(V,0);
    vector<int> kcnt(I,0);
    for(int s=0;s<S;s++) if(cnt[s]>0){ inS[s]=1; kcnt[E[s]]++; }
    for(int i=0;i<I;i++) cyc[i]=max(1,kcnt[i]);
    vector<vector<int>> slots(I);
    for(int i=0;i<I;i++) slots[i].assign(kcnt[i],-1);
    vector<char> assigned(S,0);
    sim(true,&slots,&assigned);
    for(int s=0;s<S;s++) if(inS[s] && !assigned[s]){
        auto &sl=slots[E[s]];
        for(auto &x:sl) if(x==-1){ x=s; break; }
        assigned[s]=1;
    }
    for(int i=0;i<I;i++){
        order_[i]=slots[i];
        recomputeInter(i);
    }
    long long cur=sim(false);
    mt19937 rng(12345);
    vector<int> cand; vector<long long> pref;
    auto rebuild=[&](){
        cand.clear(); pref.clear(); long long acc=0;
        for(int i=0;i<I;i++) if(order_[i].size()>=2 && waitI[i]>0){
            acc+=waitI[i]; cand.push_back(i); pref.push_back(acc);
        }
    };
    rebuild();
    vector<int> allMulti;
    for(int i=0;i<I;i++) if(order_[i].size()>=2) allMulti.push_back(i);
    const double LIMIT=8.3;
    if(!allMulti.empty()){
        while(elapsed()<LIMIT){
            int i;
            if(!cand.empty() && (rng()%10)!=0){
                long long x=(long long)(rng()%(unsigned long long)pref.back());
                int idx=upper_bound(pref.begin(),pref.end(),x)-pref.begin();
                i=cand[idx];
            } else i=allMulti[rng()%allMulti.size()];
            auto &ord=order_[i];
            int k=ord.size();
            int type=rng()%2;
            int a=-1,b=-1,s=-1,delta=0;
            if(type==0){
                a=rng()%k; b=rng()%k; if(a==b) b=(a+1)%k;
                swap(ord[a],ord[b]);
            } else {
                s=ord[rng()%k];
                if(dur[s]>1 && (rng()%2)) delta=-1; else delta=1;
                if(dur[s]+delta>D) delta=-1;
                if(dur[s]+delta<1) continue;
                dur[s]+=delta;
            }
            recomputeInter(i);
            long long sc=sim(false);
            if(sc>=cur){
                cur=sc;
                rebuild();
            } else {
                if(type==0) swap(ord[a],ord[b]);
                else dur[s]-=delta;
                recomputeInter(i);
            }
        }
    }
    // output
    string out;
    int A=0;
    for(int i=0;i<I;i++) if(!order_[i].empty()) A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++) if(!order_[i].empty()){
        out+=to_string(i)+"\n"+to_string(order_[i].size())+"\n";
        for(int s:order_[i]){ out+=nm[s]; out+=' '; out+=to_string(dur[s]); out+='\n'; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
