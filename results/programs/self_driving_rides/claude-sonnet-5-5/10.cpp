#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int R,C,F,N,B;ll T;
    scanf("%d %d %d %d %d %lld",&R,&C,&F,&N,&B,&T);
    vector<array<int,6>> r(N);
    for(auto&x:r)for(int k=0;k<6;k++)scanf("%d",&x[k]);
    vector<int> un(N);
    for(int i=0;i<N;i++)un[i]=i;
    vector<vector<int>> res(F);
    vector<array<int,2>> pos(F,{0,0});
    vector<ll> tm(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!un.empty()){
        auto [t,v]=pq.top();pq.pop();
        int px=pos[v][0],py=pos[v][1];
        int best=-1;ll bs=0,bd=1,be=0;
        for(int k=0;k<(int)un.size();k++){
            auto&x=r[un[k]];
            ll d=abs(x[2]-x[0])+abs(x[3]-x[1]);
            ll arrive=t+abs(x[0]-px)+abs(x[1]-py);
            ll st=max(arrive,(ll)x[4]);
            ll en=st+d;
            if(en>x[5])continue;
            ll sc=d+(st==x[4]?B:0);
            ll den=en-t;
            bool better=false;
            if(best<0)better=true;
            else{
                ll l=sc*bd,rr=bs*den;
                if(l>rr)better=true;
                else if(l==rr&&en<be)better=true;
            }
            if(better){best=k;bs=sc;bd=den;be=en;}
        }
        if(best<0)continue;
        int id=un[best];
        un[best]=un.back();un.pop_back();
        res[v].push_back(id);
        pos[v]={r[id][2],r[id][3]};
        pq.push({be,v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int x:res[i])printf(" %d",x);
        printf("\n");
    }
}
