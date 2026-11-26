#include <bits/stdc++.h>
using namespace std;

static char*buf;size_t bp=0,bl=0;
static inline void skip(){while(bp<bl&&(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;}
static long long readInt(){skip();long long r=0;bool neg=false;if(buf[bp]=='-'){neg=true;bp++;}while(bp<bl&&buf[bp]>='0'&&buf[bp]<='9'){r=r*10+(buf[bp]-'0');bp++;}return neg?-r:r;}
static string readStr(){skip();size_t s=bp;while(bp<bl&&!(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;return string(buf+s,bp-s);}

int D,I,S,V;long long F;
vector<int> SB,SE,SL;vector<string> SN;
vector<vector<int>> paths;
vector<vector<int>> ord; // per intersection order
vector<int> dur,off,cyc;
vector<int> lastDep,lastStamp;int stampId=0;
vector<vector<int>> buckets;
vector<int> posv;
vector<long long> waitI;vector<int> waitStamp;vector<int> touched;
// adaptive
vector<vector<int>> slotOwner;vector<char> assigned;

long long work=0;

inline long long nextGreen(int s,long long t){
    int d=dur[s];if(d==0)return LLONG_MAX/4;
    int i=SE[s];int C=cyc[i];int m=t%C;int o=off[s];
    if(m>=o&&m<o+d)return t;
    if(m<o)return t+(o-m);
    return t+(C-m+o);
}

void recompute(int i){
    int o=0;for(int s:ord[i]){off[s]=o;o+=dur[s];}cyc[i]=max(o,1);
}

long long sim(bool adaptive){
    stampId++;touched.clear();
    for(int c=0;c<V;c++){posv[c]=0;buckets[0].push_back(c);}
    long long score=0;long long steps=0;
    for(int t=0;t<=D;t++){
        auto &b=buckets[t];
        for(size_t k=0;k<b.size();k++){
            int c=b[k];steps++;
            const auto&p=paths[c];int ps=posv[c];
            if(ps==(int)p.size()-1){score+=F+D-t;continue;}
            int s=p[ps];
            long long t0=t;
            if(lastStamp[s]==stampId&&lastDep[s]+1>t0)t0=lastDep[s]+1;
            if(adaptive&&!assigned[s]&&dur[s]>0){
                int i=SE[s];int K=cyc[i];int st=t0%K;
                auto&so=slotOwner[i];
                for(int q=0;q<K;q++){int sl=(st+q)%K;if(so[sl]<0){so[sl]=s;off[s]=sl;break;}}
                assigned[s]=1;
            }
            long long g=nextGreen(s,t0);
            if(g>=D)continue;
            lastStamp[s]=stampId;lastDep[s]=(int)g;
            int i=SE[s];
            if(waitStamp[i]!=stampId){waitStamp[i]=stampId;waitI[i]=0;touched.push_back(i);}
            waitI[i]+=g-t;
            posv[c]=ps+1;
            long long arr=g+SL[p[ps+1]];
            if(arr<=D)buckets[arr].push_back(c);
        }
        b.clear();
    }
    work+=steps+D+V;
    return score;
}

int main(){
    // read all
    {
        size_t cap=1<<20;buf=(char*)malloc(cap);size_t n;
        while((n=fread(buf+bl,1,cap-bl,stdin))>0){bl+=n;if(bl==cap){cap*=2;buf=(char*)realloc(buf,cap);}}
    }
    D=readInt();I=readInt();S=readInt();V=readInt();F=readInt();
    SB.resize(S);SE.resize(S);SL.resize(S);SN.resize(S);
    unordered_map<string,int> mp;mp.reserve(S*2);
    for(int s=0;s<S;s++){SB[s]=readInt();SE[s]=readInt();SN[s]=readStr();SL[s]=readInt();mp[SN[s]]=s;}
    paths.resize(V);
    vector<long long> cnt(S,0);
    for(int c=0;c<V;c++){int P=readInt();paths[c].resize(P);for(int j=0;j<P;j++){paths[c][j]=mp[readStr()];}
        for(int j=0;j+1<P;j++)cnt[paths[c][j]]++;}
    ord.assign(I,{});dur.assign(S,0);off.assign(S,0);cyc.assign(I,1);
    for(int s=0;s<S;s++)if(cnt[s]>0){ord[SE[s]].push_back(s);dur[s]=1;}
    lastDep.assign(S,0);lastStamp.assign(S,0);buckets.assign(D+1,{});posv.assign(V,0);
    waitI.assign(I,0);waitStamp.assign(I,0);
    // adaptive initial
    slotOwner.assign(I,{});assigned.assign(S,0);
    for(int i=0;i<I;i++){cyc[i]=max((int)ord[i].size(),1);slotOwner[i].assign(ord[i].size(),-1);}
    sim(true);
    for(int i=0;i<I;i++){
        auto&so=slotOwner[i];
        for(int s:ord[i])if(!assigned[s]){for(size_t q=0;q<so.size();q++)if(so[q]<0){so[q]=s;break;}}
        ord[i].clear();for(int s:so)if(s>=0)ord[i].push_back(s);
        recompute(i);
    }
    slotOwner.clear();slotOwner.shrink_to_fit();
    long long best=sim(false);
    auto buildCand=[&](vector<int>&cand){cand.clear();for(int i:touched)if(waitI[i]>0&&ord[i].size()>=2)cand.push_back(i);};
    vector<int> cand;buildCand(cand);
    mt19937 rng(12345);
    const long long BUDGET=150000000LL;
    vector<int> savedOrd;vector<int> savedDur;
    while(work<BUDGET&&!cand.empty()){
        int i=cand[rng()%cand.size()];
        auto&o=ord[i];int k=o.size();
        savedOrd=o;savedDur.clear();for(int s:o)savedDur.push_back(dur[s]);
        int mv=rng()%4;
        if(mv<2){int a=rng()%k,b=rng()%k;if(a==b)b=(a+1)%k;swap(o[a],o[b]);}
        else if(mv==2){int a=rng()%k;if(dur[o[a]]<D)dur[o[a]]++;}
        else{int a=rng()%k;if(dur[o[a]]>1)dur[o[a]]--;else if(dur[o[a]]<D)dur[o[a]]++;}
        recompute(i);
        long long sc=sim(false);
        if(sc>=best){best=sc;buildCand(cand);}
        else{o=savedOrd;for(int q=0;q<k;q++)dur[o[q]]=savedDur[q];recompute(i);}
    }
    // output
    string out;int A=0;
    for(int i=0;i<I;i++)if(!ord[i].empty())A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++)if(!ord[i].empty()){
        out+=to_string(i)+"\n"+to_string(ord[i].size())+"\n";
        for(int s:ord[i]){int d=min(max(dur[s],1),D);out+=SN[s]+" "+to_string(d)+"\n";}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
