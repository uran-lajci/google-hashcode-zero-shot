#include <bits/stdc++.h>
using namespace std;

static const int INF = 1000000000;
int W,H,R,M,T,L,N;
vector<char> isMount;
vector<vector<pair<int,int>>> occ;
struct Task{int S; vector<int> pts; int len;};
vector<Task> tasks;
long long work=0;
const long long BUDGET=12000000;

static inline int manh(int a,int b){int ax=a%W,ay=a/W,bx=b%W,by=b/W;return abs(ax-bx)+abs(ay-by);}

static char rd_buf[1<<16]; int rd_pos=0,rd_len=0;
inline int gc(){ if(rd_pos==rd_len){rd_len=fread(rd_buf,1,sizeof(rd_buf),stdin);rd_pos=0;if(rd_len<=0)return -1;} return rd_buf[rd_pos++];}
inline int readInt(){int c=gc();while(c!='-'&&(c<'0'||c>'9'))c=gc();int s=1;if(c=='-'){s=-1;c=gc();}int x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}return x*s;}

void enterInfo(int n,int emin,int&e,int&dl){
    auto &v=occ[n]; e=emin;
    auto it=partition_point(v.begin(),v.end(),[&](const pair<int,int>&q){return q.second<e-1;});
    for(;it!=v.end();++it){
        int a=it->first,b=it->second;
        if(b<e-1)continue;
        if(a>e){ if(a>=e+2){dl=a-2;return;} e=b+2; continue;}
        e=b+2;
    }
    dl=INF;
}

vector<int> visS,closS,forbS,gT,slk,par,dlA; int stampV=0,stampF=0;
struct PN{int cell,e,dl;};

bool astar(int s,int t0,int s0,int p,vector<PN>&path){
    ++stampV;
    typedef tuple<int,int,int> TT; // f, -g, cell
    priority_queue<TT,vector<TT>,greater<TT>> pq;
    visS[s]=stampV;gT[s]=t0;slk[s]=s0;par[s]=-1;dlA[s]=INF;
    pq.emplace(t0+manh(s,p),-t0,s);
    int md=manh(s,p);
    long long cap=min(200000LL,5000LL+50LL*md);
    long long cnt=0;
    static const int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};
    while(!pq.empty()){
        auto [f,ng,c]=pq.top();pq.pop();
        int g=-ng;
        if(closS[c]==stampV)continue;
        if(g!=gT[c])continue;
        closS[c]=stampV;
        if(c==p){
            path.clear();
            int x=c;
            while(x!=s){path.push_back({x,gT[x],dlA[x]});x=par[x];}
            reverse(path.begin(),path.end());
            return true;
        }
        if(++cnt>cap||work>BUDGET)return false;
        work++;
        long long wl=(long long)g+slk[c]-2; if(wl>L-1)wl=L-1; if(wl<g)continue;
        int cx=c%W,cy=c/W;
        for(int d=0;d<4;d++){
            int nx=cx+dx[d],ny=cy+dy[d];
            if(nx<0||ny<0||nx>=W||ny>=H)continue;
            int n=ny*W+nx;
            if(isMount[n]||forbS[n]==stampF||closS[n]==stampV)continue;
            int e,dl;enterInfo(n,g+1,e,dl);
            if(e-1>wl)continue; if(e>L)continue;
            long long sn=min((long long)slk[c]-(e-1-g)-2,(long long)dl-e);
            if(sn<0)continue;
            if(visS[n]!=stampV||e<gT[n]||(e==gT[n]&&sn>slk[n])){
                visS[n]=stampV;gT[n]=e;slk[n]=(int)min(sn,(long long)INF);par[n]=c;dlA[n]=dl;
                pq.emplace(e+manh(n,p),-e,n);
            }
        }
    }
    return false;
}

struct Elem{int cell,dl,pm;};
struct Arm{vector<Elem> st;int tau;string ins;vector<int> tl;int mount;};

char dirChar(int a,int b){int ax=a%W,ay=a/W,bx=b%W,by=b/W;if(bx==ax+1)return 'R';if(bx==ax-1)return 'L';if(by==ay+1)return 'U';return 'D';}

bool moveTo(Arm&A,int p){
    int d=A.st.size()-1;
    int tau=A.tau;
    work+=(d+1)/8+1;
    int j=-1;
    vector<pair<int,int>> cand;
    for(int k=0;k<=d;k++){
        if(A.st[k].cell==p)j=k;
        cand.push_back({(d-k)+manh(A.st[k].cell,p),k});
    }
    int take=min((int)cand.size(),5);
    partial_sort(cand.begin(),cand.begin()+take,cand.end());
    vector<pair<int,int>> cs;
    cs.push_back({manh(A.st[d].cell,p),d});
    for(int i=0;i<take;i++)if(cand[i].second!=d)cs.push_back(cand[i]);
    if(j>=0){bool f=false;for(auto&c:cs)if(c.second==j)f=true;if(!f)cs.push_back({d-j,j});}
    int best=INF,bestK=-1; vector<PN> bestPath,path;
    for(auto&c:cs){
        int k=c.second;
        if(tau+c.first>=best)continue;
        if(k==j){
            int arr=tau+d-j;
            if(arr<=L&&arr<best){best=arr;bestK=k;bestPath.clear();}
            continue;
        }
        long long s0=(k==0)?INF:(long long)A.st[k].pm-tau-d;
        if(s0<2)continue;
        int t0=tau+d-k; if(t0>=L)continue;
        ++stampF; for(int i=0;i<=k;i++)forbS[A.st[i].cell]=stampF;
        work+=k/8;
        if(astar(A.st[k].cell,t0,(int)min(s0,(long long)INF),p,path)){
            int arr=path.back().e;
            if(arr<best){best=arr;bestK=k;bestPath=path;}
        }
        if(work>BUDGET)break;
    }
    if(bestK<0)return false;
    for(int r=0;r<d-bestK;r++){
        int cur=A.st.back().cell,prev=A.st[A.st.size()-2].cell;
        A.ins.push_back(dirChar(cur,prev));A.st.pop_back();A.tau++;
    }
    for(auto&pn:bestPath){
        while(A.tau<pn.e-1){A.ins.push_back('W');A.tau++;}
        A.ins.push_back(dirChar(A.st.back().cell,pn.cell));A.tau++;
        int idx=A.st.size();
        long long pm=min((long long)A.st.back().pm,(long long)pn.dl+idx);
        if(pm>2*INF)pm=2*INF;
        A.st.push_back({pn.cell,pn.dl,(int)pm});
    }
    return true;
}

