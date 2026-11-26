#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> Bv,Ev,Lv;
vector<vector<int>> path;
vector<vector<pair<int,int>>> sch; // per intersection (street,T)
vector<int> cyc,off,dur,lastc;
vector<vector<pair<int,int>>> bucket;

void upd(int i){
    int c=0;
    for(auto&p:sch[i]){off[p.first]=c;dur[p.first]=p.second;c+=p.second;}
    for(auto&p:sch[i])cyc[p.first]=c;
}
long long evalAll(){
    for(int s=0;s<S;s++)lastc[s]=-1;
    for(auto&b:bucket)b.clear();
    for(int c=0;c<V;c++)bucket[0].push_back({c,0});
    long long score=0;
    for(int t=0;t<=D;t++){
        auto&b=bucket[t];
        for(size_t k=0;k<b.size();k++){
            int car=b[k].first,idx=b[k].second;
            int s=path[car][idx];
            if(dur[s]==0)continue;
            int a=max(t,lastc[s]+1);
            int C=cyc[s],r=a%C,o=off[s];
            int c;
            if(r>=o&&r<o+dur[s])c=a;
            else if(r<o)c=a+(o-r);
            else c=a+(C-r)+o;
            if(c>=D)continue;
            lastc[s]=c;
            int n=path[car][idx+1];
            int arr=c+Lv[n];
            if(arr>D)continue;
            if(idx+1==(int)path[car].size()-1)score+=F+D-arr;
            else bucket[arr].push_back({car,idx+1});
        }
    }
    return score;
}
int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    Bv.resize(S);Ev.resize(S);Lv.resize(S);
    unordered_map<string,int> id;
    vector<string> names(S);
    char buf[64];
    for(int i=0;i<S;i++){
        scanf("%d %d %s %d",&Bv[i],&Ev[i],buf,&Lv[i]);
        names[i]=buf;id[names[i]]=i;
    }
    path.resize(V);
    long long total=0;
    vector<int> cnt(S,0);
    for(int v=0;v<V;v++){
        int p;scanf("%d",&p);
        path[v].resize(p);
        for(int j=0;j<p;j++){scanf("%s",buf);path[v][j]=id[buf];if(j<p-1)cnt[path[v][j]]++;}
        total+=p;
    }
    sch.assign(I,{});
    cyc.assign(S,1);off.assign(S,0);dur.assign(S,0);lastc.assign(S,-1);
    bucket.assign(D+1,{});
    vector<vector<int>> inc(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)inc[Ev[s]].push_back(s);
    vector<int> active;
    for(int i=0;i<I;i++){
        if(inc[i].empty())continue;
        sort(inc[i].begin(),inc[i].end(),[&](int a,int b){if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];return a<b;});
        for(int s:inc[i])sch[i].push_back({s,1});
        upd(i);
        active.push_back(i);
    }
    long long cur=evalAll();
    if(!active.empty()){
        long long budget=300000000LL;
        long long per=total+D+S+10;
        long long iters=budget/per;
        if(iters>200000)iters=200000;
        mt19937 rng(12345);
        for(long long it=0;it<iters;it++){
            int i=active[rng()%active.size()];
            auto saved=sch[i];
            int n=sch[i].size();
            int mt=rng()%3;
            if(mt==0){
                if(n<2)continue;
                int a=rng()%n,b=rng()%n;
                if(a==b)continue;
                swap(sch[i][a],sch[i][b]);
            }else if(mt==1){
                sch[i][rng()%n].second++;
            }else{
                int a=rng()%n;
                if(sch[i][a].second<=1)continue;
                sch[i][a].second--;
            }
            upd(i);
            long long sc=evalAll();
            if(sc>=cur)cur=sc;
            else{sch[i]=saved;upd(i);}
        }
    }
    int A=0;
    for(int i=0;i<I;i++)if(!sch[i].empty())A++;
    printf("%d\n",A);
    for(int i=0;i<I;i++){
        if(sch[i].empty())continue;
        printf("%d\n%d\n",i,(int)sch[i].size());
        for(auto&p:sch[i])printf("%s %d\n",names[p.first].c_str(),p.second);
    }
}
