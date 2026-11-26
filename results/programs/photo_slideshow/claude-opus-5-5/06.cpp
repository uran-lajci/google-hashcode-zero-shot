#include <bits/stdc++.h>
using namespace std;

struct Slide { vector<int> tags; int a, b; };

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int N; if(!(cin>>N)) return 0;
    unordered_map<string,int> tagId; tagId.reserve(1<<20);
    vector<vector<int>> ptags(N); vector<char> orient(N);
    for(int i=0;i<N;i++){
        char c; int M; cin>>c>>M; orient[i]=c;
        ptags[i].resize(M);
        for(int j=0;j<M;j++){
            string s; cin>>s;
            auto it=tagId.find(s);
            int id;
            if(it==tagId.end()){ id=tagId.size(); tagId.emplace(s,id);} else id=it->second;
            ptags[i][j]=id;
        }
        sort(ptags[i].begin(),ptags[i].end());
        ptags[i].erase(unique(ptags[i].begin(),ptags[i].end()),ptags[i].end());
    }
    int T=tagId.size();
    vector<int> mark(T,0); int stamp=0;
    vector<Slide> slides;
    vector<int> verts;
    for(int i=0;i<N;i++){
        if(orient[i]=='H') slides.push_back({ptags[i],i,-1});
        else verts.push_back(i);
    }
    // pair verticals
    stable_sort(verts.begin(),verts.end(),[&](int x,int y){return ptags[x].size()>ptags[y].size();});
    {
        int V=verts.size();
        vector<char> vu(V,0);
        int front=0;
        while(true){
            while(front<V && vu[front]) front++;
            if(front>=V) break;
            int x=verts[front]; vu[front]=1;
            ++stamp; for(int t:ptags[x]) mark[t]=stamp;
            int best=-1,bestU=-1,cnt=0;
            for(int k=front+1;k<V && cnt<200;k++){
                if(vu[k]) continue; cnt++;
                int y=verts[k]; int c=0;
                for(int t:ptags[y]) if(mark[t]==stamp) c++;
                int u=(int)ptags[x].size()+(int)ptags[y].size()-c;
                if(u>bestU){bestU=u;best=k;}
            }
            if(best<0) break;
            vu[best]=1;
            int y=verts[best];
            vector<int> u; set_union(ptags[x].begin(),ptags[x].end(),ptags[y].begin(),ptags[y].end(),back_inserter(u));
            slides.push_back({u,x,y});
        }
    }
    int m=slides.size();
    if(m==0){ printf("0\n"); return 0; }
    long long totTags=0; for(auto&s:slides) totTags+=s.tags.size();
    double avgT=max(1.0,(double)totTags/m);
    vector<vector<int>> inv(T);
    for(int i=0;i<m;i++) for(int t:slides[i].tags) inv[t].push_back(i);
    long long limitLL=(long long)(1.2e9/((double)m*avgT));
    int limit=(int)max(1LL,min(20000LL,limitLL));
    vector<char> used(m,0); vector<int> seen(m,0); int sstamp=0;
    vector<int> order; order.reserve(m);
    int cur=0; used[0]=1; order.push_back(0);
    int p=0;
    vector<int> ct;
    while((int)order.size()<m){
        auto& A=slides[cur].tags;
        ++stamp; for(int t:A) mark[t]=stamp;
        ++sstamp;
        int na=A.size(); int ub=na/2;
        ct=A; sort(ct.begin(),ct.end(),[&](int x,int y){return inv[x].size()<inv[y].size();});
        int best=-1,bestId=-1,evals=0; bool done=false;
        for(int t:ct){
            auto& L=inv[t];
            size_t k=0;
            while(k<L.size()){
                int s=L[k];
                if(used[s]){L[k]=L.back();L.pop_back();continue;}
                k++;
                if(seen[s]==sstamp) continue;
                seen[s]=sstamp;
                auto& B=slides[s].tags; int c=0;
                for(int x:B) if(mark[x]==stamp) c++;
                int sc=min(c,min(na-c,(int)B.size()-c));
                if(sc>best){best=sc;bestId=s;}
                evals++;
                if(best>=ub||evals>=limit){done=true;break;}
            }
            if(done) break;
        }
        if(bestId<0){
            while(used[p]) p++;
            bestId=p;
        }
        used[bestId]=1; order.push_back(bestId); cur=bestId;
    }
    // local search
    auto score=[&](int x,int y)->int{
        auto& A=slides[x].tags; auto& B=slides[y].tags;
        size_t i=0,j=0; int c=0;
        while(i<A.size()&&j<B.size()){
            if(A[i]<B[j]) i++; else if(A[i]>B[j]) j++; else {c++;i++;j++;}
        }
        return min(c,min((int)A.size()-c,(int)B.size()-c));
    };
    if(m>=3){
        mt19937 rng(12345);
        long long iters=(long long)(6e8/(8*avgT));
        iters=min(iters,(long long)3e7);
        const int W=100;
        for(long long it=0;it<iters;it++){
            int i=rng()%(m-1);
            int span=min(W,m-1-i);
            int j=i+1+rng()%span;
            if(j==i+1) continue;
            int delta;
            if(j==m-1){
                delta=score(order[i],order[j])-score(order[i],order[i+1]);
            } else {
                delta=score(order[i],order[j])+score(order[i+1],order[j+1])
                     -score(order[i],order[i+1])-score(order[j],order[j+1]);
            }
            if(delta>0) reverse(order.begin()+i+1,order.begin()+j+1);
        }
    }
    string out; out.reserve(m*14+16);
    out+=to_string(m); out+='\n';
    for(int id:order){
        out+=to_string(slides[id].a);
        if(slides[id].b>=0){out+=' ';out+=to_string(slides[id].b);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
