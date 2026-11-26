#include <bits/stdc++.h>
using namespace std;
using Clock=chrono::steady_clock;
static Clock::time_point T0;
static double el(){return chrono::duration<double>(Clock::now()-T0).count();}

int NT;
vector<vector<int>> st;
vector<int> mk; int stampv=0;
inline void markS(const vector<int>&a){++stampv;for(int t:a)mk[t]=stampv;}
inline int scMarked(int asz,const vector<int>&b){int c=0;for(int t:b)if(mk[t]==stampv)c++;return min(c,min(asz-c,(int)b.size()-c));}
inline int sc(int a,int b){markS(st[a]);return scMarked(st[a].size(),st[b]);}

int main(){
    T0=Clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int N;if(!(cin>>N))return 0;
    vector<char> ori(N);vector<vector<int>> pt(N);
    unordered_map<string,int> tid;tid.reserve(1<<20);
    for(int i=0;i<N;i++){char c;int m;cin>>c>>m;ori[i]=c;pt[i].resize(m);
        for(int k=0;k<m;k++){string s;cin>>s;auto it=tid.find(s);int id;if(it==tid.end()){id=tid.size();tid.emplace(s,id);}else id=it->second;pt[i][k]=id;}
        sort(pt[i].begin(),pt[i].end());pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());}
    NT=tid.size();mk.assign(NT+1,0);
    vector<pair<int,int>> sp;
    for(int i=0;i<N;i++)if(ori[i]=='H'){sp.push_back({i,-1});st.push_back(pt[i]);}
    // pair verticals
    vector<int> vs;for(int i=0;i<N;i++)if(ori[i]=='V')vs.push_back(i);
    sort(vs.begin(),vs.end(),[&](int a,int b){if(pt[a].size()!=pt[b].size())return pt[a].size()>pt[b].size();return a<b;});
    int V=vs.size();
    if(V>=2){
        vector<int> nx(V),pv(V);for(int i=0;i<V;i++){nx[i]=i+1;pv[i]=i-1;}
        int head=0,tail=V-1;int cnt=V;
        auto rem=[&](int x){if(pv[x]>=0)nx[pv[x]]=nx[x];else head=nx[x];if(nx[x]<V)pv[nx[x]]=pv[x];else tail=pv[x];cnt--;};
        while(cnt>=2){
            int a=head;rem(a);
            const auto&A=pt[vs[a]];markS(A);
            int best=-1,bi=-1,bs=-1;int it=0;
            for(int x=tail;x>=0&&it<300;x=pv[x],it++){
                const auto&B=pt[vs[x]];int c=0;for(int t:B)if(mk[t]==stampv)c++;
                if(best<0||c<bi||(c==bi&&(int)B.size()>bs)){best=x;bi=c;bs=B.size();}
            }
            rem(best);
            int p=vs[a],q=vs[best];vector<int> u;u.reserve(pt[p].size()+pt[q].size());
            set_union(pt[p].begin(),pt[p].end(),pt[q].begin(),pt[q].end(),back_inserter(u));
            sp.push_back({p,q});st.push_back(u);
        }
    }
    int S=st.size();
    if(S==0){printf("0\n");return 0;}
    // inverted index
    vector<vector<int>> lst(NT);
    vector<long long> offs(S+1,0);for(int s=0;s<S;s++)offs[s+1]=offs[s]+st[s].size();
    vector<int> P(offs[S]);
    for(int s=0;s<S;s++)for(size_t k=0;k<st[s].size();k++){int t=st[s][k];P[offs[s]+k]=lst[t].size();lst[t].push_back(s);}
    vector<vector<int>> full=lst;
    vector<char> used(S,0);
    auto removeS=[&](int s){used[s]=1;for(size_t k=0;k<st[s].size();k++){int t=st[s][k];int p=P[offs[s]+k];int last=lst[t].back();lst[t][p]=last;
        int kk=lower_bound(st[last].begin(),st[last].end(),t)-st[last].begin();P[offs[last]+kk]=p;lst[t].pop_back();}};
    vector<int> ord;ord.reserve(S);
    vector<int> seen(S,0);int seenStamp=0;
    int cur=0;removeS(cur);ord.push_back(cur);int fb=0;
    long long evals=0;double budget=2000;
    double tStart=el();long long evalStart=0;
    for(int step=1;step<S;step++){
        if((step&255)==0){
            double e=el();double used_t=e-tStart;long long ev=evals-evalStart;
            if(ev>1000&&used_t>0){double per=used_t/ev;double remain=max(0.05,6.0-e);double allowed=remain/(S-step);budget=allowed/per;budget=max(30.0,min(20000.0,budget));}
        }
        const auto&C=st[cur];markS(C);int asz=C.size();int ub=asz/2;
        ++seenStamp;
        int best=-1,bsc=-1;long long lim=(long long)budget;long long done=0;
        int nt=C.size();int perTag=max<long long>(10,lim/max(1,nt));
        for(int ti=0;ti<nt&&done<lim&&bsc<ub;ti++){
            auto&L=lst[C[ti]];int c=0;
            for(int idx=(int)L.size()-1;idx>=0&&c<perTag;idx--){
                int s=L[idx];if(seen[s]==seenStamp)continue;seen[s]=seenStamp;c++;done++;
                int v=scMarked(asz,st[s]);
                if(v>bsc){bsc=v;best=s;if(bsc>=ub)break;}
            }
        }
        evals+=done;
        if(best<0){while(used[fb])fb++;best=fb;}
        removeS(best);ord.push_back(best);cur=best;
    }
    // local search: segment reversal
    int n=S;
    if(n>=3){
        vector<int> e(n-1);for(int i=0;i+1<n;i++)e[i]=sc(ord[i],ord[i+1]);
        vector<int> posOf(S);for(int i=0;i<n;i++)posOf[ord[i]]=i;
        mt19937 rng(12345);const int W=400;
        long long iter=0;
        while(true){
            if((iter&1023)==0&&el()>9.3)break;
            iter++;
            int i,j;
            if(rng()&1){
                i=rng()%n;
                if(i==0)continue;
                int a=ord[i-1];const auto&A=st[a];int t=A[rng()%A.size()];auto&F=full[t];
                int s=F[rng()%F.size()];int pj=posOf[s];
                if(pj<=i||pj-i>W)continue;j=pj;
            } else {
                i=rng()%n;j=i+1+rng()%W;if(j>=n)continue;
            }
            int old=(i>0?e[i-1]:0)+(j<n-1?e[j]:0);
            int n1=(i>0?sc(ord[i-1],ord[j]):0),n2=(j<n-1?sc(ord[i],ord[j+1]):0);
            if(n1+n2>old){
                reverse(ord.begin()+i,ord.begin()+j+1);
                if(j>i)reverse(e.begin()+i,e.begin()+j);
                if(i>0)e[i-1]=n1;if(j<n-1)e[j]=n2;
                for(int k=i;k<=j;k++)posOf[ord[k]]=k;
            }
        }
    }
    string out;out.reserve(S*14+16);
    out+=to_string(S);out+='\n';
    for(int s:ord){out+=to_string(sp[s].first);if(sp[s].second>=0){out+=' ';out+=to_string(sp[s].second);}out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
