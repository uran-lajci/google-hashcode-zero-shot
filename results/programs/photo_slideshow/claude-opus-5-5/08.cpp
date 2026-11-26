#include <bits/stdc++.h>
using namespace std;

static chrono::steady_clock::time_point T0;
static double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-T0).count(); }

struct Slide{ vector<int> tags; int p1,p2; };
vector<Slide> sl;

static inline int scoreF(const vector<int>&a,const vector<int>&b){
    size_t i=0,j=0; int c=0;
    while(i<a.size()&&j<b.size()){
        if(a[i]<b[j]) i++; else if(a[i]>b[j]) j++; else {c++;i++;j++;}
    }
    int x=(int)a.size()-c, y=(int)b.size()-c;
    return min(c,min(x,y));
}

int main(){
    T0=chrono::steady_clock::now();
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int N; if(!(cin>>N)) return 0;
    unordered_map<string,int> tid; tid.reserve(1<<18);
    vector<vector<int>> pt(N); vector<char> ori(N);
    for(int i=0;i<N;i++){
        char c; int m; cin>>c>>m; ori[i]=c;
        pt[i].resize(m);
        for(int k=0;k<m;k++){
            string s; cin>>s;
            auto it=tid.find(s);
            int id;
            if(it==tid.end()){ id=(int)tid.size(); tid.emplace(s,id);} else id=it->second;
            pt[i][k]=id;
        }
        sort(pt[i].begin(),pt[i].end());
        pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());
    }
    int T=(int)tid.size();
    vector<int> mark(T+1,-1);
    // slides from H
    for(int i=0;i<N;i++) if(ori[i]=='H') sl.push_back({pt[i],i,-1});
    // pair verticals
    vector<int> vs;
    for(int i=0;i<N;i++) if(ori[i]=='V') vs.push_back(i);
    sort(vs.begin(),vs.end(),[&](int a,int b){ if(pt[a].size()!=pt[b].size()) return pt[a].size()>pt[b].size(); return a<b;});
    {
        vector<char> usedV(vs.size(),0);
        int stampv=0;
        size_t start=0;
        for(size_t i=0;i<vs.size();i++){
            if(usedV[i]) continue;
            usedV[i]=1;
            int p=vs[i]; stampv++;
            for(int t:pt[p]) mark[t]=stampv;
            int best=-1; int bu=-1; int cnt=0;
            for(size_t j=i+1;j<vs.size()&&cnt<150;j++){
                if(usedV[j]) continue;
                cnt++;
                int q=vs[j]; int c=0;
                for(int t:pt[q]) if(mark[t]==stampv) c++;
                int u=(int)pt[q].size()-c;
                if(u>bu){bu=u;best=(int)j;}
            }
            if(best<0) break;
            usedV[best]=1;
            int q=vs[best];
            vector<int> u; u.reserve(pt[p].size()+pt[q].size());
            set_union(pt[p].begin(),pt[p].end(),pt[q].begin(),pt[q].end(),back_inserter(u));
            sl.push_back({u,p,q});
        }
        (void)start;
    }
    for(auto&x:mark) x=-1;
    int S=(int)sl.size();
    if(S==0){
        // should not happen (N>=1 with at least ... ) but guard: if single V, cannot make slide
        // output nothing valid possible; output 0? Problem guarantees solvable presumably.
        printf("0\n"); return 0;
    }
    // inverted index
    vector<vector<int>> post(T);
    for(int s=0;s<S;s++) for(int t:sl[s].tags) post[t].push_back(s);
    vector<char> used(S,0);
    vector<int> seen(S,-1);
    vector<int> ord; ord.reserve(S);
    int cur=0; used[0]=1; ord.push_back(0);
    int gp=0;
    for(int step=1;step<S;step++){
        int K=400;
        if((step&255)==0){
            double e=elapsed();
            if(e>6.0) K=30; else if(e>4.0) K=100;
        }
        static int lastK=400;
        if((step&255)==0) lastK=K; K=lastK;
        const auto& A=sl[cur].tags;
        int na=(int)A.size();
        for(int t:A) mark[t]=step;
        int per=max(4,K/max(1,na));
        int best=-1,bestS=-1,bestW=INT_MAX;
        int ub=na/2;
        for(int t:A){
            auto& L=post[t];
            size_t idx=0; int cnt=0;
            while(idx<L.size()&&cnt<per){
                int s=L[idx];
                if(used[s]){ L[idx]=L.back(); L.pop_back(); continue; }
                idx++;
                if(seen[s]==step) continue;
                seen[s]=step; cnt++;
                const auto&B=sl[s].tags; int c=0;
                for(int x:B) if(mark[x]==step) c++;
                int nb=(int)B.size();
                int sc=min(c,min(na-c,nb-c));
                int w=nb-c;
                if(sc>best||(sc==best&&w<bestW)){best=sc;bestS=s;bestW=w;}
            }
            if(best>=ub) break;
        }
        if(bestS<0){
            while(used[gp]) gp++;
            bestS=gp;
        }
        used[bestS]=1; ord.push_back(bestS); cur=bestS;
    }
    // local search
    if(S>=3){
        vector<int> sc(S-1);
        for(int i=0;i+1<S;i++) sc[i]=scoreF(sl[ord[i]].tags,sl[ord[i+1]].tags);
        mt19937 rng(12345);
        const double TL=9.2;
        int W=min(S-1,1500);
        auto f=[&](int a,int b){return scoreF(sl[ord[a]].tags,sl[ord[b]].tags);};
        long long it=0;
        while(true){
            if((it&255)==0 && elapsed()>TL) break;
            it++;
            if(rng()&1){
                // 2-opt
                int i=rng()%(S-1);
                int maxj=min(S-1,i+W);
                if(maxj<=i+1) continue;
                int j=i+2+rng()%(maxj-i-1);
                if(j>maxj) continue;
                int before=sc[i]+(j<S-1?sc[j]:0);
                int n1=f(i,j);
                int n2=(j<S-1)?f(i+1,j+1):0;
                if(n1+n2>before){
                    reverse(ord.begin()+i+1,ord.begin()+j+1);
                    if(j-1>i+1) reverse(sc.begin()+i+1,sc.begin()+j);
                    sc[i]=n1; if(j<S-1) sc[j]=n2;
                }
            } else {
                int a=rng()%S;
                int lo=max(0,a-W),hi=min(S-1,a+W);
                int b=lo+rng()%(hi-lo+1);
                if(a>b) swap(a,b);
                if(b-a<2) continue;
                int before=(a>0?sc[a-1]:0)+sc[a]+sc[b-1]+(b<S-1?sc[b]:0);
                swap(ord[a],ord[b]);
                int x1=a>0?f(a-1,a):0, x2=f(a,a+1), x3=f(b-1,b), x4=b<S-1?f(b,b+1):0;
                if(x1+x2+x3+x4>=before){
                    if(a>0) sc[a-1]=x1; sc[a]=x2; sc[b-1]=x3; if(b<S-1) sc[b]=x4;
                } else swap(ord[a],ord[b]);
            }
        }
    }
    string out; out.reserve(S*14);
    out+=to_string(S); out+='\n';
    for(int s:ord){
        out+=to_string(sl[s].p1);
        if(sl[s].p2>=0){ out+=' '; out+=to_string(sl[s].p2);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
