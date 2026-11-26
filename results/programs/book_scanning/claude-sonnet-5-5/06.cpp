#include <bits/stdc++.h>
using namespace std;
int main(){
    int B,L,D;
    scanf("%d %d %d",&B,&L,&D);
    vector<int> S(B);
    for(auto&x:S)scanf("%d",&x);
    vector<int> T(L),M(L);
    vector<vector<int>> bk(L);
    for(int j=0;j<L;j++){
        int n;
        scanf("%d %d %d",&n,&T[j],&M[j]);
        bk[j].resize(n);
        for(auto&x:bk[j])scanf("%d",&x);
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];
            return a<b;});
    }
    vector<char> used(B,0);
    long long t=0;
    auto eval=[&](int j)->double{
        long long rem=D-t-T[j];
        if(rem<=0)return -1;
        long long cap=rem*(long long)M[j];
        long long cnt=0,val=0;
        for(int b:bk[j]){
            if(cnt>=cap)break;
            if(used[b])continue;
            val+=S[b];cnt++;
        }
        return (double)val/T[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=eval(j);
        if(v>0)pq.push({v,j});
    }
    string out;
    int A=0;
    bool late=false;
    while(!pq.empty()){
        auto [v,j]=pq.top();pq.pop();
        if(!late && (clock()>8.0*CLOCKS_PER_SEC))late=true;
        if(!late){
            double nv=eval(j);
            if(nv<=0)continue;
            if(!pq.empty()&&nv<pq.top().first){
                pq.push({nv,j});
                continue;
            }
        } else {
            if(D-t-T[j]<=0)continue;
        }
        long long rem=D-t-T[j];
        if(rem<=0)continue;
        long long cap=rem*(long long)M[j];
        vector<int> pick;
        for(int b:bk[j]){
            if((long long)pick.size()>=cap)break;
            if(used[b])continue;
            pick.push_back(b);
        }
        if(pick.empty())continue;
        for(int b:pick)used[b]=1;
        A++;
        out+=to_string(j)+" "+to_string(pick.size())+"\n";
        for(size_t i=0;i<pick.size();i++){
            out+=to_string(pick[i]);
            out+=(i+1<pick.size())?' ':'\n';
        }
        t+=T[j];
    }
    printf("%d\n",A);
    fputs(out.c_str(),stdout);
}
