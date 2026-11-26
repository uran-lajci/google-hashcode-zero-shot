#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<vector<int>> t(n);
    vector<char> o(n);
    char buf[32];
    for(int i=0;i<n;i++){
        char c[4];int m;
        scanf("%s %d",c,&m);
        o[i]=c[0];
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            auto it=mp.find(buf);
            int id;
            if(it==mp.end()){id=mp.size();mp[buf]=id;}
            else id=it->second;
            t[i].push_back(id);
        }
        sort(t[i].begin(),t[i].end());
    }
    vector<vector<int>> st;
    vector<array<int,2>> ph;
    vector<int> vs;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){st.push_back(t[i]);ph.push_back({i,-1});}
        else vs.push_back(i);
    }
    sort(vs.begin(),vs.end(),[&](int a,int b){return t[a].size()<t[b].size();});
    for(size_t i=0;i+1<vs.size();i+=2){
        vector<int> u;
        set_union(t[vs[i]].begin(),t[vs[i]].end(),t[vs[i+1]].begin(),t[vs[i+1]].end(),back_inserter(u));
        st.push_back(u);
        ph.push_back({vs[i],vs[i+1]});
    }
    int S=st.size();
    int T=mp.size();
    vector<vector<int>> inv(T);
    for(int i=0;i<S;i++)for(int x:st[i])inv[x].push_back(i);
    vector<size_t> sp(T,0);
    vector<char> used(S,0);
    vector<int> seen(S,-1);
    auto sc=[&](int a,int b){
        const auto&A=st[a];const auto&B=st[b];
        size_t i=0,j=0;int c=0;
        while(i<A.size()&&j<B.size()){
            if(A[i]==B[j]){c++;i++;j++;}
            else if(A[i]<B[j])i++;else j++;
        }
        return min(c,min((int)A.size()-c,(int)B.size()-c));
    };
    auto start=chrono::steady_clock::now();
    int cap=300;
    vector<int> order;
    int cur=0;used[0]=1;order.push_back(0);
    int fb=0;
    for(int step=1;step<S;step++){
        if((step&1023)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
            double frac=(double)step/S;
            if(el>8.0)cap=0;
            else if(el>7.5*frac+0.3)cap=max(10,cap*3/4);
            else if(cap<300&&el<6.5*frac)cap=min(300,cap+20);
        }
        int best=-1,bs=-1,cnt=0;
        int half=(int)st[cur].size()/2;
        if(cap>0){
            for(int x:st[cur]){
                auto&L=inv[x];
                size_t&p=sp[x];
                while(p<L.size()&&used[L[p]])p++;
                int per=0;
                for(size_t k=p;k<L.size()&&per<50;k++){
                    int s=L[k];
                    if(used[s])continue;
                    per++;
                    if(seen[s]==step)continue;
                    seen[s]=step;
                    int v=sc(cur,s);
                    cnt++;
                    if(v>bs){bs=v;best=s;}
                    if(bs>=half&&bs>0)goto done;
                    if(cnt>=cap)goto done;
                }
            }
        }
        done:
        if(best<0){
            while(used[fb])fb++;
            best=fb;
        }
        used[best]=1;order.push_back(best);cur=best;
    }
    printf("%d\n",S);
    for(int s:order){
        if(ph[s][1]<0)printf("%d\n",ph[s][0]);
        else printf("%d %d\n",ph[s][0],ph[s][1]);
    }
}
