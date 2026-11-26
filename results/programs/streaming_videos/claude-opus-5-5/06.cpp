#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline long long readInt(){
    int c=gc();while(c!='-'&&(c<'0'||c>'9')){if(c==-1)return 0;c=gc();}
    long long x=0;bool neg=false;if(c=='-'){neg=true;c=gc();}
    while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}
    return neg?-x:x;
}

struct Req{int e;long long n;int best;};
struct Item{double key;int c,v,stamp;};
struct Cmp{bool operator()(const Item&a,const Item&b)const{
    if(a.key!=b.key)return a.key<b.key;
    if(a.c!=b.c)return a.c>b.c;
    return a.v>b.v;}};

int main(){
    int V=readInt(),E=readInt(),R=readInt(),C=readInt();long long X=readInt();
    vector<int> S(V);for(int i=0;i<V;i++)S[i]=readInt();
    vector<int> LD(E);
    vector<vector<pair<int,int>>> conn(E);
    vector<int> lat((size_t)E*C,0);
    for(int e=0;e<E;e++){
        LD[e]=readInt();int K=readInt();
        for(int k=0;k<K;k++){int c=readInt(),l=readInt();
            if(c<0||c>=C)continue;
            if(lat[(size_t)e*C+c]==0){conn[e].push_back({c,l});lat[(size_t)e*C+c]=l;}
            else if(l<lat[(size_t)e*C+c]){lat[(size_t)e*C+c]=l;for(auto&p:conn[e])if(p.first==c)p.second=l;}
        }
    }
    // merge requests
    vector<vector<Req>> reqs(V);
    {
        vector<array<long long,3>> raw;raw.reserve(R);
        for(int i=0;i<R;i++){long long v=readInt(),e=readInt(),n=readInt();raw.push_back({v,e,n});}
        sort(raw.begin(),raw.end());
        for(size_t i=0;i<raw.size();){
            size_t j=i;long long tot=0;
            while(j<raw.size()&&raw[j][0]==raw[i][0]&&raw[j][1]==raw[i][1]){tot+=raw[j][2];j++;}
            int v=raw[i][0],e=raw[i][1];
            if(v>=0&&v<V&&e>=0&&e<E&&!conn[e].empty())
                reqs[v].push_back({e,tot,LD[e]});
            i=j;
        }
    }
    vector<long long> cap(C,X);
    vector<int> stamp(V,0);
    vector<Item> init;
    {
        vector<long long> g(C,0);vector<int> touched;vector<char> mark(C,0);
        for(int v=0;v<V;v++){
            if(S[v]>X)continue;
            touched.clear();
            for(auto&r:reqs[v]){
                for(auto&p:conn[r.e]){
                    int c=p.first;long long d=r.best-p.second;
                    if(d<=0)continue;
                    if(!mark[c]){mark[c]=1;touched.push_back(c);g[c]=0;}
                    g[c]+=r.n*d;
                }
            }
            for(int c:touched){
                if(g[c]>0)init.push_back({(double)g[c]/S[v],c,v,0});
                mark[c]=0;
            }
        }
    }
    priority_queue<Item,vector<Item>,Cmp> pq(Cmp(),std::move(init));
    vector<vector<int>> cached(C);
    while(!pq.empty()){
        Item it=pq.top();pq.pop();
        int c=it.c,v=it.v;
        if(S[v]>cap[c])continue;
        if(it.stamp!=stamp[v]){
            long long g=0;
            for(auto&r:reqs[v]){
                int l=lat[(size_t)r.e*C+c];
                if(l&&l<r.best)g+=r.n*(long long)(r.best-l);
            }
            if(g>0)pq.push({(double)g/S[v],c,v,stamp[v]});
            continue;
        }
        // accept
        cap[c]-=S[v];
        cached[c].push_back(v);
        stamp[v]++;
        for(auto&r:reqs[v]){
            int l=lat[(size_t)r.e*C+c];
            if(l&&l<r.best)r.best=l;
        }
    }
    string out;
    int N=0;for(int c=0;c<C;c++)if(!cached[c].empty())N++;
    out+=to_string(N)+"\n";
    for(int c=0;c<C;c++){
        if(cached[c].empty())continue;
        out+=to_string(c);
        for(int v:cached[c]){out+=' ';out+=to_string(v);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
