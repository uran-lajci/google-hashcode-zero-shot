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
        char c[4];int m;
        scanf("%s %d",c,&m);
        o[i]=c[0];
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            auto it=mp.find(buf);
            int id;
            if(it==mp.end()){id=mp.size();mp[buf]=id;}else id=it->second;
            t[i].push_back(id);
        }
        sort(t[i].begin(),t[i].end());
    }
    vector<vector<int>> sp; // photos in slide
    vector<vector<int>> st;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){sp.push_back({i});st.push_back(t[i]);}
        else vs.push_back(i);
    }
    sort(vs.begin(),vs.end(),[&](int a,int b){
        if(t[a].size()!=t[b].size())return t[a].size()<t[b].size();
        return a<b;});
    int l=0,r((int)vs.size()-1);
    while(l<r){
        int a=vs[l++],b=vs[r--];
        vector<int> u;
        set_union(t[a].begin(),t[a].end(),t[b].begin(),t[b].end(),back_inserter(u));
        sp.push_back({a,b});st.push_back(u);
    }
    int S=sp.size();
    if(S==0){printf("1\n0\n");return 0;}
    int T=mp.size();
    vector<vector<int>> post(T);
    for(int i=0;i<S;i++)for(int x:st[i])post[x].push_back(i);
    vector<char> used(S,0);
    vector<int> cnt(T);
    for(int x=0;x<T;x++)cnt[x]=post[x].size();
    auto score=[&](int a,int b){
        auto&A=st[a];auto&B=st[b];
        size_t i=0,j=0;int c=0;
        while(i<A.size()&&j<B.size()){
            if(A[i]==B[j]){c++;i++;j++;}
            else if(A[i]<B[j])i++;else j++;
        }
        return min(c,min((int)A.size()-c,(int)B.size()-c));
    };
    vector<int> order;
    int ptr=0;
    int cur=0;
    used[0]=1;order.push_back(0);
    const int CAP=100;
    while((int)order.size()<S){
        int best=-1,bs=-1,seen=0;
        vector<int> tg=st[cur];
        sort(tg.begin(),tg.end(),[&](int a,int b){return post[a].size()<post[b].size();});
        int half=st[cur].size()/2;
        for(int x:tg){
            auto&p=post[x];
            size_t k=0;
            while(k<p.size()&&seen<CAP){
                int s=p[k];
                if(used[s]){p[k]=p.back();p.pop_back();continue;}
                k++;seen++;
                int sc=score(cur,s);
                if(sc>bs){bs=sc;best=s;}
                if(bs>=half&&half>0)break;
            }
            if(seen>=CAP||(bs>=half&&half>0))break;
        }
        if(best<0){
            while(used[ptr])ptr++;
            best=ptr;
        }
        used[best]=1;order.push_back(best);cur=best;
    }
    printf("%d\n",S);
    for(int s:order){
        if(sp[s].size()==1)printf("%d\n",sp[s][0]);
        else printf("%d %d\n",sp[s][0],sp[s][1]);
    }
}
