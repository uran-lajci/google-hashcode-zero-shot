#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> SB,SE,SL;
vector<string> SN;
vector<int> pathOff,pathLen,paths;
vector<int> cntS;
vector<vector<int>> inc;
vector<long long> offS; vector<int> duS; vector<long long> cyc;
vector<vector<pair<int,int>>> sched;
const long long INF=LLONG_MAX/4;

// sim state
vector<vector<int>> q; vector<int> qh;
vector<vector<int>> arrB, evB;
vector<int> posC, arrT;
vector<long long> waitI;
long long work=0;
bool dynMode=false;
vector<vector<int>> slotOwn;

inline long long nextGreen(int s,long long t){
    if(offS[s]<0){
        if(!dynMode||cntS[s]==0) return INF;
        int i=SE[s]; long long C=cyc[i]; int r=t%C;
        auto &sl=slotOwn[i];
        for(int k=0;k<C;k++){int x=(r+k)%C; if(sl[x]<0){sl[x]=s;offS[s]=x;duS[s]=1;break;}}
        if(offS[s]<0) return INF;
    }
    long long C=cyc[SE[s]]; long long r=t%C; long long o=offS[s],d=duS[s];
    if(r>=o&&r<o+d) return t;
    if(r<o) return t+(o-r);
    return t+(C-r)+o;
}

long long simulate(){
    for(int s=0;s<S;s++){q[s].clear();qh[s]=0;}
    for(int t=0;t<=D;t++){arrB[t].clear();evB[t].clear();}
    fill(waitI.begin(),waitI.end(),0);
    vector<int> order;
    for(int c=0;c<V;c++){
        posC[c]=0;arrT[c]=0;int s=paths[pathOff[c]];
        if(q[s].empty())order.push_back(s);
        q[s].push_back(c);
    }
    for(int s:order){long long g=nextGreen(s,0); if(g<=D)evB[g].push_back(s);}
    long long score=0; long long ev=0;
    for(int t=0;t<=D;t++){
        auto &A=arrB[t];
        for(int c:A){
            int s=paths[pathOff[c]+posC[c]];
            q[s].push_back(c);
            if((int)q[s].size()-qh[s]==1){long long g=nextGreen(s,t); if(g<=D)evB[g].push_back(s);}
        }
        ev+=A.size();
        auto &E=evB[t];
        for(size_t k=0;k<E.size();k++){
            int s=E[k];
            int c=q[s][qh[s]++];
            waitI[SE[s]]+=t-arrT[c];
            posC[c]++;
            int ns=paths[pathOff[c]+posC[c]];
            long long at=t+SL[ns];
            if(posC[c]==pathLen[c]-1){
                if(at<=D)score+=F+(D-at);
            }else if(at<=D){arrT[c]=at;arrB[at].push_back(c);}
            if(qh[s]<(int)q[s].size()){long long g=nextGreen(s,t+1); if(g<=D)evB[g].push_back(s);}
        }
        ev+=E.size();
    }
    work+=ev+D+V;
    return score;
}

void applyInter(int i){
    long long o=0;
    for(auto &p:sched[i]){offS[p.first]=o;duS[p.first]=p.second;o+=p.second;}
    cyc[i]=max(1LL,o);
}

