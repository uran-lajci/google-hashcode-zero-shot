#include <bits/stdc++.h>
using namespace std;
int D,I,S,V,F;
vector<int> Bv,Ev,Lv;
vector<string> nm;
vector<vector<int>> path;
vector<vector<int>> order_; // per intersection street list
vector<vector<int>> dur;    // parallel durations
vector<int> st,en,cyc;
long long work=0;
void build(int i){
    int c=0;
    for(size_t k=0;k<order_[i].size();k++){int s=order_[i][k];st[s]=c;c+=dur[i][k];en[s]=c;}
    for(int s:order_[i])cyc[s]=c;
}
long long sim(){
    vector<deque<int>> q(S);
    vector<int> pos(V,0);
    vector<vector<int>> ev(D+2);
    vector<int> act;vector<char> inact(S,0);
    for(int c=0;c<V;c++){int s=path[c][0];q[s].push_back(c);if(!inact[s]){inact[s]=1;act.push_back(s);}}
    long long score=0;
    for(int t=0;t<=D;t++){
        for(int c:ev[t]){
            int s=path[c][pos[c]];
            if(pos[c]==(int)path[c].size()-1){score+=F+(D-t);continue;}
            q[s].push_back(c);
            if(!inact[s]){inact[s]=1;act.push_back(s);}
        }
        if(t==D)break;
        vector<int> na;
        work+=act.size()+1;
        for(int s:act){
            if(!q[s].empty()&&cyc[s]>0){
                int m=t%cyc[s];
                if(m>=st[s]&&m<en[s]){
                    int c=q[s].front();q[s].pop_front();
                    pos[c]++;
                    int ns=path[c][pos[c]];
                    int at=t+Lv[ns];
                    if(at<=D)ev[at].push_back(c);
                }
            }
            if(q[s].empty())inact[s]=0;else na.push_back(s);
        }
        // streets newly activated during event phase were added to act before; rebuild
        act.swap(na);
        // events at t+? add later via push into act at their time
    }
    return score;
}
int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    Bv.resize(S);Ev.resize(S);Lv.resize(S);nm.resize(S);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<S;i++){scanf("%d %d %s %d",&Bv[i],&Ev[i],buf,&Lv[i]);nm[i]=buf;id[nm[i]]=i;}
    path.resize(V);
    vector<int> cnt(S,0);
    for(int c=0;c<V;c++){int p;scanf("%d",&p);path[c].resize(p);for(int j=0;j<p;j++){scanf("%s",buf);path[c][j]=id[buf];if(j<p-1)cnt[path[c][j]]++;}}
    order_.assign(I,{});dur.assign(I,{});
    st.assign(S,0);en.assign(S,0);cyc.assign(S,0);
    // order by first-arrival time
    vector<long long> first(S,LLONG_MAX);
    for(int c=0;c<V;c++){
        long long t=0;
        for(size_t j=0;j+1<path[c].size();j++){
            int s=path[c][j];
            first[s]=min(first[s],t);
            t+=Lv[path[c][j+1]];
        }
    }
    vector<int> ids;
    for(int s=0;s<S;s++)if(cnt[s])ids.push_back(s);
    sort(ids.begin(),ids.end(),[&](int a,int b){return first[a]<first[b]||(first[a]==first[b]&&a<b);});
    for(int s:ids){order_[Ev[s]].push_back(s);dur[Ev[s]].push_back(1);}
    vector<int> act;
    for(int i=0;i<I;i++)if(!order_[i].empty()){build(i);act.push_back(i);}
    long long best=sim();
    mt19937 rng(12345);
    long long budget=400000000LL;
    while(work<budget&&!act.empty()){
        int i=act[rng()%act.size()];
        int n=order_[i].size();
        int type=rng()%2;
        auto oo=order_[i];auto dd=dur[i];
        if(type==0){
            if(n<2)continue;
            int a=rng()%(n-1);
            swap(order_[i][a],order_[i][a+1]);swap(dur[i][a],dur[i][a+1]);
        }else{
            int a=rng()%n;
            if(rng()%2)dur[i][a]++;else{if(dur[i][a]<=1)continue;dur[i][a]--;}
        }
        build(i);
        long long sc=sim();
        if(sc>=best)best=sc;
        else{order_[i]=oo;dur[i]=dd;build(i);}
    }
    printf("%d\n",(int)act.size());
    for(int i:act){
        printf("%d\n%d\n",i,(int)order_[i].size());
        for(size_t k=0;k<order_[i].size();k++)printf("%s %d\n",nm[order_[i][k]].c_str(),dur[i][k]);
    }
}
