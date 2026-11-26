#include <bits/stdc++.h>
using namespace std;
static int ceilDist(long long dr,long long dc){
    long long s=dr*dr+dc*dc;
    long long r=(long long)sqrtl((long double)s);
    while(r*r<s)r++;
    while(r>0&&(r-1)*(r-1)>=s)r--;
    return (int)r;
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
    vector<vector<pair<int,int>>> need(C);
    vector<long long> cnt(C);
    for(int i=0;i<C;i++){
        int L;
        scanf("%d %d",&orr[i],&oc[i]);
        scanf("%d",&L);
        cnt[i]=L;
        map<int,int> m;
        for(int j=0;j<L;j++){int p;scanf("%d",&p);m[p]++;}
        for(auto&kv:m)need[i].push_back(kv);
        sort(need[i].begin(),need[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first])return wt[a.first]>wt[b.first];
            return a.first<b.first;});
    }
    vector<int> ord(C);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return cnt[a]<cnt[b];});
    vector<int> dr(D,wr[0]),dc(D,wc[0]);
    typedef pair<int,int> pii;
    priority_queue<pii,vector<pii>,greater<pii>> pq;
    for(int i=0;i<D;i++)pq.push({0,i});
    vector<string> out;
    for(int o:ord){
        auto&nd=need[o];
        for(;;){
            bool any=false;
            for(auto&x:nd)if(x.second>0)any=true;
            if(!any)break;
            if(pq.empty())break;
            auto [tm,d]=pq.top();
            int cr=dr[d],cc=dc[d];
            int t=tm;
            int cap=ML;
            vector<string> cmds;
            vector<array<int,3>> loads; // w,p,n
            vector<pair<int,int>> deliv(nd.size(),{0,0});
            vector<int> got(nd.size(),0);
            for(;;){
                int bi=-1,bw=-1,bd=INT_MAX;
                for(size_t k=0;k<nd.size();k++){
                    int rem=nd[k].second-got[k];
                    int p=nd[k].first;
                    if(rem<=0||wt[p]>cap)continue;
                    for(int w=0;w<W;w++){
                        if(st[w][p]<=0)continue;
                        int dd=ceilDist(wr[w]-cr,wc[w]-cc);
                        if(dd<bd){bd=dd;bi=k;bw=w;}
                    }
                    if(bi>=0)break;
                }
                if(bi<0)break;
                int w=bw;
                bool first=true;
                for(size_t k=0;k<nd.size();k++){
                    int p=nd[k].first;
                    int rem=nd[k].second-got[k];
                    if(rem<=0||wt[p]>cap||st[w][p]<=0)continue;
                    int n=min({rem,st[w][p],cap/wt[p]});
                    if(n<=0)continue;
                    st[w][p]-=n;cap-=n*wt[p];got[k]+=n;
                    loads.push_back({w,p,n});
                    if(first){t+=bd;first=false;}
                    t+=1;
                    cmds.push_back(to_string(d)+" L "+to_string(w)+" "+to_string(p)+" "+to_string(n));
                }
                cr=wr[w];cc=wc[w];
            }
            bool ok=!loads.empty();
            if(ok){
                t+=ceilDist(orr[o]-cr,oc[o]-cc);
                bool f=true;
                for(size_t k=0;k<nd.size();k++){
                    if(got[k]<=0)continue;
                    if(!f)t+=0;
                    t+=1;f=false;
                    cmds.push_back(to_string(d)+" D "+to_string(o)+" "+to_string(nd[k].first)+" "+to_string(got[k]));
                }
                if(t>T)ok=false;
            }
            if(!ok){
                for(auto&l:loads)st[l[0]][l[1]]+=l[2];
                // abandon this order
                for(auto&x:nd)x.second=0;
                break;
            }
            pq.pop();
            for(size_t k=0;k<nd.size();k++)nd[k].second-=got[k];
            dr[d]=orr[o];dc[d]=oc[o];
            pq.push({t,d});
            for(auto&s:cmds)out.push_back(s);
        }
    }
    printf("%d\n",(int)out.size());
    string buf;
    for(auto&s:out){buf+=s;buf+='\n';}
    fputs(buf.c_str(),stdout);
}
