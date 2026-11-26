#include <bits/stdc++.h>
using namespace std;

static vector<char> buf; static size_t bp=0;
static inline void skipws(){while(bp<buf.size()&&isspace((unsigned char)buf[bp]))bp++;}
static inline string tok(){skipws();size_t s=bp;while(bp<buf.size()&&!isspace((unsigned char)buf[bp]))bp++;return string(buf.begin()+s,buf.begin()+bp);}
static inline long long num(){skipws();long long v=0;while(bp<buf.size()&&isdigit((unsigned char)buf[bp])){v=v*10+(buf[bp]-'0');bp++;}return v;}

struct Slide{int a,b;vector<int> tags;};
vector<Slide> S;

static inline int inter(const vector<int>&A,const vector<int>&B){
    size_t i=0,j=0;int c=0;
    while(i<A.size()&&j<B.size()){
        if(A[i]<B[j])i++;else if(A[i]>B[j])j++;else{c++;i++;j++;}
    }
    int a=A.size(),b=B.size();
    return min(c,min(a-c,b-c));
}
static vector<int> unite(const vector<int>&A,const vector<int>&B){
    vector<int> r;r.reserve(A.size()+B.size());
    set_union(A.begin(),A.end(),B.begin(),B.end(),back_inserter(r));return r;
}

