#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<char> o(n);
    vector<vector<int>> t(n);
    char buf[32];
    for(int i=0;i<n;i++){
        int m;
        scanf(" %c %d",&o[i],&m);
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            string s(buf);
            auto it=mp.find(s);
            int id;
            if(it==mp.end()){id=mp.size();mp[s]=id;}else id=it->second;
            t[i].push_back(id);
        }
        sort(t[i].begin(),t[i].end());
    }
    int T=mp.size();
    vector<vector<int>> sl;
    vector<array<int,2>> ph;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){sl.push_back(t[i]);ph.push_back({i,-1});}
        else vs.push_back(i);
    }
    sort(vs.begin(),vs.end(),[&](int a,int b){
        if(t[a].size()!=t[b].size())return t[a].size()<t[b].size();
        return a<b;});
    int l=0,r=(int)vs.size()-1;
    while(l<r){
        int a=vs[l],b=vs[r];
        vector<int> u;
        set_union(t[a].begin(),t[a].end(),t[b].begin(),t[b].end(),back_inserter(u));
        sl.push_back(u);
        ph.push_back({a,b});
        l++;r--;
    }
    int S=sl.size();
    vector<vector<int>> inv(T);
    for(int i=0;i<S;i++)for(int x:sl[i])inv[x].push_back(i);
    vector<int> cur(T,0);
    vector<char> used(S,0);
    vector<int> mark(S,-1);
    vector<int> order;
    int ptr=0;
    int c=0;
    used[0]=1;
    order.push_back(0);
    const int CAND=150;
    for(int step=1;step<S;step++){
        int best=-1,bs=-1;
        int cnt=0;
        const auto&A=sl[c];
        for(int x:A){
            auto&L=inv[x];
            int &cu=cur[x];
            while(cu<(int)L.size()&&used[L[cu]])cu++;
            int k=0;
            for(int p=cu;p<(int)L.size()&&k<6&&cnt<CAND;p++){
                int s=L[p];
                if(used[s])continue;
                k++;
                if(mark[s]==step)continue;
                mark[s]=step;
                cnt++;
                const auto&B=sl[s];
                int i=0,j=0,cm=0;
                while(i<(int)A.size()&&j<(int)B.size()){
                    if(A[i]<B[j])i++;
                    else if(A[i]>B[j])j++;
                    else{cm++;i++;j++;}
                }
                int sc=min(cm,min((int)A.size()-cm,(int)B.size()-cm));
                if(sc>bs){bs=sc;best=s;}
            }
            if(cnt>=CAND)break;
        }
        if(best<0){
            while(used[ptr])ptr++;
            best=ptr;
        }
        used[best]=1;
        order.push_back(best);
        c=best;
    }
    printf("%d\n",S);
    for(int s:order){
        if(ph[s][1]<0)printf("%d\n",ph[s][0]);
        else printf("%d %d\n",ph[s][0],ph[s][1]);
    }
}
