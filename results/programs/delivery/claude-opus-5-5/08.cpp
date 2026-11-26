#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,Cc,D,T,CAP,P,W,C;
vector<int> wt;
vector<int> whr,whc;
vector<int> stock0;
vector<int> orr,orc;
vector<vector<pair<int,int>>> items; // (type,count) sorted weight desc
long long opsCount=0;

static inline int cdist(int r1,int c1,int r2,int c2){
    ll dr=r1-r2,dc=c1-c2; ll x=dr*dr+dc*dc;
    int s=(int)sqrt((double)x);
    while((ll)s*s<x)s++;
    while(s>0&&(ll)(s-1)*(s-1)>=x)s--;
    return s;
}
struct Cmd{int d;char t;int a,b,c;};

ll simulate(const vector<int>&seq,bool emit,vector<Cmd>&cmds){
    vector<int> stock=stock0;
    vector<int> fr(D,0),pr(D,whr[0]),pc(D,whc[0]);
    opsCount+=(ll)W*P+(ll)D*C;
    ll score=0;
    vector<pair<int,int>> stChanges; // (index, amount)
    vector<tuple<int,int,int,int>> drChanges; // d,free,r,c
    vector<pair<int,int>> load; // (idx in rem, k)
    vector<pair<int,int>> bestLoad;
    for(int o:seq){
        vector<pair<int,int>> rem=items[o];
        int remCount=0; for(auto&p:rem)remCount+=p.second;
        stChanges.clear(); drChanges.clear();
        size_t cmdSave=cmds.size();
        bool ok=true; int maxFinish=0;
        while(remCount>0){
            int bestFinish=INT_MAX,bestW=-1,bestD=-1,bestWeight=-1;
            opsCount+=(ll)W*(rem.size()+D);
            for(int w=0;w<W;w++){
                int used=0,nt=0;
                const int* st=&stock[(size_t)w*P];
                for(auto&p:rem){
                    if(p.second==0)continue;
                    int t=p.first; int s=st[t]; if(!s)continue;
                    int k=min(p.second,min(s,(CAP-used)/wt[t]));
                    if(k>0){used+=k*wt[t];nt++;}
                }
                if(nt==0)continue;
                int dwo=cdist(whr[w],whc[w],orr[o],orc[o]);
                int ba=INT_MAX,bd=-1;
                for(int d=0;d<D;d++){
                    int a=fr[d]+cdist(pr[d],pc[d],whr[w],whc[w]);
                    if(a<ba){ba=a;bd=d;}
                }
                int fin=ba+2*nt+dwo;
                if(fin<bestFinish||(fin==bestFinish&&used>bestWeight)){
                    bestFinish=fin;bestW=w;bestD=bd;bestWeight=used;
                }
            }
            if(bestW<0||bestFinish>T){ok=false;break;}
            int w=bestW,d=bestD;
            drChanges.emplace_back(d,fr[d],pr[d],pc[d]);
            int used=0;
            bestLoad.clear();
            for(size_t i=0;i<rem.size();i++){
                auto&p=rem[i];
                if(p.second==0)continue;
                int t=p.first; int s=stock[(size_t)w*P+t]; if(!s)continue;
                int k=min(p.second,min(s,(CAP-used)/wt[t]));
                if(k>0){used+=k*wt[t];bestLoad.push_back({(int)i,k});}
            }
            for(auto&l:bestLoad){
                int t=rem[l.first].first;
                stock[(size_t)w*P+t]-=l.second;
                stChanges.push_back({w*P+t,l.second});
                rem[l.first].second-=l.second; remCount-=l.second;
                cmds.push_back({d,'L',w,t,l.second});
            }
            for(auto&l:bestLoad){
                int t=rem[l.first].first;
                cmds.push_back({d,'D',o,t,l.second});
            }
            fr[d]=bestFinish;pr[d]=orr[o];pc[d]=orc[o];
            maxFinish=max(maxFinish,bestFinish);
        }
        if(!ok){
            for(auto&s:stChanges)stock[s.first]+=s.second;
            for(int i=(int)drChanges.size()-1;i>=0;i--){
                auto [d,f,r,c]=drChanges[i];
                fr[d]=f;pr[d]=r;pc[d]=c;
            }
            cmds.resize(cmdSave);
            continue;
        }
        ll t=maxFinish-1;
        score+=((ll)(T-t)*100+T-1)/T;
        if(!emit)cmds.clear();
    }
    return score;
}