int main(){
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    {
        std::ios::sync_with_stdio(false);
        char tmp[1<<16];size_t r;
        while((r=fread(tmp,1,sizeof(tmp),stdin))>0)buf.insert(buf.end(),tmp,tmp+r);
    }
    int N=num();
    vector<char> ori(N);vector<vector<int>> pt(N);
    unordered_map<string,int> tagid;tagid.reserve(1<<20);
    for(int i=0;i<N;i++){
        string o=tok();ori[i]=o.empty()?'H':o[0];
        int m=num();pt[i].reserve(m);
        for(int k=0;k<m;k++){
            string t=tok();
            auto it=tagid.find(t);int id;
            if(it==tagid.end()){id=tagid.size();tagid.emplace(t,id);}else id=it->second;
            pt[i].push_back(id);
        }
        sort(pt[i].begin(),pt[i].end());pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());
    }
    int NT=tagid.size();
    for(int i=0;i<N;i++)if(ori[i]=='H')S.push_back({i,-1,pt[i]});
    // vertical pairing
    {
        vector<int> vs;for(int i=0;i<N;i++)if(ori[i]=='V')vs.push_back(i);
        stable_sort(vs.begin(),vs.end(),[&](int x,int y){return pt[x].size()>pt[y].size();});
        int m=vs.size();
        vector<int> nx(m+1),pv(m+1);
        // nodes 0..m-1, sentinel m
        for(int i=0;i<m;i++){nx[i]=i+1;pv[i]=i-1;}
        int head=m>0?0:m; // head index
        vector<int> stamp(NT,0);int st=0;
        auto rem=[&](int i){
            if(pv[i]>=0)nx[pv[i]]=nx[i];else head=nx[i];
            if(nx[i]<m)pv[nx[i]]=pv[i];
        };
        const int W=100;
        while(head<m){
            int u=head;rem(u);
            if(head>=m)break;
            st++;for(int t:pt[vs[u]])stamp[t]=st;
            int best=-1,bu=-1,bc=INT_MAX;int c=head,k=0;
            while(c<m&&k<W){
                int cm=0;for(int t:pt[vs[c]])if(stamp[t]==st)cm++;
                int un=pt[vs[u]].size()+pt[vs[c]].size()-cm;
                if(un>bu||(un==bu&&cm<bc)){bu=un;bc=cm;best=c;}
                c=nx[c];k++;
            }
            rem(best);
            S.push_back({vs[u],vs[best],unite(pt[vs[u]],pt[vs[best]])});
        }
    }
    int n=S.size();
    if(n==0){printf("0\n");return 0;}
    // greedy chain
    vector<int> ord;ord.reserve(n);
    {
        vector<vector<int>> idx(NT);
        for(int s=0;s<n;s++)for(int t:S[s].tags)idx[t].push_back(s);
        vector<char> used(n,0);vector<int> cnt(n,0);vector<int> touched;touched.reserve(10000);
        int cur=0;int ptr=0;
        while(true){
            used[cur]=1;ord.push_back(cur);
            if((int)ord.size()==n)break;
            const auto&ct=S[cur].tags;int a=ct.size();
            int per=max(40,4000/max(1,a));
            for(int t:ct){
                auto&L=idx[t];size_t i=0;int k=0;
                while(i<L.size()&&k<per){
                    int s=L[i];
                    if(used[s]){L[i]=L.back();L.pop_back();continue;}
                    if(cnt[s]==0)touched.push_back(s);
                    cnt[s]++;i++;k++;
                }
            }
            int best=-1,bs=-1,bb=INT_MAX;
            for(int s:touched){
                int c=cnt[s];int b=S[s].tags.size();
                int sc=min(c,min(a-c,b-c));
                if(sc>bs||(sc==bs&&b<bb)){bs=sc;bb=b;best=s;}
                cnt[s]=0;
            }
            touched.clear();
            if(best<0||bs<=0){
                if(best<0){
                    while(used[ptr])ptr++;
                    best=ptr;
                }
            }
            cur=best;
        }
    }
    // local search
    {
        vector<vector<int>> idx(NT);
        for(int s=0;s<n;s++)for(int t:S[s].tags)idx[t].push_back(s);
        vector<int> pos(n);for(int i=0;i<n;i++)pos[ord[i]]=i;
        vector<int> vsl;for(int s=0;s<n;s++)if(S[s].b>=0)vsl.push_back(s);
        mt19937 rng(12345);
        long long work=0;const long long BUDGET=1500000000LL;
        const int MAXL=3000;
        auto sc=[&](int x,int y){work+=S[x].tags.size()+S[y].tags.size();return inter(S[x].tags,S[y].tags);};
        long long it=0;
        if(n>=3)
        while(work<BUDGET){
            it++;
            if((it&1023)==0&&elapsed()>9.3)break;
            bool doV=!vsl.empty()&&(rng()&1);
            if(!doV){
                int i=1+rng()%(n-1);
                int p=ord[i-1];
                const auto&pt2=S[p].tags;if(pt2.empty())continue;
                int t=pt2[rng()%pt2.size()];
                auto&L=idx[t];if(L.size()<2)continue;
                int s=L[rng()%L.size()];
                int j=pos[s];work+=4;
                if(j>i){
                    if(j-i>MAXL)continue;
                    int d=sc(p,ord[j])-sc(p,ord[i]);
                    if(j+1<n)d+=sc(ord[i],ord[j+1])-sc(ord[j],ord[j+1]);
                    if(d>=0){
                        reverse(ord.begin()+i,ord.begin()+j+1);
                        for(int k=i;k<=j;k++)pos[ord[k]]=k;
                        work+=2*(j-i+1);
                    }
                }else if(j<i-2){
                    int e=i-2;
                    if(e-j>MAXL)continue;
                    int d=sc(s,p)-sc(ord[e],p);
                    if(j-1>=0)d+=sc(ord[j-1],ord[e])-sc(ord[j-1],ord[j]);
                    if(d>=0){
                        reverse(ord.begin()+j,ord.begin()+e+1);
                        for(int k=j;k<=e;k++)pos[ord[k]]=k;
                        work+=2*(e-j+1);
                    }
                }
            }else{
                int A=vsl[rng()%vsl.size()];
                int p=pos[A];
                int q=p+(int)(rng()%201)-100;
                if(q<0||q>=n||q==p)continue;
                int B=ord[q];if(S[B].b<0)continue;
                int ks[4]={p-1,p,q-1,q};vector<int> E;
                for(int k:ks)if(k>=0&&k+1<n)E.push_back(k);
                sort(E.begin(),E.end());E.erase(unique(E.begin(),E.end()),E.end());
                int before=0;for(int k:E)before+=sc(ord[k],ord[k+1]);
                int x=rng()&1,y=rng()&1;
                int &pa=x?S[A].b:S[A].a;int &pb=y?S[B].b:S[B].a;
                swap(pa,pb);
                vector<int> oa=std::move(S[A].tags),ob=std::move(S[B].tags);
                S[A].tags=unite(pt[S[A].a],pt[S[A].b]);
                S[B].tags=unite(pt[S[B].a],pt[S[B].b]);
                work+=2*(oa.size()+ob.size());
                int after=0;for(int k:E)after+=sc(ord[k],ord[k+1]);
                if(after<before){
                    swap(pa,pb);
                    S[A].tags=std::move(oa);S[B].tags=std::move(ob);
                }
            }
        }
    }
    string out;out.reserve(n*14+16);
    out+=to_string(n);out+='\n';
    for(int s:ord){
        out+=to_string(S[s].a);
        if(S[s].b>=0){out+=' ';out+=to_string(S[s].b);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
