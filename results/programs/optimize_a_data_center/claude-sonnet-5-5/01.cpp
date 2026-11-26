#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    // segments per row
    vector<vector<pair<int,int>>> seg(R); // start,len
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
    vector<long long> rowcap(R,0);
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    typedef pair<long long,int> pli;
    priority_queue<pli,vector<pli>,greater<pli>> pq;
    for(int r=0;r<R;r++)pq.push({0,r});
    for(int idx=0;idx<M;idx++){
        int k=ord[idx];
        vector<pli> tmp;
        bool placed=false;
        while(!pq.empty()){
            pli t=pq.top();pq.pop();
            int r=t.second;
            bool ok=false;
            for(auto&sg:seg[r]){
                if(sg.second>=z[k]){
                    ar[k]=r;as[k]=sg.first;
                    sg.first+=z[k];sg.second-=z[k];
                    ok=true;break;
                }
            }
            if(ok){
                rowcap[r]+=c[k];
                pq.push({rowcap[r],r});
                placed=true;break;
            }
            // row can't fit this size; drop it only for this server
            tmp.push_back(t);
        }
        for(auto&t:tmp)pq.push(t);
    }
    // pool assignment
    vector<int> pl;
    for(int k=0;k<M;k++)if(ar[k]>=0)pl.push_back(k);
    stable_sort(pl.begin(),pl.end(),[&](int a,int b){return c[a]>c[b];});
    vector<long long> tot(P,0),mx(P,0);
    vector<vector<int>> pr(P,vector<int>(R,0));
    priority_queue<pli,vector<pli>,greater<pli>> hp;
    for(int p=0;p<P;p++)hp.push({0,p});
    for(int k:pl){
        pli t=hp.top();hp.pop();
        int p=t.second;
        ap[k]=p;
        tot[p]+=c[k];
        pr[p][ar[k]]+=c[k];
        mx[p]=max(mx[p],(long long)pr[p][ar[k]]);
        hp.push({tot[p]-mx[p],p});
    }
    for(int k=0;k<M;k++){
        if(ar[k]<0)puts("x");
        else printf("%d %d %d\n",ar[k],as[k],ap[k]);
    }
}
