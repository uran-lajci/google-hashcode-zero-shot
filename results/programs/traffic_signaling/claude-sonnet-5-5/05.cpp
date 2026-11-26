#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> sB,sE,sL;
vector<string> nm;
vector<vector<int>> path;
vector<int> cyc,offs,dur;
long long ops=0;

vector<int> head,nxtc,pos_,ftime;
vector<vector<int>> q;
vector<int> qh;
vector<char> inQ;

long long sim(){
    ops+=D+1;
    for(int t=0;t<=D;t++)head[t]=-1;
    for(int s=0;s<S;s++){q[s].clear();qh[s]=0;inQ[s]=0;}
    vector<int> act;
    for(int c=0;c<V;c++){
        pos_[c]=0;
        int s=path[c][0];
        q[s].push_back(c);
        if(!inQ[s]){inQ[s]=1;act.push_back(s);}
    }
    long long score=0;
    vector<int> na;
    for(int t=0;t<=D;t++){
        int c=head[t];
        while(c!=-1){
            int nx=nxtc[c];
            int s=path[c][pos_[c]];
            q[s].push_back(c);
            if(!inQ[s]){inQ[s]=1;act.push_back(s);}
            c=nx;
        }
        head[t]=-1;
        ops+=1+act.size();
        na.clear();
        for(int s:act){
            if(dur[s]>0){
                int m=t%cyc[s];
                if(m>=offs[s]&&m<offs[s]+dur[s]){
                    int car=q[s][qh[s]++];
                    pos_[car]++;
                    int ns=path[car][pos_[car]];
                    int at=t+sL[ns];
                    if(pos_[car]==(int)path[car].size()-1){
                        if(at<=D)score+=F+D-at;
                    } else if(at<=D){
                        nxtc[car]=head[at];head[at]=car;
                    }
                }
            }
            if(qh[s]<(int)q[s].size())na.push_back(s);
            else inQ[s]=0;
        }
        act.swap(na);
    }
    return score;
}

int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    sB.resize(S);sE.resize(S);sL.resize(S);nm.resize(S);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<S;i++){
        scanf("%d %d %s %d",&sB[i],&sE[i],buf,&sL[i]);
        nm[i]=buf;id[nm[i]]=i;
    }
    path.resize(V);
    vector<int> used(S,0),startc(S,0);
    for(int c=0;c<V;c++){
        int p;scanf("%d",&p);
        path[c].resize(p);
        for(int j=0;j<p;j++){scanf("%s",buf);path[c][j]=id[buf];}
        for(int j=0;j+1<p;j++)used[path[c][j]]++;
        startc[path[c][0]]++;
    }
    cyc.assign(S,1);offs.assign(S,0);dur.assign(S,0);
    head.assign(D+1,-1);nxtc.assign(V,-1);pos_.assign(V,0);
    q.assign(S,{});qh.assign(S,0);inQ.assign(S,0);
    vector<vector<pair<int,int>>> sch(I);
    for(int s=0;s<S;s++)if(used[s]>0&&startc[s]>0)sch[sE[s]].push_back({s,1});
    for(int s=0;s<S;s++)if(used[s]>0&&startc[s]==0)sch[sE[s]].push_back({s,1});
    auto apply=[&](int i){
        int C=0;
        for(auto&p:sch[i])C+=p.second;
        int o=0;
        for(auto&p:sch[i]){
            cyc[p.first]=C;offs[p.first]=o;dur[p.first]=p.second;o+=p.second;
        }
    };
    vector<int> act;
    for(int i=0;i<I;i++)if(!sch[i].empty()){apply(i);act.push_back(i);}
    long long cur=sim();
    mt19937 rng(12345);
    if(!act.empty()){
        while(ops<600000000LL){
            int i=act[rng()%act.size()];
            auto old=sch[i];
            int n=sch[i].size();
            int mv=rng()%3;
            if(n>=2&&mv==0){
                int a=rng()%n,b=rng()%n;
                if(a==b)continue;
                swap(sch[i][a],sch[i][b]);
            } else if(n>=2&&mv==1){
                int a=rng()%n;
                swap(sch[i][a],sch[i][(a+1)%n]);
            } else {
                int a=rng()%n;
                if(rng()&1)sch[i][a].second++;
                else if(sch[i][a].second>1)sch[i][a].second--;
                else continue;
                if(sch[i][a].second>D)sch[i][a].second=D;
            }
            apply(i);
            long long ns=sim();
            if(ns>=cur)cur=ns;
            else{sch[i]=old;apply(i);}
        }
    }
    printf("%d\n",(int)act.size());
    for(int i:act){
        printf("%d\n%d\n",i,(int)sch[i].size());
        for(auto&p:sch[i])printf("%s %d\n",nm[p.first].c_str(),p.second);
    }
}
