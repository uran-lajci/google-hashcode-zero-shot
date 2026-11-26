#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> sB,sE,sL;
vector<vector<int>> path;
vector<vector<int>> arr;
vector<vector<int>> q;
vector<int> qh;
vector<char> inAct;
vector<int> startOf,durOf,cyc; // per street / per intersection
long long ops=0;

long long sim(){
    for(int t=0;t<=D;t++)arr[t].clear();
    for(int s=0;s<S;s++){q[s].clear();qh[s]=0;inAct[s]=0;}
    vector<int> pos(V,0);
    vector<int> act,nact;
    for(int c=0;c<V;c++){
        int s=path[c][0];
        q[s].push_back(c);
        if(!inAct[s]){inAct[s]=1;act.push_back(s);}
    }
    long long score=0;
    for(int t=0;t<=D;t++){
        ops++;
        for(int c:arr[t]){
            ops++;
            int s=path[c][pos[c]];
            if(pos[c]==(int)path[c].size()-1){score+=F+D-t;continue;}
            q[s].push_back(c);
            if(!inAct[s]){inAct[s]=1;act.push_back(s);}
        }
        if(t==D)break;
        nact.clear();
        for(int s:act){
            ops++;
            bool green=false;
            if(durOf[s]>0){
                int cy=cyc[sE[s]];
                int m=cy==durOf[s]?0:t%cy;
                green=(m>=startOf[s]&&m<startOf[s]+durOf[s]);
            }
            if(green&&qh[s]<(int)q[s].size()){
                int c=q[s][qh[s]++];
                pos[c]++;
                int ns=path[c][pos[c]];
                int at=t+sL[ns];
                if(at<=D)arr[at].push_back(c);
            }
            if(qh[s]<(int)q[s].size())nact.push_back(s);
            else inAct[s]=0;
        }
        act.swap(nact);
    }
    return score;
}

int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    sB.resize(S);sE.resize(S);sL.resize(S);
    unordered_map<string,int> id;
    vector<string> names(S);
    char buf[64];
    for(int i=0;i<S;i++){
        scanf("%d %d %s %d",&sB[i],&sE[i],buf,&sL[i]);
        names[i]=buf;id[names[i]]=i;
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
    arr.assign(D+1,{});
    q.assign(S,{});qh.assign(S,0);inAct.assign(S,0);
    startOf.assign(S,0);durOf.assign(S,0);cyc.assign(I,1);
    vector<vector<pair<int,int>>> sched(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)sched[sE[s]].push_back({s,1});
    for(int i=0;i<I;i++){
        stable_sort(sched[i].begin(),sched[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){return cnt[a.first]>cnt[b.first];});
    }
    auto apply=[&](int i){
        int t=0;
        for(auto&p:sched[i]){startOf[p.first]=t;durOf[p.first]=p.second;t+=p.second;}
        cyc[i]=max(t,1);
    };
    vector<int> multi;
    for(int i=0;i<I;i++){apply(i);if(sched[i].size()>=2)multi.push_back(i);}
    long long best=sim();
    mt19937_64 rng(12345);
    const long long BUDGET=1200000000LL;
    while(!multi.empty()&&ops<BUDGET){
        int i=multi[rng()%multi.size()];
        auto old=sched[i];
        int n=old.size();
        int type=rng()%3;
        if(type==0){
            int a=rng()%n,b=rng()%n;
            if(a==b)continue;
            if(type==0&&n>=2){b=(a+1)%n;}
            swap(sched[i][a],sched[i][b]);
        }else{
            int a=rng()%n;
            int d=(type==1)?1:-1;
            int nd=sched[i][a].second+d;
            if(nd<1||nd>8)continue;
            sched[i][a].second=nd;
        }
        apply(i);
        long long sc=sim();
        if(sc>=best)best=sc;
        else{sched[i]=old;apply(i);}
    }
    int A=0;
    for(int i=0;i<I;i++)if(!sched[i].empty())A++;
    printf("%d\n",A);
    for(int i=0;i<I;i++)if(!sched[i].empty()){
        printf("%d\n%d\n",i,(int)sched[i].size());
        for(auto&p:sched[i])printf("%s %d\n",names[p.first].c_str(),p.second);
    }
}
