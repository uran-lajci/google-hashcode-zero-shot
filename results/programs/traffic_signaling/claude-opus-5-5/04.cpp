#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> SB,SE,SL;
vector<string> SN;
vector<int> pathOff, pathData;
vector<vector<int>> ord, dur;
vector<int> offS, durS, cyc;
vector<long long> waitI;
vector<int> lastP, firstArr;
vector<vector<pair<int,int>>> buckets;

void rebuild(int x){
    int c=0;
    for(size_t j=0;j<ord[x].size();j++){int s=ord[x][j];offS[s]=c;durS[s]=dur[x][j];c+=dur[x][j];}
    cyc[x]=c;
}
inline long long nextGreen(int s,long long x){
    int C=cyc[SE[s]]; long long r=x%C; int o=offS[s],d=durS[s];
    if(r>=o&&r<o+d) return x;
    if(r<o) return x+(o-r);
    return x+(C-r)+o;
}
long long simulate(){
    for(int s=0;s<S;s++){lastP[s]=-1;firstArr[s]=INT_MAX;}
    for(int i=0;i<I;i++) waitI[i]=0;
    long long score=0;
    for(int c=0;c<V;c++) buckets[0].push_back({c,0});
    for(int t=0;t<D;t++){
        auto &bk=buckets[t];
        for(size_t q=0;q<bk.size();q++){
            int c=bk[q].first,k=bk[q].second;
            int s=pathData[pathOff[c]+k];
            if(firstArr[s]==INT_MAX) firstArr[s]=t;
            if(offS[s]<0) continue;
            long long x=max<long long>(t,lastP[s]+1);
            long long g=nextGreen(s,x);
            if(g>=D) continue;
            lastP[s]=(int)g;
            waitI[SE[s]]+=g-t;
            int P=pathOff[c+1]-pathOff[c];
            int ns=pathData[pathOff[c]+k+1];
            long long arr=g+SL[ns];
            if(k+1==P-1){ if(arr<=D) score+=F+D-arr; }
            else if(arr<D) buckets[arr].push_back({c,k+1});
        }
        bk.clear();
    }
    buckets[D].clear();
    return score;
}
int main(){
    auto t0=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>D>>I>>S>>V>>F;
    SB.resize(S);SE.resize(S);SL.resize(S);SN.resize(S);
    unordered_map<string,int> mp; mp.reserve(S*2);
    for(int i=0;i<S;i++){cin>>SB[i]>>SE[i]>>SN[i]>>SL[i];mp[SN[i]]=i;}
    pathOff.push_back(0);
    for(int c=0;c<V;c++){int P;cin>>P;for(int j=0;j<P;j++){string n;cin>>n;pathData.push_back(mp[n]);}pathOff.push_back(pathData.size());}
    vector<long long> cnt(S,0);
    for(int c=0;c<V;c++){
        long long sum=0;
        for(int j=pathOff[c]+1;j<pathOff[c+1];j++) sum+=SL[pathData[j]];
        if(sum>D) continue;
        for(int j=pathOff[c];j<pathOff[c+1]-1;j++) cnt[pathData[j]]++;
    }
    ord.assign(I,{});dur.assign(I,{});
    offS.assign(S,-1);durS.assign(S,0);cyc.assign(I,1);waitI.assign(I,0);
    lastP.assign(S,-1);firstArr.assign(S,INT_MAX);
    buckets.assign(D+1,{});
    // initial: streets with initial cars first
    vector<int> initCnt(S,0);
    for(int c=0;c<V;c++) initCnt[pathData[pathOff[c]]]++;
    for(int s=0;s<S;s++) if(cnt[s]>0) ord[SE[s]].push_back(s);
    for(int i=0;i<I;i++){
        sort(ord[i].begin(),ord[i].end(),[&](int a,int b){if(initCnt[a]!=initCnt[b])return initCnt[a]>initCnt[b];return a<b;});
        dur[i].assign(ord[i].size(),1);
        rebuild(i);
    }
    long long best=simulate();
    vector<vector<int>> bestOrd=ord,bestDur=dur;
    // refine order by first arrival, with various duration modes
    for(int mode=0;mode<3;mode++){
        for(int it=0;it<4;it++){
            vector<int> fa=firstArr;
            for(int i=0;i<I;i++){
                if(ord[i].empty())continue;
                sort(ord[i].begin(),ord[i].end(),[&](int a,int b){if(fa[a]!=fa[b])return fa[a]<fa[b];return a<b;});
                long long mn=LLONG_MAX; for(int s:ord[i]) mn=min(mn,cnt[s]);
                for(size_t j=0;j<ord[i].size();j++){
                    int d=1;
                    if(mode>0){ long long cap=mode==1?4:8; d=(int)min<long long>(cap,max<long long>(1,(cnt[ord[i][j]]+mn/2)/mn)); }
                    d=min(d,D);
                    dur[i][j]=d;
                }
                rebuild(i);
            }
            long long sc=simulate();
            if(sc>best){best=sc;bestOrd=ord;bestDur=dur;}
        }
    }
    ord=bestOrd;dur=bestDur;
    for(int i=0;i<I;i++) rebuild(i);
    best=simulate();
    // hill climb
    long long E=pathData.size();
    long long iters=min<long long>(200000,(long long)(3.5e8/(double)(E+S+D+I)));
    mt19937 rng(12345);
    vector<int> cand; for(int i=0;i<I;i++) if(!ord[i].empty()) cand.push_back(i);
    vector<double> cum;
    auto buildCum=[&](){cum.resize(cand.size());double a=0;for(size_t j=0;j<cand.size();j++){a+=1.0+waitI[cand[j]];cum[j]=a;}};
    if(!cand.empty()){
        buildCum();
        for(long long it=0;it<iters;it++){
            if((it&7)==0){double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();if(el>9.3)break;}
            double r=uniform_real_distribution<double>(0,cum.back())(rng);
            int idx=lower_bound(cum.begin(),cum.end(),r)-cum.begin(); if(idx>=(int)cand.size())idx=cand.size()-1;
            int x=cand[idx];
            int n=ord[x].size();
            vector<int> so=ord[x],sd=dur[x];
            int mv=rng()%3;
            if(n<2&&mv==0) mv=1;
            if(n==1){ continue; } // single street: always green anyway
            if(mv==0){int a=rng()%n,b=rng()%n;if(a==b)b=(a+1)%n;swap(ord[x][a],ord[x][b]);swap(dur[x][a],dur[x][b]);}
            else if(mv==1){int a=rng()%n;if(dur[x][a]>=D)continue;dur[x][a]++;}
            else{int a=rng()%n;if(dur[x][a]<=1){swap(ord[x][a],ord[x][(a+1)%n]);swap(dur[x][a],dur[x][(a+1)%n]);}else dur[x][a]--;}
            rebuild(x);
            long long sc=simulate();
            if(sc>=best){ if(sc>best){best=sc;} buildCum(); }
            else{ord[x]=so;dur[x]=sd;rebuild(x);}
        }
    }
    // output
    string out; int A=0;
    for(int i=0;i<I;i++) if(!ord[i].empty()) A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++){
        if(ord[i].empty())continue;
        out+=to_string(i)+"\n"+to_string(ord[i].size())+"\n";
        for(size_t j=0;j<ord[i].size();j++) out+=SN[ord[i][j]]+" "+to_string(max(1,min(D,dur[i][j])))+"\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
}
