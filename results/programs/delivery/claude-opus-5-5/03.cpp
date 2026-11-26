#include <bits/stdc++.h>
using namespace std;

int R,Cc,D,T,ML,P,W,C;
vector<int> wt;
vector<int> wr,wc;
vector<int> stock0;
vector<int> orr,occ;
vector<vector<pair<int,int>>> need0; // sorted by weight desc
vector<int> totW;
vector<int> nearD;
vector<vector<int>> whOrder; // nearest warehouses per order (capped)

static inline int cdist(int r1,int c1,int r2,int c2){
    long long dr=r1-r2,dc=c1-c2; long long d2=dr*dr+dc*dc;
    long long s=(long long)sqrt((double)d2);
    while(s*s<d2) s++;
    while(s>0&&(s-1)*(s-1)>=d2) s--;
    return (int)s;
}

struct Cmd{int d; char t; int a,b,c;};
long long opsTotal=0;

long long evaluate(const vector<int>& perm, vector<Cmd>* out){
    vector<int> stock=stock0;
    vector<long long> dfree(D,0); vector<int> dr(D,wr[0]),dc(D,wc[0]);
    vector<Cmd> cmds; cmds.reserve(1024);
    long long score=0;
    vector<array<int,3>> slog;
    vector<int> idx(D); iota(idx.begin(),idx.end(),0);
    vector<long long> sfree; vector<int> sr,sc;
    for(int o:perm){
        auto need=need0[o]; long long remW=totW[o];
        sfree=dfree; sr=dr; sc=dc; size_t cpos=cmds.size(); slog.clear();
        bool fail=false; long long comp=0;
        int Rr=orr[o],Co=occ[o];
        while(remW>0){
            vector<int> cand;
            if(D<=64){cand=idx;}
            else{
                cand=idx;
                nth_element(cand.begin(),cand.begin()+64,cand.end(),[&](int a,int b){return dfree[a]!=dfree[b]?dfree[a]<dfree[b]:a<b;});
                cand.resize(64);
            }
            double bestCost=1e18; int bw=-1,bd=-1; long long bfin=0;
            vector<pair<int,int>> bestLoad,load;
            int cntW=0;
            for(int w:whOrder[o]){
                // load plan
                load.clear(); int cap=ML; long long cw=0;
                for(auto&pr:need){ if(pr.second<=0) continue;
                    int av=min(pr.second,stock[(size_t)w*P+pr.first]);
                    if(av<=0) continue;
                    int k=min(av,cap/wt[pr.first]);
                    if(k>0){load.push_back({pr.first,k});cap-=k*wt[pr.first];cw+=(long long)k*wt[pr.first];}
                    opsTotal++;
                }
                if(load.empty()) continue;
                cntW++;
                int nL=load.size();
                int dwo=cdist(wr[w],wc[w],Rr,Co);
                double pen=(double)(remW-cw)/ML*(2.0*nearD[o]+2);
                for(int d:cand){
                    long long fin=dfree[d]+cdist(dr[d],dc[d],wr[w],wc[w])+nL+dwo+nL;
                    double cost=fin+pen;
                    opsTotal++;
                    if(cost<bestCost){bestCost=cost;bw=w;bd=d;bfin=fin;bestLoad=load;}
                }
                if(cntW>=32) break;
            }
            if(bw<0||bfin>T){fail=true;break;}
            for(auto&l:bestLoad){
                stock[(size_t)bw*P+l.first]-=l.second; slog.push_back({bw,l.first,l.second});
                cmds.push_back({bd,'L',bw,l.first,l.second});
                for(auto&pr:need) if(pr.first==l.first){pr.second-=l.second;break;}
                remW-=(long long)l.second*wt[l.first];
            }
            for(auto&l:bestLoad) cmds.push_back({bd,'D',o,l.first,l.second});
            dfree[bd]=bfin; dr[bd]=Rr; dc[bd]=Co;
            comp=max(comp,bfin);
        }
        if(fail){
            dfree=sfree; dr=sr; dc=sc; cmds.resize(cpos);
            for(auto&s:slog) stock[(size_t)s[0]*P+s[1]]+=s[2];
        } else {
            long long t=comp-1;
            score+=((T-t)*100+T-1)/T;
        }
    }
    if(out) *out=move(cmds);
    return score;
}

