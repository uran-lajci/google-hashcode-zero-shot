#include <bits/stdc++.h>
using namespace std;
int main(){
    int n;
    scanf("%d",&n);
    unordered_map<string,int> mp;
    vector<vector<int>> tg(n);
    vector<char> ori(n);
    char buf[64];
    for(int i=0;i<n;i++){
        int m;
        scanf("%s %d",buf,&m);
        ori[i]=buf[0];
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            auto it=mp.find(buf);
            int id;
            if(it==mp.end()){id=mp.size();mp[buf]=id;}
            else id=it->second;
            tg[i].push_back(id);
        }
        sort(tg[i].begin(),tg[i].end());
    }
    vector<vector<int>> sp;
    vector<vector<int>> st;
    vector<int> v;
    for(int i=0;i<n;i++){
        if(ori[i]=='H'){sp.push_back({i});st.push_back(tg[i]);}
        else v.push_back(i);
    }
    sort(v.begin(),v.end(),[&](int a,int b){
        if(tg[a].size()!=tg[b].size())return tg[a].size()<tg[b].size();
        return a<b;});
    int l=0,r=(int)v.size()-1;
    while(l<r){
        int a=v[l++],b=v[r--];
        sp.push_back({a,b});
        vector<int> u;
        set_union(tg[a].begin(),tg[a].end(),tg[b].begin(),tg[b].end(),back_inserter(u));
        st.push_back(u);
    }
    int S=sp.size();
    vector<vector<int>> inv(mp.size());
    for(int i=0;i<S;i++)for(int t:st[i])inv[t].push_back(i);
    vector<int> ptr(mp.size(),0);
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
    int cur=0;
    used[0]=1;
    order.push_back(0);
    int fb=1;
    clock_t t0=clock();
    int cap=150;
    for(int step=1;step<S;step++){
        if((step&1023)==0){
            double el=(double)(clock()-t0)/CLOCKS_PER_SEC;
            double frac=(double)step/S;
            if(el>7.0*frac+0.5)cap=max(10,cap/2);
            else if(el<4.0*frac&&cap<150)cap=min(150,cap*2);
        }
        int best=-1,bs=-1;
        int sz=st[cur].size();
        int per=max(5,cap/max(1,min(sz,10)));
        int cnt=0;
        for(int t:st[cur]){
            auto&L=inv[t];
            int&p=ptr[t];
            while(p<(int)L.size()&&used[L[p]])p++;
            int seen=0;
            for(int k=p;k<(int)L.size()&&seen<per;k++){
                int c=L[k];
                if(used[c])continue;
                seen++;
                int s=score(cur,c);
                if(s>bs){bs=s;best=c;}
            }
            cnt+=seen;
            if(cnt>=cap)break;
            if(bs>=sz/2&&bs>0)break;
        }
        if(best<0){
            while(used[fb])fb++;
            best=fb;
        }
        used[best]=1;
        order.push_back(best);
        cur=best;
    }
    printf("%d\n",S);
    for(int s:order){
        if(sp[s].size()==1)printf("%d\n",sp[s][0]);
        else printf("%d %d\n",sp[s][0],sp[s][1]);
    }
}
