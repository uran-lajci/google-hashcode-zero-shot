#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    scanf("%d %d %d",&B,&L,&D);
    vector<int> s(B);
    for(auto&x:s)scanf("%d",&x);
    vector<int> N(L),T(L),M(L),off(L+1,0);
    vector<int> flat;
    flat.reserve(1000000);
    for(int j=0;j<L;j++){
        scanf("%d %d %d",&N[j],&T[j],&M[j]);
        off[j]=flat.size();
        for(int i=0;i<N[j];i++){int b;scanf("%d",&b);flat.push_back(b);}
        sort(flat.begin()+off[j],flat.end(),[&](int a,int b){
            if(s[a]!=s[b])return s[a]>s[b];
            return a<b;});
    }
    off[L]=flat.size();
    vector<char> taken(B,0);
    long long t=0;
    auto eval=[&](int j)->double{
        long long left=(long long)D-t-T[j];
        if(left<=0)return -1;
        long long cap=left*M[j];
        long long cnt=0,val=0;
        for(int i=off[j];i<off[j+1]&&cnt<cap;i++){
            if(!taken[flat[i]]){val+=s[flat[i]];cnt++;}
        }
        return (double)val/T[j];
    };
    typedef pair<double,int> P;
    auto cmp=[](const P&a,const P&b){
        if(a.first!=b.first)return a.first<b.first;
        return a.second>b.second;};
    priority_queue<P,vector<P>,decltype(cmp)> pq(cmp);
    for(int j=0;j<L;j++){
        double k=eval(j);
        if(k>0)pq.push({k,j});
    }
    auto st=chrono::steady_clock::now();
    string out;
    int A=0;
    while(!pq.empty()){
        P p=pq.top();pq.pop();
        int j=p.second;
        double k=eval(j);
        if(k<=0)continue;
        bool late=chrono::duration<double>(chrono::steady_clock::now()-st).count()>7.5;
        if(!late&&!pq.empty()){
            P q=pq.top();
            if(cmp(P(k,j),q)){
                pq.push({k,j});
                continue;
            }
        }
        long long left=(long long)D-t-T[j];
        long long cap=left*M[j];
        vector<int> ch;
        for(int i=off[j];i<off[j+1]&&(long long)ch.size()<cap;i++){
            if(!taken[flat[i]]){ch.push_back(flat[i]);}
        }
        if(ch.empty())continue;
        for(int b:ch)taken[b]=1;
        A++;
        out+=to_string(j)+" "+to_string(ch.size())+"\n";
        for(size_t i=0;i<ch.size();i++){
            out+=to_string(ch[i]);
            out+=(i+1<ch.size()?' ':'\n');
        }
        t+=T[j];
    }
    printf("%d\n",A);
    fputs(out.c_str(),stdout);
}
