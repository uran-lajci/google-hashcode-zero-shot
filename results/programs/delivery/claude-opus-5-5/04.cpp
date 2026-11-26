#include <bits/stdc++.h>
using namespace std;

int R,Cc,D,T,CAP,P,W,C;
vector<int> wt;
vector<int> wr,wc;
vector<int> stockv; // W*P
struct Order{int r,c; vector<pair<int,int>> it; long long tw=0;};
vector<Order> ord;

static inline int cdist(int r1,int c1,int r2,int c2){
    long long dr=r1-r2,dc=c1-c2; long long d2=dr*dr+dc*dc;
    if(d2==0) return 0;
    long long r=(long long)ceil(sqrt((double)d2));
    while(r>0 && (r-1)*(r-1)>=d2) r--;
    while(r*r<d2) r++;
    return (int)r;
}
inline int& stk(int w,int p){return stockv[(size_t)w*P+p];}

// pack items of order o from warehouse w with capacity cap, considering extra used amounts
int packOrder(int w,int o,int cap,vector<pair<int,int>>&out,const unordered_map<int,int>*used){
    int carried=0;
    for(auto&pr:ord[o].it){
        int p=pr.first,cnt=pr.second;
        int s=stk(w,p);
        if(used){auto f=used->find(p); if(f!=used->end()) s-=f->second;}
        if(s<=0) continue;
        int k=min(cnt,s);
        k=min(k,cap/wt[p]);
        if(k<=0) continue;
        out.push_back({p,k});
        cap-=k*wt[p]; carried+=k*wt[p];
    }
    return carried;
}

