#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    scanf("%d %d %d",&B,&L,&D);
    vector<int> S(B);
    for(auto&x:S)scanf("%d",&x);
    vector<int> T(L),M(L);
    vector<vector<int>> bk(L);
    for(int j=0;j<L;j++){
        int n;
        scanf("%d %d %d",&n,&T[j],&M[j]);
        bk[j].resize(n);
        for(auto&x:bk[j])scanf("%d",&x);
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];
            return a<b;});
    }
    vector<char> taken(B,0);
    vector<int> start(L,0);
    long long t=0;
    auto eval=[&](int j,long long t)->double{
        long long rem=D-t-T[j];
        if(rem<=0)return -1;
        long long cap=rem*(long long)M[j];
        size_t &st=*(new size_t(0));
        delete &st;
        long long cnt=0,sum=0;
        for(size_t i=start[j];i<bk[j].size()&&cnt<cap;i++){
            int b=bk[j][i];
            if(taken[b])continue;
            sum+=S[b];cnt++;
        }
        if(cnt==0)return -1;
        return (double)sum/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=eval(j,0);
        if(v>=0)pq.push({v,-j});
    }
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()){
        auto [v,nj]=pq.top();pq.pop();
        int j=-nj;
        // drop taken books from the front
        while(start[j]<(int)bk[j].size()&&taken[bk[j][start[j]]])start[j]++;
        double nv=eval(j,t);
        if(nv<0)continue;
        if(!pq.empty()&&nv<pq.top().first){
            pq.push({nv,-j});
            continue;
        }
        long long rem=D-t-T[j];
        long long cap=rem*(long long)M[j];
        vector<int> chosen;
        for(size_t i=start[j];i<bk[j].size()&&(long long)chosen.size()<cap;i++){
            int b=bk[j][i];
            if(taken[b])continue;
            taken[b]=1;
            chosen.push_back(b);
        }
        if(chosen.empty())continue;
        res.push_back({j,chosen});
        t+=T[j];
    }
    string out;
    out+=to_string(res.size())+"\n";
    for(auto&r:res){
        out+=to_string(r.first)+" "+to_string(r.second.size())+"\n";
        for(size_t i=0;i<r.second.size();i++){
            if(i)out+=' ';
            out+=to_string(r.second[i]);
        }
        out+="\n";
    }
    fputs(out.c_str(),stdout);
}
