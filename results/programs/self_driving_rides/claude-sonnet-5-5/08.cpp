#include <bits/stdc++.h>
using namespace std;
int main(){
    int R,C,F,N,B,T;
    scanf("%d %d %d %d %d %d",&R,&C,&F,&N,&B,&T);
    vector<array<int,6>> r(N);
    for(auto&x:r)for(int k=0;k<6;k++)scanf("%d",&x[k]);
    vector<int> rem(N);
    for(int i=0;i<N;i++)rem[i]=i;
    vector<int> px(F,0),py(F,0),tm(F,0);
    vector<vector<int>> res(F);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
    for(int i=0;i<F;i++)pq.push({0,i});
    while(!pq.empty()&&!rem.empty()){
        auto [t,v]=pq.top();pq.pop();
        double best=-1;int bi=-1;int bf=0;
        for(int j=0;j<(int)rem.size();j++){
            auto&q=r[rem[j]];
            int d=abs(q[2]-q[0])+abs(q[3]-q[1]);
            int arr=t+abs(q[0]-px[v])+abs(q[1]-py[v]);
            int st=max(arr,q[4]);
            int fin=st+d;
            if(fin>q[5])continue;
            int pts=d+(st==q[4]?B:0);
            double val=(double)pts/(double)(fin-t);
            if(val>best){best=val;bi=j;bf=fin;}
        }
        if(bi<0)continue;
        int id=rem[bi];
        rem[bi]=rem.back();rem.pop_back();
        res[v].push_back(id);
        px[v]=r[id][2];py[v]=r[id][3];tm[v]=bf;
        pq.push({bf,v});
    }
    for(int i=0;i<F;i++){
        printf("%d",(int)res[i].size());
        for(int x:res[i])printf(" %d",x);
        printf("\n");
    }
}
