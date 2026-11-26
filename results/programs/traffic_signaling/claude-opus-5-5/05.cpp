#include <bits/stdc++.h>
using namespace std;

static char* buf; static size_t blen, bpos=0;
static inline void skipws(){ while(bpos<blen && isspace((unsigned char)buf[bpos])) bpos++; }
static inline long long readInt(){ skipws(); long long x=0; bool neg=false; if(buf[bpos]=='-'){neg=true;bpos++;} while(bpos<blen && isdigit((unsigned char)buf[bpos])){x=x*10+(buf[bpos]-'0');bpos++;} return neg?-x:x; }
static inline string readStr(){ skipws(); size_t st=bpos; while(bpos<blen && !isspace((unsigned char)buf[bpos])) bpos++; return string(buf+st,bpos-st); }

int D,I,S,V; long long F;
vector<int> SB,SE,SL; vector<string> SN;
vector<vector<int>> paths;
vector<vector<int>> ord; vector<int> dur, off, cyc;
vector<int> lastPass, posc;
vector<vector<int>> buckets;
vector<int> usedStreets;
const int INF=INT_MAX/2;
long long work=0;
// adaptive
bool adaptiveMode=false;
vector<int> slotOf; vector<vector<int>> slotOwner;

static inline int nextGreen(int s,int t){
    int d=dur[s]; if(d==0) return INF;
    int i=SE[s]; int C=cyc[i]; if(d==C) return t;
    int r=t%C; int o=off[s];
    if(r>=o && r<o+d) return t;
    return t+((o-r+C)%C);
}
void recompute(int i){ int o=0; for(int s:ord[i]){off[s]=o;o+=dur[s];} cyc[i]=o; }

long long sim(vector<int>& waitList){
    waitList.clear();
    for(int s:usedStreets) lastPass[s]=-1;
    for(int c=0;c<V;c++){posc[c]=0;}
    for(int t=0;t<D;t++) buckets[t].clear();
    for(int c=0;c<V;c++) buckets[0].push_back(c);
    long long score=0;
    work += S/4 + D;
    for(int t=0;t<D;t++){
        auto &bk=buckets[t];
        work += bk.size();
        for(size_t k=0;k<bk.size();k++){
            int c=bk[k];
            const vector<int>& p=paths[c];
            int s=p[posc[c]];
            int t0=max(t,lastPass[s]+1);
            if(adaptiveMode && slotOf[s]<0){
                int i=SE[s]; int kk=slotOwner[i].size();
                int r=t0%kk;
                for(int j=0;j<kk;j++){ int q=(r+j)%kk; if(slotOwner[i][q]<0){slotOwner[i][q]=s;slotOf[s]=q;off[s]=q;break;} }
            }
            int pass=nextGreen(s,t0);
            if(pass>=D) continue;
            if(pass>t) waitList.push_back(s);
            lastPass[s]=pass;
            int pp=++posc[c];
            int ns=p[pp];
            int arr=pass+SL[ns];
            if(pp==(int)p.size()-1){ if(arr<=D) score+=F+(D-arr); }
            else if(arr<D) buckets[arr].push_back(c);
        }
    }
    return score;
}

