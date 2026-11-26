#include <bits/stdc++.h>
using namespace std;

static vector<char> buf; static size_t bp=0;
static inline void skipws(){while(bp<buf.size()&&(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;}
static inline string tok(){skipws();size_t s=bp;while(bp<buf.size()&&!(buf[bp]==' '||buf[bp]=='\n'||buf[bp]=='\r'||buf[bp]=='\t'))bp++;return string(buf.data()+s,bp-s);}
static inline long long num(){skipws();long long x=0;while(bp<buf.size()&&buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}return x;}

vector<int> off, flat;
static inline int sc(int a,int b){
    const int*x=flat.data()+off[a],*xe=flat.data()+off[a+1];
    const int*y=flat.data()+off[b],*ye=flat.data()+off[b+1];
    int na=xe-x,nb=ye-y,c=0;
    while(x<xe&&y<ye){if(*x<*y)x++;else if(*x>*y)y++;else{c++;x++;y++;}}
    return min(c,min(na-c,nb-c));
}

int main(){
    {
        char tmp[1<<16];size_t r;
        while((r=fread(tmp,1,sizeof(tmp),stdin))>0)buf.insert(buf.end(),tmp,tmp+r);
    }
    int N=num();
    unordered_map<string,int> tid; tid.reserve(1<<20);
    vector<vector<int>> pt(N); vector<char> ori(N);
    for(int i=0;i<N;i++){
        string o=tok(); ori[i]=o.empty()?'H':o[0];
        int m=num(); pt[i].reserve(m);
        for(int k=0;k<m;k++){
            string t=tok();
            auto it=tid.find(t);
            int id;
            if(it==tid.end()){id=tid.size();tid.emplace(t,id);} else id=it->second;
            pt[i].push_back(id);
        }
        sort(pt[i].begin(),pt[i].end());
        pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());
    }
    int T=tid.size();
    vector<int> mark(T+1,0); int stamp=0;
    // slides
    vector<pair<int,int>> slides;
    vector<int> V;
    for(int i=0;i<N;i++){ if(ori[i]=='H') slides.push_back({i,-1}); else V.push_back(i);}
    if(V.size()>=2){
        stable_sort(V.begin(),V.end(),[&](int a,int b){return pt[a].size()>pt[b].size();});
        int nv=V.size();
        long long totv=0; for(int v:V) totv+=pt[v].size();
        double avg=max(1.0,(double)totv/nv);
        long long K=(long long)(3e8/(nv*avg)); K=max(1LL,min(200LL,K));
        vector<int> nx(nv+1),pv(nv+1);
        // nodes 0..nv-1, sentinel nv
        for(int i=0;i<nv;i++){nx[i]=i+1;pv[i]=i-1<0?nv:i-1;}
        nx[nv]=0;pv[nv]=nv-1;
        auto rem=[&](int i){nx[pv[i]]=nx[i];pv[nx[i]]=pv[i];};
        vector<char> used(nv,0);
        for(int i=0;i<nv;i++){
            if(used[i])continue;
            used[i]=1;rem(i);
            int a=V[i];stamp++;
            for(int t:pt[a])mark[t]=stamp;
            int best=-1,bu=-1,cnt=0;
            for(int j=nx[nv];j!=nv&&cnt<K;j=nx[j],cnt++){
                int b=V[j];int c=0;
                for(int t:pt[b]) if(mark[t]==stamp)c++;
                int u=(int)pt[a].size()+(int)pt[b].size()-c;
                if(u>bu){bu=u;best=j;}
            }
            if(best<0)break;
            used[best]=1;rem(best);
            slides.push_back({a,V[best]});
        }
    }
    int S=slides.size();
    if(S==0){printf("1\n0\n");return 0;}
    off.assign(S+1,0);
    for(int s=0;s<S;s++){
        auto [a,b]=slides[s];
        vector<int> u;
        if(b<0)u=pt[a];
        else{u.resize(pt[a].size()+pt[b].size());u.erase(set_union(pt[a].begin(),pt[a].end(),pt[b].begin(),pt[b].end(),u.begin()),u.end());}
        flat.insert(flat.end(),u.begin(),u.end());
        off[s+1]=flat.size();
    }
    long long totTags=flat.size();
    double avgT=max(1.0,(double)totTags/S);
    // inverted index
    vector<vector<pair<int,int>>> lists(T);
    vector<int> pos(flat.size());
    for(int s=0;s<S;s++)for(int k=off[s];k<off[s+1];k++){
        int t=flat[k];pos[k]=lists[t].size();lists[t].push_back({s,k});
    }
    vector<char> usedS(S,0);
    auto removeS=[&](int s){
        usedS[s]=1;
        for(int k=off[s];k<off[s+1];k++){
            int t=flat[k];auto&L=lists[t];int p=pos[k];
            auto last=L.back();L[p]=last;pos[last.second]=p;L.pop_back();
        }
    };
    long long cap=(long long)(1.5e9/S/avgT); cap=max(50LL,min(5000LL,cap));
    vector<int> order;order.reserve(S);
    vector<int> vis(S,0);int vst=0;
    int cur=0;removeS(0);order.push_back(0);int ptr=0;
    while((int)order.size()<S){
        stamp++;vst++;
        for(int k=off[cur];k<off[cur+1];k++)mark[flat[k]]=stamp;
        int na=off[cur+1]-off[cur];int ub=na/2;
        int best=-1,bs=-1,bsz=INT_MAX;long long checked=0,iter=0;
        bool done=false;
        for(int k=off[cur];k<off[cur+1]&&!done;k++){
            auto&L=lists[flat[k]];
            for(auto&e:L){
                if(++iter>cap*4){done=true;break;}
                int s=e.first;if(vis[s]==vst)continue;vis[s]=vst;
                int c=0;for(int q=off[s];q<off[s+1];q++)if(mark[flat[q]]==stamp)c++;
                int nb=off[s+1]-off[s];
                int v=min(c,min(na-c,nb-c));
                if(v>bs||(v==bs&&nb<bsz)){bs=v;best=s;bsz=nb;}
                if(bs>=ub){done=true;break;}
                if(++checked>=cap){done=true;break;}
            }
        }
        if(best<0){while(usedS[ptr])ptr++;best=ptr;}
        removeS(best);order.push_back(best);cur=best;
    }
    // local search: segment reversal
    if(S>=3){
        mt19937_64 rng(12345);
        long long iters=(long long)(4e8/avgT); iters=max(0LL,min(30000000LL,iters));
        int W=min(S-1,500);
        for(long long it=0;it<iters;it++){
            int i=rng()%S;
            int len=1+rng()%W;
            int j=i+len; if(j>=S)continue;
            int d=0;
            if(i>0)d+=sc(order[i-1],order[j])-sc(order[i-1],order[i]);
            if(j<S-1)d+=sc(order[i],order[j+1])-sc(order[j],order[j+1]);
            if(d>0)reverse(order.begin()+i,order.begin()+j+1);
        }
    }
    string out;out.reserve(S*14+16);
    out+=to_string(S);out+='\n';
    for(int s:order){
        out+=to_string(slides[s].first);
        if(slides[s].second>=0){out+=' ';out+=to_string(slides[s].second);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
