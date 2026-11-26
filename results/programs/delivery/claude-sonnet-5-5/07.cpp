#include <bits/stdc++.h>
using namespace std;
static int cd(long long ar,long long ac,long long br,long long bc){
    long long d2=(ar-br)*(ar-br)+(ac-bc)*(ac-bc);
    long long s=(long long)sqrt((double)d2);
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
    vector<uint16_t> st((size_t)W*P);
    for(int i=0;i<W;i++){
        scanf("%d %d",&wr[i],&wc[i]);
        for(int p=0;p<P;p++){int x;scanf("%d",&x);st[(size_t)i*P+p]=x;}
    }
    int C;scanf("%d",&C);
    vector<int> orr(C),oc(C);
    vector<vector<pair<int,int>>> rem(C);
    vector<long long> tw(C,0);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&oc[i]);
        int L;scanf("%d",&L);
        map<int,int> m;
        for(int j=0;j<L;j++){int p;scanf("%d",&p);m[p]++;tw[i]+=wt[p];}
        for(auto&kv:m)rem[i].push_back(kv);
    }
    vector<int> ord(C);
    iota(ord.begin(),ord.end(),0);
    vector<int> d0(C);
    for(int i=0;i<C;i++)d0[i]=cd(orr[i],oc[i],wr[0],wc[0]);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        if(tw[a]!=tw[b])return tw[a]<tw[b];
        if(d0[a]!=d0[b])return d0[a]<d0[b];
        return a<b;});
    vector<int> pr(D,wr[0]),pc(D,wc[0]);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
    for(int i=0;i<D;i++)pq.push({0,i});
    vector<string> out;
    size_t cur=0;
    auto t0=chrono::steady_clock::now();
    int iter=0;
    while(!pq.empty()){
        auto [t,d]=pq.top();pq.pop();
        if(t>=T)continue;
        if((++iter&63)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>8.5)break;
        }
        while(cur<ord.size()&&rem[ord[cur]].empty())cur++;
        if(cur>=ord.size())break;
        int o=ord[cur];
        int p0=rem[o][0].first;
        int best=-1;long long bc=LLONG_MAX;
        for(int w=0;w<W;w++){
            if(st[(size_t)w*P+p0]==0)continue;
            long long c=(long long)cd(pr[d],pc[d],wr[w],wc[w])+cd(wr[w],wc[w],orr[o],oc[o]);
            if(c<bc){bc=c;best=w;}
        }
        if(best<0){continue;}
        int w=best;
        int cap=ML;
        vector<array<int,2>> take;
        for(auto&e:rem[o]){
            int k=min({e.second,(int)st[(size_t)w*P+e.first],cap/wt[e.first]});
            if(k>0){take.push_back({e.first,k});cap-=k*wt[e.first];}
        }
        int n=take.size();
        int d1=cd(pr[d],pc[d],wr[w],wc[w]);
        int d2=cd(wr[w],wc[w],orr[o],oc[o]);
        long long end=(long long)t+d1+1+(n-1)+d2+1+(n-1);
        if(end>T)continue;
        for(auto&x:take){
            out.push_back(to_string(d)+" L "+to_string(w)+" "+to_string(x[0])+" "+to_string(x[1]));
            st[(size_t)w*P+x[0]]-=x[1];
        }
        for(auto&x:take){
            out.push_back(to_string(d)+" D "+to_string(o)+" "+to_string(x[0])+" "+to_string(x[1]));
            for(auto&e:rem[o])if(e.first==x[0])e.second-=x[1];
        }
        auto&v=rem[o];
        v.erase(remove_if(v.begin(),v.end(),[](const pair<int,int>&e){return e.second==0;}),v.end());
        pr[d]=orr[o];pc[d]=oc[o];
        pq.push({(int)end,d});
    }
    printf("%d\n",(int)out.size());
    string s;
    for(auto&l:out){s+=l;s+='\n';}
    fputs(s.c_str(),stdout);
}
