#include <bits/stdc++.h>
using namespace std;
int D,I,S,V,F;
vector<int> sE,sL;vector<string> sN;
vector<vector<int>> path;
vector<vector<int>> ord,dur;
vector<int> cyc;
vector<vector<int>> cand;
long long sim(){
    vector<vector<pair<int,int>>> sched(I);
    // per street: offset start and dur
    vector<int> st(S,-1),du(S,0);
    for(int i=0;i<I;i++){int o=0;for(size_t k=0;k<ord[i].size();k++){st[ord[i][k]]=o;du[ord[i][k]]=dur[i][k];o+=dur[i][k];}cyc[i]=o;}
    vector<deque<int>> q(S);
    vector<int> pos(V,0);
    vector<vector<int>> bucket(D+2);
    for(int c=0;c<V;c++)q[path[c][0]].push_back(c);
    long long score=0;
    vector<int> active;vector<char> inA(S,0);
    for(int s=0;s<S;s++)if(!q[s].empty()){active.push_back(s);inA[s]=1;}
    for(int t=0;t<=D;t++){
        for(int c:bucket[t]){int s=path[c][pos[c]];q[s].push_back(c);if(!inA[s]){inA[s]=1;active.push_back(s);}}
        bucket[t].clear();
        vector<int> na;
        for(int s:active){
            if(q[s].empty()){inA[s]=0;continue;}
            na.push_back(s);
            if(st[s]<0)continue;
            int e=sE[s];int m=t%cyc[e];
            if(m>=st[s]&&m<st[s]+du[s]){
                int c=q[s].front();q[s].pop_front();
                pos[c]++;int ns=path[c][pos[c]];int at=t+sL[ns];
                if(at>D)continue;
                if(pos[c]==(int)path[c].size()-1){score+=F+(D-at);}
                else bucket[at].push_back(c);
            }
        }
        active.swap(na);
    }
    return score;
}
int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    sE.resize(S);sL.resize(S);sN.resize(S);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<S;i++){int b;scanf("%d %d %s %d",&b,&sE[i],buf,&sL[i]);sN[i]=buf;id[sN[i]]=i;}
    path.resize(V);
    vector<int> cnt(S,0);
    for(int c=0;c<V;c++){int p;scanf("%d",&p);path[c].resize(p);for(int j=0;j<p;j++){scanf("%s",buf);path[c][j]=id[buf];if(j<p-1)cnt[path[c][j]]++;}}
    ord.assign(I,{});dur.assign(I,{});cyc.assign(I,1);
    vector<vector<int>> in(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)in[sE[s]].push_back(s);
    for(int i=0;i<I;i++){
        sort(in[i].begin(),in[i].end(),[&](int a,int b){return cnt[a]>cnt[b];});
        ord[i]=in[i];dur[i].assign(in[i].size(),1);
    }
    vector<int> act;
    for(int i=0;i<I;i++)if(!ord[i].empty())act.push_back(i);
    long long best=sim();
    auto start=chrono::steady_clock::now();
    mt19937 rng(12345);
    while(!act.empty()){
        double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
        if(el>7.0)break;
        int i=act[rng()%act.size()];
        int n=ord[i].size();
        auto so=ord[i];auto sd=dur[i];
        int r=rng()%3;
        if(r==0&&n>1){int a=rng()%n,b=rng()%n;swap(ord[i][a],ord[i][b]);swap(dur[i][a],dur[i][b]);}
        else if(r==1){int a=rng()%n;dur[i][a]=1+rng()%3;}
        else if(n>1){int a=rng()%n;int s=ord[i][a];int d=dur[i][a];ord[i].erase(ord[i].begin()+a);dur[i].erase(dur[i].begin()+a);int b=rng()%n;ord[i].insert(ord[i].begin()+b,s);dur[i].insert(dur[i].begin()+b,d);}
        long long sc=sim();
        if(sc>=best)best=sc;else{ord[i]=so;dur[i]=sd;}
    }
    printf("%d\n",(int)act.size());
    for(int i:act){
        printf("%d\n%d\n",i,(int)ord[i].size());
        for(size_t k=0;k<ord[i].size();k++)printf("%s %d\n",sN[ord[i][k]].c_str(),dur[i][k]);
    }
}
