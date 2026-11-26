#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    scanf("%d %d %d",&B,&L,&D);
    vector<int> s(B);
    for(auto&x:s)scanf("%d",&x);
    vector<int> N(L),T(L),M(L);
    vector<vector<int>> bk(L);
    for(int j=0;j<L;j++){
        scanf("%d %d %d",&N[j],&T[j],&M[j]);
        bk[j].resize(N[j]);
        for(auto&x:bk[j])scanf("%d",&x);
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(s[a]!=s[b])return s[a]>s[b];
            return a<b;});
    }
    vector<char> taken(B,0);
    long long t=0;
    auto eval=[&](int j,long long tt)->double{
        long long rem=D-tt-T[j];
        if(rem<=0)return -1;
        long long cap=rem*(long long)M[j];
        long long cnt=0,val=0;
        for(int b:bk[j]){
            if(cnt>=cap)break;
            if(taken[b])continue;
            cnt++;val+=s[b];
        }
        if(val==0)return -1;
        return (double)val/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=eval(j,0);
        if(v>=0)pq.push({v,j});
    }
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()){
        auto [v,j]=pq.top();pq.pop();
        double nv=eval(j,t);
        if(nv<0)continue;
        if(!pq.empty()&&nv<pq.top().first-1e-12){
            pq.push({nv,j});
            continue;
        }
        long long rem=D-t-T[j];
        long long cap=rem*(long long)M[j];
        vector<int> ch;
        for(int b:bk[j]){
            if((long long)ch.size()>=cap)break;
            if(taken[b])continue;
            taken[b]=1;
            ch.push_back(b);
        }
        if(ch.empty())continue;
        res.push_back({j,ch});
        t+=T[j];
        if(t>=D)break;
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
