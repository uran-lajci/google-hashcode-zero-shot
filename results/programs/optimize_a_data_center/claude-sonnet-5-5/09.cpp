#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    vector<vector<pair<int,int>>> seg(R);
    vector<int> mxs(R,0);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[r][s]){s++;continue;}
            int e=s;
            while(e<S&&!bad[r][e])e++;
            seg[r].push_back({s,e-s});
            mxs[r]=max(mxs[r],e-s);
            s=e;
        }
    }
    vector<int> ord(M);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)c[a]*z[b]>(long long)c[b]*z[a];
    });
    vector<long long> rc(R,0);
    set<pair<long long,int>> pq;
    for(int r=0;r<R;r++)pq.insert({0,r});
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    for(int idx:ord){
        vector<pair<long long,int>> held;
        int chosen=-1;
        while(!pq.empty()){
            auto it=pq.begin();
            auto pr=*it;
            pq.erase(it);
            if(mxs[pr.second]>=z[idx]){chosen=pr.second;break;}
            held.push_back(pr);
        }
        for(auto&h:held)pq.insert(h);
        if(chosen<0)continue;
        int r=chosen;
        for(auto&sg:seg[r]){
            if(sg.second>=z[idx]){
                ar[idx]=r;as[idx]=sg.first;
                sg.first+=z[idx];
                sg.second-=z[idx];
                break;
            }
        }
        int m=0;
        for(auto&sg:seg[r])m=max(m,sg.second);
        mxs[r]=m;
        rc[r]+=c[idx];
        pq.insert({rc[r],r});
    }
    vector<vector<long long>> cap(P,vector<long long>(R,0));
    vector<long long> tot(P,0),mx(P,0);
    set<pair<long long,int>> ps;
    for(int p=0;p<P;p++)ps.insert({0,p});
    for(int idx:ord){
        if(ar[idx]<0)continue;
        auto it=ps.begin();
        int p=it->second;
        ps.erase(it);
        int r=ar[idx];
        cap[p][r]+=c[idx];
        tot[p]+=c[idx];
        mx[p]=max(mx[p],cap[p][r]);
        ap[idx]=p;
        ps.insert({tot[p]-mx[p],p});
    }
    for(int i=0;i<M;i++){
        if(ar[i]<0)puts("x");
        else printf("%d %d %d\n",ar[i],as[i],ap[i]);
    }
}
