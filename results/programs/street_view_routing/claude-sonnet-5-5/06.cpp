#include <bits/stdc++.h>
using namespace std;
struct Arc{int to,id,c;};
int main(){
    int N,M,T,C,S;
    if(scanf("%d %d %d %d %d",&N,&M,&T,&C,&S)!=5)return 0;
    for(int i=0;i<N;i++){double a,b;scanf("%lf %lf",&a,&b);}
    vector<vector<Arc>> g(N);
    vector<int> L(M);
    for(int j=0;j<M;j++){
        int a,b,d,c,l;scanf("%d %d %d %d %d",&a,&b,&d,&c,&l);
        L[j]=l;
        g[a].push_back({b,j,c});
        if(d==2)g[b].push_back({a,j,c});
    }
    vector<char> cov(M,0);
    vector<vector<int>> path(C,vector<int>(1,S));
    vector<int> cur(C,S),tm(C,0);
    vector<char> done(C,0);
    vector<long long> dist(N);
    vector<int> stamp(N,0),par(N),parArc(N);
    int st=0;
    int active=C;
    while(active>0){
        for(int car=0;car<C;car++){
            if(done[car])continue;
            int rem=T-tm[car];
            int radius=600;
            int bestV=-1,bestArc=-1;
            while(true){
                st++;
                double bestVal=-1;bestV=-1;
                priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
                int s=cur[car];
                stamp[s]=st;dist[s]=0;par[s]=-1;pq.push({0,s});
                vector<char> fin;
                while(!pq.empty()){
                    auto [d,u]=pq.top();pq.pop();
                    if(d!=dist[u])continue;
                    if(d>radius||d>rem)break;
                    for(int k=0;k<(int)g[u].size();k++){
                        const Arc&a=g[u][k];
                        long long nd=d+a.c;
                        if(nd>rem)continue;
                        if(!cov[a.id]){
                            double val=(double)L[a.id]/nd;
                            if(val>bestVal){bestVal=val;bestV=u;bestArc=k;}
                        }
                        if(nd<=radius&&(stamp[a.to]!=st||nd<dist[a.to])){
                            stamp[a.to]=st;dist[a.to]=nd;par[a.to]=u;parArc[a.to]=a.id;
                            pq.push({nd,a.to});
                        }
                    }
                }
                if(bestV>=0)break;
                if(radius>=rem){break;}
                radius*=2;
            }
            if(bestV<0){done[car]=1;active--;continue;}
            // reconstruct path to bestV
            vector<int> seq;
            int v=bestV;
            while(v!=cur[car]){seq.push_back(v);v=par[v];}
            reverse(seq.begin(),seq.end());
            int u=cur[car];
            for(int w:seq){
                // find arc u->w with parArc
                cov[parArc[w]]=1;
                int best=INT_MAX;
                for(auto&a:g[u])if(a.to==w&&a.id==parArc[w])best=a.c;
                tm[car]+=best;
                path[car].push_back(w);
                u=w;
            }
            const Arc&fa=g[bestV][bestArc];
            cov[fa.id]=1;
            tm[car]+=fa.c;
            path[car].push_back(fa.to);
            cur[car]=fa.to;
        }
    }
    string out=to_string(C)+"\n";
    for(int c=0;c<C;c++){
        out+=to_string(path[c].size())+"\n";
        for(int x:path[c]){out+=to_string(x);out+='\n';}
    }
    fputs(out.c_str(),stdout);
}
