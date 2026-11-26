#include <bits/stdc++.h>
using namespace std;
int D,I,S,V,F;
vector<int> Bq,Eq,Lq;
vector<vector<int>> paths;
vector<vector<int>> order_,dur;
vector<int> posIn; 
long long work=0;
long long simulate(){
    vector<int> cyc(I,0);
    vector<vector<int>> off(I);
    for(int i=0;i<I;i++){
        int c=0;
        for(size_t k=0;k<order_[i].size();k++){off[i].push_back(c);c+=dur[i][k];}
        cyc[i]=c;
    }
    vector<int> slotStart(S,-1),slotEnd(S,-1);
    for(int i=0;i<I;i++)for(size_t k=0;k<order_[i].size();k++){
        slotStart[order_[i][k]]=off[i][k];
        slotEnd[order_[i][k]]=off[i][k]+dur[i][k];
    }
    vector<deque<int>> q(S);
    vector<int> idx(V,0);
    vector<vector<pair<int,int>>> arr(D+2);
    vector<int> active;
    vector<char> inAct(S,0);
    for(int v=0;v<V;v++){
        int s=paths[v][0];
        q[s].push_back(v);
        if(!inAct[s]){inAct[s]=1;active.push_back(s);}
    }
    long long score=0;
    for(int t=0;t<=D;t++){
        for(auto&p:arr[t]){
            int v=p.first;
            if(idx[v]==(int)paths[v].size()-1){
                score+=F+(D-t);
            } else {
                int s=paths[v][idx[v]];
                q[s].push_back(v);
                if(!inAct[s]){inAct[s]=1;active.push_back(s);}
            }
        }
        if(t==D)break;
        work+=active.size()+1;
        vector<int> na;
        for(int s:active){
            if(!q[s].empty() && slotStart[s]>=0){
                int e=Eq[s];
                int m=t%cyc[e];
                if(m>=slotStart[s]&&m<slotEnd[s]){
                    int v=q[s].front();q[s].pop_front();
                    idx[v]++;
                    int ns=paths[v][idx[v]];
                    int at=t+Lq[ns];
                    if(at<=D)arr[at].push_back({v,0});
                }
            }
            if(!q[s].empty())na.push_back(s);
            else inAct[s]=0;
        }
        active=na;
    }
    return score;
}
int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    Bq.resize(S);Eq.resize(S);Lq.resize(S);
    vector<string> names(S);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<S;i++){
        scanf("%d %d %s %d",&Bq[i],&Eq[i],buf,&Lq[i]);
        names[i]=buf;id[names[i]]=i;
    }
    paths.resize(V);
    vector<int> cnt(S,0);
    for(int v=0;v<V;v++){
        int p;scanf("%d",&p);
        for(int j=0;j<p;j++){
            scanf("%s",buf);
            int s=id[buf];
            paths[v].push_back(s);
            if(j<p-1)cnt[s]++;
        }
    }
    order_.assign(I,{});dur.assign(I,{});
    for(int s=0;s<S;s++)if(cnt[s]>0)order_[Eq[s]].push_back(s);
    vector<int> used;
    for(int i=0;i<I;i++){
        sort(order_[i].begin(),order_[i].end(),[&](int a,int b){
            if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];
            return a<b;});
        dur[i].assign(order_[i].size(),1);
        if(!order_[i].empty())used.push_back(i);
    }
    long long best=simulate();
    mt19937 rng(12345);
    const long long budget=1200000000LL;
    while(work<budget && !used.empty()){
        int i=used[rng()%used.size()];
        int n=order_[i].size();
        int type=rng()%2;
        if(type==0){
            if(n<2)continue;
            int a=rng()%(n-1);
            swap(order_[i][a],order_[i][a+1]);
            swap(dur[i][a],dur[i][a+1]);
            long long sc=simulate();
            if(sc>=best)best=sc;
            else{
                swap(order_[i][a],order_[i][a+1]);
                swap(dur[i][a],dur[i][a+1]);
            }
        } else {
            int a=rng()%n;
            int old=dur[i][a];
            int nd=1+rng()%3;
            if(nd==old)continue;
            dur[i][a]=nd;
            long long sc=simulate();
            if(sc>=best)best=sc;
            else dur[i][a]=old;
        }
    }
    printf("%d\n",(int)used.size());
    for(int i:used){
        printf("%d\n%d\n",i,(int)order_[i].size());
        for(size_t k=0;k<order_[i].size();k++)
            printf("%s %d\n",names[order_[i][k]].c_str(),dur[i][k]);
    }
}
