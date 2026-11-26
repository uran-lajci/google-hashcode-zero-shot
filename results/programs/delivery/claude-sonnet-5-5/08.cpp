#include <bits/stdc++.h>
using namespace std;
static int dist(int r1,int c1,int r2,int c2){
    long long dx=r1-r2,dy=c1-c2;long long s=dx*dx+dy*dy;
    long long d=(long long)sqrt((double)s);
    while(d*d<s)d++;
    while(d>0&&(d-1)*(d-1)>=s)d--;
    return (int)d;
}
int main(){
    int R,C_,D,T,cap;
    scanf("%d %d %d %d %d",&R,&C_,&D,&T,&cap);
    int P;scanf("%d",&P);
    vector<int> wt(P);
    for(auto&x:wt)scanf("%d",&x);
    int W;scanf("%d",&W);
    vector<int> wr(W),wc(W);
    vector<uint16_t> st((size_t)W*P);
    for(int i=0;i<W;i++){
        scanf("%d %d",&wr[i],&wc[i]);
        for(int p=0;p<P;p++){int x;scanf("%d",&x);st[(size_t)i*P+p]=x;}
    }
    int N;scanf("%d",&N);
    vector<int> orr(N),oc(N);
    vector<vector<pair<int,int>>> rem(N);
    vector<long long> ow(N,0);
    for(int i=0;i<N;i++){
        scanf("%d %d",&orr[i],&oc[i]);
        int L;scanf("%d",&L);
        map<int,int> m;
        for(int j=0;j<L;j++){int t;scanf("%d",&t);m[t]++;ow[i]+=wt[t];}
        for(auto&kv:m)rem[i].push_back(kv);
    }
    vector<int> ord(N);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return ow[a]<ow[b];});
    vector<int> dr(D,wr[0]),dc(D,wc[0]);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
    for(int d=0;d<D;d++)pq.push({0,d});
    vector<string> out;
    size_t ptr=0;
    auto done=[&](int o){for(auto&p:rem[o])if(p.second>0)return false;return true;};
    while(!pq.empty()){
        auto [tm,d]=pq.top();pq.pop();
        while(ptr<ord.size()&&done(ord[ptr]))ptr++;
        if(ptr>=ord.size())break;
        int o=ord[ptr];
        // plan
        int left=cap;
        struct Ld{int w,t,k;};
        vector<Ld> loads;
        map<pair<int,int>,int> taken;
        vector<pair<int,int>> deliv; // idx in rem, count
        for(size_t i=0;i<rem[o].size();i++){
            int t=rem[o][i].first,c=rem[o][i].second;
            if(c<=0)continue;
            int fit=left/wt[t];
            if(fit<=0)continue;
            int want=min(c,fit);
            while(want>0){
                int bw=-1,bd=INT_MAX;
                for(int w=0;w<W;w++){
                    int s=st[(size_t)w*P+t];
                    if(s<=0)continue;
                    int dd=abs(wr[w]-orr[o])+abs(wc[w]-oc[o]);
                    if(dd<bd){bd=dd;bw=w;}
                }
                if(bw<0)break;
                int k=min(want,(int)st[(size_t)bw*P+t]);
                st[(size_t)bw*P+t]-=k;
                loads.push_back({bw,t,k});
                want-=k;left-=k*wt[t];
                deliv.push_back({(int)i,k});
            }
        }
        // restore stock helper
        auto restore=[&](){for(auto&l:loads)st[(size_t)l.w*P+l.t]+=l.k;};
        if(loads.empty()){continue;}
        // order loads by warehouse nearest neighbor
        vector<int> ws;
        for(auto&l:loads)ws.push_back(l.w);
        sort(ws.begin(),ws.end());ws.erase(unique(ws.begin(),ws.end()),ws.end());
        vector<int> seq;
        int cr=dr[d],cc=dc[d];long long t2=tm;
        vector<bool> used(ws.size(),false);
        for(size_t s=0;s<ws.size();s++){
            int b=-1,bd=INT_MAX;
            for(size_t j=0;j<ws.size();j++)if(!used[j]){
                int dd=abs(wr[ws[j]]-cr)+abs(wc[ws[j]]-cc);
                if(dd<bd){bd=dd;b=j;}
            }
            used[b]=true;seq.push_back(ws[b]);
            int w=ws[b];
            int cnt=0;for(auto&l:loads)if(l.w==w)cnt++;
            t2+=dist(cr,cc,wr[w],wc[w])+cnt;
            cr=wr[w];cc=wc[w];
        }
        t2+=dist(cr,cc,orr[o],oc[o]);
        t2+=deliv.size();
        if(t2>T){restore();continue;} // retire drone
        for(int w:seq){
            for(auto&l:loads)if(l.w==w)
                out.push_back(to_string(d)+" L "+to_string(w)+" "+to_string(l.t)+" "+to_string(l.k));
        }
        for(auto&x:deliv){
            rem[o][x.first].second-=x.second;
            out.push_back(to_string(d)+" D "+to_string(o)+" "+to_string(rem[o][x.first].first)+" "+to_string(x.second));
        }
        dr[d]=orr[o];dc[d]=oc[o];
        pq.push({(int)t2,d});
    }
    printf("%zu\n",out.size());
    string s;
    for(auto&x:out){s+=x;s+='\n';}
    fputs(s.c_str(),stdout);
}
