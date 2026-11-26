#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> sb,se,sl;
vector<vector<int>> path;
vector<vector<pair<int,int>>> sch; // per intersection: (street,dur)
long long opsCount;

long long simulate(){
    vector<vector<int>> cyc(I); // not used
    vector<int> cycleLen(I,0);
    vector<vector<int>> slot(I);
    for(int i=0;i<I;i++){
        int tot=0;
        for(auto&p:sch[i])tot+=p.second;
        cycleLen[i]=tot;
        slot[i].resize(tot);
        int k=0;
        for(auto&p:sch[i])for(int d=0;d<p.second;d++)slot[i][k++]=p.first;
    }
    vector<deque<int>> q(S);
    vector<int> pos(V,0);
    vector<vector<int>> bucket(D+1);
    for(int c=0;c<V;c++)q[path[c][0]].push_back(c);
    vector<int> active;
    for(int i=0;i<I;i++)if(cycleLen[i]>0)active.push_back(i);
    long long score=0;
    for(int t=0;t<=D;t++){
        for(int c:bucket[t])q[path[c][pos[c]]].push_back(c);
        opsCount+=bucket[t].size();
        for(int i:active){
            int s=slot[i][t%cycleLen[i]];
            if(q[s].empty())continue;
            int c=q[s].front();q[s].pop_front();
            pos[c]++;
            int ns=path[c][pos[c]];
            int at=t+sl[ns];
            if(at>D)continue;
            if(pos[c]==(int)path[c].size()-1){
                score+=F+D-at;
            } else bucket[at].push_back(c);
        }
        opsCount+=active.size();
    }
    return score;
}

int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    sb.resize(S);se.resize(S);sl.resize(S);
    vector<string> name(S);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<S;i++){
        scanf("%d %d %s %d",&sb[i],&se[i],buf,&sl[i]);
        name[i]=buf;id[name[i]]=i;
    }
    path.resize(V);
    vector<int> cnt(S,0);
    for(int c=0;c<V;c++){
        int p;scanf("%d",&p);
        path[c].resize(p);
        for(int j=0;j<p;j++){
            scanf("%s",buf);
            path[c][j]=id[buf];
            if(j<p-1)cnt[path[c][j]]++;
        }
    }
    sch.assign(I,{});
    vector<vector<int>> inc(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)inc[se[s]].push_back(s);
    for(int i=0;i<I;i++){
        sort(inc[i].begin(),inc[i].end(),[&](int a,int b){
            if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];
            return a<b;});
        for(int s:inc[i])sch[i].push_back({s,1});
    }
    opsCount=0;
    long long best=simulate();
    long long perSim=max(1LL,opsCount);
    long long iters=min(3000LL,max(0LL,300000000LL/perSim));
    vector<int> cand;
    for(int i=0;i<I;i++)if(sch[i].size()>=2)cand.push_back(i);
    mt19937 rng(12345);
    if(!cand.empty()){
        for(long long it=0;it<iters;it++){
            int i=cand[rng()%cand.size()];
            auto old=sch[i];
            int n=sch[i].size();
            if(rng()%2){
                int a=rng()%n,b=rng()%n;
                if(a==b)continue;
                swap(sch[i][a],sch[i][b]);
            } else {
                int a=rng()%n;
                if(rng()%2)sch[i][a].second++;
                else{
                    if(sch[i][a].second<=1)continue;
                    sch[i][a].second--;
                }
            }
            long long sc=simulate();
            if(sc>=best)best=sc;
            else sch[i]=old;
        }
    }
    int A=0;
    for(int i=0;i<I;i++)if(!sch[i].empty())A++;
    printf("%d\n",A);
    for(int i=0;i<I;i++){
        if(sch[i].empty())continue;
        printf("%d\n%d\n",i,(int)sch[i].size());
        for(auto&p:sch[i])printf("%s %d\n",name[p.first].c_str(),min(p.second,D));
    }
}
