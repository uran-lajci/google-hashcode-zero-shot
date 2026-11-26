#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R, vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    vector<vector<pair<int,int>>> seg(R);
    vector<int> mx(R,0);
    for(int r=0;r<R;r++){
        int i=0;
        while(i<S){
            if(bad[r][i]){i++;continue;}
            int j=i;
            while(j<S&&!bad[r][j])j++;
            seg[r].push_back({i,j-i});
            mx[r]=max(mx[r],j-i);
            i=j;
        }
    }
    vector<int> ord(M);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)c[a]*z[b]>(long long)c[b]*z[a];
    });
    vector<long long> load(R,0);
    set<pair<long long,int>> rs;
    for(int r=0;r<R;r++)rs.insert({0,r});
    vector<int> ar(M,-1),as(M,0),ap(M,0);
    for(int k:ord){
        for(auto it=rs.begin();it!=rs.end();++it){
            int r=it->second;
            if(mx[r]<z[k])continue;
            for(auto&sg:seg[r]){
                if(sg.second>=z[k]){
                    ar[k]=r;as[k]=sg.first;
                    sg.first+=z[k];sg.second-=z[k];
                    break;
                }
            }
            int m=0;
            for(auto&sg:seg[r])m=max(m,sg.second);
            mx[r]=m;
            long long nl=it->first+c[k];
            rs.erase(it);
            rs.insert({nl,r});
            break;
        }
    }
    vector<vector<int>> rc(P, vector<int>(R,0));
    vector<long long> tot(P,0);
    vector<int> pm(P,0);
    set<pair<long long,int>> ps;
    for(int p=0;p<P;p++)ps.insert({0,p});
    for(int k:ord){
        if(ar[k]<0)continue;
        int r=ar[k];
        // among pools with the lowest gc, pick lowest rc in row r
        auto it=ps.begin();
        long long g=it->first;
        int best=it->second;
        int cnt=0;
        for(auto jt=ps.begin();jt!=ps.end()&&jt->first==g&&cnt<50;++jt,++cnt){
            if(rc[jt->second][r]<rc[best][r])best=jt->second;
        }
        int p=best;
        ps.erase({tot[p]-pm[p],p});
        rc[p][r]+=c[k];
        tot[p]+=c[k];
        pm[p]=max(pm[p],rc[p][r]);
        ps.insert({tot[p]-pm[p],p});
        ap[k]=p;
    }
    string out;
    for(int i=0;i<M;i++){
        if(ar[i]<0)out+="x\n";
        else out+=to_string(ar[i])+" "+to_string(as[i])+" "+to_string(ap[i])+"\n";
    }
    fputs(out.c_str(),stdout);
}
