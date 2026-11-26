#include <bits/stdc++.h>
using namespace std;

static char*buf;static size_t bp=0,bl=0;
static void skipws(){while(bp<bl&&(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;}
static long long rint(){skipws();long long x=0;bool neg=false;if(buf[bp]=='-'){neg=true;bp++;}while(bp<bl&&buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}return neg?-x:x;}
static string rstr(){skipws();size_t s=bp;while(bp<bl&&!(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;return string(buf+s,bp-s);}

int D,I,S,V,F;
vector<int> stB,stE,stL;vector<string> stName;
vector<int> pathStart,pathLen,pathFlat;
vector<int> offs,durs,cyc;
vector<vector<pair<int,int>>> order_; // (street,dur)
vector<int> ownerBase;vector<int> owner;
vector<int> lastPass,posv;vector<long long> waitI;
vector<vector<int>> bucket;
long long work=0;

static inline int nextGreen(int t,int C,int o,int d){
    int r=t%C;
    if(r<o)return t+(o-r);
    if(r<o+d)return t;
    return t+(C-r)+o;
}

long long simulate(bool greedy){
    fill(lastPass.begin(),lastPass.end(),-1);
    fill(waitI.begin(),waitI.end(),0);
    long long score=0;
    for(int c=0;c<V;c++){posv[c]=0;}
    bucket[0].clear();
    for(int c=0;c<V;c++)bucket[0].push_back(c);
    for(int t=0;t<=D;t++){
        auto&bk=bucket[t];
        work+=bk.size();
        for(size_t q=0;q<bk.size();q++){
            int c=bk[q];
            int k=posv[c];
            int s=pathFlat[pathStart[c]+k];
            if(durs[s]==0)continue;
            int i=stE[s];
            int C=cyc[i];
            if(offs[s]<0){
                // greedy assignment
                int r=t%C;int base=ownerBase[i];
                for(int j=0;j<C;j++){int sl=(r+j)%C;if(owner[base+sl]<0){owner[base+sl]=s;offs[s]=sl;break;}}
            }
            int tt=max(t,lastPass[s]+1);
            int p=nextGreen(tt,C,offs[s],durs[s]);
            if(p>=D)continue;
            lastPass[s]=p;
            waitI[i]+=p-t;
            int ns=pathFlat[pathStart[c]+k+1];
            int arr=p+stL[ns];
            if(k+1==pathLen[c]-1){
                if(arr<=D)score+=F+(D-arr);
            }else if(arr<D){
                posv[c]=k+1;
                bucket[arr].push_back(c);
            }
        }
        bk.clear();
    }
    return score;
}

void recompute(int i){
    int o=0;
    for(auto&pr:order_[i]){offs[pr.first]=o;durs[pr.first]=pr.second;o+=pr.second;}
    cyc[i]=max(o,1);
}

int main(){
    auto t0=chrono::steady_clock::now();
    // read all
    {
        size_t cap=1<<20;buf=(char*)malloc(cap);
        while(true){
            if(bl==cap){cap*=2;buf=(char*)realloc(buf,cap);}
            size_t r=fread(buf+bl,1,cap-bl,stdin);
            if(r==0)break;bl+=r;
        }
    }
    D=rint();I=rint();S=rint();V=rint();F=rint();
    stB.resize(S);stE.resize(S);stL.resize(S);stName.resize(S);
    unordered_map<string,int> mp;mp.reserve(S*2);
    for(int s=0;s<S;s++){stB[s]=rint();stE[s]=rint();stName[s]=rstr();stL[s]=rint();mp[stName[s]]=s;}
    pathStart.resize(V);pathLen.resize(V);
    for(int c=0;c<V;c++){
        int P=rint();pathStart[c]=pathFlat.size();pathLen[c]=P;
        for(int j=0;j<P;j++){string nm=rstr();pathFlat.push_back(mp[nm]);}
    }
    vector<long long> cnt(S,0);
    for(int c=0;c<V;c++){
        long long tot=0;
        for(int j=1;j<pathLen[c];j++)tot+=stL[pathFlat[pathStart[c]+j]];
        if(tot>D)continue;
        for(int j=0;j<pathLen[c]-1;j++)cnt[pathFlat[pathStart[c]+j]]++;
    }
    offs.assign(S,-1);durs.assign(S,0);cyc.assign(I,1);
    vector<vector<int>> inc(I);
    for(int s=0;s<S;s++)if(cnt[s]>0){inc[stE[s]].push_back(s);durs[s]=1;}
    ownerBase.assign(I,0);
    int tot=0;
    for(int i=0;i<I;i++){ownerBase[i]=tot;cyc[i]=max((int)inc[i].size(),1);tot+=inc[i].size();}
    owner.assign(tot,-1);
    lastPass.assign(S,-1);posv.assign(V,0);waitI.assign(I,0);
    bucket.assign(D+1,{});
    simulate(true);
    order_.assign(I,{});
    for(int i=0;i<I;i++){
        if(inc[i].empty())continue;
        int base=ownerBase[i];int C=inc[i].size();
        vector<int> un;
        for(int s:inc[i])if(offs[s]<0)un.push_back(s);
        size_t ui=0;
        for(int j=0;j<C;j++)if(owner[base+j]<0&&ui<un.size()){owner[base+j]=un[ui];offs[un[ui]]=j;ui++;}
        for(int j=0;j<C;j++)order_[i].push_back({owner[base+j],1});
        recompute(i);
    }
    long long best=simulate(false);
    vector<int> cand;vector<long long> pref;
    auto rebuild=[&](){
        cand.clear();pref.clear();long long acc=0;
        for(int i=0;i<I;i++)if(order_[i].size()>=2&&waitI[i]>0){acc+=waitI[i];cand.push_back(i);pref.push_back(acc);}
    };
    rebuild();
    mt19937_64 rng(12345);
    const long long BUDGET=150000000LL;
    long long iter=0;
    while(!cand.empty()&&work<BUDGET){
        iter++;
        if((iter&7)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>9.2)break;
        }
        long long x=rng()%pref.back();
        int ci=upper_bound(pref.begin(),pref.end(),x)-pref.begin();
        int i=cand[ci];
        auto old=order_[i];
        auto&o=order_[i];int n=o.size();
        int m=rng()%3;
        if(m==2){int a=rng()%n;if(o[a].second>1)o[a].second--;else m=0;}
        else if(m==1){int a=rng()%n;if(o[a].second<D)o[a].second++;else m=0;}
        if(m==0){int a=rng()%n,b=rng()%(n-1);if(b>=a)b++;swap(o[a],o[b]);}
        recompute(i);
        long long sc=simulate(false);
        if(sc>=best){best=sc;rebuild();if(cand.empty())break;}
        else{order_[i]=old;recompute(i);}
    }
    // output
    string out;
    int A=0;for(int i=0;i<I;i++)if(!order_[i].empty())A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++){
        if(order_[i].empty())continue;
        out+=to_string(i)+"\n"+to_string(order_[i].size())+"\n";
        for(auto&pr:order_[i]){out+=stName[pr.first];out+=' ';out+=to_string(min(pr.second,D));out+='\n';}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