int main(){
    auto tstart=chrono::steady_clock::now();
    {
        vector<char> v; v.reserve(1<<24);
        char tmp[1<<16]; size_t n;
        while((n=fread(tmp,1,sizeof(tmp),stdin))>0) v.insert(v.end(),tmp,tmp+n);
        blen=v.size(); buf=(char*)malloc(blen+1); memcpy(buf,v.data(),blen); buf[blen]=0;
    }
    D=readInt(); I=readInt(); S=readInt(); V=readInt(); F=readInt();
    SB.resize(S);SE.resize(S);SL.resize(S);SN.resize(S);
    unordered_map<string,int> mp; mp.reserve(S*2);
    for(int s=0;s<S;s++){ SB[s]=readInt(); SE[s]=readInt(); SN[s]=readStr(); SL[s]=readInt(); mp[SN[s]]=s; }
    paths.resize(V);
    vector<int> usedCnt(S,0);
    for(int c=0;c<V;c++){ int P=readInt(); paths[c].resize(P); for(int j=0;j<P;j++){ paths[c][j]=mp[readStr()]; } for(int j=0;j<P-1;j++) usedCnt[paths[c][j]]++; }
    ord.assign(I,{}); dur.assign(S,0); off.assign(S,0); cyc.assign(I,0);
    lastPass.assign(S,-1); posc.assign(V,0); buckets.assign(D+1,{});
    for(int s=0;s<S;s++) if(usedCnt[s]>0){ usedStreets.push_back(s); dur[s]=1; }
    // adaptive initial
    slotOf.assign(S,-1); slotOwner.assign(I,{});
    vector<int> degU(I,0); for(int s:usedStreets) degU[SE[s]]++;
    for(int i=0;i<I;i++){ slotOwner[i].assign(degU[i],-1); cyc[i]=degU[i]; }
    adaptiveMode=true;
    vector<int> waitList, tmpWait;
    sim(waitList);
    adaptiveMode=false;
    for(int s:usedStreets) if(slotOf[s]<0){
        int i=SE[s]; for(int q=0;q<(int)slotOwner[i].size();q++) if(slotOwner[i][q]<0){slotOwner[i][q]=s;slotOf[s]=q;break;}
    }
    for(int i=0;i<I;i++){ for(int s:slotOwner[i]) ord[i].push_back(s); recompute(i); }
    slotOwner.clear(); slotOwner.shrink_to_fit();
    long long cur=sim(waitList);
    vector<int> multi; for(int i=0;i<I;i++) if(ord[i].size()>=2) multi.push_back(i);
    uint64_t rs=88172645463325252ULL;
    auto rng=[&]()->uint64_t{ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; };
    const long long BUDGET=250000000LL;
    vector<int> saveOrd, saveDur;
    long long iter=0;
    while(!multi.empty() && work<BUDGET){
        if((++iter&7)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-tstart).count();
            if(el>8.3) break;
        }
        int s,i;
        if(!waitList.empty() && rng()%10<8){ s=waitList[rng()%waitList.size()]; i=SE[s]; if(ord[i].size()<2){work+=10;continue;} }
        else { i=multi[rng()%multi.size()]; s=ord[i][rng()%ord[i].size()]; }
        auto &o=ord[i]; int k=o.size();
        saveOrd=o; saveDur.clear(); for(int u:o) saveDur.push_back(dur[u]);
        int type=rng()%5;
        bool ok=true;
        if(type==0){ if(dur[s]<D) dur[s]++; else ok=false; }
        else if(type==1){ int u=o[rng()%k]; if(dur[u]>1) dur[u]--; else ok=false; }
        else if(type==2){ if(dur[s]>1) dur[s]--; else { int u=o[rng()%k]; if(u!=s && dur[u]>1) dur[u]--; else ok=false; } }
        else if(type==3){ int a=find(o.begin(),o.end(),s)-o.begin(); int b=rng()%k; if(a==b) ok=false; else swap(o[a],o[b]); }
        else { int a=find(o.begin(),o.end(),s)-o.begin(); int b=rng()%k; if(a==b) ok=false; else { o.erase(o.begin()+a); o.insert(o.begin()+b,s);} }
        if(!ok){ work+=10; continue; }
        recompute(i);
        long long sc=sim(tmpWait);
        if(sc>=cur){ cur=sc; swap(waitList,tmpWait); }
        else { o=saveOrd; for(int j=0;j<k;j++) dur[o[j]]=saveDur[j]; recompute(i); }
    }
    // output
    string out; out.reserve(1<<22);
    int A=0; for(int i=0;i<I;i++) if(!ord[i].empty()) A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++) if(!ord[i].empty()){
        out+=to_string(i)+"\n"+to_string(ord[i].size())+"\n";
        for(int s:ord[i]){ out+=SN[s]; out+=' '; out+=to_string(min(max(dur[s],1),D)); out+='\n'; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
