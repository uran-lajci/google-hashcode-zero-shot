#include <bits/stdc++.h>
using namespace std;
typedef vector<int> V;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<V> tg(n);
    vector<char> o(n);
    char buf[32];
    for(int i=0;i<n;i++){
        char c[4];int m;
        scanf("%s %d",c,&m);
        o[i]=c[0];
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            auto it=mp.emplace(buf,(int)mp.size()).first;
            tg[i].push_back(it->second);
        }
        sort(tg[i].begin(),tg[i].end());
    }
    vector<V> st;
    vector<array<int,2>> ids;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){st.push_back(tg[i]);ids.push_back({i,-1});}
        else vs.push_back(i);
    }
    stable_sort(vs.begin(),vs.end(),[&](int a,int b){return tg[a].size()<tg[b].size();});
    int l=0,r=(int)vs.size()-1;
    while(l<r){
        int a=vs[l++],b=vs[r--];
        V u;
        set_union(tg[a].begin(),tg[a].end(),tg[b].begin(),tg[b].end(),back_inserter(u));
        st.push_back(u);
        ids.push_back({a,b});
    }
    int S=st.size();
    int T=mp.size();
    vector<V> post(T);
    for(int i=0;i<S;i++)for(int t:st[i])post[t].push_back(i);
    vector<int> pst(T,0);
    vector<char> used(S,0);
    auto score=[&](const V&a,const V&b){
        int i=0,j=0,c=0;
        while(i<(int)a.size()&&j<(int)b.size()){
            if(a[i]==b[j]){c++;i++;j++;}
            else if(a[i]<b[j])i++;else j++;
        }
        return min(c,min((int)a.size()-c,(int)b.size()-c));
    };
    auto t0=chrono::steady_clock::now();
    vector<int> order;
    int cur=0,ptr=0;
    used[0]=1;
    order.push_back(0);
    bool timeout=false;
    for(int step=1;step<S;step++){
        if((step&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>7.5)timeout=true;
        }
        int best=-1,bs=-1;
        if(!timeout){
            vector<int> ts=st[cur];
            sort(ts.begin(),ts.end(),[&](int a,int b){return post[a].size()<post[b].size();});
            int evals=0;
            for(int t:ts){
                V&p=post[t];
                int &s=pst[t];
                while(s<(int)p.size()&&used[p[s]])s++;
                int cnt=0;
                for(int k=s;k<(int)p.size()&&cnt<20;k++){
                    int x=p[k];
                    if(used[x])continue;
                    cnt++;evals++;
                    int sc=score(st[cur],st[x]);
                    if(sc>bs){bs=sc;best=x;}
                }
                if(evals>=200)break;
            }
        }
        if(best<0){
            while(used[ptr])ptr++;
            best=ptr;
        }
        used[best]=1;
        order.push_back(best);
        cur=best;
    }
    printf("%d\n",S);
    for(int x:order){
        if(ids[x][1]<0)printf("%d\n",ids[x][0]);
        else printf("%d %d\n",ids[x][0],ids[x][1]);
    }
}
