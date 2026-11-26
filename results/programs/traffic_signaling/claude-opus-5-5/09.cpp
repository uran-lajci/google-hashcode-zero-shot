#include <bits/stdc++.h>
using namespace std;

static char*buf;static size_t bp=0,bl=0;
static inline void skipws(){while(bp<bl&&(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;}
static inline long long readInt(){skipws();long long x=0;bool neg=false;if(buf[bp]=='-'){neg=true;bp++;}while(bp<bl&&buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}return neg?-x:x;}
static inline string readStr(){skipws();size_t s=bp;while(bp<bl&&!(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;return string(buf+s,bp-s);}

int D,I,S,V,F;
vector<int> SB,SE,SL;vector<string> SN;
vector<int> pstart,plen,pathv;
vector<vector<int>> order_;
vector<int> dur,off,cyc;
vector<vector<int>> bucket;
vector<int> lastp,posv;
vector<int> waitStreets; vector<long long> waitCum;
bool buildMode=false;
vector<int> slotBase; vector<int> slotOwner;

static inline int nextGreen(int t,int s){
    int i=SE[s];int C=cyc[i];int o=off[s],d=dur[s];
    int r=t%C;
    if(r>=o&&r<o+d)return t;
    if(r<o)return t+(o-r);
    return t+(C-r)+o;
}

long long evalSim(bool recordWait){
    for(auto&b:bucket)b.clear();
    fill(lastp.begin(),lastp.end(),-1);
    if(recordWait){waitStreets.clear();waitCum.clear();}
    long long score=0,wc=0;
    for(int c=0;c<V;c++){posv[c]=0;bucket[0].push_back(c);}
    for(int t=0;t<D;t++){
        auto&bk=bucket[t];
        for(size_t k=0;k<bk.size();k++){
            int c=bk[k];
            int s=pathv[pstart[c]+posv[c]];
            int i=SE[s];
            if(cyc[i]==0||dur[s]==0)continue;
            if(buildMode&&off[s]==-1){
                int C=cyc[i];int st=t%C;
                for(int j=0;j<C;j++){int sl=(st+j)%C;if(slotOwner[slotBase[i]+sl]==-1){slotOwner[slotBase[i]+sl]=s;off[s]=sl;break;}}
            }
            int e=max(t,lastp[s]+1);
            int p=nextGreen(e,s);
            if(p>=D)continue;
            lastp[s]=p;
            if(recordWait&&p>t){wc+=p-t;waitStreets.push_back(s);waitCum.push_back(wc);}
            posv[c]++;
            int ns=pathv[pstart[c]+posv[c]];
            int arr=p+SL[ns];
            if(posv[c]==plen[c]-1){ if(arr<=D)score+=F+(D-arr);}
            else if(arr<D)bucket[arr].push_back(c);
        }
    }
    return score;
}

void recompute(int i){int o=0;for(int s:order_[i]){off[s]=o;o+=dur[s];}cyc[i]=o;}

int main(){
    // read all
    {
        size_t cap=1<<20;buf=(char*)malloc(cap);
        size_t n;
        while((n=fread(buf+bl,1,cap-bl,stdin))>0){bl+=n;if(bl==cap){cap*=2;buf=(char*)realloc(buf,cap);}}
    }
    auto T0=chrono::steady_clock::now();
    D=readInt();I=readInt();S=readInt();V=readInt();F=readInt();
    SB.resize(S);SE.resize(S);SL.resize(S);SN.resize(S);
    unordered_map<string,int> mp;mp.reserve(S*2);
    for(int s=0;s<S;s++){SB[s]=readInt();SE[s]=readInt();SN[s]=readStr();SL[s]=readInt();mp[SN[s]]=s;}
    pstart.resize(V);plen.resize(V);
    for(int c=0;c<V;c++){int P=readInt();pstart[c]=pathv.size();plen[c]=P;for(int j=0;j<P;j++){string nm=readStr();pathv.push_back(mp[nm]);}}
    vector<long long> cnt(S,0);
    for(int c=0;c<V;c++){
        long long tot=0;for(int j=1;j<plen[c];j++)tot+=SL[pathv[pstart[c]+j]];
        if(tot>D)continue;
        for(int j=0;j<plen[c]-1;j++)cnt[pathv[pstart[c]+j]]++;
    }
    vector<vector<int>> used(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)used[SE[s]].push_back(s);
    order_.assign(I,{});dur.assign(S,0);off.assign(S,0);cyc.assign(I,0);
    bucket.assign(D+1,{});lastp.assign(S,-1);posv.assign(V,0);

    // Schedule B: by count desc, dur 1
    for(int i=0;i<I;i++){order_[i]=used[i];sort(order_[i].begin(),order_[i].end(),[&](int a,int b){if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];return a<b;});for(int s:order_[i])dur[s]=1;recompute(i);}
    long long scB=evalSim(false);
    vector<vector<int>> ordB=order_;
    // Schedule A: adaptive build
    slotBase.assign(I,0);int tot=0;
    for(int i=0;i<I;i++){slotBase[i]=tot;tot+=used[i].size();cyc[i]=used[i].size();for(int s:used[i]){dur[s]=1;off[s]=-1;}}
    slotOwner.assign(tot,-1);
    buildMode=true;evalSim(false);buildMode=false;
    for(int i=0;i<I;i++){
        int C=used[i].size();
        vector<int> rem;for(int s:used[i])if(off[s]==-1)rem.push_back(s);
        size_t ri=0;
        for(int sl=0;sl<C;sl++)if(slotOwner[slotBase[i]+sl]==-1){slotOwner[slotBase[i]+sl]=rem[ri++];}
        order_[i].clear();for(int sl=0;sl<C;sl++)order_[i].push_back(slotOwner[slotBase[i]+sl]);
        recompute(i);
    }
    long long scA=evalSim(false);
    if(scB>scA){order_=ordB;for(int i=0;i<I;i++)recompute(i);}
    long long cur=evalSim(true);
    vector<int> ws=waitStreets;vector<long long> wcum=waitCum;

    mt19937_64 rng(12345);
    const double LIMIT=9.0;
    int noImp=0;
    while(true){
        double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
        if(el>LIMIT)break;
        if(ws.empty())break;
        long long r=rng()%wcum.back();
        int idx=upper_bound(wcum.begin(),wcum.end(),r)-wcum.begin();
        if(idx>=(int)ws.size())idx=ws.size()-1;
        int s=ws[idx];int i=SE[s];
        auto&ord=order_[i];
        int m=ord.size();
        int type=rng()%10;
        vector<int> saveOrd=ord;int savedA=-1,saveDA=0;
        bool changed=false;
        if(type<4&&m>=2){
            int a=find(ord.begin(),ord.end(),s)-ord.begin();
            int b=rng()%m;if(b==a)b=(b+1)%m;
            swap(ord[a],ord[b]);changed=true;
        }else if(type<7){
            if(dur[s]<D){savedA=s;saveDA=dur[s];dur[s]++;changed=true;}
        }else if(m>=2){
            int o=ord[rng()%m];if(o==s)o=ord[(find(ord.begin(),ord.end(),s)-ord.begin()+1)%m];
            if(dur[o]>1){savedA=o;saveDA=dur[o];dur[o]--;changed=true;}
            else {int a=find(ord.begin(),ord.end(),s)-ord.begin();int b=rng()%m;if(b==a)b=(b+1)%m;swap(ord[a],ord[b]);changed=true;}
        }
        if(!changed)continue;
        recompute(i);
        long long sc=evalSim(true);
        if(sc>cur||(sc==cur&&noImp%200<100)){
            if(sc>cur)noImp=0;else noImp++;
            cur=sc;ws=waitStreets;wcum=waitCum;
        }else{
            noImp++;
            ord=saveOrd;if(savedA>=0)dur[savedA]=saveDA;recompute(i);
        }
    }
    // output
    string out;int A=0;
    string body;
    for(int i=0;i<I;i++){
        if(order_[i].empty())continue;
        A++;
        body+=to_string(i)+"\n"+to_string(order_[i].size())+"\n";
        for(int s:order_[i])body+=SN[s]+" "+to_string(dur[s])+"\n";
    }
    out=to_string(A)+"\n"+body;
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
