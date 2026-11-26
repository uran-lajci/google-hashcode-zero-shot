#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<string> nm(C);
    vector<long long> c(C),r(C);
    vector<vector<int>> dep(C);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<C;i++){
        long long a,b;
        scanf("%s %lld %lld",buf,&a,&b);
        nm[i]=buf;id[nm[i]]=i;c[i]=a;r[i]=b;
        int n;scanf("%d",&n);
        for(int j=0;j<n;j++){scanf("%s",buf);dep[i].push_back(id[buf]);}
    }
    struct Tg{int f;long long d,g;};
    vector<Tg> tg(T);
    for(auto&t:tg){long long d,g;scanf("%s %lld %lld",buf,&d,&g);t.f=id[buf];t.d=d;t.g=g;}
    vector<long long> lb(C);
    for(int i=0;i<C;i++){
        long long m=0;
        for(int d:dep[i])m=max(m,lb[d]);
        lb[i]=m+c[i];
    }
    sort(tg.begin(),tg.end(),[](const Tg&a,const Tg&b){return a.d<b.d;});
    const long long INF=LLONG_MAX/4;
    vector<long long> av((size_t)C*S,INF);
    vector<char> done(C,0);
    vector<long long> fr(S,0);
    vector<pair<int,int>> out;
    vector<int> mark(C,0);
    int stamp=0;
    for(auto&t:tg){
        if(lb[t.f]>t.d)continue;
        if(done[t.f]){continue;}
        stamp++;
        vector<int> need;
        vector<int> st={t.f};
        mark[t.f]=stamp;
        while(!st.empty()){
            int x=st.back();st.pop_back();
            need.push_back(x);
            for(int d:dep[x])if(!done[d]&&mark[d]!=stamp){mark[d]=stamp;st.push_back(d);}
        }
        sort(need.begin(),need.end());
        // undo logs
        vector<pair<size_t,long long>> avLog;
        vector<pair<int,long long>> frLog;
        size_t outSize=out.size();
        long long fin_t=0;
        for(int f:need){
            // candidates
            vector<int> cand;
            int ls=0;
            for(int s=1;s<S;s++)if(fr[s]<fr[ls])ls=s;
            cand.push_back(ls);
            int cnt=0;
            for(int d:dep[f]){
                if(cnt>=3)break;
                // server with min av
                int bs=0;
                for(int s=1;s<S;s++)if(av[(size_t)d*S+s]<av[(size_t)d*S+bs])bs=s;
                cand.push_back(bs);cnt++;
            }
            long long best=INF;int bsv=0;
            for(int s:cand){
                long long stt=fr[s];
                for(int d:dep[f])stt=max(stt,av[(size_t)d*S+s]);
                long long fi=stt+c[f];
                if(fi<best){best=fi;bsv=s;}
            }
            frLog.push_back({bsv,fr[bsv]});
            fr[bsv]=best;
            for(int s=0;s<S;s++){
                size_t k=(size_t)f*S+s;
                long long v=(s==bsv)?best:best+r[f];
                if(v<av[k]){avLog.push_back({k,av[k]});av[k]=v;}
            }
            out.push_back({f,bsv});
            fin_t=best;
        }
        if(fin_t>t.d){
            for(int i=(int)avLog.size()-1;i>=0;i--)av[avLog[i].first]=avLog[i].second;
            for(int i=(int)frLog.size()-1;i>=0;i--)fr[frLog[i].first]=frLog[i].second;
            out.resize(outSize);
        } else {
            for(int f:need)done[f]=1;
        }
    }
    if(out.empty()){
        // must output at least one step: compile a file with no dependencies
        for(int i=0;i<C;i++)if(dep[i].empty()){out.push_back({i,0});break;}
    }
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",nm[p.first].c_str(),p.second);
}