bool doTask(Arm&A,const Task&tk){
    vector<Elem> sv=A.st;int st=A.tau;size_t is=A.ins.size();
    for(int p:tk.pts){
        if(A.st.back().cell==p)continue;
        if(!moveTo(A,p)){A.st=sv;A.tau=st;A.ins.resize(is);return false;}
    }
    return true;
}

int main(){
    W=readInt();H=readInt();R=readInt();M=readInt();T=readInt();L=readInt();
    N=W*H;
    isMount.assign(N,0);
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x=readInt(),y=readInt();mounts[i]=y*W+x;isMount[mounts[i]]=1;}
    tasks.resize(T);
    for(int i=0;i<T;i++){
        tasks[i].S=readInt();int P=readInt();tasks[i].pts.resize(P);
        for(int k=0;k<P;k++){int x=readInt(),y=readInt();tasks[i].pts[k]=y*W+x;}
        int len=0;for(int k=1;k<P;k++)len+=manh(tasks[i].pts[k-1],tasks[i].pts[k]);
        tasks[i].len=len;
    }
    occ.assign(N,{});
    visS.assign(N,0);closS.assign(N,0);forbS.assign(N,0);gT.assign(N,0);slk.assign(N,0);par.assign(N,0);dlA.assign(N,0);
    vector<char> assigned(T,0),mountTried(M,0);
    vector<int> failStamp(T,-1);
    vector<Arm> result;
    int attempts=0;
    while((int)result.size()<R&&attempts<3*R&&work<=BUDGET){
        attempts++;
        int bm=-1;double bw=-1;
        for(int m=0;m<M;m++){
            if(mountTried[m])continue;
            double w=0;
            for(int t=0;t<T;t++){
                if(assigned[t])continue;
                double dd=manh(mounts[m],tasks[t].pts[0])+tasks[t].len+1;
                if(dd>L)continue;
                w+=tasks[t].S/(dd*dd);
            }
            if(w>bw){bw=w;bm=m;}
        }
        if(bm<0||bw<=0)break;
        mountTried[bm]=1;
        Arm A;A.mount=mounts[bm];A.tau=0;A.st.push_back({mounts[bm],INF,2*INF});
        int fails=0;
        while(work<=BUDGET){
            int top=A.st.back().cell;
            int bt=-1;double br=-1;
            for(int t=0;t<T;t++){
                if(assigned[t]||failStamp[t]==attempts)continue;
                int est=manh(top,tasks[t].pts[0])+tasks[t].len;
                if(A.tau+est>L)continue;
                double r=tasks[t].S/(double)(est+1);
                if(r>br){br=r;bt=t;}
            }
            work+=T/64;
            if(bt<0)break;
            if(doTask(A,tasks[bt])){assigned[bt]=1;A.tl.push_back(bt);}
            else{failStamp[bt]=attempts;if(++fails>20)break;}
        }
        if(A.tl.empty())continue;
        int d=A.st.size()-1;
        int r=min(d,L-A.tau);
        for(int i=0;i<r;i++){
            int cur=A.st.back().cell,prev=A.st[A.st.size()-2].cell;
            A.ins.push_back(dirChar(cur,prev));A.st.pop_back();A.tau++;
        }
        // record intervals
        vector<pair<int,int>> sk; sk.push_back({A.mount,0});
        auto addI=[&](int cell,int a,int b){
            auto &v=occ[cell];
            auto it=lower_bound(v.begin(),v.end(),make_pair(a,b));
            v.insert(it,make_pair(a,b));
        };
        for(int s=0;s<(int)A.ins.size();s++){
            char c=A.ins[s];
            if(c=='W')continue;
            int cur=sk.back().first;int x=cur%W,y=cur/W;
            if(c=='R')x++;else if(c=='L')x--;else if(c=='U')y++;else y--;
            int n=y*W+x;
            if(sk.size()>=2&&sk[sk.size()-2].first==n){
                addI(sk.back().first,sk.back().second,s);sk.pop_back();
            } else sk.push_back({n,s+1});
        }
        for(size_t i=1;i<sk.size();i++)addI(sk[i].first,sk[i].second,INF);
        result.push_back(move(A));
    }
    string out;
    out+=to_string(result.size())+"\n";
    for(auto&A:result){
        out+=to_string(A.mount%W)+" "+to_string(A.mount/W)+" "+to_string(A.tl.size())+" "+to_string(A.ins.size())+"\n";
        for(size_t i=0;i<A.tl.size();i++){if(i)out+=' ';out+=to_string(A.tl[i]);}
        out+="\n";
        for(size_t i=0;i<A.ins.size();i++){if(i)out+=' ';out+=A.ins[i];}
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
