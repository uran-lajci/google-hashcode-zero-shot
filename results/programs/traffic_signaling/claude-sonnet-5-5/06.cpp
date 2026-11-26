#include <bits/stdc++.h>
using namespace std;
int D,I,S,V,F;
vector<int> sB,sE,sL;
vector<vector<int>> path;
vector<vector<int>> order_, dur;
vector<int> inter; // intersections used
vector<int> cyc;
vector<vector<int>> lut; // per intersection: green street at t mod cyc (-1 if none)
void build(int i){
    int c=0;for(int d:dur[i])c+=d;
    cyc[i]=c;
    lut[i].assign(c,-1);
    int p=0;
    for(size_t k=0;k<order_[i].size();k++)for(int j=0;j<dur[i][k];j++)lut[i][p++]=order_[i][k];
}
long long simulate(){
    vector<deque<int>> q(S);
    vector<int> pos(V,0);
    vector<vector<pair<int,int>>> arr(D+2);
    for(int c=0;c<V;c++)q[path[c][0]].push_back(c);
    long long score=0;
    for(int t=0;t<=D;t++){
        for(auto&a:arr[t])q[a.first].push_back(a.second);
        for(int i:inter){
            if(cyc[i]==0)continue;
            int s=lut[i][t%cyc[i]];
            if(s<0||q[s].empty())continue;
            int c=q[s].front();q[s].pop_front();
            pos[c]++;
            int ns=path[c][pos[c]];
            int at=t+sL[ns];
            if(pos[c]==(int)path[c].size()-1){
                if(at<=D)score+=F+(D-at);
            } else if(at<=D) arr[at].push_back({ns,c});
        }
    }
    return score;
}
int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    sB.resize(S);sE.resize(S);sL.resize(S);
    unordered_map<string,int> id;
    vector<string> names(S);
    char buf[100];
    for(int i=0;i<S;i++){
        scanf("%d %d %s %d",&sB[i],&sE[i],buf,&sL[i]);
        names[i]=buf;id[names[i]]=i;
    }
    path.resize(V);
    for(int c=0;c<V;c++){
        int p;scanf("%d",&p);
        path[c].resize(p);
        for(int j=0;j<p;j++){scanf("%s",buf);path[c][j]=id[buf];}
    }
    vector<int> cnt(S,0),first(S,0);
    for(int c=0;c<V;c++){
        for(size_t j=0;j+1<path[c].size();j++)cnt[path[c][j]]++;
        first[path[c][0]]++;
    }
    order_.assign(I,{});dur.assign(I,{});cyc.assign(I,0);lut.assign(I,{});
    for(int s=0;s<S;s++)if(cnt[s]>0)order_[sE[s]].push_back(s);
    for(int i=0;i<I;i++){
        if(order_[i].empty())continue;
        sort(order_[i].begin(),order_[i].end(),[&](int a,int b){
            if(first[a]!=first[b])return first[a]>first[b];
            return a<b;});
        dur[i].assign(order_[i].size(),1);
        inter.push_back(i);
        build(i);
    }
    long long best=simulate();
    mt19937 rng(12345);
    // fixed iteration budget based on measured cost
    auto st=chrono::steady_clock::now();
    simulate();
    double one=chrono::duration<double>(chrono::steady_clock::now()-st).count();
    if(one<1e-5)one=1e-5;
    long long iters=(long long)(7.0/one);
    if(iters>20000)iters=20000;
    for(long long it=0;it<iters&&!inter.empty();it++){
        int i=inter[rng()%inter.size()];
        int n=order_[i].size();
        int type=rng()%2;
        auto oo=order_[i];auto od=dur[i];
        if(type==0){
            if(n<2)continue;
            int a=rng()%n,b=rng()%n;
            if(a==b)continue;
            swap(order_[i][a],order_[i][b]);
            swap(dur[i][a],dur[i][b]);
        } else {
            int a=rng()%n;
            if(rng()%2)dur[i][a]++;
            else{if(dur[i][a]<=1)continue;dur[i][a]--;}
            if(dur[i][a]>D)continue;
        }
        build(i);
        long long sc=simulate();
        if(sc>=best)best=sc;
        else{order_[i]=oo;dur[i]=od;build(i);}
    }
    printf("%d\n",(int)inter.size());
    for(int i:inter){
        printf("%d\n%d\n",i,(int)order_[i].size());
        for(size_t k=0;k<order_[i].size();k++)
            printf("%s %d\n",names[order_[i][k]].c_str(),dur[i][k]);
    }
}
