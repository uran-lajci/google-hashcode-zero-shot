#include <bits/stdc++.h>
using namespace std;

int R_,C_,D,T,CAP,P,W,NO;
vector<int> wt;
vector<int> wr,wc;
vector<int> st0;
struct Order{int r,c;vector<pair<int,int>> items;long long totW;};
vector<Order> ords;
vector<vector<int>> candW;
vector<int> d0;
chrono::steady_clock::time_point t0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();}

static inline int cdist(int r1,int c1,int r2,int c2){
    long long dr=r1-r2,dc=c1-c2; long long d2=dr*dr+dc*dc;
    long long s=(long long)sqrt((double)d2);
    while(s*s<d2)s++;
    while(s>0&&(s-1)*(s-1)>=d2)s--;
    return (int)s;
}
struct Cmd{int d;char t;int a,b,c;};

struct Result{long long score;vector<Cmd> cmds;bool ok;};

Result run(int keyType,double alpha,double limit){
    Result res;res.score=0;res.ok=true;
    vector<int> st=st0;
    vector<int> f(D,0),pr(D,wr[0]),pc(D,wc[0]);
    vector<double> key(NO);
    for(int o=0;o<NO;o++){
        double trips=ceil((double)ords[o].totW/CAP);
        if(keyType==0) key[o]=trips*2*d0[o]+ords[o].items.size()*0.01;
        else if(keyType==1) key[o]=ords[o].totW+d0[o]*0.001;
        else key[o]=trips*(2*d0[o]+ords[o].items.size()*2.0/trips);
    }
    vector<int> idx(NO);iota(idx.begin(),idx.end(),0);
    stable_sort(idx.begin(),idx.end(),[&](int a,int b){return key[a]<key[b];});
    vector<int> tf,tr,tc;
    for(int o:idx){
        if(elapsed()>limit){res.ok=false;break;}
        Order&O=ords[o];
        vector<pair<int,int>> rem=O.items;
        long long remW=O.totW;
        tf=f;tr=pr;tc=pc;
        vector<pair<int,int>> undo; // stock index, amount
        vector<Cmd> tcmds;
        int maxEnd=0;bool fail=false;
        while(remW>0){
            double bestM=1e18;int bd=-1,bw=-1;vector<pair<int,int>> bpack;long long bload=0;
            for(int w:candW[o]){
                vector<pair<int,int>> pack;long long load=0;
                for(auto&it:rem){
                    if(it.second<=0)continue;
                    int p=it.first;
                    long long k=min<long long>(it.second,st[(long long)w*P+p]);
                    k=min<long long>(k,(CAP-load)/wt[p]);
                    if(k>0){pack.push_back({p,(int)k});load+=k*wt[p];}
                }
                if(pack.empty())continue;
                int nl=pack.size();
                int dwo=cdist(wr[w],wc[w],O.r,O.c);
                double pen=alpha*(double)(remW-load)/CAP*2*d0[o];
                for(int d=0;d<D;d++){
                    long long end=(long long)tf[d]+cdist(tr[d],tc[d],wr[w],wc[w])+nl+dwo+nl;
                    if(end>T)continue;
                    double m=end+pen;
                    if(m<bestM){bestM=m;bd=d;bw=w;bpack=pack;bload=load;}
                }
            }
            if(bd<0){fail=true;break;}
            int w=bw,d=bd;
            int end=tf[d]+cdist(tr[d],tc[d],wr[w],wc[w])+(int)bpack.size()+cdist(wr[w],wc[w],O.r,O.c)+(int)bpack.size();
            for(auto&pk:bpack){
                st[(long long)w*P+pk.first]-=pk.second;
                undo.push_back({w*P+pk.first,pk.second});
                tcmds.push_back({d,'L',w,pk.first,pk.second});
            }
            for(auto&pk:bpack){
                tcmds.push_back({d,'D',o,pk.first,pk.second});
                for(auto&it:rem)if(it.first==pk.first){it.second-=pk.second;break;}
            }
            remW-=bload;
            tf[d]=end;tr[d]=O.r;tc[d]=O.c;
            maxEnd=max(maxEnd,end);
        }
        if(fail){
            for(auto&u:undo)st[u.first]+=u.second;
            continue;
        }
        f=tf;pr=tr;pc=tc;
        for(auto&c:tcmds)res.cmds.push_back(c);
        long long t=maxEnd-1;
        res.score+=((long long)(T-t)*100+T-1)/T;
    }
    return res;
}

int main(){
    t0=chrono::steady_clock::now();
    scanf("%d %d %d %d %d",&R_,&C_,&D,&T,&CAP);
    scanf("%d",&P);wt.resize(P);for(auto&x:wt)scanf("%d",&x);
    scanf("%d",&W);wr.resize(W);wc.resize(W);st0.assign((size_t)W*P,0);
    for(int w=0;w<W;w++){scanf("%d %d",&wr[w],&wc[w]);for(int p=0;p<P;p++)scanf("%d",&st0[(size_t)w*P+p]);}
    scanf("%d",&NO);ords.resize(NO);
    for(int o=0;o<NO;o++){
        int L;scanf("%d %d",&ords[o].r,&ords[o].c);scanf("%d",&L);
        map<int,int> m;long long tw=0;
        for(int i=0;i<L;i++){int p;scanf("%d",&p);m[p]++;tw+=wt[p];}
        for(auto&kv:m)ords[o].items.push_back(kv);
        sort(ords[o].items.begin(),ords[o].items.end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first])return wt[a.first]>wt[b.first];return a.first<b.first;});
        ords[o].totW=tw;
    }
    candW.resize(NO);d0.resize(NO);
    int K=min(W,W<=50?W:30);
    for(int o=0;o<NO;o++){
        vector<pair<int,int>> v(W);
        for(int w=0;w<W;w++)v[w]={cdist(wr[w],wc[w],ords[o].r,ords[o].c),w};
        if(K<W){nth_element(v.begin(),v.begin()+K,v.end());v.resize(K);}
        sort(v.begin(),v.end());
        for(auto&x:v)candW[o].push_back(x.second);
        d0[o]=max(1,v[0].first);
    }
    // ensure that each order can find stock: if K<W, items might be unavailable in nearest K; acceptable (order just skipped)
    Result best=run(0,1.0,9.0);
    vector<pair<int,double>> cfgs={{0,0.0},{0,2.0},{1,1.0},{2,1.0},{2,0.5},{0,0.5},{1,0.0},{2,2.0}};
    for(auto&cf:cfgs){
        if(elapsed()>4.0)break;
        Result r=run(cf.first,cf.second,8.5);
        if(r.ok&&r.score>best.score)best=move(r);
    }
    string out;out.reserve(best.cmds.size()*20+20);
    out+=to_string(best.cmds.size());out+='\n';
    char buf[64];
    for(auto&c:best.cmds){
        snprintf(buf,sizeof(buf),"%d %c %d %d %d\n",c.d,c.t,c.a,c.b,c.c);
        out+=buf;
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
