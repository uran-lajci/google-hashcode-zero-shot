#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int main(){
    size_t len=fread(buf,1,sizeof(buf),stdin);
    (void)len;
    // handle larger input by reading all
    string data;
    {
        data.assign(buf,len);
        while(len==sizeof(buf)){
            len=fread(buf,1,sizeof(buf),stdin);
            data.append(buf,len);
        }
    }
    size_t p=0;
    auto rd=[&]()->long long{
        while(p<data.size()&&(data[p]<'0'||data[p]>'9'))p++;
        long long x=0;
        while(p<data.size()&&data[p]>='0'&&data[p]<='9'){x=x*10+data[p]-'0';p++;}
        return x;
    };
    int B=rd(),L=rd(),D=rd();
    vector<int> S(B);
    for(auto&x:S)x=rd();
    vector<vector<int>> bk(L);
    vector<int> T(L),M(L);
    for(int j=0;j<L;j++){
        int n=rd();T[j]=rd();M[j]=rd();
        bk[j].resize(n);
        for(auto&x:bk[j])x=rd();
        sort(bk[j].begin(),bk[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];
            return a<b;});
    }
    vector<char> used(B,0);
    auto eval=[&](int j,long long t)->double{
        long long R=D-t-T[j];
        if(R<=0)return 0;
        long long cap=R*(long long)M[j];
        long long cnt=0,sum=0;
        for(int b:bk[j]){
            if(cnt>=cap)break;
            if(used[b])continue;
            sum+=S[b];cnt++;
        }
        return (double)sum/T[j];
    };
    priority_queue<pair<double,int>> pq;
    // tie-break: lower id preferred -> store negative id via custom compare
    auto cmpPush=[&](double v,int j){pq.push({v,-j});};
    for(int j=0;j<L;j++){
        double v=eval(j,0);
        if(v>0)cmpPush(v,j);
    }
    long long t=0;
    vector<pair<int,vector<int>>> res;
    while(!pq.empty()&&t<D){
        auto top=pq.top();pq.pop();
        int j=-top.second;
        double v=eval(j,t);
        if(v<=0)continue;
        if(!pq.empty()&&v<pq.top().first){
            cmpPush(v,j);
            continue;
        }
        long long R=D-t-T[j];
        long long cap=R*(long long)M[j];
        vector<int> chosen;
        for(int b:bk[j]){
            if((long long)chosen.size()>=cap)break;
            if(used[b])continue;
            used[b]=1;
            chosen.push_back(b);
        }
        if(chosen.empty())continue;
        res.push_back({j,chosen});
        t+=T[j];
    }
    string out;
    out+=to_string(res.size())+"\n";
    for(auto&r:res){
        out+=to_string(r.first)+" "+to_string(r.second.size())+"\n";
        for(size_t i=0;i<r.second.size();i++){
            if(i)out+=' ';
            out+=to_string(r.second[i]);
        }
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
}
