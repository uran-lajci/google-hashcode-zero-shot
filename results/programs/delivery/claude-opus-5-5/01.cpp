#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R_,C_,D,T,CAP,P,W,C;
vector<int> wt, wr, wc, stock0, orr, occ;
vector<vector<pair<int,int>>> need0;
vector<vector<int>> wsorted, nbr;
vector<int> d0;

static inline int dist(int r1,int c1,int r2,int c2){
    ll dr=r1-r2, dc=c1-c2; ll x=dr*dr+dc*dc;
    ll s=(ll)sqrt((double)x);
    while(s*s<x) s++;
    while(s>0&&(s-1)*(s-1)>=x) s--;
    return (int)s;
}

struct U{char k; int a,b,c,d,e;};

struct Result{ ll score; vector<vector<array<int,4>>> cmds; };

Result run(const vector<int>& perm, double alpha){
    vector<int> st=stock0;
    vector<vector<pair<int,int>>> need=need0;
    vector<int> remCnt(C,0); vector<ll> remW(C,0);
    for(int o=0;o<C;o++) for(auto&pr:need[o]){remCnt[o]+=pr.second; remW[o]+=(ll)pr.second*wt[pr.first];}
    vector<int> lastDel(C,-1); vector<char> failed(C,0);
    vector<ll> drFree(D,0); vector<int> drR(D,wr[0]), drC(D,wc[0]);
    vector<vector<array<int,4>>> cmds(D);
    vector<U> lg;
    auto undo=[&](size_t to){
        while(lg.size()>to){
            U u=lg.back(); lg.pop_back();
            if(u.k=='S') st[u.a]=u.b;
            else if(u.k=='N') need[u.a][u.b].second=u.c;
            else if(u.k=='R'){ remCnt[u.a]=u.b; remW[u.a]=(ll)u.c; }
            else if(u.k=='L') lastDel[u.a]=u.b;
            else if(u.k=='D'){ drFree[u.a]=u.b; drR[u.a]=u.c; drC[u.a]=u.d; cmds[u.a].resize(u.e);}
        }
    };
    auto pack=[&](int w,int o,int capLeft,vector<pair<int,int>>&pk){
        pk.clear();
        for(int i=0;i<(int)need[o].size();i++){
            int p=need[o][i].first,n=need[o][i].second; if(!n) continue;
            int s=st[(size_t)w*P+p]; if(!s) continue;
            int take=min(n,min(s,capLeft/wt[p]));
            if(take>0){pk.push_back({i,take}); capLeft-=take*wt[p];}
        }
        return capLeft;
    };
    auto applyPack=[&](int w,int o,const vector<pair<int,int>>&pk){
        lg.push_back({'R',o,remCnt[o],(int)remW[o],0,0});
        for(auto&q:pk){
            int p=need[o][q.first].first;
            size_t si=(size_t)w*P+p;
            lg.push_back({'S',(int)si,st[si],0,0,0}); st[si]-=q.second;
            lg.push_back({'N',o,q.first,need[o][q.first].second,0,0});
            need[o][q.first].second-=q.second;
            remCnt[o]-=q.second; remW[o]-=(ll)q.second*wt[p];
        }
    };
    int K = W<=20?W:8;
    vector<pair<int,int>> pk, pk2;
    vector<int> cw;
    for(int o:perm){
        size_t ls=lg.size(); bool ok=true;
        while(remCnt[o]>0){
            cw.clear();
            for(int w:wsorted[o]){
                bool any=false;
                for(auto&pr:need[o]) if(pr.second&&st[(size_t)w*P+pr.first]){any=true;break;}
                if(any){cw.push_back(w); if((int)cw.size()>=K) break;}
            }
            double bestCost=1e300; int bd=-1,bw=-1;
            for(int w:cw){
                int cl=pack(w,o,CAP,pk); if(pk.empty()) continue;
                ll taken=CAP-cl; ll Rr=remW[o]-taken;
                int dwo=dist(wr[w],wc[w],orr[o],occ[o]);
                double pen=(double)Rr/CAP*2.0*d0[o];
                for(int d=0;d<D;d++){
                    ll f=drFree[d]+dist(drR[d],drC[d],wr[w],wc[w])+2*(ll)pk.size()+dwo;
                    if(f>T) continue;
                    double cost=f+pen;
                    if(cost<bestCost){bestCost=cost;bd=d;bw=w;}
                }
            }
            if(bd<0){ok=false;break;}
            int d=bd,w=bw;
            lg.push_back({'D',d,(int)drFree[d],drR[d],drC[d],(int)cmds[d].size()});
            int capLeft=pack(w,o,CAP,pk);
            applyPack(w,o,pk);
            vector<pair<int,int>> loads; // product,count
            auto addLoad=[&](int p,int c){
                for(auto&l:loads) if(l.first==p){l.second+=c;return;}
                loads.push_back({p,c});
            };
            vector<pair<int,vector<pair<int,int>>>> stops; // order, (product,count)
            {
                vector<pair<int,int>> dl;
                for(auto&q:pk){int p=need[o][q.first].first; addLoad(p,q.second); dl.push_back({p,q.second});}
                stops.push_back({o,dl});
            }
            ll dw=dist(drR[d],drC[d],wr[w],wc[w]);
            int dwo=dist(wr[w],wc[w],orr[o],occ[o]);
            ll tO=dwo+(ll)pk.size();
            ll accum=0; int cr=orr[o],cc=occ[o]; int accepted=0;
            if(alpha>=0 && capLeft>0){
                for(int n:nbr[o]){
                    if(accepted>=3||capLeft<=0) break;
                    if(n==o||remCnt[n]==0||failed[n]) continue;
                    int dd=dist(cr,cc,orr[n],occ[n]);
                    if(dd>alpha*dwo+1) continue;
                    int cl2=pack(w,n,capLeft,pk2); if(pk2.empty()) continue;
                    int newTypes=0;
                    for(auto&q:pk2){int p=need[n][q.first].first; bool f=false; for(auto&l:loads) if(l.first==p){f=true;break;} if(!f)newTypes++;}
                    ll total=drFree[d]+dw+(ll)loads.size()+newTypes+tO+accum+dd+(ll)pk2.size();
                    if(total>T) continue;
                    vector<pair<int,int>> dl;
                    for(auto&q:pk2){int p=need[n][q.first].first; addLoad(p,q.second); dl.push_back({p,q.second});}
                    applyPack(w,n,pk2);
                    stops.push_back({n,dl});
                    accum+=dd+(ll)pk2.size(); cr=orr[n]; cc=occ[n]; capLeft=cl2; accepted++;
                }
            }
            ll t=drFree[d]+dw;
            for(auto&l:loads){cmds[d].push_back({0,w,l.first,l.second}); t+=1;}
            // t now after loads (first includes flight dw)
            int pr_=wr[w],pc_=wc[w];
            for(auto&s:stops){
                int n=s.first;
                t+=dist(pr_,pc_,orr[n],occ[n]);
                for(auto&q:s.second){cmds[d].push_back({1,n,q.first,q.second}); t+=1;}
                lg.push_back({'L',n,lastDel[n],0,0,0});
                lastDel[n]=max(lastDel[n],(int)(t-1));
                pr_=orr[n];pc_=occ[n];
            }
            drFree[d]=t; drR[d]=pr_; drC[d]=pc_;
            if(t>T){ok=false;break;} // safety
        }
        if(!ok){undo(ls); failed[o]=1;}
        else lg.clear();
    }
    ll score=0;
    for(int o=0;o<C;o++) if(remCnt[o]==0&&lastDel[o]>=0&&lastDel[o]<T){
        ll tt=lastDel[o]; score+=((ll)(T-tt)*100+T-1)/T;
    }
    return {score,cmds};
}