int main(){
    if(scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&CAP)!=5) return 0;
    scanf("%d",&P); wt.resize(P); for(auto&x:wt) scanf("%d",&x);
    scanf("%d",&W); wr.resize(W); wc.resize(W); stockv.assign((size_t)W*P,0);
    for(int w=0;w<W;w++){ scanf("%d %d",&wr[w],&wc[w]); for(int p=0;p<P;p++) scanf("%d",&stockv[(size_t)w*P+p]); }
    scanf("%d",&C); ord.resize(C);
    for(int i=0;i<C;i++){
        int L; scanf("%d %d",&ord[i].r,&ord[i].c); scanf("%d",&L);
        map<int,int> m; for(int j=0;j<L;j++){int p; scanf("%d",&p); m[p]++;}
        for(auto&kv:m){ord[i].it.push_back(kv); ord[i].tw+= (long long)wt[kv.first]*kv.second;}
        sort(ord[i].it.begin(),ord[i].it.end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first]) return wt[a.first]>wt[b.first]; return a.first<b.first;});
    }
    // keys
    vector<double> key(C);
    for(int i=0;i<C;i++){
        int nd=INT_MAX;
        for(int w=0;w<W;w++){
            bool has=false; for(auto&pr:ord[i].it) if(stk(w,pr.first)>0){has=true;break;}
            if(has) nd=min(nd,cdist(wr[w],wc[w],ord[i].r,ord[i].c));
        }
        if(nd==INT_MAX) nd=100000;
        long long trips=(ord[i].tw+CAP-1)/CAP;
        key[i]=(double)trips*2.0*nd + ord[i].it.size()*2;
    }
    vector<int> idx(C); iota(idx.begin(),idx.end(),0);
    stable_sort(idx.begin(),idx.end(),[&](int a,int b){return key[a]<key[b];});
    vector<int> dr(D,wr[0]),dcc(D,wc[0]); vector<long long> tm(D,0);
    vector<char> fin(C,0);
    vector<string> cmds;
    char buf[64];
    for(int si=0;si<C;si++){
        int o=idx[si];
        if(fin[o]) continue;
        while(!ord[o].it.empty()){
            // pack per warehouse
            vector<vector<pair<int,int>>> packs(W);
            vector<int> carr(W,0); int mx=0;
            for(int w=0;w<W;w++){
                carr[w]=packOrder(w,o,CAP,packs[w],nullptr);
                mx=max(mx,carr[w]);
            }
            if(mx==0){fin[o]=1;break;}
            long long bestF=LLONG_MAX; int bd=-1,bw=-1;
            for(int w=0;w<W;w++){
                if(carr[w]*2<mx) continue;
                int nt=packs[w].size();
                int dwo=cdist(wr[w],wc[w],ord[o].r,ord[o].c);
                for(int d=0;d<D;d++){
                    long long f=tm[d]+cdist(dr[d],dcc[d],wr[w],wc[w])+nt+dwo+nt;
                    if(f<bestF){bestF=f;bd=d;bw=w;}
                }
            }
            if(bestF>T){fin[o]=1;break;}
            int d=bd,w=bw;
            auto mainPack=packs[w];
            int capLeft=CAP-carr[w];
            // extras
            unordered_map<int,int> used;
            for(auto&pr:mainPack) used[pr.first]+=pr.second;
            vector<pair<int,vector<pair<int,int>>>> extras;
            long long tEnd=bestF;
            int curR=ord[o].r,curC=ord[o].c;
            set<int> chosen;
            long long extraLoadsTypes=0;
            for(int rep=0;rep<4 && capLeft>0;rep++){
                int bj=-1; int bdist=INT_MAX;
                for(int sj=si+1,cnt=0;sj<C && cnt<60;sj++){
                    int j=idx[sj]; if(fin[j]||chosen.count(j)) continue; cnt++;
                    int dd=cdist(curR,curC,ord[j].r,ord[j].c);
                    int dwj=cdist(wr[w],wc[w],ord[j].r,ord[j].c);
                    if(dd*2>dwj) continue;
                    if(dd<bdist){
                        // check some packable
                        bool ok=false;
                        for(auto&pr:ord[j].it){
                            int s=stk(w,pr.first); auto f=used.find(pr.first); if(f!=used.end()) s-=f->second;
                            if(s>0 && wt[pr.first]<=capLeft){ok=true;break;}
                        }
                        if(ok){bdist=dd;bj=j;}
                    }
                }
                if(bj<0) break;
                vector<pair<int,int>> pk;
                int cw=packOrder(w,bj,capLeft,pk,&used);
                if(cw==0) break;
                // new load types
                int newTypes=0;
                for(auto&pr:pk) if(!used.count(pr.first)) newTypes++;
                long long add=newTypes+bdist+(long long)pk.size();
                if(tEnd+add>T) break;
                tEnd+=add;
                extraLoadsTypes+=newTypes;
                for(auto&pr:pk) used[pr.first]+=pr.second;
                capLeft-=cw;
                extras.push_back({bj,pk});
                chosen.insert(bj);
                curR=ord[bj].r; curC=ord[bj].c;
            }
            // emit loads
            vector<pair<int,int>> loads;
            {
                map<int,int> m; vector<int> orderP;
                for(auto&pr:mainPack){ if(!m.count(pr.first)) orderP.push_back(pr.first); m[pr.first]+=pr.second;}
                for(auto&e:extras) for(auto&pr:e.second){ if(!m.count(pr.first)) orderP.push_back(pr.first); m[pr.first]+=pr.second;}
                for(int p:orderP) loads.push_back({p,m[p]});
            }
            for(auto&pr:loads){
                snprintf(buf,sizeof buf,"%d L %d %d %d",d,w,pr.first,pr.second);
                cmds.push_back(buf);
                stk(w,pr.first)-=pr.second;
            }
            auto deliver=[&](int oo,const vector<pair<int,int>>&pk){
                for(auto&pr:pk){
                    snprintf(buf,sizeof buf,"%d D %d %d %d",d,oo,pr.first,pr.second);
                    cmds.push_back(buf);
                    for(auto&q:ord[oo].it) if(q.first==pr.first){q.second-=pr.second;break;}
                }
                vector<pair<int,int>> nv;
                for(auto&q:ord[oo].it) if(q.second>0) nv.push_back(q);
                ord[oo].it=nv;
                if(nv.empty()) fin[oo]=1;
            };
            deliver(o,mainPack);
            for(auto&e:extras) deliver(e.first,e.second);
            // recompute time exactly
            long long t=tm[d]+cdist(dr[d],dcc[d],wr[w],wc[w])+loads.size();
            int pr_=wr[w],pc_=wc[w];
            t+=cdist(pr_,pc_,ord[o].r,ord[o].c)+mainPack.size(); pr_=ord[o].r; pc_=ord[o].c;
            for(auto&e:extras){ t+=cdist(pr_,pc_,ord[e.first].r,ord[e.first].c)+e.second.size(); pr_=ord[e.first].r; pc_=ord[e.first].c;}
            tm[d]=t; dr[d]=pr_; dcc[d]=pc_;
        }
        fin[o]=1;
    }
    string out; out.reserve(cmds.size()*16+16);
    out+=to_string(cmds.size()); out+='\n';
    for(auto&s:cmds){out+=s; out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
