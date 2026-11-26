#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    unordered_map<string,int> id;
    vector<string> nm(C);
    vector<long long> c(C),r(C);
    vector<vector<int>> dep(C);
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
    sort(tg.begin(),tg.end(),[](const Tg&a,const Tg&b){return a.d<b.d;});
    vector<long long> fin(C,-1);
    vector<int> srv(C,-1);
    vector<long long> fr(S,0);
    vector<pair<int,int>> out;
    vector<int> mark(C,0);
    int stamp=0;
    long long work=0;
    for(auto&t:tg){
        if(fin[t.f]>=0){continue;}
        if(work>300000000LL)break;
        stamp++;
        vector<int> need;
        vector<int> st={t.f};
        mark[t.f]=stamp;
        while(!st.empty()){
            int u=st.back();st.pop_back();
            need.push_back(u);
            for(int d:dep[u])if(fin[d]<0&&mark[d]!=stamp){mark[d]=stamp;st.push_back(d);}
        }
        sort(need.begin(),need.end());
        vector<long long> sf=fr;
        vector<pair<int,int>> placed;
        bool ok=true;
        for(int f:need){
            long long bestF=LLONG_MAX;int bs=0;
            for(int s=0;s<S;s++){
                long long st2=fr[s];
                for(int d:dep[f]){
                    long long av=(srv[d]==s)?fin[d]:fin[d]+r[d];
                    if(av>st2)st2=av;
                }
                work+=dep[f].size()+1;
                if(st2+c[f]<bestF){bestF=st2+c[f];bs=s;}
            }
            fin[f]=bestF;srv[f]=bs;fr[bs]=bestF;
            placed.push_back({f,bs});
            if(bestF>t.d&&f==t.f){ok=false;}
            if(bestF>t.d){ok=false;break;}
        }
        if(ok){
            for(auto&p:placed)out.push_back(p);
        } else {
            for(auto&p:placed){fin[p.first]=-1;srv[p.first]=-1;}
            fr=sf;
        }
    }
    if(out.empty()){
        // must output at least one valid step
        int f=0;
        for(int i=0;i<C;i++)if(dep[i].empty()){f=i;break;}
        if(!dep[f].empty()){printf("1\n%s 0\n",nm[0].c_str());return 0;}
        printf("1\n%s 0\n",nm[f].c_str());
        return 0;
    }
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",nm[p.first].c_str(),p.second);
}