int main(){
    scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&ML);
    scanf("%d",&P); wt.resize(P); for(auto&x:wt) scanf("%d",&x);
    scanf("%d",&W); wr.resize(W); wc.resize(W); stock0.assign((size_t)W*P,0);
    for(int i=0;i<W;i++){scanf("%d %d",&wr[i],&wc[i]); for(int p=0;p<P;p++) scanf("%d",&stock0[(size_t)i*P+p]);}
    scanf("%d",&C); orr.resize(C); occ.resize(C); need0.resize(C); totW.assign(C,0); nearD.resize(C); whOrder.resize(C);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&occ[i]); int L; scanf("%d",&L);
        map<int,int> m; for(int j=0;j<L;j++){int t;scanf("%d",&t);m[t]++;totW[i]+=wt[t];}
        for(auto&kv:m) need0[i].push_back(kv);
        sort(need0[i].begin(),need0[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){return wt[a.first]!=wt[b.first]?wt[a.first]>wt[b.first]:a.first<b.first;});
        // warehouses that have any needed item, sorted by distance
        vector<pair<int,int>> ws;
        for(int w=0;w<W;w++){
            bool any=false; for(auto&pr:need0[i]) if(stock0[(size_t)w*P+pr.first]>0){any=true;break;}
            if(any) ws.push_back({cdist(wr[w],wc[w],orr[i],occ[i]),w});
        }
        sort(ws.begin(),ws.end());
        nearD[i]=ws.empty()?0:ws[0].first;
        for(size_t k=0;k<ws.size()&&k<256;k++) whOrder[i].push_back(ws[k].second);
    }
    vector<double> key0(C),key1(C);
    for(int i=0;i<C;i++){
        long long trips=(totW[i]+ML-1)/ML;
        key0[i]=trips*(2.0*nearD[i]+2)+2.0*need0[i].size();
        key1[i]=(double)totW[i]/ML*(2.0*nearD[i]+2)+2.0*need0[i].size();
    }
    auto mk=[&](vector<double>&k){vector<int> p(C);iota(p.begin(),p.end(),0);
        stable_sort(p.begin(),p.end(),[&](int a,int b){return k[a]<k[b];});return p;};
    vector<int> best=mk(key0); long long bs=evaluate(best,nullptr);
    long long perEval=max<long long>(1,opsTotal);
    const long long BUDGET=250000000LL;
    if(opsTotal+perEval<BUDGET){
        vector<int> p1=mk(key1); long long s1=evaluate(p1,nullptr);
        if(s1>bs){bs=s1;best=p1;}
    }
    mt19937 rng(12345);
    while(opsTotal+perEval<BUDGET && C>1){
        vector<int> p=best;
        int moves=1+rng()%3;
        for(int m=0;m<moves;m++){
            if(rng()%2){
                int a=rng()%C; int span=max(1,min(C-1,20));
                int b=a+(int)(rng()%(2*span+1))-span; b=max(0,min(C-1,b));
                swap(p[a],p[b]);
            } else {
                int a=rng()%C; int b=rng()%C;
                int v=p[a]; p.erase(p.begin()+a); p.insert(p.begin()+b,v);
            }
        }
        long long s=evaluate(p,nullptr);
        if(s>=bs){bs=s;best=p;}
    }
    vector<Cmd> cmds; evaluate(best,&cmds);
    string outS; outS.reserve(cmds.size()*20+20);
    outS+=to_string(cmds.size()); outS+='\n';
    for(auto&c:cmds){
        outS+=to_string(c.d); outS+=' '; outS+=c.t; outS+=' ';
        outS+=to_string(c.a); outS+=' '; outS+=to_string(c.b); outS+=' '; outS+=to_string(c.c); outS+='\n';
    }
    fwrite(outS.data(),1,outS.size(),stdout);
    return 0;
}
