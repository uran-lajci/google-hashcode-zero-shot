#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,Cc,D,T,MAXL,P,W,C;
vector<int> wt;
vector<int> wr,wc;
vector<vector<int>> stock0;
vector<int> orr,occ;
vector<vector<pair<int,int>>> need0; // product, count

static inline int idist(int r1,int c1,int r2,int c2){
    ll dr=r1-r2,dc=c1-c2; ll d2=dr*dr+dc*dc;
    ll s=(ll)sqrt((double)d2);
    while(s*s<d2)s++;
    while(s>0&&(s-1)*(s-1)>=d2)s--;
    return (int)s;
}

struct Cmd{int d;char t;int a,b,c;};
struct Drone{int r,c;int t;};

ll bestScore=-1; vector<Cmd> bestCmds;

int main(){
    auto t0=chrono::steady_clock::now();
    scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&MAXL);
    scanf("%d",&P); wt.resize(P); for(auto&x:wt)scanf("%d",&x);
    scanf("%d",&W); wr.resize(W);wc.resize(W);stock0.assign(W,vector<int>(P));
    for(int i=0;i<W;i++){scanf("%d %d",&wr[i],&wc[i]);for(int p=0;p<P;p++)scanf("%d",&stock0[i][p]);}
    scanf("%d",&C); orr.resize(C);occ.resize(C);need0.resize(C);
    vector<ll> oweight(C,0); vector<int> oitems(C,0), ond(C,0);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&occ[i]); int L; scanf("%d",&L);
        map<int,int> m; for(int j=0;j<L;j++){int x;scanf("%d",&x);m[x]++;oweight[i]+=wt[x];}
        oitems[i]=L;
        for(auto&kv:m)need0[i].push_back(kv);
        sort(need0[i].begin(),need0[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first])return wt[a.first]>wt[b.first];return a.first<b.first;});
        int nd=INT_MAX; for(int w=0;w<W;w++)nd=min(nd,idist(orr[i],occ[i],wr[w],wc[w]));
        ond[i]=nd;
    }
    int variants=8;
    for(int v=0;v<variants;v++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(v>0 && el>7.0)break;
        int keyType=v%4; double beta=(v/4)?1.0:0.0;
        vector<double> key(C);
        for(int i=0;i<C;i++){
            ll trips=(oweight[i]+MAXL-1)/MAXL;
            if(keyType==0)key[i]=trips*(2.0*ond[i]+2)+need0[i].size();
            else if(keyType==1)key[i]=oweight[i];
            else if(keyType==2)key[i]=oitems[i]*1000.0+ond[i];
            else key[i]=trips*1e6+ond[i]*10.0+need0[i].size();
        }
        vector<int> ord(C); iota(ord.begin(),ord.end(),0);
        stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key[a]<key[b];});
        vector<vector<int>> stock=stock0;
        vector<Drone> dr(D,{wr[0],wc[0],0});
        vector<Cmd> cmds; ll score=0;
        vector<pair<int,int>> loads; // product,count
        vector<pair<int,int>> bestLoads;
        for(int o:ord){
            vector<Drone> drSave=dr; size_t cmdSave=cmds.size();
            vector<array<int,3>> slog;
            vector<pair<int,int>> need=need0[o];
            ll remW=oweight[o];
            bool ok=true; int compl_=0;
            while(remW>0){
                double bestCost=1e300; int bd=-1,bw=-1,bfin=0; bestLoads.clear();
                for(int w=0;w<W;w++){
                    loads.clear(); int cap=MAXL; ll dw=0;
                    for(auto&pr:need){
                        if(pr.second==0)continue;
                        int p=pr.first; int s=stock[w][p]; if(s<=0)continue;
                        int k=min(pr.second,min(s,cap/wt[p]));
                        if(k>0){loads.push_back({p,k});cap-=k*wt[p];dw+=(ll)k*wt[p];}
                    }
                    if(loads.empty())continue;
                    int dwo=idist(wr[w],wc[w],orr[o],occ[o]);
                    int fixedPart=(int)loads.size()*2+dwo;
                    double bonus=beta*(double)dw/MAXL*(2.0*ond[o]+2);
                    for(int d=0;d<D;d++){
                        int fin=dr[d].t+idist(dr[d].r,dr[d].c,wr[w],wc[w])+fixedPart;
                        double cost=fin-bonus;
                        if(cost<bestCost){bestCost=cost;bd=d;bw=w;bfin=fin;bestLoads=loads;}
                    }
                }
                if(bd<0||bfin>T){ok=false;break;}
                for(auto&l:bestLoads){
                    stock[bw][l.first]-=l.second; slog.push_back({bw,l.first,l.second});
                    cmds.push_back({bd,'L',bw,l.first,l.second});
                }
                for(auto&l:bestLoads){
                    cmds.push_back({bd,'D',o,l.first,l.second});
                    for(auto&pr:need)if(pr.first==l.first){pr.second-=l.second;break;}
                    remW-=(ll)l.second*wt[l.first];
                }
                dr[bd].t=bfin; dr[bd].r=orr[o]; dr[bd].c=occ[o];
                compl_=max(compl_,bfin);
            }
            if(!ok){
                dr=drSave; cmds.resize(cmdSave);
                for(auto&s:slog)stock[s[0]][s[1]]+=s[2];
                continue;
            }
            int t=compl_-1;
            score+=((ll)(T-t)*100+T-1)/T;
        }
        if(score>bestScore){bestScore=score;bestCmds=cmds;}
    }
    string out; out.reserve(bestCmds.size()*24+32);
    out+=to_string(bestCmds.size())+"\n";
    char buf[96];
    for(auto&c:bestCmds){
        snprintf(buf,sizeof(buf),"%d %c %d %d %d\n",c.d,c.t,c.a,c.b,c.c);
        out+=buf;
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
