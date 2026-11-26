#include <bits/stdc++.h>
using namespace std;

static string inbuf; static size_t ip=0;
static inline void skipws(){while(ip<inbuf.size()&&(unsigned char)inbuf[ip]<=' ')ip++;}
static inline long long readInt(){skipws();long long r=0;bool neg=false;if(inbuf[ip]=='-'){neg=true;ip++;}
    while(ip<inbuf.size()&&inbuf[ip]>='0'&&inbuf[ip]<='9'){r=r*10+(inbuf[ip]-'0');ip++;}return neg?-r:r;}
static inline string readTok(){skipws();size_t s=ip;while(ip<inbuf.size()&&(unsigned char)inbuf[ip]>' ')ip++;return inbuf.substr(s,ip-s);}

int D,I,S,V,F;
vector<int> SB,SE,SL;
vector<string> names;
vector<int> flat, pst, plen; int NC=0;
vector<vector<int>> ord;
vector<int> durS,startS,cyc;
vector<int> headT,nxtC,posC;
vector<int> lastPass;
vector<long long> waitI;

void recompute(int i){int acc=0;for(int s:ord[i]){startS[s]=acc;acc+=durS[s];}cyc[i]=acc;}

long long simulate(vector<long long>&w){
    for(int i=0;i<I;i++) w[i]=0;
    fill(lastPass.begin(),lastPass.end(),-1);
    fill(headT.begin(),headT.end(),-1);
    for(int c=NC-1;c>=0;c--){posC[c]=0;nxtC[c]=headT[0];headT[0]=c;}
    long long score=0;
    for(int t=0;t<=D;t++){
        int c=headT[t];
        while(c!=-1){
            int nc=nxtC[c];
            int s=flat[pst[c]+posC[c]];
            int i=SE[s];
            int tt=max(t,lastPass[s]+1);
            int C=cyc[i];
            int r=tt%C, st=startS[s], du=durS[s];
            int dep;
            if(r>=st&&r<st+du) dep=tt;
            else if(r<st) dep=tt+(st-r);
            else dep=tt+(C-r)+st;
            lastPass[s]=dep;
            w[i]+=dep-t;
            if(dep<=D){
                posC[c]++;
                int ns=flat[pst[c]+posC[c]];
                int arr=dep+SL[ns];
                if(posC[c]==plen[c]-1){
                    if(arr<=D) score+=F+(D-arr);
                } else if(arr<=D){
                    nxtC[c]=headT[arr];headT[arr]=c;
                }
            }
            c=nc;
        }
    }
    return score;
}

int main(){
    {std::ios::sync_with_stdio(false);
     std::ostringstream ss; ss<<cin.rdbuf(); inbuf=ss.str();}
    D=readInt();I=readInt();S=readInt();V=readInt();F=readInt();
    SB.resize(S);SE.resize(S);SL.resize(S);names.resize(S);
    unordered_map<string,int> mp; mp.reserve(S*2);
    for(int s=0;s<S;s++){SB[s]=readInt();SE[s]=readInt();names[s]=readTok();SL[s]=readInt();mp[names[s]]=s;}
    vector<long long> cnt(S,0);
    vector<int> firstArr(S,INT_MAX);
    for(int v=0;v<V;v++){
        int P=readInt();
        vector<int> p(P);
        for(int k=0;k<P;k++){p[k]=mp[readTok()];}
        long long tot=0;for(int k=1;k<P;k++)tot+=SL[p[k]];
        if(tot>D) continue;
        pst.push_back(flat.size());plen.push_back(P);
        for(int k=0;k<P;k++)flat.push_back(p[k]);
        long long t=0;
        for(int k=0;k<P-1;k++){
            if(k>0)t+=SL[p[k]];
            cnt[p[k]]++;
            if(t<firstArr[p[k]])firstArr[p[k]]=(int)t;
        }
        NC++;
    }
    ord.assign(I,{});
    for(int s=0;s<S;s++) if(cnt[s]>0) ord[SE[s]].push_back(s);
    for(int i=0;i<I;i++) sort(ord[i].begin(),ord[i].end(),[&](int a,int b){if(firstArr[a]!=firstArr[b])return firstArr[a]<firstArr[b];return a<b;});
    durS.assign(S,1);startS.assign(S,0);cyc.assign(I,1);
    headT.assign(D+2,-1);nxtC.assign(max(NC,1),-1);posC.assign(max(NC,1),0);
    lastPass.assign(S,-1);waitI.assign(I,0);
    vector<long long> wtmp(I,0);

    // initial variants
    vector<int> caps={1,2,3,5,D};
    long long best=-1; vector<int> bestDur(S,1);
    for(int cap:caps){
        for(int i=0;i<I;i++){
            if(ord[i].empty())continue;
            long long mn=LLONG_MAX;for(int s:ord[i])mn=min(mn,cnt[s]);
            for(int s:ord[i]){
                long long d=(cnt[s]+mn/2)/mn; if(d<1)d=1; if(d>cap)d=cap;
                durS[s]=(int)d;
            }
            recompute(i);
        }
        long long sc=simulate(wtmp);
        if(sc>best){best=sc;bestDur=durS;waitI=wtmp;}
    }
    durS=bestDur;
    for(int i=0;i<I;i++) if(!ord[i].empty()) recompute(i);
    long long cur=best;

    vector<int> cand;
    for(int i=0;i<I;i++) if(ord[i].size()>=2) cand.push_back(i);
    long long work=(long long)flat.size()+D+S+I+1000;
    long long iters=(long long)(2.5e8/(double)work);
    if(iters>200000)iters=200000;
    if(cand.empty())iters=0;
    mt19937_64 rng(12345);
    vector<long long> pref(cand.size()+1,0);
    auto buildPref=[&](){for(size_t k=0;k<cand.size();k++)pref[k+1]=pref[k]+waitI[cand[k]]+1;};
    if(!cand.empty())buildPref();
    for(long long it=0;it<iters;it++){
        int i;
        if(rng()%2){ i=cand[rng()%cand.size()]; }
        else{
            long long x=(long long)(rng()%(unsigned long long)pref.back());
            int k=upper_bound(pref.begin(),pref.end(),x)-pref.begin()-1;
            i=cand[k];
        }
        auto &o=ord[i]; int n=o.size();
        int type=rng()%3;
        int a=rng()%n,b=rng()%n; int oldd=0; bool ok=true;
        if(type==0){ if(a==b)b=(a+1)%n; swap(o[a],o[b]); }
        else if(type==1){ oldd=durS[o[a]]; durS[o[a]]++; }
        else { oldd=durS[o[a]]; if(oldd<=1)ok=false; else durS[o[a]]--; }
        if(!ok){it--;continue;}
        recompute(i);
        long long sc=simulate(wtmp);
        if(sc>=cur){
            cur=sc; swap(waitI,wtmp); buildPref();
        } else {
            if(type==0) swap(o[a],o[b]); else durS[o[a]]=oldd;
            recompute(i);
        }
    }

    string out; int A=0;
    for(int i=0;i<I;i++) if(!ord[i].empty()) A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++){
        if(ord[i].empty())continue;
        out+=to_string(i)+"\n"+to_string(ord[i].size())+"\n";
        for(int s:ord[i]){ out+=names[s]; out+=' '; out+=to_string(min(durS[s],D)); out+='\n'; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
