#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> sB,sE,sL;
vector<string> sName;
vector<vector<int>> paths;
vector<vector<int>> ord; // per intersection order
vector<int> dur, off, cyc;
vector<char> inSched;
vector<int> lastPass;
vector<vector<int>> buckets;
vector<int> posC;
vector<long long> waitI;
long long eventCount=0;

static inline int nextGreen(int s,int t){
    int C=cyc[sE[s]]; int o=off[s], d=dur[s];
    int r=t%C;
    if(r>=o && r<o+d) return t;
    if(r<o) return t+(o-r);
    return t+(C-r)+o;
}
void recompute(int in){
    int o=0;
    for(int s:ord[in]){ off[s]=o; o+=dur[s]; }
    cyc[in]=o;
}
long long sim(){
    long long score=0;
    fill(lastPass.begin(),lastPass.end(),-1);
    fill(waitI.begin(),waitI.end(),0);
    for(auto&b:buckets) b.clear();
    for(int c=0;c<V;c++){posC[c]=0; buckets[0].push_back(c);}
    for(int t=0;t<=D;t++){
        auto &bk=buckets[t];
        eventCount+=bk.size()+1;
        for(size_t k=0;k<bk.size();k++){
            int c=bk[k];
            int s=paths[c][posC[c]];
            if(!inSched[s]) continue;
            int e=max(t,lastPass[s]+1);
            int g=nextGreen(s,e);
            lastPass[s]=g;
            if(g>D) continue;
            if(g>t) waitI[sE[s]]+=g-t;
            int p=++posC[c];
            int s2=paths[c][p];
            int arr=g+sL[s2];
            if(p==(int)paths[c].size()-1){
                if(arr<=D) score+=F+(D-arr);
            } else if(arr<=D) buckets[arr].push_back(c);
        }
        bk.clear();
    }
    return score;
}

static char buf[1<<16]; int bl=0,bp=0;
inline int gc(){ if(bp==bl){ bl=fread(buf,1,sizeof(buf),stdin); bp=0; if(bl<=0) return -1;} return buf[bp++]; }
int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')) c=gc(); int x=0; while(c>='0'&&c<='9'){x=x*10+c-'0';c=gc();} return x; }
string readStr(){ int c=gc(); while(c==' '||c=='\n'||c=='\r') c=gc(); string s; while(c!=-1&&c!=' '&&c!='\n'&&c!='\r'){s.push_back((char)c);c=gc();} return s; }

int main(){
    auto T0=chrono::steady_clock::now();
    D=readInt();I=readInt();S=readInt();V=readInt();F=readInt();
    sB.resize(S);sE.resize(S);sL.resize(S);sName.resize(S);
    unordered_map<string,int> mp; mp.reserve(S*2);
    for(int i=0;i<S;i++){ sB[i]=readInt(); sE[i]=readInt(); sName[i]=readStr(); sL[i]=readInt(); mp[sName[i]]=i; }
    paths.resize(V);
    for(int c=0;c<V;c++){ int P=readInt(); paths[c].resize(P); for(int j=0;j<P;j++) paths[c][j]=mp[readStr()]; }
    vector<long long> cnt(S,0); vector<long long> earliest(S,LLONG_MAX);
    for(int c=0;c<V;c++){
        long long tot=0; auto&p=paths[c];
        for(size_t j=1;j<p.size();j++) tot+=sL[p[j]];
        if(tot>D) continue;
        long long t=0;
        for(size_t j=0;j+1<p.size();j++){
            if(j>0) t+=sL[p[j]];
            cnt[p[j]]++; earliest[p[j]]=min(earliest[p[j]],t);
        }
    }
    ord.assign(I,{}); dur.assign(S,1); off.assign(S,0); cyc.assign(I,0); inSched.assign(S,0);
    lastPass.assign(S,-1); buckets.assign(D+1,{}); posC.assign(V,0); waitI.assign(I,0);
    for(int s=0;s<S;s++) if(cnt[s]>0){ ord[sE[s]].push_back(s); inSched[s]=1; }
    for(int in=0;in<I;in++){
        sort(ord[in].begin(),ord[in].end(),[&](int a,int b){
            if(earliest[a]!=earliest[b]) return earliest[a]<earliest[b];
            if(cnt[a]!=cnt[b]) return cnt[a]>cnt[b];
            return a<b;});
    }
    // variants
    auto applyVariant=[&](int cap){
        for(int in=0;in<I;in++){
            if(ord[in].empty()) continue;
            long long mn=LLONG_MAX; for(int s:ord[in]) mn=min(mn,cnt[s]);
            for(int s:ord[in]){
                long long d= cap<=1?1:min<long long>(cap,max<long long>(1,(cnt[s]+mn/2)/mn));
                d=min<long long>(d,D); dur[s]=(int)d;
            }
            recompute(in);
        }
    };
    int caps[3]={1,2,4}; long long bestSc=-1; int bestCap=1;
    for(int k=0;k<3;k++){ applyVariant(caps[k]); long long sc=sim(); if(sc>bestSc){bestSc=sc;bestCap=caps[k];} }
    applyVariant(bestCap);
    long long cur=sim();
    vector<int> multi;
    for(int in=0;in<I;in++) if(ord[in].size()>=2) multi.push_back(in);
    vector<int> waitList;
    auto rebuild=[&](){ waitList.clear(); for(int in:multi) if(waitI[in]>0) waitList.push_back(in); };
    rebuild();
    mt19937 rng(12345);
    const long long BUDGET=300000000LL;
    long long startEv=eventCount;
    int iter=0;
    if(!multi.empty()){
        while(eventCount-startEv<BUDGET){
            if((++iter&15)==0){
                double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
                if(el>8.8) break;
            }
            int in;
            if(!waitList.empty() && rng()%10<7) in=waitList[rng()%waitList.size()];
            else in=multi[rng()%multi.size()];
            auto &o=ord[in];
            vector<int> saved=o; int mt=rng()%3; int chS=-1,oldD=0;
            if(mt==0){
                int a=rng()%o.size(), b=rng()%o.size(); if(a==b) continue; swap(o[a],o[b]);
            } else if(mt==1){
                int a=rng()%o.size(), b=rng()%o.size(); if(a==b) continue;
                int x=o[a]; o.erase(o.begin()+a); o.insert(o.begin()+b,x);
            } else {
                chS=o[rng()%o.size()]; oldD=dur[chS];
                int nd=oldD+((rng()&1)?1:-1);
                if(nd<1||nd>D) continue;
                dur[chS]=nd;
            }
            recompute(in);
            long long sc=sim();
            if(sc>=cur){ cur=sc; rebuild(); }
            else{
                o=saved; if(chS>=0) dur[chS]=oldD; recompute(in);
            }
        }
    }
    // output
    string out; int A=0;
    for(int in=0;in<I;in++) if(!ord[in].empty()) A++;
    out+=to_string(A)+"\n";
    for(int in=0;in<I;in++){
        if(ord[in].empty()) continue;
        out+=to_string(in)+"\n"+to_string(ord[in].size())+"\n";
        for(int s:ord[in]){ out+=sName[s]; out+=' '; out+=to_string(dur[s]); out+='\n'; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
