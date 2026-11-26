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
            auto it=mp.emplace(buf,(int)mp.size());
            t[i].push_back(it.first->second);
        }
        sort(t[i].begin(),t[i].end());
    }
    vector<vector<int>> st;
    vector<array<int,2>> ids;
    vector<int> v;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){st.push_back(t[i]);ids.push_back({i,-1});}
        else v.push_back(i);
    }
    stable_sort(v.begin(),v.end(),[&](int a,int b){return t[a].size()<t[b].size();});
    for(size_t i=0;i+1<v.size();i+=2){
        vector<int> u;
        set_union(t[v[i]].begin(),t[v[i]].end(),t[v[i+1]].begin(),t[v[i+1]].end(),back_inserter(u));
        st.push_back(u);
        ids.push_back({v[i],v[i+1]});
    }
    int S=st.size();
    int T=mp.size();
    vector<vector<int>> post(T);
    for(int i=0;i<S;i++)for(int x:st[i])post[x].push_back(i);
    vector<int> ptr(T,0);
    vector<char> used(S,0);
    vector<int> stamp(S,-1);
    auto score=[&](int a,int b){
        auto&A=st[a];auto&B=st[b];
        size_t i=0,j=0;int c=0;
        while(i<A.size()&&j<B.size()){
            if(A[i]==B[j]){c++;i++;j++;}
            else if(A[i]<B[j])i++;else j++;
        }
        return min(c,(int)min(A.size()-c,B.size()-c));
    };
    vector<int> order;
    int cur=0;
    used[0]=1;
    order.push_back(0);
    int g=0;
    for(int step=1;step<S;step++){
        int best=-1,bs=-1,cnt=0;
        for(int x:st[cur]){
            auto&p=post[x];
            int&q=ptr[x];
            while(q<(int)p.size()&&used[p[q]])q++;
            int k=0;
            for(int idx=q;idx<(int)p.size()&&k<12;idx++){
                int c=p[idx];
                if(used[c]||stamp[c]==step)continue;
                stamp[c]=step;
                k++;cnt++;
                int s=score(cur,c);
                if(s>bs){bs=s;best=c;}
            }
            if(cnt>=150)break;
        }
        if(best<0){
            while(used[g])g++;
            best=g;
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
