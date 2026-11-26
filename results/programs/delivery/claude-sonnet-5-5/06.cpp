#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int cdist(ll dr, ll dc){
    ll x=dr*dr+dc*dc;
    ll d=(ll)ceil(sqrt((double)x));
    while(d>0&&(d-1)*(d-1)>=x)d--;
    while(d*d<x)d++;
    return (int)d;
}
int main(){
    int R,Cc,D,T,ML;
    scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&ML);
    int P;scanf("%d",&P);
    vector<int> wt(P);
    for(auto&x:wt)scanf("%d",&x);
    int W;scanf("%d",&W);
    vector<int> wr(W),wc(W);
    vector<int> stock((size_t)W*P);
    for(int i=0;i<W;i++){
        scanf("%d %d",&wr[i],&wc[i]);
        for(int j=0;j<P;j++)scanf("%d",&stock[(size_t)i*P+j]);
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
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
    for(int i=0;i<D;i++)pq.push({0,i});
    vector<string> cmds;
    const int K=40;
    vector<int> idx(W);
    for(int oi:ord){
        map<int,int> m;
        for(int x:items[oi])m[x]++;
        vector<pair<int,int>> pend(m.begin(),m.end());
        sort(pend.begin(),pend.end(),[&](const pair<int,int>&a,const pair<int,int>&b){
            if(wt[a.first]!=wt[b.first])return wt[a.first]>wt[b.first];
            return a.first<b.first;});
        int orow=orr[oi],ocol=oc[oi];
        vector<int> cand;
        iota(idx.begin(),idx.end(),0);
        if(W>K){
            auto dd=[&](int w){ll a=wr[w]-orow,b=wc[w]-ocol;return a*a+b*b;};
            nth_element(idx.begin(),idx.begin()+K,idx.end(),[&](int a,int b){
                ll x=dd(a),y=dd(b);if(x!=y)return x<y;return a<b;});
            cand.assign(idx.begin(),idx.begin()+K);
        } else cand=idx;
        auto remaining=[&](){for(auto&p:pend)if(p.second>0)return true;return false;};
        while(remaining()){
            auto top=pq.top();pq.pop();
            int t=top.first,id=top.second;
            int cr=dr[id],cc=dc[id];
            int cap=ML;
            int time=t;
            struct Ld{int w,pi,n;};
            vector<Ld> loads;
            vector<string> local;
            vector<int> loadedPer(pend.size(),0);
            while(cap>0){
                double best=0;int bw=-1;
                auto eval=[&](const vector<int>&cs){
                    for(int w:cs){
                        int c=cap;ll fill=0;
                        for(size_t k=0;k<pend.size()&&c>0;k++){
                            if(pend[k].second<=0)continue;
                            int s=stock[(size_t)w*P+pend[k].first];
                            int n=min({pend[k].second,s,c/wt[pend[k].first]});
                            if(n>0){fill+=(ll)n*wt[pend[k].first];c-=n*wt[pend[k].first];}
                        }
                        if(fill<=0)continue;
                        double d=cdist(cr-wr[w],cc-wc[w])+cdist(wr[w]-orow,wc[w]-ocol)+1;
                        double sc=fill/d;
                        if(sc>best){best=sc;bw=w;}
                    }
                };
                eval(cand);
                if(bw<0&&W>K&&loads.empty()){
                    eval(idx);
                }
                if(bw<0)break;
                int w=bw;
                time+=cdist(cr-wr[w],cc-wc[w])+1;
                cr=wr[w];cc=wc[w];
                for(size_t k=0;k<pend.size()&&cap>0;k++){
                    if(pend[k].second<=0)continue;
                    int &s=stock[(size_t)w*P+pend[k].first];
                    int n=min({pend[k].second,s,cap/wt[pend[k].first]});
                    if(n>0){
                        s-=n;pend[k].second-=n;cap-=n*wt[pend[k].first];
                        loadedPer[k]+=n;
                        loads.push_back({w,(int)k,n});
                        local.push_back(to_string(id)+" L "+to_string(w)+" "+to_string(pend[k].first)+" "+to_string(n));
                    }
                }
            }
            bool ok=!loads.empty();
            if(ok){
                time+=cdist(cr-orow,cc-ocol);
                cr=orow;cc=ocol;
                for(size_t k=0;k<pend.size();k++)if(loadedPer[k]>0){
                    time+=1;
                    local.push_back(to_string(id)+" D "+to_string(oi)+" "+to_string(pend[k].first)+" "+to_string(loadedPer[k]));
                }
                if(time>T)ok=false;
            }
            if(!ok){
                for(auto&l:loads){
                    stock[(size_t)l.w*P+pend[l.pi].first]+=l.n;
                    pend[l.pi].second+=l.n;
                }
                pq.push({t,id});
                break;
            }
            for(auto&s:local)cmds.push_back(s);
            dr[id]=cr;dc[id]=cc;
            pq.push({time,id});
        }
    }
    string out=to_string(cmds.size())+"\n";
    for(auto&s:cmds){out+=s;out+='\n';}
    fputs(out.c_str(),stdout);
}
