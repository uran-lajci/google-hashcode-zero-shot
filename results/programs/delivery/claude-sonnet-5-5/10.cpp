#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static ll cd(ll ax,ll ay,ll bx,ll by){
    ll x=(ax-bx)*(ax-bx)+(ay-by)*(ay-by);
    ll d=(ll)sqrt((double)x);
    while(d*d<x)d++;
    while(d>0&&(d-1)*(d-1)>=x)d--;
    return d;
}
int main(){
    int R,Cc,D,T,ML;
    scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&ML);
    int P;scanf("%d",&P);
    vector<int> wt(P);
    for(auto&x:wt)scanf("%d",&x);
    int W;scanf("%d",&W);
    vector<int> wr(W),wc(W);
    vector<vector<int>> st(W,vector<int>(P));
    for(int i=0;i<W;i++){
        scanf("%d %d",&wr[i],&wc[i]);
        for(int j=0;j<P;j++)scanf("%d",&st[i][j]);
    }
    int C;scanf("%d",&C);
    vector<int> orr(C),oc(C),ol(C);
    vector<map<int,int>> need(C);
    for(int i=0;i<C;i++){
        scanf("%d %d %d",&orr[i],&oc[i],&ol[i]);
        for(int j=0;j<ol[i];j++){int p;scanf("%d",&p);need[i][p]++;}
    }
    vector<int> ord(C);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return ol[a]<ol[b];});
    vector<ll> dt(D,0),dr(D,wr[0]),dcc(D,wc[0]);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<D;i++)pq.push({0,i});
    string out;
    ll Q=0;
    for(int o:ord){
        auto &nd=need[o];
        bool fail=false;
        while(!fail){
            // remaining?
            bool any=false;
            for(auto&kv:nd)if(kv.second>0){any=true;break;}
            if(!any)break;
            auto top=pq.top();pq.pop();
            int d=top.second;
            struct Rec{int w,p,k;};
            vector<Rec> recs;
            ll cap=ML;
            for(auto&kv:nd){
                int p=kv.first;
                int cnt=kv.second;
                while(cnt>0&&cap>=wt[p]){
                    int bw=-1;ll bd=LLONG_MAX;
                    for(int w=0;w<W;w++){
                        if(st[w][p]<=0)continue;
                        ll dd=(ll)(wr[w]-orr[o])*(wr[w]-orr[o])+(ll)(wc[w]-oc[o])*(wc[w]-oc[o]);
                        if(dd<bd){bd=dd;bw=w;}
                    }
                    if(bw<0)break;
                    ll k=min<ll>({(ll)cnt,(ll)st[bw][p],cap/wt[p]});
                    if(k<=0)break;
                    st[bw][p]-=k;cnt-=k;cap-=k*wt[p];
                    recs.push_back({bw,p,(int)k});
                }
            }
            if(recs.empty()){pq.push(top);fail=true;break;}
            stable_sort(recs.begin(),recs.end(),[](const Rec&a,const Rec&b){return a.w<b.w;});
            ll t=dt[d],r=dr[d],c=dcc[d];
            for(auto&x:recs){
                t+=cd(r,c,wr[x.w],wc[x.w])+1;r=wr[x.w];c=wc[x.w];
            }
            bool first=true;
            for(auto&x:recs){
                if(first){t+=cd(r,c,orr[o],oc[o]);r=orr[o];c=oc[o];first=false;}
                t+=1;
            }
            if(t>T){
                for(auto&x:recs)st[x.w][x.p]+=x.k;
                pq.push(top);
                fail=true;break;
            }
            for(auto&x:recs){
                out+=to_string(d)+" L "+to_string(x.w)+" "+to_string(x.p)+" "+to_string(x.k)+"\n";
                Q++;
            }
            for(auto&x:recs){
                out+=to_string(d)+" D "+to_string(o)+" "+to_string(x.p)+" "+to_string(x.k)+"\n";
                Q++;
                nd[x.p]-=x.k;
            }
            dt[d]=t;dr[d]=r;dcc[d]=c;
            pq.push({t,d});
        }
    }
    printf("%lld\n",Q);
    fputs(out.c_str(),stdout);
}
