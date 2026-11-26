#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<char> o(n);
    vector<vector<int>> t(n);
    char buf[64];
    for(int i=0;i<n;i++){
        char c;int m;
        scanf(" %c %d",&c,&m);
        o[i]=c;
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
    vector<vector<int>> st;
    vector<pair<int,int>> ph;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){st.push_back(t[i]);ph.push_back({i,-1});}
        else vs.push_back(i);
    }
    sort(vs.begin(),vs.end(),[&](int a,int b){
        if(t[a].size()!=t[b].size())return t[a].size()<t[b].size();
        return a<b;});
    for(size_t i=0,j=vs.size();i+1<j;){
        int a=vs[i],b=vs[j-1];
        vector<int> u;
        set_union(t[a].begin(),t[a].end(),t[b].begin(),t[b].end(),back_inserter(u));
        st.push_back(u);ph.push_back({a,b});
        i++;j--;
    }
    int S=st.size();
    int T=mp.size();
    vector<vector<int>> post(T);
    for(int i=0;i<S;i++)for(int x:st[i])post[x].push_back(i);
    vector<int> ptr(T,0);
    vector<char> used(S,0);
    auto score=[&](int a,int b){
        const vector<int>&A=st[a],&B=st[b];
        size_t i=0,j=0;int c=0;
        while(i<A.size()&&j<B.size()){
            if(A[i]==B[j]){c++;i++;j++;}
            else if(A[i]<B[j])i++;else j++;
        }
        return min(c,min((int)A.size()-c,(int)B.size()-c));
    };
    vector<int> order;
    int cur=0;used[0]=1;order.push_back(0);
    int nextFree=1;
    for(int step=1;step<S;step++){
        vector<pair<int,int>> tg;
        for(int x:st[cur])tg.push_back({(int)post[x].size(),x});
        sort(tg.begin(),tg.end());
        int best=-1,bs=-1,cnt=0;
        for(size_t k=0;k<tg.size()&&k<10&&cnt<200;k++){
            int x=tg[k].second;
            auto&p=post[x];
            while(ptr[x]<(int)p.size()&&used[p[ptr[x]]])ptr[x]++;
            int taken=0;
            for(size_t q=ptr[x];q<p.size()&&taken<30&&cnt<200;q++){
                int s=p[q];
                if(used[s])continue;
                taken++;cnt++;
                int sc=score(cur,s);
                if(sc>bs){bs=sc;best=s;}
            }
        }
        if(best<0){
            while(used[nextFree])nextFree++;
            best=nextFree;
        }
        used[best]=1;order.push_back(best);cur=best;
    }
    printf("%d\n",S);
    for(int s:order){
        if(ph[s].second<0)printf("%d\n",ph[s].first);
        else printf("%d %d\n",ph[s].first,ph[s].second);
    }
}