int main(){
    auto st=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>D>>I>>S>>V>>F;
    SB.resize(S);SE.resize(S);SL.resize(S);SN.resize(S);
    unordered_map<string,int> mp; mp.reserve(S*2);
    for(int s=0;s<S;s++){cin>>SB[s]>>SE[s]>>SN[s]>>SL[s];mp[SN[s]]=s;}
    pathOff.resize(V);pathLen.resize(V);
    for(int c=0;c<V;c++){int P;cin>>P;pathOff[c]=paths.size();pathLen[c]=P;
        for(int k=0;k<P;k++){string x;cin>>x;paths.push_back(mp[x]);}}
    cntS.assign(S,0);
    for(int c=0;c<V;c++){
        long long m=0;for(int k=1;k<pathLen[c];k++)m+=SL[paths[pathOff[c]+k]];
        if(m>D)continue;
        for(int k=0;k<pathLen[c]-1;k++)cntS[paths[pathOff[c]+k]]++;
    }
    inc.assign(I,{});
    for(int s=0;s<S;s++)if(cntS[s]>0)inc[SE[s]].push_back(s);
    q.assign(S,{});qh.assign(S,0);arrB.assign(D+1,{});evB.assign(D+1,{});
    posC.assign(V,0);arrT.assign(V,0);waitI.assign(I,0);
    offS.assign(S,-1);duS.assign(S,1);cyc.assign(I,1);
    // dynamic initial
    dynMode=true;slotOwn.assign(I,{});
    for(int i=0;i<I;i++){cyc[i]=max<size_t>(1,inc[i].size());slotOwn[i].assign(inc[i].size(),-1);}
    simulate();
    dynMode=false;
    sched.assign(I,{});
    for(int i=0;i<I;i++){
        auto &sl=slotOwn[i];
        for(int s:inc[i])if(offS[s]<0){for(auto &x:sl)if(x<0){x=s;offS[s]=0;break;}}
        for(int x:sl)if(x>=0)sched[i].push_back({x,1});
    }
    for(int i=0;i<I;i++)applyInter(i);
    long long best=simulate();
    // candidate b: proportional durations
    {
        auto sb=sched;
        for(int i=0;i<I;i++){
            if(sched[i].size()<2)continue;
            int mn=INT_MAX;for(auto&p:sched[i])mn=min(mn,cntS[p.first]);
            for(auto&p:sched[i]){int d=(int)llround((double)cntS[p.first]/mn);p.second=max(1,min(d,min(8,D)));}
            applyInter(i);
        }
        long long sc=simulate();
        if(sc>best){best=sc;}
        else{sched=sb;for(int i=0;i<I;i++)applyInter(i);simulate();}
    }
    // local search
    mt19937 rng(12345);
    vector<int> cand;for(int i=0;i<I;i++)if(sched[i].size()>=2)cand.push_back(i);
    vector<long long> bestWait=waitI;
    vector<double> cum(cand.size());
    auto rebuild=[&](){double a=0;for(size_t k=0;k<cand.size();k++){a+=bestWait[cand[k]]+1.0;cum[k]=a;}};
    if(!cand.empty())rebuild();
    const long long BUDGET=150000000LL;
    int it=0;
    while(!cand.empty()&&work<BUDGET){
        if((++it&7)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>9.0)break;
        }
        double r=uniform_real_distribution<double>(0,cum.back())(rng);
        int idx=lower_bound(cum.begin(),cum.end(),r)-cum.begin();
        if(idx>=(int)cand.size())idx=cand.size()-1;
        int i=cand[idx];
        auto &sc=sched[i];int n=sc.size();
        auto bak=sc;
        int mv=rng()%2;
        if(mv==0){int a=rng()%n,b=rng()%n;if(a==b)b=(a+1)%n;swap(sc[a],sc[b]);}
        else{int a=rng()%n;if(sc[a].second>1&&(rng()&1))sc[a].second--;else if(sc[a].second<D)sc[a].second++;else continue;}
        applyInter(i);
        long long s2=simulate();
        if(s2>=best){
            bool imp=s2>best;best=s2;
            if(imp){bestWait=waitI;rebuild();}
        }else{sc=bak;applyInter(i);}
    }
    // output
    string out;int A=0;
    for(int i=0;i<I;i++)if(!sched[i].empty())A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++){
        if(sched[i].empty())continue;
        out+=to_string(i)+"\n"+to_string(sched[i].size())+"\n";
        for(auto&p:sched[i])out+=SN[p.first]+" "+to_string(p.second)+"\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
}
