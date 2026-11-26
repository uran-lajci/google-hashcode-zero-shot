#include <bits/stdc++.h>
using namespace std;

static const int INF = 1000000000;
int W,H,R,M,T,L;
vector<char> isMount;
vector<vector<pair<int,int>>> resv;
vector<int> inSt;
vector<int> gsc,par_,stmp; int stampv=0;
long long work=0;
const long long BUDGET=80000000LL;

static char buf[1<<25]; int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } int s=1; if(c=='-'){s=-1;c=gc();} int x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return x*s; }

inline int manh(int a,int b){ return abs(a%W-b%W)+abs(a/W-b/W); }
inline bool occ(int c,int t){
    auto &v=resv[c]; if(v.empty()) return false;
    int lo=0,hi=v.size();
    while(lo<hi){int mid=(lo+hi)/2; if(v[mid].second<t) lo=mid+1; else hi=mid;}
    return lo<(int)v.size() && v[lo].first<=t;
}
inline int firstEndGE(int c,int t){
    auto &v=resv[c];
    int lo=0,hi=v.size();
    while(lo<hi){int mid=(lo+hi)/2; if(v[mid].second<t) lo=mid+1; else hi=mid;}
    return lo;
}

int astar(int s,int g,int keepIdx,int t0,int limit,bool ta,vector<int>&path,int cap){
    path.clear();
    if(s==g) return 0;
    stampv++;
    int gx=g%W, gy=g/W;
    auto h=[&](int c){return abs(c%W-gx)+abs(c/W-gy);};
    if(h(s)>limit) return -1;
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<pair<long long,int>>> pq;
    const long long K=4194304LL;
    gsc[s]=0; stmp[s]=stampv; par_[s]=-1;
    pq.push({(long long)h(s)*K,s});
    int expanded=0;
    while(!pq.empty()){
        auto [key,c]=pq.top(); pq.pop();
        long long exp=(long long)(gsc[c]+h(c))*K-gsc[c];
        if(key!=exp) continue;
        work++;
        if(++expanded>cap) return -1;
        if(c==g){
            int x=c; while(x!=s){ path.push_back(x); x=par_[x]; }
            reverse(path.begin(),path.end());
            return gsc[c];
        }
        int cx=c%W, cy=c/W;
        int nbs[4]; int k=0;
        if(cx+1<W) nbs[k++]=c+1;
        if(cx>0) nbs[k++]=c-1;
        if(cy+1<H) nbs[k++]=c+W;
        if(cy>0) nbs[k++]=c-W;
        for(int q=0;q<k;q++){
            int nb=nbs[q];
            if(isMount[nb]) continue;
            if(inSt[nb] && inSt[nb]-1<=keepIdx) continue;
            int ng=gsc[c]+1;
            int hn=h(nb);
            if((long long)ng+hn>limit) continue;
            if(stmp[nb]==stampv && gsc[nb]<=ng) continue;
            if(ta && (occ(nb,t0+ng-1)||occ(nb,t0+ng))) continue;
            stmp[nb]=stampv; gsc[nb]=ng; par_[nb]=c;
            pq.push({(long long)(ng+hn)*K-ng,nb});
        }
    }
    return -1;
}

// arm state
vector<int> st,dl,en,pm; int tcur; string mv; vector<array<int,3>> lg;

char dirc(int a,int b){ if(b==a+1) return 'R'; if(b==a-1) return 'L'; if(b==a+W) return 'U'; return 'D'; }
void doRetract(){
    int n=st.size()-1; int c=st[n];
    lg.push_back({c,en[n],tcur});
    mv+=dirc(c,st[n-1]);
    inSt[c]=0; st.pop_back(); dl.pop_back(); en.pop_back(); pm.pop_back();
    tcur++;
}
void doExpand(int c,int dc){
    int n=st.size()-1;
    mv+=dirc(st[n],c);
    st.push_back(c); dl.push_back(dc); en.push_back(tcur);
    pm.push_back(min((long long)pm[n],(long long)dc+n+1)>=INF?INF:min(pm[n],dc+n+1));
    inSt[c]=n+2;
    tcur++;
}

