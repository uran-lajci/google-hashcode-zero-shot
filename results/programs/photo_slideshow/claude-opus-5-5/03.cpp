#include <bits/stdc++.h>
using namespace std;

static vector<vector<int>> st; // slide tags sorted

static inline int trans(int a,int b){
    const vector<int>&A=st[a];const vector<int>&B=st[b];
    size_t i=0,j=0;int c=0;
    while(i<A.size()&&j<B.size()){
        if(A[i]<B[j])i++;else if(A[i]>B[j])j++;else{c++;i++;j++;}
    }
    int x=(int)A.size()-c,y=(int)B.size()-c;
    return min(c,min(x,y));
}

int main(){
    // fast input
    std::ios::sync_with_stdio(false);
    string data((istreambuf_iterator<char>(cin)),istreambuf_iterator<char>());
    size_t p=0;
    auto skip=[&](){while(p<data.size()&&isspace((unsigned char)data[p]))p++;};
    auto token=[&]()->string{skip();size_t s=p;while(p<data.size()&&!isspace((unsigned char)data[p]))p++;return data.substr(s,p-s);};
    int N=stoi(token());
    unordered_map<string,int> tid;tid.reserve(1<<21);
    vector<char> ori(N);
    vector<vector<int>> pt(N);
    for(int i=0;i<N;i++){
        ori[i]=token()[0];
        int m=stoi(token());
        pt[i].resize(m);
        for(int k=0;k<m;k++){
            string t=token();
            auto it=tid.find(t);
            int id;
            if(it==tid.end()){id=tid.size();tid.emplace(t,id);}else id=it->second;
            pt[i][k]=id;
        }
        sort(pt[i].begin(),pt[i].end());
        pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());
    }
    int T=tid.size();
    vector<int> stamp(T+1,0);int curStamp=0;

    vector<pair<int,int>> slidePh; // second=-1 for H
    for(int i=0;i<N;i++) if(ori[i]=='H') slidePh.push_back({i,-1});
    // pair verticals
    vector<int> vs;
    for(int i=0;i<N;i++) if(ori[i]=='V') vs.push_back(i);
    stable_sort(vs.begin(),vs.end(),[&](int a,int b){return pt[a].size()>pt[b].size();});
    {
        int m=vs.size();
        vector<int> nx(m+1),pv(m+1);
        // linked list over indices 0..m-1, head sentinel m
        for(int i=0;i<m;i++){nx[i]=i+1;pv[i]=i-1;}
        // use -1 as head prev, m as end
        int head=m>0?0:m;
        auto rem=[&](int i){
            if(pv[i]>=0) nx[pv[i]]=nx[i]; else head=nx[i];
            if(nx[i]<m) pv[nx[i]]=pv[i];
        };
        while(head<m){
            int a=head;rem(a);
            if(head>=m)break;
            curStamp++;
            for(int t:pt[vs[a]])stamp[t]=curStamp;
            int best=-1,bc=INT_MAX,cnt=0;
            for(int j=head;j<m&&cnt<200;j=nx[j],cnt++){
                int c=0;for(int t:pt[vs[j]])if(stamp[t]==curStamp)c++;
                if(c<bc){bc=c;best=j;if(c==0)break;}
            }
            rem(best);
            slidePh.push_back({vs[a],vs[best]});
        }
    }
    int S=slidePh.size();
    if(S==0){ // cannot form a valid slide; output nothing meaningful
        printf("0\n");return 0;
    }
    st.resize(S);
    for(int s=0;s<S;s++){
        auto &v=st[s];
        v=pt[slidePh[s].first];
        if(slidePh[s].second>=0){
            auto &w=pt[slidePh[s].second];
            vector<int> u;u.reserve(v.size()+w.size());
            set_union(v.begin(),v.end(),w.begin(),w.end(),back_inserter(u));
            v.swap(u);
        }
    }
    // inverted index
    vector<vector<int>> inv(T);
    for(int s=0;s<S;s++)for(int t:st[s])inv[t].push_back(s);
    vector<char> used(S,0);
    vector<int> ord;ord.reserve(S);
    int cur=0;used[0]=1;ord.push_back(0);
    int ptr=0;
    const int CAP=2000;
    vector<int> tg;
    for(int step=1;step<S;step++){
        curStamp++;
        const auto &A=st[cur];
        for(int t:A)stamp[t]=curStamp;
        int a=A.size();int ub=a/2;
        tg=A;
        sort(tg.begin(),tg.end(),[&](int x,int y){return inv[x].size()<inv[y].size();});
        int best=-1,bs=-1,checks=0;bool done=false;
        for(int t:tg){
            auto &L=inv[t];
            for(size_t k=0;k<L.size();){
                int s=L[k];
                if(used[s]){L[k]=L.back();L.pop_back();continue;}
                int c=0;const auto&B=st[s];
                for(int x:B)if(stamp[x]==curStamp)c++;
                int sc=min(c,min(a-c,(int)B.size()-c));
                if(sc>bs){bs=sc;best=s;}
                k++;checks++;
                if(checks>=CAP||bs>=ub){done=true;break;}
            }
            if(done)break;
        }
        if(best<0){
            while(used[ptr])ptr++;
            best=ptr;
        }
        used[best]=1;ord.push_back(best);cur=best;
    }
    // local search: segment reversal
    if(S>=3){
        mt19937 rng(12345);
        const int W=60;
        long long iters=6000000;
        for(long long it=0;it<iters;it++){
            int i=(int)(rng()%(S))-1; // boundary before i+1
            int maxj=min(S-1,i+W);
            if(maxj<i+2)continue;
            int j=i+2+(int)(rng()%(maxj-(i+2)+1));
            int before=0,after=0;
            if(i>=0){before+=trans(ord[i],ord[i+1]);after+=trans(ord[i],ord[j]);}
            if(j+1<S){before+=trans(ord[j],ord[j+1]);after+=trans(ord[i+1],ord[j+1]);}
            if(after>=before){
                reverse(ord.begin()+i+1,ord.begin()+j+1);
            }
        }
    }
    string out;out.reserve(S*14+16);
    out+=to_string(S);out+='\n';
    for(int s:ord){
        out+=to_string(slidePh[s].first);
        if(slidePh[s].second>=0){out+=' ';out+=to_string(slidePh[s].second);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