int main(){
    if(scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&CAP)!=5)return 0;
    scanf("%d",&P); wt.resize(P); for(auto&x:wt)scanf("%d",&x);
    scanf("%d",&W); whr.resize(W);whc.resize(W); stock0.assign((size_t)W*P,0);
    for(int w=0;w<W;w++){scanf("%d %d",&whr[w],&whc[w]); for(int p=0;p<P;p++)scanf("%d",&stock0[(size_t)w*P+p]);}
    scanf("%d",&C); orr.resize(C);orc.resize(C);items.resize(C);
    vector<ll> totW(C,0);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&orc[i]); int L; scanf("%d",&L);
        map<int,int> m; for(int j=0;j<L;j++){int t;scanf("%d",&t);m[t]++;totW[i]+=wt[t];}
        for(auto&p:m)items[i].push_back(p);
        sort(items[i].begin(),items[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first])return wt[a.first]>wt[b.first]; return a.first<b.first;});
    }
    // estimates
    vector<double> dmin(C,1e18);
    for(int i=0;i<C;i++){
        // nearest warehouse having any item of order
        for(int w=0;w<W;w++){
            bool has=false;
            for(auto&p:items[i]) if(stock0[(size_t)w*P+p.first]>0){has=true;break;}
            if(!has)continue;
            dmin[i]=min(dmin[i],(double)cdist(whr[w],whc[w],orr[i],orc[i]));
        }
        if(dmin[i]>1e17)dmin[i]=0;
    }
    vector<vector<int>> cands;
    for(int v=0;v<4;v++){
        vector<double> key(C);
        for(int i=0;i<C;i++){
            double trips=ceil((double)totW[i]/CAP);
            double nt=items[i].size();
            if(v==0)key[i]=trips*(2*dmin[i]+2*nt);
            else if(v==1)key[i]=trips*(2*dmin[i]+2*nt)+ (double)totW[i]/CAP*20;
            else if(v==2)key[i]=(double)totW[i];
            else key[i]=trips*2*dmin[i]+nt*4;
        }
        vector<int> s(C); iota(s.begin(),s.end(),0);
        stable_sort(s.begin(),s.end(),[&](int a,int b){return key[a]<key[b];});
        cands.push_back(s);
    }
    vector<Cmd> dummy;
    vector<int> best; ll bestScore=-1;
    for(auto&s:cands){
        ll sc=simulate(s,false,dummy);
        if(sc>bestScore){bestScore=sc;best=s;}
    }
    const ll BUDGET=1000000000LL;
    mt19937 rng(12345);
    vector<int> cur=best; ll curScore=bestScore;
    while(opsCount<BUDGET && C>1){
        vector<int> nb=cur;
        int typ=rng()%2;
        int i=rng()%C;
        int span=max(2,min(C,100));
        int j=i+(int)(rng()%(2*span+1))-span;
        if(j<0)j=0; if(j>=C)j=C-1;
        if(i==j)continue;
        if(typ==0)swap(nb[i],nb[j]);
        else{int x=nb[i];nb.erase(nb.begin()+i);nb.insert(nb.begin()+j,x);}
        ll sc=simulate(nb,false,dummy);
        if(sc>=curScore){cur=nb;curScore=sc;if(sc>bestScore){bestScore=sc;best=cur;}}
    }
    vector<Cmd> cmds;
    simulate(best,true,cmds);
    string out;
    out.reserve(cmds.size()*20+20);
    out+=to_string(cmds.size());out+='\n';
    for(auto&c:cmds){
        out+=to_string(c.d);out+=' ';out+=c.t;out+=' ';
        out+=to_string(c.a);out+=' ';out+=to_string(c.b);out+=' ';out+=to_string(c.c);out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