bool route(int g,int&outI,vector<int>&outPath){
    int n=st.size()-1;
    vector<int> tmp;
    for(int pass=0;pass<2;pass++){
        bool ta=(pass==0);
        int best=INF,bi=-1;
        int d=astar(st[n],g,n,tcur,INF,ta,tmp,300000);
        if(d>=0){best=d;bi=n;outPath=tmp;}
        vector<pair<int,int>> cand;
        for(int i=n-1;i>=0;i--){
            if(n-i>=best) break;
            int lb=(n-i)+manh(st[i],g);
            if(lb<best) cand.push_back({lb,i});
            work++;
        }
        sort(cand.begin(),cand.end());
        int tries=0; bool tried0=(n==0);
        for(auto &pr:cand){
            int lb=pr.first,i=pr.second;
            if(lb>=best) break;
            if(tries>=4) break;
            tries++;
            if(i==0) tried0=true;
            int lim=(best>=INF)?INF:best-(n-i)-1;
            int dd=astar(st[i],g,i,tcur+(n-i),lim,ta,tmp,100000);
            if(dd>=0){best=dd+(n-i);bi=i;outPath=tmp;}
        }
        if(bi<0 && !tried0){
            int dd=astar(st[0],g,0,tcur+n,INF,ta,tmp,300000);
            if(dd>=0){best=dd+n;bi=0;outPath=tmp;}
        }
        if(bi>=0){outI=bi;return true;}
    }
    return false;
}

