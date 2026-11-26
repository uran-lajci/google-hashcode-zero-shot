#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    // segments per row: start,len
    vector<vector<pair<int,int>>> seg(R);
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
    set<pair<long long,int>> rs;
    for(int r=0;r<R;r++)rs.insert({0,r});
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    auto tryRow=[&](int r,int i)->bool{
        for(auto&sg:seg[r]){
            if(sg.second>=z[i]){
                as[i]=sg.first;ar[i]=r;
                sg.first+=z[i];sg.second-=z[i];
                return true;
            }
        }
        return false;
    };
    for(int i:ord){
        int placed=-1;
        int tried=0;
        for(auto it=rs.begin();it!=rs.end()&&tried<50;++it,++tried){
            if(tryRow(it->second,i)){placed=it->second;break;}
        }
        if(placed<0){
            int k=0;
            for(auto it=rs.begin();it!=rs.end();++it,++k){
                if(k<50)continue;
                if(tryRow(it->second,i)){placed=it->second;break;}
            }
        }
        if(placed>=0){
            rs.erase({rowcap[placed],placed});
            rowcap[placed]+=c[i];
            rs.insert({rowcap[placed],placed});
        }
    }
    // pools
    vector<vector<long long>> pr(P,vector<long long>(R,0));
    vector<long long> tot(P,0),mx(P,0);
    set<pair<long long,int>> ps;
    for(int p=0;p<P;p++)ps.insert({0,p});
    for(int i:ord){
        if(ar[i]<0)continue;
        int r=ar[i];
        int bestp=-1;long long bestg=-1;
        int k=0;
        for(auto it=ps.begin();it!=ps.end()&&k<8;++it,++k){
            int p=it->second;
            long long nm=max(mx[p],pr[p][r]+c[i]);
            long long g=tot[p]+c[i]-nm;
            if(g>bestg){bestg=g;bestp=p;}
        }
        int p=bestp;
        ps.erase({tot[p]-mx[p],p});
        pr[p][r]+=c[i];
        tot[p]+=c[i];
        mx[p]=max(mx[p],pr[p][r]);
        ps.insert({tot[p]-mx[p],p});
        ap[i]=p;
    }
    for(int i=0;i<M;i++){
        if(ar[i]<0)puts("x");
        else printf("%d %d %d\n",ar[i],as[i],ap[i]);
    }
}
