#include <bits/stdc++.h>
using namespace std;

static char*buf; static size_t bp=0,bl=0;
static inline long long rd(){
    while(bp<bl && (buf[bp]<'0'||buf[bp]>'9')) bp++;
    long long x=0;
    while(bp<bl && buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}
    return x;
}

int V,E,R,C,X;
vector<int> S, LDv;
vector<vector<pair<int,int>>> conn;
vector<int> lat; // E*C, 0 = not connected
struct Req{int e; long long n;};
vector<vector<Req>> vreq; // per video
vector<vector<int>> vreqIdx; // global index for cur
int NR;

struct Cand{double s; int v,c;};
struct Cmp{ bool operator()(const Cand&a,const Cand&b)const{
    if(a.s!=b.s) return a.s<b.s;
    if(a.v!=b.v) return a.v>b.v;
    return a.c>b.c;}};

int main(){
    auto t0=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();};
    // read all
    {
        size_t cap=1<<24; buf=(char*)malloc(cap);
        size_t r;
        while((r=fread(buf+bl,1,cap-bl,stdin))>0){bl+=r; if(bl==cap){cap*=2;buf=(char*)realloc(buf,cap);}}
    }
    V=rd();E=rd();R=rd();C=rd();X=rd();
    S.resize(V); for(auto&s:S)s=rd();
    LDv.resize(E); conn.resize(E); lat.assign((size_t)E*C,0);
    for(int e=0;e<E;e++){
        LDv[e]=rd(); int K=rd();
        for(int k=0;k<K;k++){int c=rd(),l=rd(); conn[e].push_back({c,l}); lat[(size_t)e*C+c]=l;}
    }
    vector<pair<long long,long long>> rq; rq.reserve(R);
    for(int i=0;i<R;i++){
        long long v=rd(),e=rd(),n=rd();
        if(conn[e].empty()) continue;
        if(S[v]>X) continue;
        rq.push_back({v*E+e,n});
    }
    sort(rq.begin(),rq.end());
    vreq.resize(V); vreqIdx.resize(V);
    NR=0;
    vector<int> reqE; vector<long long> reqN;
    for(size_t i=0;i<rq.size();){
        size_t j=i; long long n=0;
        while(j<rq.size()&&rq[j].first==rq[i].first){n+=rq[j].second;j++;}
        int v=rq[i].first/E, e=rq[i].first%E;
        vreq[v].push_back({e,n}); vreqIdx[v].push_back(NR); reqE.push_back(e); reqN.push_back(n); NR++;
        i=j;
    }
    // initial gains
    vector<array<int,2>> initVC; vector<long long> initG;
    {
        vector<long long> acc(C,0); vector<int> touched;
        for(int v=0;v<V;v++){
            if(vreq[v].empty()) continue;
            for(auto&r:vreq[v]){
                for(auto&pc:conn[r.e]){
                    if(acc[pc.first]==0) touched.push_back(pc.first);
                    acc[pc.first]+=r.n*(LDv[r.e]-pc.second);
                }
            }
            sort(touched.begin(),touched.end());
            for(int c:touched){ initVC.push_back({v,c}); initG.push_back(acc[c]); acc[c]=0;}
            touched.clear();
        }
    }
    vector<int> cur(NR);
    vector<long long> used(C);
    long long bestScore=-1; vector<vector<int>> best(C);
    double alphas[]={1.0,0.9,1.1,0.8,1.2,0.95,1.05,0.7,1.3,0.85,1.15};
    double lastRun=0;
    for(int ai=0;ai<11;ai++){
        double st=el();
        if(ai>0 && st+1.4*lastRun>8.5) break;
        double alpha=alphas[ai];
        for(int i=0;i<NR;i++) cur[i]=LDv[reqE[i]];
        fill(used.begin(),used.end(),0);
        vector<vector<int>> place(C);
        vector<Cand> heap; heap.reserve(initVC.size());
        vector<double> pw(1001);
        for(int s=1;s<=1000;s++) pw[s]=pow((double)s,alpha);
        for(size_t i=0;i<initVC.size();i++){
            int v=initVC[i][0];
            heap.push_back({(double)initG[i]/pw[S[v]],v,initVC[i][1]});
        }
        Cmp cmp;
        make_heap(heap.begin(),heap.end(),cmp);
        while(!heap.empty()){
            pop_heap(heap.begin(),heap.end(),cmp);
            Cand cd=heap.back(); heap.pop_back();
            int v=cd.v,c=cd.c;
            if(used[c]+S[v]>X) continue;
            long long g=0;
            auto&rs=vreq[v]; auto&ix=vreqIdx[v];
            for(size_t k=0;k<rs.size();k++){
                int l=lat[(size_t)rs[k].e*C+c];
                if(l>0 && l<cur[ix[k]]) g+=rs[k].n*(cur[ix[k]]-l);
            }
            if(g<=0) continue;
            double ns=(double)g/pw[S[v]];
            if(!heap.empty() && ns<heap.front().s){
                heap.push_back({ns,v,c}); push_heap(heap.begin(),heap.end(),cmp); continue;
            }
            used[c]+=S[v]; place[c].push_back(v);
            for(size_t k=0;k<rs.size();k++){
                int l=lat[(size_t)rs[k].e*C+c];
                if(l>0 && l<cur[ix[k]]) cur[ix[k]]=l;
            }
        }
        long long sc=0;
        for(int i=0;i<NR;i++) sc+=reqN[i]*(LDv[reqE[i]]-cur[i]);
        if(sc>bestScore){bestScore=sc; best=place;}
        lastRun=el()-st;
    }
    // output
    string out; int N=0;
    for(int c=0;c<C;c++) if(!best[c].empty()) N++;
    out+=to_string(N)+"\n";
    for(int c=0;c<C;c++){
        if(best[c].empty()) continue;
        out+=to_string(c);
        for(int v:best[c]){out+=' ';out+=to_string(v);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
