#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline long long readInt(){
    int c=gc();while(c!='-'&&(c<'0'||c>'9')){if(c==-1)return 0;c=gc();}
    bool neg=false;if(c=='-'){neg=true;c=gc();}
    long long x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}
    return neg?-x:x;
}
inline long long dist(int r1,int c1,int r2,int c2){
    long long dr=r1-r2,dc=c1-c2;long long d2=dr*dr+dc*dc;
    long long s=(long long)sqrt((double)d2);
    while(s*s<d2)s++;
    while(s>0&&(s-1)*(s-1)>=d2)s--;
    return s;
}
struct Drone{int r,c;long long t;};
struct Cmd{int d;char tag;int a,p,n;};

int main(){
    int R=readInt(),Cc=readInt(),D=readInt();long long T=readInt();int cap=readInt();
    (void)R;(void)Cc;
    int P=readInt();
    vector<int> wt(P);for(auto&x:wt)x=readInt();
    int W=readInt();
    vector<int> wr(W),wc(W);
    vector<int> stock((size_t)W*P);
    for(int i=0;i<W;i++){wr[i]=readInt();wc[i]=readInt();for(int p=0;p<P;p++)stock[(size_t)i*P+p]=readInt();}
    int C=readInt();
    vector<int> orr(C),oc(C);
    vector<vector<pair<int,int>>> items(C); // product,count sorted by weight desc
    for(int i=0;i<C;i++){
        orr[i]=readInt();oc[i]=readInt();int L=readInt();
        map<int,int> m;for(int j=0;j<L;j++){int p=readInt();m[p]++;}
        for(auto&kv:m)items[i].push_back(kv);
        sort(items[i].begin(),items[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first])return wt[a.first]>wt[b.first];return a.first<b.first;});
    }
    // estimate costs
    vector<pair<double,int>> ordList;
    for(int i=0;i<C;i++){
        long long tw=0;for(auto&kv:items[i])tw+=(long long)wt[kv.first]*kv.second;
        long long trips=(tw+cap-1)/cap;
        long long dmin=LLONG_MAX;
        for(int w=0;w<W;w++){
            bool useful=false;
            for(auto&kv:items[i]) if(stock[(size_t)w*P+kv.first]>0){useful=true;break;}
            if(!useful)continue;
            dmin=min(dmin,dist(wr[w],wc[w],orr[i],oc[i]));
        }
        if(dmin==LLONG_MAX)dmin=100000;
        double cost=(double)trips*(2.0*dmin+2)+items[i].size();
        ordList.push_back({cost,i});
    }
    sort(ordList.begin(),ordList.end());
    vector<Drone> drones(D,{wr[0],wc[0],0});
    vector<Cmd> cmds;
    const int K=16;
    for(auto&oe:ordList){
        int o=oe.second;
        vector<Drone> saveD=drones;
        vector<tuple<int,int,int>> slog;
        size_t cmdStart=cmds.size();
        vector<pair<int,int>> rem=items[o];
        bool ok=true;
        // sorted warehouses by distance
        vector<pair<long long,int>> wl(W);
        for(int w=0;w<W;w++)wl[w]={dist(wr[w],wc[w],orr[o],oc[o]),w};
        size_t sortedUpTo=0;
        if(W<=128){sort(wl.begin(),wl.end());sortedUpTo=W;}
        else{partial_sort(wl.begin(),wl.begin()+128,wl.end());sortedUpTo=128;}
        while(true){
            long long remW=0;for(auto&kv:rem)remW+=(long long)wt[kv.first]*kv.second;
            if(remW==0)break;
            vector<int> cands;
            for(size_t i=0;i<(size_t)W&&(int)cands.size()<K;i++){
                if(i>=sortedUpTo){sort(wl.begin()+sortedUpTo,wl.end());sortedUpTo=W;}
                int w=wl[i].second;
                for(auto&kv:rem) if(kv.second>0&&stock[(size_t)w*P+kv.first]>0){cands.push_back(w);break;}
            }
            if(cands.empty()){ok=false;break;}
            long long bestCost=LLONG_MAX,bestFin=0;int bestD=-1,bestW=-1;
            vector<pair<int,int>> bestLoad;
            for(int w:cands){
                vector<pair<int,int>> load;int used=0;long long lw=0;
                for(auto&kv:rem){
                    if(kv.second<=0)continue;
                    int p=kv.first;
                    int take=min(kv.second,stock[(size_t)w*P+p]);
                    take=min(take,(cap-used)/wt[p]);
                    if(take>0){load.push_back({p,take});used+=take*wt[p];lw+=(long long)take*wt[p];}
                }
                if(load.empty())continue;
                int k=load.size();
                long long dwo=dist(wr[w],wc[w],orr[o],oc[o]);
                long long after=remW-lw;
                long long pen=(after+cap-1)/cap*(2*dwo+2);
                for(int d=0;d<D;d++){
                    long long fin=drones[d].t+dist(drones[d].r,drones[d].c,wr[w],wc[w])+k+dwo+k;
                    long long cost=fin+pen;
                    if(cost<bestCost){bestCost=cost;bestFin=fin;bestD=d;bestW=w;bestLoad=load;}
                }
            }
            if(bestD<0||bestFin>T){ok=false;break;}
            int w=bestW,d=bestD;
            for(auto&l:bestLoad){
                stock[(size_t)w*P+l.first]-=l.second;slog.push_back({w,l.first,l.second});
                cmds.push_back({d,'L',w,l.first,l.second});
            }
            for(auto&l:bestLoad){
                cmds.push_back({d,'D',o,l.first,l.second});
                for(auto&kv:rem)if(kv.first==l.first){kv.second-=l.second;break;}
            }
            drones[d].t=bestFin;drones[d].r=orr[o];drones[d].c=oc[o];
        }
        if(!ok){
            drones=saveD;
            for(auto&s:slog)stock[(size_t)get<0>(s)*P+get<1>(s)]+=get<2>(s);
            cmds.resize(cmdStart);
        }
    }
    string out;out.reserve(cmds.size()*20+20);
    out+=to_string(cmds.size());out+='\n';
    for(auto&c:cmds){
        out+=to_string(c.d);out+=' ';out+=c.tag;out+=' ';
        out+=to_string(c.a);out+=' ';out+=to_string(c.p);out+=' ';out+=to_string(c.n);out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
