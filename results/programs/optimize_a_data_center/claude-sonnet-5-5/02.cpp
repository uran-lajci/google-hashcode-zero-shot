#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,S,U,P,M;
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<vector<char>> bad(R, vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);
    // segments
    vector<vector<array<int,2>>> seg(R); // start,end(exclusive)
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[r][s]){s++;continue;}
            int e=s;
            while(e<S&&!bad[r][e])e++;
            seg[r].push_back({s,e});
            s=e;
        }
    }
    vector<int> ord(M);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)c[a]*z[b]>(long long)c[b]*z[a];
    });
    vector<int> ar(M,-1),as(M,-1),ap(M,-1);
    vector<long long> load(R,0);
    set<pair<long long,int>> rows;
    for(int r=0;r<R;r++)if(!seg[r].empty())rows.insert({0,r});
    for(int idx=0;idx<M;idx++){
        int i=ord[idx];
        vector<int> failed;
        bool placed=false;
        while(!rows.empty()){
            auto it=rows.begin();
            int r=it->second;
            rows.erase(it);
            int found=-1;
            for(int k=0;k<(int)seg[r].size();k++){
                if(seg[r][k][1]-seg[r][k][0]>=z[i]){found=k;break;}
            }
            if(found<0){failed.push_back(r);continue;}
            ar[i]=r;as[i]=seg[r][found][0];
            seg[r][found][0]+=z[i];
            load[r]+=c[i];
            rows.insert({load[r],r});
            placed=true;
            break;
        }
        for(int r:failed)rows.insert({load[r],r});
        (void)placed;
    }
    // pool assignment
    vector<int> pl;
    for(int i=0;i<M;i++)if(ar[i]>=0)pl.push_back(i);
    stable_sort(pl.begin(),pl.end(),[&](int a,int b){return c[a]>c[b];});
    vector<long long> tot(P,0),mx(P,0);
    vector<int> rs((size_t)P*R,0);
    set<pair<long long,int>> ps;
    for(int p=0;p<P;p++)ps.insert({0,p});
    for(int i:pl){
        auto it=ps.begin();
        int p=it->second;
        ps.erase(it);
        ap[i]=p;
        tot[p]+=c[i];
        int &v=rs[(size_t)p*R+ar[i]];
        v+=c[i];
        mx[p]=max<long long>(mx[p],v);
        ps.insert({tot[p]-mx[p],p});
    }
    string out;
    for(int i=0;i<M;i++){
        if(ar[i]<0)out+="x\n";
        else out+=to_string(ar[i])+" "+to_string(as[i])+" "+to_string(ap[i])+"\n";
    }
    fputs(out.c_str(),stdout);
}
