#include <bits/stdc++.h>
using namespace std;

static int T, W;
static vector<vector<int>> st;      // slide tags
static vector<uint64_t> bits;
static bool useBits;

static inline int commonCnt(int a, int b){
    if(useBits){
        const uint64_t* x=&bits[(size_t)a*W]; const uint64_t* y=&bits[(size_t)b*W];
        int c=0; for(int k=0;k<W;k++) c+=__builtin_popcountll(x[k]&y[k]); return c;
    }
    const vector<int>&A=st[a],&B=st[b];
    size_t i=0,j=0; int c=0;
    while(i<A.size()&&j<B.size()){ if(A[i]<B[j])i++; else if(A[i]>B[j])j++; else {c++;i++;j++;} }
    return c;
}
static inline int score(int a,int b){
    int c=commonCnt(a,b);
    int x=(int)st[a].size()-c, y=(int)st[b].size()-c;
    return min(c,min(x,y));
}
static int interVec(const vector<int>&A,const vector<int>&B){
    size_t i=0,j=0; int c=0;
    while(i<A.size()&&j<B.size()){ if(A[i]<B[j])i++; else if(A[i]>B[j])j++; else {c++;i++;j++;} }
    return c;
}

int main(){
    auto t0=chrono::steady_clock::now();
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int N; if(!(cin>>N)) return 0;
    unordered_map<string,int> mp; mp.reserve(1<<20);
    vector<char> ori(N); vector<vector<int>> pt(N);
    string s;
    for(int i=0;i<N;i++){
        int m; cin>>ori[i]>>m; pt[i].resize(m);
        for(int k=0;k<m;k++){ cin>>s; auto it=mp.find(s); int id;
            if(it==mp.end()){ id=mp.size(); mp.emplace(s,id);} else id=it->second;
            pt[i][k]=id; }
        sort(pt[i].begin(),pt[i].end());
        pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());
    }
    T=mp.size();
    // build slides
    vector<pair<int,int>> slides;
    vector<int> vs;
    for(int i=0;i<N;i++){ if(ori[i]=='H') slides.push_back({i,-1}); else vs.push_back(i); }
    sort(vs.begin(),vs.end(),[&](int a,int b){ if(pt[a].size()!=pt[b].size()) return pt[a].size()>pt[b].size(); return a<b;});
    {
        int V=vs.size(); vector<char> u(V,0);
        int front=0, back=V-1;
        while(true){
            while(front<V&&u[front])front++;
            if(front>=V)break;
            u[front]=1;
            while(back>=0&&u[back])back--;
            if(back<0)break;
            int best=-1,bv=INT_MAX,cnt=0;
            for(int k=back;k>front&&cnt<100;k--){
                if(u[k])continue; cnt++;
                int c=interVec(pt[vs[front]],pt[vs[k]]);
                if(c<bv){bv=c;best=k; if(c==0)break;}
            }
            if(best<0)break;
            u[best]=1;
            slides.push_back({vs[front],vs[best]});
        }
    }
    int S=slides.size();
    if(S==0){ printf("0\n"); return 0; }
    st.resize(S);
    for(int i=0;i<S;i++){
        auto [a,b]=slides[i];
        if(b<0) st[i]=pt[a];
        else { st[i].clear(); set_union(pt[a].begin(),pt[a].end(),pt[b].begin(),pt[b].end(),back_inserter(st[i])); }
    }
    useBits = T<=2048;
    double avgLen=0; for(auto&v:st) avgLen+=v.size(); avgLen/=S;
    double cost;
    if(useBits){ W=(T+63)/64; bits.assign((size_t)S*W,0);
        for(int i=0;i<S;i++) for(int t:st[i]) bits[(size_t)i*W+(t>>6)]|=1ULL<<(t&63);
        cost=W+2; }
    else cost=2*avgLen+2;
    long long LIMIT=(long long)(6e8/((double)S*cost));
    LIMIT=max(50LL,min(LIMIT,20000LL));
    // inverted index
    vector<vector<int>> inv(T);
    for(int i=0;i<S;i++) for(int t:st[i]) inv[t].push_back(i);
    vector<char> used(S,0);
    vector<int> order; order.reserve(S);
    int cur=0; used[0]=1; order.push_back(0);
    int ptr=0;
    for(int step=1;step<S;step++){
        int best=-1,bid=-1; long long checks=0;
        int maxP=st[cur].size()/2;
        bool stop=false;
        for(int t:st[cur]){
            auto &L=inv[t];
            for(size_t k=0;k<L.size();){
                int x=L[k];
                if(used[x]){ L[k]=L.back(); L.pop_back(); continue; }
                int sc=score(cur,x);
                if(sc>best){best=sc;bid=x;}
                k++; checks++;
                if(checks>=LIMIT||best>=maxP){stop=true;break;}
            }
            if(stop)break;
        }
        if(bid<0){ while(used[ptr])ptr++; bid=ptr; }
        used[bid]=1; order.push_back(bid); cur=bid;
    }
    // 2-opt local search with windows
    if(S>=3){
        uint64_t rs=88172645463325252ULL;
        auto rnd=[&]()->uint64_t{ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; };
        long long ITER=8000000;
        const int WIN=100;
        for(long long it=0;it<ITER;it++){
            if((it&0xFFFF)==0){
                double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
                if(el>8.5)break;
            }
            int i=rnd()%(S-1);
            int maxd=min(WIN,S-1-i);
            int j=i+1+(int)(rnd()%maxd);
            if(j==i+1) continue;
            int a=order[i],b=order[i+1],c=order[j];
            int oldv=score(a,b), newv=score(a,c);
            if(j+1<S){ int d=order[j+1]; oldv+=score(c,d); newv+=score(b,d); }
            if(newv>oldv) reverse(order.begin()+i+1,order.begin()+j+1);
        }
    }
    string out; out.reserve(S*14);
    out+=to_string(S); out+='\n';
    for(int x:order){
        out+=to_string(slides[x].first);
        if(slides[x].second>=0){ out+=' '; out+=to_string(slides[x].second); }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
