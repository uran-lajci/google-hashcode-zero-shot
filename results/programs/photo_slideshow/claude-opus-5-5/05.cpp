#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline bool readTok(string&s){
    s.clear();int c=gc();while(c!=-1&&isspace(c))c=gc();
    if(c==-1)return false;
    while(c!=-1&&!isspace(c)){s.push_back((char)c);c=gc();}
    return true;
}

struct Slide{vector<int> t;int a,b;};
vector<Slide> sl;

static inline int scoreOf(const vector<int>&A,const vector<int>&B){
    size_t i=0,j=0;int c=0;
    while(i<A.size()&&j<B.size()){
        if(A[i]<B[j])i++;else if(A[i]>B[j])j++;else{c++;i++;j++;}
    }
    int a=A.size()-c,b=B.size()-c;
    return min(c,min(a,b));
}

int main(){
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    string s;
    if(!readTok(s)){printf("0\n");return 0;}
    int N=stoi(s);
    unordered_map<string,int> tagId;tagId.reserve(1<<20);
    vector<vector<int>> ptags(N);vector<char> isV(N);
    for(int i=0;i<N;i++){
        readTok(s);isV[i]=(s[0]=='V');
        readTok(s);int m=stoi(s);
        ptags[i].resize(m);
        for(int k=0;k<m;k++){
            readTok(s);
            auto it=tagId.find(s);
            int id;
            if(it==tagId.end()){id=tagId.size();tagId.emplace(s,id);}else id=it->second;
            ptags[i][k]=id;
        }
        sort(ptags[i].begin(),ptags[i].end());
        ptags[i].erase(unique(ptags[i].begin(),ptags[i].end()),ptags[i].end());
    }
    int TG=tagId.size();
    vector<int> mark(TG+1,0);int stamp=0;
    for(int i=0;i<N;i++)if(!isV[i])sl.push_back({ptags[i],i,-1});
    // pair verticals
    vector<int> vs;
    for(int i=0;i<N;i++)if(isV[i])vs.push_back(i);
    stable_sort(vs.begin(),vs.end(),[&](int x,int y){return ptags[x].size()>ptags[y].size();});
    int V=vs.size();
    if(V>=2){
        vector<int> nx(V+1),pv(V+1);
        // indices 0..V-1, sentinel V
        for(int i=0;i<V;i++){nx[i]=i+1;pv[i]=i-1;}
        int head=0;
        auto rem=[&](int i){
            if(pv[i]>=0)nx[pv[i]]=nx[i];else head=nx[i];
            if(nx[i]<V)pv[nx[i]]=pv[i];
        };
        const int K=150;
        while(head<V){
            int h=head;rem(h);
            if(head>=V)break;
            ++stamp;
            for(int t:ptags[vs[h]])mark[t]=stamp;
            int best=-1,bi=-1,cnt=0;
            for(int c=head;c<V&&cnt<K;c=nx[c],cnt++){
                int com=0;for(int t:ptags[vs[c]])if(mark[t]==stamp)com++;
                int u=ptags[vs[h]].size()+ptags[vs[c]].size()-com;
                if(u>best){best=u;bi=c;}
            }
            rem(bi);
            vector<int> u;
            const auto&A=ptags[vs[h]];const auto&B=ptags[vs[bi]];
            set_union(A.begin(),A.end(),B.begin(),B.end(),back_inserter(u));
            sl.push_back({u,vs[h],vs[bi]});
        }
    }
    int S=sl.size();
    if(S==0){printf("0\n");return 0;}
    vector<vector<int>> inv(TG);
    for(int i=0;i<S;i++)for(int t:sl[i].t)inv[t].push_back(i);
    // greedy
    vector<char> used(S,0);
    vector<int> order;order.reserve(S);
    int cur=0;used[0]=1;order.push_back(0);
    int fb=0;
    const int CAP=600;
    for(int step=1;step<S;step++){
        ++stamp;
        const auto&CT=sl[cur].t;
        for(int t:CT)mark[t]=stamp;
        int a=CT.size(),ub=a/2;
        int best=-1,bestId=-1,evals=0;
        bool stop=false;
        for(int t:CT){
            auto&L=inv[t];
            for(size_t k=0;k<L.size();){
                int x=L[k];
                if(used[x]){L[k]=L.back();L.pop_back();continue;}
                int c=0;const auto&XT=sl[x].t;
                for(int y:XT)if(mark[y]==stamp)c++;
                int sc=min(c,min(a-c,(int)XT.size()-c));
                if(sc>best){best=sc;bestId=x;}
                evals++;k++;
                if(evals>=CAP||best>=ub){stop=true;break;}
            }
            if(stop)break;
        }
        if(bestId<0){
            while(used[fb])fb++;
            bestId=fb;
        }
        used[bestId]=1;order.push_back(bestId);cur=bestId;
    }
    // rebuild inverted index for local search
    for(auto&L:inv)L.clear();
    for(int i=0;i<S;i++)for(int t:sl[i].t)inv[t].push_back(i);
    vector<int> pos(S);
    for(int i=0;i<S;i++)pos[order[i]]=i;
    uint64_t rs=88172645463325252ULL;
    auto rnd=[&](){rs^=rs<<13;rs^=rs>>7;rs^=rs<<17;return rs;};
    auto sc=[&](int p,int q)->int{ // positions
        if(p<0||q<0||p>=S||q>=S)return 0;
        return scoreOf(sl[order[p]].t,sl[order[q]].t);
    };
    auto reverseSeg=[&](int l,int r){
        while(l<r){swap(order[l],order[r]);pos[order[l]]=l;pos[order[r]]=r;l++;r--;}
    };
    const double TL=9.3;
    if(S>=3){
        long long it=0;
        while(true){
            if((it&255)==0&&elapsed()>TL)break;
            it++;
            int i=1+rnd()%(S-1); // boundary between i-1 and i
            int L=order[i-1];
            const auto&LT=sl[L].t;
            int t=LT[rnd()%LT.size()];
            const auto&lst=inv[t];
            if(lst.size()<2)continue;
            int x=lst[rnd()%lst.size()];
            int j=pos[x];
            if(j==i-1||j==i)continue;
            if(j>i){
                // reverse [i,j]: (i-1,i),(j,j+1) -> (i-1,j),(i,j+1)
                int len=j-i+1;
                if(len>30000)continue;
                int d=sc(i-1,j)+sc(i,j+1)-sc(i-1,i)-sc(j,j+1);
                if(d>0||(d==0&&len<=300))reverseSeg(i,j);
            }else{
                // j<i-1: reverse [j+1,i-1]: (j,j+1),(i-1,i) -> (j,i-1),(j+1,i)
                int len=i-1-j;
                if(len>30000)continue;
                int d=sc(j,i-1)+sc(j+1,i)-sc(j,j+1)-sc(i-1,i);
                if(d>0||(d==0&&len<=300))reverseSeg(j+1,i-1);
            }
        }
    }
    string out;out.reserve(S*14+16);
    out+=to_string(S);out+='\n';
    for(int i=0;i<S;i++){
        const Slide&z=sl[order[i]];
        out+=to_string(z.a);
        if(z.b>=0){out+=' ';out+=to_string(z.b);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
