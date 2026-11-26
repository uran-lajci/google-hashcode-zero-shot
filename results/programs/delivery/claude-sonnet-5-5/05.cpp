#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int cdist(ll r1,ll c1,ll r2,ll c2){
    ll d2=(r1-r2)*(r1-r2)+(c1-c2)*(c1-c2);
    ll s=(ll)sqrt((double)d2);
    while(s*s<d2)s++;
    while(s>0&&(s-1)*(s-1)>=d2)s--;
    return (int)s;
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
    vector<int> orr(C),oc(C);
    vector<vector<int>> items(C);
    vector<ll> tw(C,0);
    for(int i=0;i<C;i++){
        int L;
        scanf("%d %d",&orr[i],&oc[i]);
        scanf("%d",&L);
        items[i].resize(L);
        for(auto&x:items[i]){scanf("%d",&x);tw[i]+=wt[x];}
    }
    vector<int> ord(C);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return tw[a]<tw[b];});
    vector<int> dr(D,wr[0]),dc(D,wc[0]);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<D;i++)pq.push({0,i});
    string out;
    ll Q=0;
    char buf[100];
    for(int o:ord){
        // count per type
        map<int,int> cnt;
        for(int x:items[o])cnt[x]++;
        // per warehouse: list of (type,count)
        map<int,vector<pair<int,int>>> byw;
        for(auto&kv:cnt){
            int ty=kv.first,need=kv.second;
            while(need>0){
                int best=-1;int bd=INT_MAX;
                for(int w=0;w<W;w++){
                    if(st[w][ty]>0){
                        int d=cdist(wr[w],wc[w],orr[o],oc[o]);
                        if(d<bd){bd=d;best=w;}
                    }
                }
                if(best<0)break;
                int take=min(need,st[best][ty]);
                st[best][ty]-=take;need-=take;
                byw[best].push_back({ty,take});
            }
        }
        for(auto&kv:byw){
            int w=kv.first;
            // expand into units, split into trips
            vector<pair<int,int>> cur; // type,count
            ll load=0;
            vector<vector<pair<int,int>>> trips;
            for(auto&tc:kv.second){
                int ty=tc.first,c=tc.second;
                while(c>0){
                    int can=(ML-load)/wt[ty];
                    if(can==0){trips.push_back(cur);cur.clear();load=0;continue;}
                    int t=min(can,c);
                    cur.push_back({ty,t});
                    load+=(ll)t*wt[ty];c-=t;
                }
            }
            if(!cur.empty())trips.push_back(cur);
            for(auto&trip:trips){
                auto pr=pq.top();pq.pop();
                int d=pr.second;
                ll t=pr.first;
                t+=cdist(dr[d],dc[d],wr[w],wc[w])+1;
                t+=(ll)(trip.size()-1);
                t+=cdist(wr[w],wc[w],orr[o],oc[o]);
                t+=(ll)trip.size();
                if(t>T){pq.push(pr);continue;}
                for(auto&tc:trip){
                    sprintf(buf,"%d L %d %d %d\n",d,w,tc.first,tc.second);
                    out+=buf;Q++;
                }
                for(auto&tc:trip){
                    sprintf(buf,"%d D %d %d %d\n",d,o,tc.first,tc.second);
                    out+=buf;Q++;
                }
                dr[d]=orr[o];dc[d]=oc[o];
                pq.push({t,d});
            }
        }
    }
    printf("%lld\n%s",Q,out.c_str());
}