int main(){
    W=readInt();H=readInt();R=readInt();M=readInt();T=readInt();L=readInt();
    int N=W*H;
    isMount.assign(N,0);
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x=readInt(),y=readInt(); mounts[i]=y*W+x; isMount[mounts[i]]=1;}
    vector<long long> S(T); vector<vector<int>> pts(T); vector<int> internalLen(T,0);
    for(int i=0;i<T;i++){
        S[i]=readInt(); int P=readInt(); pts[i].resize(P);
        for(int j=0;j<P;j++){int x=readInt(),y=readInt(); pts[i][j]=y*W+x;}
        for(int j=1;j<P;j++) internalLen[i]+=manh(pts[i][j-1],pts[i][j]);
    }
    resv.assign(N,{});
    inSt.assign(N,0); gsc.assign(N,0); par_.assign(N,0); stmp.assign(N,0);

    // mount selection
    vector<int> bestD(T,INF), owner(T,-1);
    auto evalMount=[&](int m)->long long{
        vector<pair<double,int>> v;
        for(int t=0;t<T;t++){
            int d=manh(m,pts[t][0]);
            if(d<bestD[t]){ int cost=d+internalLen[t]+1; v.push_back({(double)S[t]/cost,t}); }
        }
        sort(v.begin(),v.end(),[](const pair<double,int>&a,const pair<double,int>&b){ if(a.first!=b.first) return a.first>b.first; return a.second<b.second;});
        long long budget=L,sum=0;
        for(auto &p:v){ int t=p.second; int cost=manh(m,pts[t][0])+internalLen[t]+1; if(cost<=budget){budget-=cost;sum+=S[t];} }
        return sum;
    };
    priority_queue<pair<long long,int>> mq;
    for(int i=0;i<M;i++){
        int c=mounts[i],cx=c%W,cy=c/W; bool ok=false;
        if(cx+1<W&&!isMount[c+1])ok=true;
        if(cx>0&&!isMount[c-1])ok=true;
        if(cy+1<H&&!isMount[c+W])ok=true;
        if(cy>0&&!isMount[c-W])ok=true;
        if(!ok) continue;
        long long v=evalMount(c);
        if(v>0) mq.push({v,i});
    }
    vector<int> chosen;
    while((int)chosen.size()<R && !mq.empty()){
        auto [v,i]=mq.top(); mq.pop();
        long long v2=evalMount(mounts[i]);
        if(v2<=0) continue;
        if(mq.empty() || v2>=mq.top().first){
            int k=chosen.size(); chosen.push_back(i);
            for(int t=0;t<T;t++){ int d=manh(mounts[i],pts[t][0]); if(d<bestD[t]){bestD[t]=d;owner[t]=k;} }
        } else mq.push({v2,i});
    }

    vector<char> assigned(T,0);
    vector<int> failedFor(T,-1);
    struct Out{int mount; vector<int> tasks; string moves;};
    vector<Out> outs;
    const int WAITCAP=30;

    for(int k=0;k<(int)chosen.size();k++){
        int mc=mounts[chosen[k]];
        st.assign(1,mc); dl.assign(1,INF); en.assign(1,0); pm.assign(1,INF);
        inSt[mc]=1; tcur=0; mv.clear(); lg.clear();
        vector<int> done;

        auto execTask=[&](int task)->bool{
            vector<int> sst=st,sdl=dl,sen=en,spm=pm; int st0=tcur; size_t smv=mv.size(), slg=lg.size();
            auto fail=[&](){
                for(int c:st) inSt[c]=0;
                st=sst;dl=sdl;en=sen;pm=spm;tcur=st0;mv.resize(smv);lg.resize(slg);
                for(size_t i=0;i<st.size();i++) inSt[st[i]]=i+1;
            };
            vector<int> path;
            for(int p:pts[task]){
                int replans=0;
                while(st.back()!=p){
                    if(work>BUDGET+20000000LL){fail();return false;}
                    int ki;
                    if(!route(p,ki,path)){fail();return false;}
                    while((int)st.size()-1>ki){ if(tcur+1>L){fail();return false;} doRetract(); }
                    bool blocked=false;
                    for(int c:path){
                        while(true){
                            int n=st.size()-1;
                            if(tcur+1>L || (long long)pm[n]-n-tcur-2<0){fail();return false;}
                            auto &v=resv[c];
                            int lo=firstEndGE(c,tcur);
                            if(lo<(int)v.size() && v[lo].first<=tcur+1){
                                int e=v[lo].second;
                                if(e>=INF/2){blocked=true;break;}
                                int need=e+1-tcur;
                                if(need>WAITCAP || (long long)pm[n]-n-(tcur+need)-2<0){blocked=true;break;}
                                mv+='W'; tcur++; continue;
                            }
                            int dc=(lo<(int)v.size())?v[lo].first-1:INF;
                            doExpand(c,dc);
                            break;
                        }
                        if(blocked) break;
                    }
                    if(blocked){ if(++replans>4){fail();return false;} }
                }
            }
            return true;
        };

        while(work<BUDGET){
            int g=st.back();
            int best=-1; double bestR=-1;
            for(int t=0;t<T;t++){
                if(assigned[t]||failedFor[t]==k) continue;
                long long est=manh(g,pts[t][0])+internalLen[t];
                if(tcur+est>L) continue;
                double w=(owner[t]<=k)?1.0:0.25;
                double r=S[t]*w/(est+1.0);
                if(r>bestR){bestR=r;best=t;}
            }
            if(best<0) break;
            if(execTask(best)){assigned[best]=1;done.push_back(best);}
            else failedFor[best]=k;
        }
        // final retract for finite deadlines
        while(st.size()>1 && pm[st.size()-1]<INF/2){
            if(tcur+1>L) break; // should not happen
            doRetract();
        }
        // commit reservations
        if(!done.empty()){
            for(auto &e:lg){
                auto &v=resv[e[0]];
                auto it=upper_bound(v.begin(),v.end(),make_pair(e[1],INT_MAX));
                v.insert(it,{e[1],e[2]});
            }
            for(size_t i=1;i<st.size();i++){
                auto &v=resv[st[i]];
                auto it=upper_bound(v.begin(),v.end(),make_pair(en[i],INT_MAX));
                v.insert(it,{en[i],INF});
            }
            outs.push_back({mc,done,mv});
        }
        for(int c:st) inSt[c]=0;
    }

    string o;
    o+=to_string(outs.size())+"\n";
    for(auto &a:outs){
        o+=to_string(a.mount%W)+" "+to_string(a.mount/W)+" "+to_string(a.tasks.size())+" "+to_string(a.moves.size())+"\n";
        for(size_t i=0;i<a.tasks.size();i++){ if(i) o+=' '; o+=to_string(a.tasks[i]); }
        o+="\n";
        for(size_t i=0;i<a.moves.size();i++){ if(i) o+=' '; o+=a.moves[i]; }
        o+="\n";
    }
    fwrite(o.data(),1,o.size(),stdout);
    return 0;
}
