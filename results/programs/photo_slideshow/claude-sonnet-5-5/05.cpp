#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<vector<int>> tg(n);
    vector<char> o(n);
    char buf[64];
    for(int i=0;i<n;i++){
        int m;
        scanf(" %c %d",&o[i],&m);
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            string s(buf);
            auto it=mp.find(s);
            int id;
            if(it==mp.end()){id=mp.size();mp[s]=id;}else id=it->second;
            tg[i].push_back(id);
        }
        sort(tg[i].begin(),tg[i].end());
    }
    int T=mp.size();
    vector<vector<int>> st;
    vector<array<int,2>> ph;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){st.push_back(tg[i]);ph.push_back({i,-1});}
        else vs.push_back(i);
    }
    sort(vs.begin(),vs.end(),[&](int a,int b){return tg[a].size()<tg[b].size();});
    for(size_t k=0;k+1<vs.size();k+=2){
        int a=vs[k],b=vs[k+1];
        vector<int> u;
        set_union(tg[a].begin(),tg[a].end(),tg[b].begin(),tg[b].end(),back_inserter(u));
        st.push_back(u);
        ph.push_back({a,b});
    }
    int S=st.size();
    vector<vector<int>> inv(T);
    for(int i=0;i<S;i++)for(int t:st[i])inv[t].push_back(i);
    vector<int> head(T,0);
    vector<char> used(S,0);
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
    int cur=0;used[0]=1;order.push_back(0);
    int gp=1;
    auto t0=chrono::steady_clock::now();
    bool fast=false;
    for(int step=1;step<S;step++){
        if(!fast && (step&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>7.0)fast=true;
        }
        int best=-1,bs=-1;
        int ceil_=(int)st[cur].size()/2;
        int perTag=fast?3:40;
        int total=fast?30:400;
        int evals=0;
        for(int t:st[cur]){
            auto&L=inv[t];
            int&h=head[t];
            while(h<(int)L.size()&&used[L[h]])h++;
            int cnt=0;
            for(int p=h;p<(int)L.size()&&cnt<perTag&&p-h<perTag*8;p++){
                int c=L[p];
                if(used[c])continue;
                cnt++;evals++;
                int s=score(cur,c);
                if(s>bs){bs=s;best=c;}
            }
            if(bs>=ceil_&&bs>0)break;
            if(evals>=total)break;
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
    for(int c:order){
        if(ph[c][1]<0)printf("%d\n",ph[c][0]);
        else printf("%d %d\n",ph[c][0],ph[c][1]);
    }
}
