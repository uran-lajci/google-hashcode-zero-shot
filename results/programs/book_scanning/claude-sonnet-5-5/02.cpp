#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    scanf("%d %d %d",&B,&L,&D);
    vector<int> s(B);
    for(auto&x:s)scanf("%d",&x);
    vector<vector<int>> bk(L);
    vector<int> T(L),M(L);
    for(int j=0;j<L;j++){
        int n;
        scanf("%d %d %d",&n,&T[j],&M[j]);
        bk[j].resize(n);
        for(auto&x:bk[j])scanf("%d",&x);
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(s[a]!=s[b])return s[a]>s[b];
            return a<b;});
    }
    vector<char> used(B,0);
    vector<int> front(L,0);
    long long t=0;
    auto eval=[&](int j,long long tt,vector<int>*out)->double{
        long long days=D-tt-T[j];
        if(days<=0)return 0;
        long long cap=days*M[j];
        auto&v=bk[j];
        int&f=front[j];
        while(f<(int)v.size()&&used[v[f]])f++;
        long long sum=0,c=0;
        for(size_t i=f;i<v.size()&&c<cap;i++){
            if(used[v[i]])continue;
            sum+=s[v[i]];c++;
            if(out)out->push_back(v[i]);
        }
        return (double)sum/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double r=eval(j,0,nullptr);
        if(r>0)pq.push({r,-j});
    }
    auto start=chrono::steady_clock::now();
    vector<pair<int,vector<int>>> res;
    int iter=0;
    bool late=false;
    while(!pq.empty()){
        auto [r,nj]=pq.top();pq.pop();
        int j=-nj;
        if(!late&&(++iter&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
            if(el>7.0)late=true;
        }
        double nr=eval(j,t,nullptr);
        if(nr<=0)continue;
        if(!late&&!pq.empty()&&nr<pq.top().first){
            pq.push({nr,-j});
            continue;
        }
        vector<int> out;
        eval(j,t,&out);
        if(out.empty())continue;
        for(int b:out)used[b]=1;
        t+=T[j];
        res.push_back({j,out});
    }
    string o;
    o+=to_string(res.size())+"\n";
    for(auto&p:res){
        o+=to_string(p.first)+" "+to_string(p.second.size())+"\n";
        for(size_t i=0;i<p.second.size();i++){
            if(i)o+=' ';
            o+=to_string(p.second[i]);
        }
        o+="\n";
    }
    fputs(o.c_str(),stdout);
}
