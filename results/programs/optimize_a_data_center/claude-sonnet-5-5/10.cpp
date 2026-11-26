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
    vector<int> pr(M,-1),ps(M,-1),pp(M,-1);
    int ptr=0;
    for(int id:ord){
        int zz=z[id];
        for(int t=0;t<R;t++){
            int r=(ptr+t)%R;
            bool done=false;
            for(auto&sg:seg[r]){
                if(sg.second>=zz){
                    pr[id]=r;ps[id]=sg.first;
                    sg.first+=zz;sg.second-=zz;
                    done=true;break;
                }
            }
            if(done){ptr=(r+1)%R;break;}
        }
    }
    vector<int> placed;
    for(int id:ord)if(pr[id]>=0)placed.push_back(id);
    stable_sort(placed.begin(),placed.end(),[&](int a,int b){return c[a]>c[b];});
    vector<int> rs((size_t)P*R,0),tot(P,0),mx(P,0);
    typedef pair<int,int> pi;
    priority_queue<pi,vector<pi>,greater<pi>> pq;
    for(int p=0;p<P;p++)pq.push({0,p});
    for(int id:placed){
        auto t=pq.top();pq.pop();
        int p=t.second;
        pp[id]=p;
        int r=pr[id];
        rs[(size_t)p*R+r]+=c[id];
        tot[p]+=c[id];
        mx[p]=max(mx[p],rs[(size_t)p*R+r]);
        pq.push({tot[p]-mx[p],p});
    }
    for(int i=0;i<M;i++){
        if(pr[i]<0)puts("x");
        else printf("%d %d %d\n",pr[i],ps[i],pp[i]);
    }
}
