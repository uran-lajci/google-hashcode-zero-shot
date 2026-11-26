#include <bits/stdc++.h>
using namespace std;

int V,E,R,C,X;
vector<int> S;
vector<int> LD;
vector<vector<pair<int,int>>> epc; // endpoint -> (cache, lat)
vector<int> lat; // E*C, 0 = none
struct Req{int e; long long n;};
vector<vector<Req>> vreq; // per video

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;} return buf[bufPos++]; }
inline int readInt(){int c=gc();while(c!='-'&&(c<'0'||c>'9')){if(c==-1)return 0;c=gc();}int x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}return x;}

int main(){
    V=readInt();E=readInt();R=readInt();C=readInt();X=readInt();
    S.resize(V);for(auto&s:S)s=readInt();
    LD.resize(E);epc.resize(E);lat.assign((size_t)E*C,0);
    for(int e=0;e<E;e++){
        LD[e]=readInt();int K=readInt();
        for(int k=0;k<K;k++){int c=readInt(),l=readInt();
            if(l<LD[e]){ if(lat[(size_t)e*C+c]==0){epc[e].push_back({c,l});lat[(size_t)e*C+c]=l;}
                else if(l<lat[(size_t)e*C+c]){lat[(size_t)e*C+c]=l;for(auto&p:epc[e])if(p.first==c)p.second=l;}}
        }
    }
    vector<tuple<int,int,int>> rq(R);
    for(int i=0;i<R;i++){int v=readInt(),e=readInt(),n=readInt();rq[i]=make_tuple(v,e,n);}
    sort(rq.begin(),rq.end());
    vreq.resize(V);
    long long totalReq=0;
    for(size_t i=0;i<rq.size();){
        size_t j=i;long long n=0;int v=get<0>(rq[i]),e=get<1>(rq[i]);
        while(j<rq.size()&&get<0>(rq[j])==v&&get<1>(rq[j])==e){n+=get<2>(rq[j]);j++;}
        totalReq+=n;
        if(!epc[e].empty()&&S[v]<=X) vreq[v].push_back({e,n});
        i=j;
    }
    // count candidate pairs
    long long pairsWork=0;
    for(int v=0;v<V;v++)for(auto&r:vreq[v])pairsWork+=epc[r.e].size();
    vector<double> alphas={1.0};
    if(pairsWork<3000000) alphas={1.0,0.85,1.15,0.7,1.3};
    else if(pairsWork<10000000) alphas={1.0,0.85,1.15};

    vector<long long> gain((size_t)C*V,0);
    long long bestScore=-1; vector<vector<int>> bestSol;
    for(double alpha:alphas){
        vector<vector<int>> cur(V);
        for(int v=0;v<V;v++){cur[v].resize(vreq[v].size());for(size_t i=0;i<vreq[v].size();i++)cur[v][i]=LD[vreq[v][i].e];}
        fill(gain.begin(),gain.end(),0);
        for(int v=0;v<V;v++)for(auto&r:vreq[v])for(auto&p:epc[r.e])
            gain[(size_t)p.first*V+v]+=r.n*(LD[r.e]-p.second);
        vector<double> sp(V);for(int v=0;v<V;v++)sp[v]=pow((double)S[v],alpha);
        typedef tuple<double,int,int> T;
        vector<T> hv;
        for(int c=0;c<C;c++)for(int v=0;v<V;v++){long long g=gain[(size_t)c*V+v];if(g>0)hv.emplace_back(g/sp[v],c,v);}
        priority_queue<T> pq(less<T>(),move(hv));
        vector<int> rem(C,X);
        vector<vector<int>> sol(C);
        while(!pq.empty()){
            auto [p,c,v]=pq.top();pq.pop();
            if(S[v]>rem[c])continue;
            long long g=0;
            auto&vr=vreq[v];auto&cv=cur[v];
            for(size_t i=0;i<vr.size();i++){int l=lat[(size_t)vr[i].e*C+c];if(l>0&&l<cv[i])g+=vr[i].n*(cv[i]-l);}
            if(g<=0)continue;
            double np=g/sp[v];
            if(pq.empty()||np>=get<0>(pq.top())-1e-9){
                rem[c]-=S[v];sol[c].push_back(v);
                for(size_t i=0;i<vr.size();i++){int l=lat[(size_t)vr[i].e*C+c];if(l>0&&l<cv[i])cv[i]=l;}
            } else pq.emplace(np,c,v);
        }
        long long sc=0;
        for(int v=0;v<V;v++)for(size_t i=0;i<vreq[v].size();i++)sc+=vreq[v][i].n*(long long)(LD[vreq[v][i].e]-cur[v][i]);
        if(sc>bestScore){bestScore=sc;bestSol=sol;}
    }
    string out;
    int N=0;for(int c=0;c<C;c++)if(!bestSol[c].empty())N++;
    out+=to_string(N)+"\n";
    for(int c=0;c<C;c++)if(!bestSol[c].empty()){
        out+=to_string(c);
        for(int v:bestSol[c]){out+=' ';out+=to_string(v);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
