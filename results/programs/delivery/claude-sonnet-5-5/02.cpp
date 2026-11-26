#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int cd(ll r1,ll c1,ll r2,ll c2){
    ll d2=(r1-r2)*(r1-r2)+(c1-c2)*(c1-c2);
    ll s=(ll)sqrtl((long double)d2);
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
    vector<vector<int>> stock(W,vector<int>(P));
    vector<vector<int>> has(P);
    for(int i=0;i<W;i++){
        scanf("%d %d",&wr[i],&wc[i]);
        for(int p=0;p<P;p++){
            scanf("%d",&stock[i][p]);
            if(stock[i][p]>0)has[p].push_back(i);
        }
    }
    int C;scanf("%d",&C);
    vector<int> orr(C),oc(C);
    vector<vector<int>> items(C);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&oc[i]);
        int L;scanf("%d",&L);
        items[i].resize(L);
        for(auto&x:items[i])scanf("%d",&x);
    }
    vector<int> ord(C);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return items[a].size()<items[b].size();
    });
    vector<int> dr(D,wr[0]),dc(D,wc[0]);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
    for(int d=0;d<D;d++)pq.push({0,d});
    vector<string> out;
    bool stop=false;
    for(int oi=0;oi<C&&!stop;oi++){
        int o=ord[oi];
        vector<int> rem=items[o];
        sort(rem.begin(),rem.end(),[&](int a,int b){return wt[a]>wt[b];});
        while(!rem.empty()){
            if(pq.empty()){stop=true;break;}
            auto [tm,d]=pq.top();pq.pop();
            vector<int> trip,rest;
            int w=0;
            for(int p:rem){
                if(w+wt[p]<=ML){w+=wt[p];trip.push_back(p);}
                else rest.push_back(p);
            }
            int cr=dr[d],cc=dc[d];
            int t=tm;
            vector<array<int,3>> loads; // wh,prod,count
            vector<pair<int,int>> undo;
            bool ok=true;
            for(int p:trip){
                int best=-1,bd=INT_MAX;
                for(int wi:has[p]){
                    if(stock[wi][p]<=0)continue;
                    ll dd=(ll)(wr[wi]-cr)*(wr[wi]-cr)+(ll)(wc[wi]-cc)*(wc[wi]-cc);
                    int v=dd>INT_MAX?INT_MAX:(int)dd;
                    if(v<bd){bd=v;best=wi;}
                }
                if(best<0){ok=false;break;}
                stock[best][p]--;
                undo.push_back({best,p});
                if(!loads.empty()&&loads.back()[0]==best&&loads.back()[1]==p){
                    loads.back()[2]++;
                } else {
                    // try merging with earlier same wh/prod not needed
                    t+=cd(cr,cc,wr[best],wc[best])+1;
                    cr=wr[best];cc=wc[best];
                    loads.push_back({best,p,1});
                }
            }
            if(ok){
                map<int,int> cnt;
                for(int p:trip)cnt[p]++;
                for(auto&kv:cnt){
                    t+=cd(cr,cc,orr[o],oc[o])+1;
                    cr=orr[o];cc=oc[o];
                }
                if(t>T)ok=false;
                if(ok){
                    for(auto&l:loads)
                        out.push_back(to_string(d)+" L "+to_string(l[0])+" "+to_string(l[1])+" "+to_string(l[2]));
                    for(auto&kv:cnt)
                        out.push_back(to_string(d)+" D "+to_string(o)+" "+to_string(kv.first)+" "+to_string(kv.second));
                    dr[d]=cr;dc[d]=cc;
                    pq.push({t,d});
                    rem=rest;
                    continue;
                }
            }
            for(auto&u:undo)stock[u.first][u.second]++;
            // drone dropped
            // loop continues with another drone
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&s:out)printf("%s\n",s.c_str());
}
