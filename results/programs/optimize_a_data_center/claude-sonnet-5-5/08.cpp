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
    vector<set<pair<int,int>>> seg(R); // (len,start)
    vector<long long> freeSl(R,0);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[r][s]){s++;continue;}
            int e=s;
            while(e<S&&!bad[r][e])e++;
            seg[r].insert({e-s,s});
            freeSl[r]+=e-s;
            s=e;
        }
    }
    vector<int> ord(M);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)c[a]*z[b]>(long long)c[b]*z[a];
    });
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    set<pair<long long,int>> rowSet;
    for(int r=0;r<R;r++)rowSet.insert({-freeSl[r],r});
    vector<int> placed;
    for(int idx:ord){
        int zz=z[idx];
        // find row with most free that has a fitting segment
        int found=-1;
        pair<int,int> sg;
        for(auto&pr:rowSet){
            if(-pr.first<zz)break;
            int r=pr.second;
            auto it=seg[r].lower_bound({zz,-1});
            if(it!=seg[r].end()){found=r;sg=*it;break;}
        }
        if(found<0)continue;
        int r=found;
        rowSet.erase({-freeSl[r],r});
        seg[r].erase(sg);
        ar[idx]=r;as[idx]=sg.second;
        if(sg.first>zz)seg[r].insert({sg.first-zz,sg.second+zz});
        freeSl[r]-=zz;
        rowSet.insert({-freeSl[r],r});
        placed.push_back(idx);
    }
    // pool assignment
    vector<int> cap((size_t)P*R,0);
    vector<long long> tot(P,0);
    vector<int> mx(P,0);
    set<pair<long long,int>> ps;
    for(int p=0;p<P;p++)ps.insert({0,p});
    for(int idx:placed){
        int r=ar[idx];
        // pick among pools with minimal gc; tie-break lowest cap in row
        long long g=ps.begin()->first;
        int best=-1;
        int bc=INT_MAX;
        int cnt=0;
        for(auto it=ps.begin();it!=ps.end()&&it->first==g&&cnt<50;++it,++cnt){
            int p=it->second;
            int v=cap[(size_t)p*R+r];
            if(v<bc){bc=v;best=p;}
        }
        int p=best;
        ps.erase({tot[p]-mx[p],p});
        cap[(size_t)p*R+r]+=c[idx];
        tot[p]+=c[idx];
        mx[p]=max(mx[p],cap[(size_t)p*R+r]);
        ps.insert({tot[p]-mx[p],p});
        ap[idx]=p;
    }
    string out;
    for(int i=0;i<M;i++){
        if(ar[i]<0)out+="x\n";
        else out+=to_string(ar[i])+" "+to_string(as[i])+" "+to_string(ap[i])+"\n";
    }
    fputs(out.c_str(),stdout);
}
