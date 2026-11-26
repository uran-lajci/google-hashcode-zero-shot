#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n;cin>>n;
    unordered_map<string,int> mp;
    vector<vector<int>> t(n);
    vector<char> o(n);
    for(int i=0;i<n;i++){
        string s;int m;cin>>s>>m;o[i]=s[0];
        for(int j=0;j<m;j++){
            string x;cin>>x;
            auto it=mp.find(x);
            int id;
            if(it==mp.end()){id=mp.size();mp[x]=id;}else id=it->second;
            t[i].push_back(id);
        }
        sort(t[i].begin(),t[i].end());
    }
    vector<int> vs;
    vector<vector<int>> slides;
    vector<array<int,2>> ph;
    for(int i=0;i<n;i++){
        if(o[i]=='H'){slides.push_back(t[i]);ph.push_back({i,-1});}
        else vs.push_back(i);
    }
    sort(vs.begin(),vs.end(),[&](int a,int b){
        if(t[a].size()!=t[b].size())return t[a].size()<t[b].size();
        return a<b;});
    int k=vs.size();
    for(int i=0;i<k/2;i++){
        int a=vs[i],b=vs[k-1-i];
        vector<int> u;
        set_union(t[a].begin(),t[a].end(),t[b].begin(),t[b].end(),back_inserter(u));
        slides.push_back(u);ph.push_back({a,b});
    }
    int S=slides.size();
    if(S==0){printf("1\n%d\n",vs.empty()?0:vs[0]);return 0;}
    int T=mp.size();
    vector<vector<int>> post(T);
    for(int i=0;i<S;i++)for(int x:slides[i])post[x].push_back(i);
    vector<int> head(T,0);
    vector<char> used(S,0);
    auto score=[&](int a,int b){
        auto&A=slides[a];auto&B=slides[b];
        int i=0,j=0,c=0;
        while(i<(int)A.size()&&j<(int)B.size()){
            if(A[i]==B[j]){c++;i++;j++;}
            else if(A[i]<B[j])i++;else j++;
        }
        return min(c,min((int)A.size()-c,(int)B.size()-c));
    };
    auto st=chrono::steady_clock::now();
    vector<int> order;order.reserve(S);
    int cur=0;used[0]=1;order.push_back(0);
    int ptr=0;
    for(int step=1;step<S;step++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        int lim=el>8?5:150;
        int best=-1,bs=-1;
        int cnt=0;
        auto&A=slides[cur];
        int target=A.size()/2;
        for(int x:A){
            auto&p=post[x];
            int&h=head[x];
            while(h<(int)p.size()&&used[p[h]])h++;
            int seen=0;
            for(int q=h;q<(int)p.size()&&seen<20;q++){
                int c=p[q];
                if(used[c])continue;
                seen++;cnt++;
                int sc=score(cur,c);
                if(sc>bs){bs=sc;best=c;}
            }
            if(cnt>=lim||bs>=target)break;
        }
        if(best<0){
            while(used[ptr])ptr++;
            best=ptr;
        }
        used[best]=1;order.push_back(best);cur=best;
    }
    string out=to_string(S)+"\n";
    for(int s:order){
        out+=to_string(ph[s][0]);
        if(ph[s][1]>=0)out+=" "+to_string(ph[s][1]);
        out+="\n";
    }
    fputs(out.c_str(),stdout);
}
