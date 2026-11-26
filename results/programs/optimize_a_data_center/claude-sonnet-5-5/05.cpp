#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R, vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    // segments per row: free start pointer
    vector<vector<pair<int,int>>> seg(R); // (pos, len) remaining
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
    vector<int> ord(M);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)c[a]*z[b]>(long long)c[b]*z[a];
    });
    vector<long long> rowCap(R,0);
    vector<int> ar(M,-1),as_(M,-1),ap(M,-1);
    typedef pair<long long,int> PI;
    for(int k=0;k<M;k++){
        int i=ord[k];
        priority_queue<PI,vector<PI>,greater<PI>> pq;
        for(int r=0;r<R;r++)pq.push({rowCap[r],r});
        // find least-loaded row where it fits
        for(int r=0;r<R;r++){}
        int best=-1;
        long long bv=LLONG_MAX;
        for(int r=0;r<R;r++){
            if(rowCap[r]>=bv)continue;
            for(auto&sg:seg[r]){
                if(sg.second>=z[i]){best=r;bv=rowCap[r];break;}
            }
        }
        if(best<0)continue;
        // best-fit segment in that row
        int bi=-1;
        for(int j=0;j<(int)seg[best].size();j++){
            if(seg[best][j].second>=z[i]&&(bi<0||seg[best][j].second<seg[best][bi].second))bi=j;
        }
        auto&sg=seg[best][bi];
        ar[i]=best;as_[i]=sg.first;
        sg.first+=z[i];sg.second-=z[i];
        rowCap[best]+=c[i];
    }
    vector<vector<long long>> cap(P, vector<long long>(R,0));
    vector<long long> tot(P,0),mx(P,0);
    priority_queue<PI,vector<PI>,greater<PI>> h;
    for(int p=0;p<P;p++)h.push({0,p});
    for(int k=0;k<M;k++){
        int i=ord[k];
        if(ar[i]<0)continue;
        PI t=h.top();h.pop();
        int p=t.second;
        ap[i]=p;
        cap[p][ar[i]]+=c[i];
        tot[p]+=c[i];
        mx[p]=max(mx[p],cap[p][ar[i]]);
        h.push({tot[p]-mx[p],p});
    }
    for(int i=0;i<M;i++){
        if(ar[i]<0)puts("x");
        else printf("%d %d %d\n",ar[i],as_[i],ap[i]);
    }
}