int main(){
    scanf("%d %d %d %d %d",&R_,&C_,&D,&T,&CAP);
    scanf("%d",&P); wt.resize(P); for(auto&x:wt)scanf("%d",&x);
    scanf("%d",&W); wr.resize(W);wc.resize(W); stock0.assign((size_t)W*P,0);
    for(int w=0;w<W;w++){scanf("%d %d",&wr[w],&wc[w]); for(int p=0;p<P;p++)scanf("%d",&stock0[(size_t)w*P+p]);}
    scanf("%d",&C); orr.resize(C);occ.resize(C); need0.resize(C);
    for(int o=0;o<C;o++){
        scanf("%d %d",&orr[o],&occ[o]); int L; scanf("%d",&L);
        map<int,int> m; for(int i=0;i<L;i++){int p;scanf("%d",&p);m[p]++;}
        for(auto&kv:m) need0[o].push_back(kv);
        sort(need0[o].begin(),need0[o].end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first]) return wt[a.first]>wt[b.first]; return a.first<b.first;});
    }
    wsorted.resize(C); d0.resize(C);
    for(int o=0;o<C;o++){
        vector<pair<int,int>> v(W);
        for(int w=0;w<W;w++) v[w]={dist(wr[w],wc[w],orr[o],occ[o]),w};
        sort(v.begin(),v.end());
        wsorted[o].resize(W); for(int w=0;w<W;w++) wsorted[o][w]=v[w].second;
        d0[o]=v[0].first;
    }
    nbr.resize(C);
    {
        int NB=min(C-1,10);
        vector<pair<ll,int>> v;
        for(int o=0;o<C;o++){
            v.clear();
            for(int n=0;n<C;n++) if(n!=o){ll dr=orr[o]-orr[n],dc=occ[o]-occ[n]; v.push_back({dr*dr+dc*dc,n});}
            if(NB>0){
                partial_sort(v.begin(),v.begin()+NB,v.end());
                for(int i=0;i<NB;i++) nbr[o].push_back(v[i].second);
            }
        }
    }
    vector<ll> wsum(C,0); vector<int> types(C,0);
    for(int o=0;o<C;o++){for(auto&pr:need0[o]) wsum[o]+=(ll)pr.second*wt[pr.first]; types[o]=need0[o].size();}
    auto makePerm=[&](int key){
        vector<double> k(C);
        for(int o=0;o<C;o++){
            ll trips=(wsum[o]+CAP-1)/CAP;
            if(key==0) k[o]=trips*2.0*d0[o]+types[o];
            else if(key==1) k[o]=wsum[o];
            else k[o]=trips*1e6+d0[o];
        }
        vector<int> perm(C); iota(perm.begin(),perm.end(),0);
        stable_sort(perm.begin(),perm.end(),[&](int a,int b){return k[a]<k[b];});
        return perm;
    };
    vector<pair<int,double>> cfg={{0,-1},{0,0.5},{1,-1},{1,0.5},{2,0.5},{0,1.0}};
    Result best; best.score=-1;
    for(auto&c:cfg){
        Result r=run(makePerm(c.first),c.second);
        if(r.score>best.score) best=move(r);
    }
    ll Q=0; for(auto&v:best.cmds) Q+=v.size();
    string out; out.reserve(Q*20+20);
    out+=to_string(Q)+"\n";
    for(int d=0;d<D;d++) for(auto&c:best.cmds[d]){
        out+=to_string(d); out+=(c[0]==0?" L ":" D ");
        out+=to_string(c[1])+" "+to_string(c[2])+" "+to_string(c[3])+"\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
