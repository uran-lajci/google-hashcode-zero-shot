#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R, vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    vector<int> ord(M);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)c[a]*z[b]>(long long)c[b]*z[a];
    });
    // segments per row
    vector<vector<pair<int,int>>> seg(R); // start, length remaining
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[r][s]){s++;continue;}
            int e=s;
            while(e<S&&!bad[r][e])e++;
            seg[r].push_back({s,e-s});
            s=e;
        }
    }
    vector<long long> rowcap(R,0);
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    vector<int> placed;
    for(int idx:ord){
        vector<int> rs(R);
        iota(rs.begin(),rs.end(),0);
        sort(rs.begin(),rs.end(),[&](int a,int b){
            if(rowcap[a]!=rowcap[b])return rowcap[a]<rowcap[b];
            return a<b;
        });
        for(int r:rs){
            bool done=false;
            for(auto&sg:seg[r]){
                if(sg.second>=z[idx]){
                    ar[idx]=r;as[idx]=sg.first;
                    sg.first+=z[idx];sg.second-=z[idx];
                    rowcap[r]+=c[idx];
                    done=true;break;
                }
            }
            if(done)break;
        }
        if(ar[idx]>=0)placed.push_back(idx);
    }
    // pool assignment
    vector<vector<int>> cap(P, vector<int>(R,0));
    vector<long long> tot(P,0),mx(P,0);
    typedef tuple<long long,long long,int> T;
    priority_queue<T,vector<T>,greater<T>> pq;
    for(int p=0;p<P;p++)pq.push(T(0,0,p));
    sort(placed.begin(),placed.end(),[&](int a,int b){
        if(c[a]!=c[b])return c[a]>c[b];
        return a<b;
    });
    for(int idx:placed){
        auto [g,t,p]=pq.top();pq.pop();
        ap[idx]=p;
        int r=ar[idx];
        cap[p][r]+=c[idx];
        tot[p]+=c[idx];
        mx[p]=max(mx[p],(long long)cap[p][r]);
        pq.push(T(tot[p]-mx[p],tot[p],p));
    }
    for(int i=0;i<M;i++){
        if(ar[i]<0)puts("x");
        else printf("%d %d %d\n",ar[i],as[i],ap[i]);
    }
}
