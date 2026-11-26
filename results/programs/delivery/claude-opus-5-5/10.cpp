#include <bits/stdc++.h>
using namespace std;

static chrono::steady_clock::time_point T0;
static double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-T0).count(); }

int R,Cc,D,T,MaxL,P,W,C;
vector<int> wt;
vector<int> wr,wc;
vector<int> origStock; // W*P
vector<int> orr,occ;
vector<vector<pair<int,int>>> items; // per order (type,count) sorted by weight desc
vector<long long> totW;

static inline int dist(int r1,int c1,int r2,int c2){
    long long dr=r1-r2,dc=c1-c2; long long x=dr*dr+dc*dc;
    long long d=(long long)sqrt((double)x);
    while(d*d<x) d++;
    while(d>0&&(d-1)*(d-1)>=x) d--;
    return (int)d;
}

struct Cmd{int d; char t; int a,b,c;};
struct Drone{long long t; int r,c;};

long long bestScore=-1; vector<Cmd> bestCmds;

void run(const vector<int>& order, double limit){
    vector<int> stock=origStock;
    vector<Drone> dr(D,{0,wr[0],wc[0]});
    vector<Cmd> cmds; long long score=0;
    vector<array<int,3>> slog;
    vector<pair<int,int>> rem;
    vector<int> whs(W);
    vector<int> whd(W);
    int cnt=0;
    for(int o:order){
        if((++cnt&15)==0 && elapsed()>limit) break;
        vector<Drone> snap=dr;
        size_t csz=cmds.size(); slog.clear();
        rem=items[o];
        for(int w=0;w<W;w++){whs[w]=w; whd[w]=dist(wr[w],wc[w],orr[o],occ[o]);}
        sort(whs.begin(),whs.end(),[&](int a,int b){return whd[a]!=whd[b]?whd[a]<whd[b]:a<b;});
        long long lastTurn=-1; bool ok=true;
        while(true){
            bool any=false; for(auto&p:rem) if(p.second>0){any=true;break;}
            if(!any) break;
            long long bestEnd=LLONG_MAX; int bw=-1,bd=-1; vector<pair<int,int>> bestPack;
            int found=0;
            vector<pair<int,int>> pack;
            for(int wi=0;wi<W && found<8;wi++){
                int w=whs[wi];
                pack.clear(); int cap=MaxL;
                const int* st=&stock[(size_t)w*P];
                for(auto&p:rem){
                    if(p.second<=0) continue;
                    int ty=p.first; int k=min(p.second,st[ty]); k=min(k,cap/wt[ty]);
                    if(k>0){pack.push_back({ty,k}); cap-=k*wt[ty];}
                }
                if(pack.empty()) continue;
                found++;
                long long bt=LLONG_MAX; int bdd=-1;
                for(int d=0;d<D;d++){
                    long long v=dr[d].t+dist(dr[d].r,dr[d].c,wr[w],wc[w]);
                    if(v<bt){bt=v;bdd=d;}
                }
                long long end=bt+2*(long long)pack.size()+whd[w];
                if(end<=T && end<bestEnd){bestEnd=end;bw=w;bd=bdd;bestPack=pack;}
            }
            if(bw<0){ok=false;break;}
            for(auto&p:bestPack){
                cmds.push_back({bd,'L',bw,p.first,p.second});
                stock[(size_t)bw*P+p.first]-=p.second;
                slog.push_back({bw,p.first,p.second});
            }
            for(auto&p:bestPack){
                cmds.push_back({bd,'D',o,p.first,p.second});
                for(auto&q:rem) if(q.first==p.first){q.second-=p.second;break;}
            }
            dr[bd].t=bestEnd; dr[bd].r=orr[o]; dr[bd].c=occ[o];
            lastTurn=max(lastTurn,bestEnd-1);
        }
        if(!ok){
            dr=snap; cmds.resize(csz);
            for(auto&s:slog) stock[(size_t)s[0]*P+s[1]]+=s[2];
        } else {
            long long t=lastTurn;
            score+=((long long)(T-t)*100+T-1)/T;
        }
    }
    if(score>bestScore){bestScore=score;bestCmds=cmds;}
}

int main(){
    T0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&MaxL)!=5) return 0;
    scanf("%d",&P); wt.resize(P); for(auto&x:wt) scanf("%d",&x);
    scanf("%d",&W); wr.resize(W); wc.resize(W); origStock.assign((size_t)W*P,0);
    for(int w=0;w<W;w++){scanf("%d %d",&wr[w],&wc[w]); for(int p=0;p<P;p++) scanf("%d",&origStock[(size_t)w*P+p]);}
    scanf("%d",&C); orr.resize(C); occ.resize(C); items.resize(C); totW.assign(C,0);
    vector<int> nItems(C);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&occ[i]); int L; scanf("%d",&L); nItems[i]=L;
        map<int,int> m; for(int j=0;j<L;j++){int x; scanf("%d",&x); m[x]++; totW[i]+=wt[x];}
        for(auto&kv:m) items[i].push_back(kv);
        sort(items[i].begin(),items[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            return wt[a.first]!=wt[b.first]?wt[a.first]>wt[b.first]:a.first<b.first;});
    }
    vector<long long> nearD(C,0);
    for(int i=0;i<C;i++){
        long long b=LLONG_MAX;
        for(int w=0;w<W;w++) b=min(b,(long long)dist(wr[w],wc[w],orr[i],occ[i]));
        nearD[i]=b;
    }
    int NV=6;
    double lastDur=0;
    for(int v=0;v<NV;v++){
        double st=elapsed();
        if(v>0 && st+lastDur*1.3>8.5) break;
        vector<double> key(C);
        for(int i=0;i<C;i++){
            double trips=(double)((totW[i]+MaxL-1)/MaxL);
            double trf=(double)totW[i]/MaxL;
            double nt=(double)items[i].size();
            switch(v){
                case 0: key[i]=trips*(2.0*nearD[i]+2*nt/trips+1); break;
                case 1: key[i]=trf*(2.0*nearD[i])+2*nt; break;
                case 2: key[i]=(double)totW[i]; break;
                case 3: key[i]=trips*1e6+nearD[i]; break;
                case 4: key[i]=(double)nItems[i]*1e6+nearD[i]; break;
                default: key[i]=trips*(nearD[i]+5)+nt; break;
            }
        }
        vector<int> ord(C); iota(ord.begin(),ord.end(),0);
        stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key[a]<key[b];});
        run(ord,9.0);
        lastDur=elapsed()-st;
    }
    string out; out.reserve(bestCmds.size()*20+20);
    out+=to_string(bestCmds.size()); out+='\n';
    char buf[64];
    for(auto&c:bestCmds){
        int n=snprintf(buf,sizeof(buf),"%d %c %d %d %d\n",c.d,c.t,c.a,c.b,c.c);
        out.append(buf,n);
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
