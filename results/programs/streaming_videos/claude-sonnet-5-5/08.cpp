#include <bits/stdc++.h>
using namespace std;
int V,E,R,C,X;
vector<int> sz;
vector<int> LD;
vector<vector<int>> lat; // E x C
vector<vector<pair<int,int>>> reqs; // per video (e,n)
vector<vector<int>> cur; // per video aligned with reqs
vector<vector<int>> conn;
struct It{double k;int c,v;bool operator<(const It&o)const{return k<o.k;}};
int main(){
    scanf("%d %d %d %d %d",&V,&E,&R,&C,&X);
    sz.resize(V);
    for(auto&s:sz)scanf("%d",&s);
    LD.resize(E);
    lat.assign(E,vector<int>(C,0));
    conn.resize(E);
    for(int e=0;e<E;e++){
        int k;scanf("%d %d",&LD[e],&k);
        for(int i=0;i<k;i++){int c,l;scanf("%d %d",&c,&l);lat[e][c]=l;conn[e].push_back(c);}
    }
    reqs.resize(V);
    for(int i=0;i<R;i++){
        int v,e,n;scanf("%d %d %d",&v,&e,&n);
        reqs[v].push_back({e,n});
    }
    cur.resize(V);
    for(int v=0;v<V;v++)for(auto&p:reqs[v])cur[v].push_back(LD[p.first]);
    auto gain=[&](int c,int v)->long long{
        long long g=0;
        for(size_t i=0;i<reqs[v].size();i++){
            int e=reqs[v][i].first;
            int l=lat[e][c];
            if(l&&cur[v][i]>l)g+=(long long)reqs[v][i].second*(cur[v][i]-l);
        }
        return g;
    };
    priority_queue<It> pq;
    vector<long long> acc(C);
    vector<int> touched;
    vector<char> seen(C,0);
    for(int v=0;v<V;v++){
        touched.clear();
        for(size_t i=0;i<reqs[v].size();i++){
            int e=reqs[v][i].first;
            for(int c:conn[e]){
                if(!seen[c]){seen[c]=1;touched.push_back(c);acc[c]=0;}
                acc[c]+=(long long)reqs[v][i].second*(LD[e]-lat[e][c]);
            }
        }
        for(int c:touched){
            seen[c]=0;
            if(acc[c]>0&&sz[v]<=X)pq.push({(double)acc[c]/sz[v],c,v});
        }
    }
    vector<int> rem(C,X);
    vector<vector<int>> out(C);
    vector<char> in((size_t)C*V,0);
    while(!pq.empty()){
        It t=pq.top();pq.pop();
        int c=t.c,v=t.v;
        if(in[(size_t)c*V+v]||sz[v]>rem[c])continue;
        long long g=gain(c,v);
        if(g<=0)continue;
        double k=(double)g/sz[v];
        if(k<t.k-1e-12){
            if(!pq.empty()&&pq.top().k>k){pq.push({k,c,v});continue;}
        }
        in[(size_t)c*V+v]=1;
        rem[c]-=sz[v];
        out[c].push_back(v);
        for(size_t i=0;i<reqs[v].size();i++){
            int l=lat[reqs[v][i].first][c];
            if(l&&cur[v][i]>l)cur[v][i]=l;
        }
    }
    int n=0;
    for(int c=0;c<C;c++)if(!out[c].empty())n++;
    printf("%d\n",n);
    for(int c=0;c<C;c++)if(!out[c].empty()){
        printf("%d",c);
        for(int v:out[c])printf(" %d",v);
        printf("\n");
    }
}
