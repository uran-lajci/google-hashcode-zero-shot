#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    // segments
    vector<vector<array<int,2>>> seg(R); // start, remaining
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
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    for(int k:ord){
        // rows by load
        vector<int> rs(R);
        iota(rs.begin(),rs.end(),0);
        int best=-1;
        // find least-loaded row that fits
        long long bl=LLONG_MAX;int bseg=-1;
        for(int r=0;r<R;r++){
            if(rowCap[r]>=bl)continue;
            int f=-1;
            for(int j=0;j<(int)seg[r].size();j++)
                if(seg[r][j][1]>=z[k]){f=j;break;}
            if(f>=0){bl=rowCap[r];best=r;bseg=f;}
        }
        if(best<0)continue;
        auto&sg=seg[best][bseg];
        ar[k]=best;as[k]=sg[0];
        sg[0]+=z[k];sg[1]-=z[k];
        rowCap[best]+=c[k];
    }
    // pools
    vector<vector<long long>> ps(P,vector<long long>(R,0));
    vector<long long> tot(P,0),mx(P,0);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    for(int p=0;p<P;p++)pq.push({0,p});
    for(int k:ord){
        if(ar[k]<0)continue;
        auto t=pq.top();pq.pop();
        int p=t.second;
        ap[k]=p;
        tot[p]+=c[k];
        ps[p][ar[k]]+=c[k];
        mx[p]=max(mx[p],ps[p][ar[k]]);
        pq.push({tot[p]-mx[p],p});
    }
    string out;
    for(int i=0;i<M;i++){
        if(ar[i]<0)out+="x\n";
        else out+=to_string(ar[i])+" "+to_string(as[i])+" "+to_string(ap[i])+"\n";
    }
    fputs(out.c_str(),stdout);
}
