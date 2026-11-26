#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<vector<int>> t(n);
    vector<char> o(n);
    char buf[64];
    for(int i=0;i<n;i++){
        int m;
        scanf(" %c %d",&o[i],&m);
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            auto it=mp.emplace(buf,(int)mp.size()).first;
            t[i].push_back(it->second);
        }
        sort(t[i].begin(),t[i].end());
    }
    vector<vector<int>> st;
    vector<array<int,2>> ids;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){st.push_back(t[i]);ids.push_back({i,-1});}
        else vs.push_back(i);
    }
    stable_sort(vs.begin(),vs.end(),[&](int a,int b){return t[a].size()<t[b].size();});
    int l=0,r=(int)vs.size()-1;
    while(l<r){
        int a=vs[l++],b=vs[r--];
        vector<int> u;
        set_union(t[a].begin(),t[a].end(),t[b].begin(),t[b].end(),back_inserter(u));
        st.push_back(u);
        ids.push_back({a,b});
    }
    int S=st.size();
    int T=mp.size();
    vector<vector<int>> lst(T);
    for(int i=0;i<S;i++)for(int x:st[i])lst[x].push_back(i);
    vector<char> used(S,0);
    vector<int> stamp(S,-1);
    auto score=[&](int a,int b){
        auto&A=st[a];auto&B=st[b];
        int c=0;size_t i=0,j=0;
        while(i<A.size()&&j<B.size()){
            if(A[i]==B[j]){c++;i++;j++;}
            else if(A[i]<B[j])i++;else j++;
        }
        return min(c,min((int)A.size()-c,(int)B.size()-c));
    };
    auto st0=chrono::steady_clock::now();
    vector<int> order;
    if(S==0){printf("1\n0\n");return 0;}
    int cur=0,gp=0;
    used[0]=1;
    order.push_back(0);
    for(int step=1;step<S;step++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st0).count();
        int total=el>7?10:150;
        int best=-1,bs=-1;
        int cnt=0;
        int opt=st[cur].size()/2;
        auto&C=st[cur];
        int K=max(2,total*4/(int)max<size_t>(1,C.size()));
        for(int x:C){
            auto&L=lst[x];
            while(!L.empty()&&used[L.back()])L.pop_back();
            int seen=0;
            for(int p=(int)L.size()-1;p>=0&&seen<K;p--){
                int c=L[p];
                if(used[c])continue;
                seen++;
                if(stamp[c]==step)continue;
                stamp[c]=step;
                int s=score(cur,c);
                cnt++;
                if(s>bs){bs=s;best=c;}
            }
            if(bs>=opt||cnt>=total)break;
        }
        if(best<0){
            while(used[gp])gp++;
            best=gp;
        }
        used[best]=1;
        order.push_back(best);
        cur=best;
    }
    printf("%d\n",S);
    for(int s:order){
        if(ids[s][1]<0)printf("%d\n",ids[s][0]);
        else printf("%d %d\n",ids[s][0],ids[s][1]);
    }
}
