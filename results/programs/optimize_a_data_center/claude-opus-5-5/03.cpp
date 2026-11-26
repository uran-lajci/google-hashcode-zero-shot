#include <bits/stdc++.h>
using namespace std;

int main(){
    int R,S,U,P,M;
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<char> bad((size_t)R*S,0);
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[(size_t)r*S+s]=1;}
    vector<int> z(M),c(M);
    for(int i=0;i<M;i++)scanf("%d %d",&z[i],&c[i]);

    // segments per row
    vector<vector<pair<int,int>>> seg(R);
    vector<int> maxSeg(R,0);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[(size_t)r*S+s]){s++;continue;}
            int st=s; while(s<S&&!bad[(size_t)r*S+s])s++;
            seg[r].push_back({st,s-st});
            maxSeg[r]=max(maxSeg[r],s-st);
        }
    }
    multiset<int> gms; for(int r=0;r<R;r++)gms.insert(maxSeg[r]);
    set<pair<long long,int>> rowCap; vector<long long> rc(R,0);
    for(int r=0;r<R;r++)rowCap.insert({0,r});

    vector<int> ord(M); iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        long long l=(long long)c[a]*z[b], rr=(long long)c[b]*z[a];
        if(l!=rr) return l>rr;
        if(c[a]!=c[b]) return c[a]>c[b];
        return a<b;
    });
    vector<int> prow(M,-1),pslot(M,-1);
    vector<int> placed;
    for(int s:ord){
        if(gms.empty()|| *gms.rbegin()<z[s]) continue;
        for(auto it=rowCap.begin();it!=rowCap.end();++it){
            int r=it->second;
            if(maxSeg[r]<z[s])continue;
            int best=-1;
            for(int k=0;k<(int)seg[r].size();k++){
                if(seg[r][k].second>=z[s]&&(best<0||seg[r][k].second<seg[r][best].second))best=k;
            }
            prow[s]=r;pslot[s]=seg[r][best].first;
            seg[r][best].first+=z[s];seg[r][best].second-=z[s];
            gms.erase(gms.find(maxSeg[r]));
            int m=0;for(auto&q:seg[r])m=max(m,q.second);
            maxSeg[r]=m;gms.insert(m);
            rowCap.erase(it);
            rc[r]+=c[s];rowCap.insert({rc[r],r});
            placed.push_back(s);
            break;
        }
    }
    int n=placed.size();
    vector<int> pool(M,-1);
    vector<long long> rs((size_t)P*R,0),tot(P,0),mx(P,0);
    auto gcOf=[&](int p){return tot[p]-mx[p];};
    // initial assignment
    {
        vector<int> o=placed;
        sort(o.begin(),o.end(),[&](int a,int b){if(c[a]!=c[b])return c[a]>c[b];return a<b;});
        set<pair<long long,int>> st;
        for(int p=0;p<P;p++)st.insert({0,p});
        for(int s:o){
            auto it=st.begin();int p=it->second;st.erase(it);
            pool[s]=p;
            rs[(size_t)p*R+prow[s]]+=c[s];tot[p]+=c[s];
            mx[p]=max(mx[p],rs[(size_t)p*R+prow[s]]);
            st.insert({gcOf(p),p});
        }
    }
    vector<vector<int>> mem(P);vector<int> pos(M,-1);
    for(int s:placed){pos[s]=mem[pool[s]].size();mem[pool[s]].push_back(s);}
    auto recompMx=[&](int p){long long m=0;size_t b=(size_t)p*R;for(int r=0;r<R;r++)m=max(m,rs[b+r]);return m;};
    auto removeMem=[&](int s){int p=pool[s];int i=pos[s];int last=mem[p].back();mem[p][i]=last;pos[last]=i;mem[p].pop_back();};
    auto addMem=[&](int s,int p){pool[s]=p;pos[s]=mem[p].size();mem[p].push_back(s);};

    if(n>0&&P>=1){
        mt19937_64 rng(12345);
        double cost=2.0*R+P+20;
        long long iters=(long long)(6e8/cost);
        iters=min(iters,(long long)50000000);
        for(long long it=0;it<iters;it++){
            int a;
            if(rng()&1){a=0;long long g=gcOf(0);for(int p=1;p<P;p++){long long x=gcOf(p);if(x<g){g=x;a=p;}}}
            else a=rng()%P;
            bool doMove=(rng()&1)||mem[a].empty();
            if(doMove){
                int s=placed[rng()%n];int b=pool[s];if(b==a)continue;
                long long ga=gcOf(a),gb=gcOf(b),oma=mx[a],omb=mx[b];
                size_t ia=(size_t)a*R+prow[s],ib=(size_t)b*R+prow[s];
                rs[ib]-=c[s];tot[b]-=c[s];rs[ia]+=c[s];tot[a]+=c[s];
                mx[a]=max(oma,rs[ia]);
                mx[b]=(rs[ib]+c[s]==omb)?recompMx(b):omb;
                if(min(gcOf(a),gcOf(b))>=min(ga,gb)){
                    removeMem(s);addMem(s,a);
                }else{
                    rs[ib]+=c[s];tot[b]+=c[s];rs[ia]-=c[s];tot[a]-=c[s];mx[a]=oma;mx[b]=omb;
                }
            }else{
                int s=mem[a][rng()%mem[a].size()];
                int t=placed[rng()%n];int b=pool[t];if(b==a)continue;
                long long ga=gcOf(a),gb=gcOf(b),oma=mx[a],omb=mx[b];
                size_t as_=(size_t)a*R+prow[s],at=(size_t)a*R+prow[t];
                size_t bs=(size_t)b*R+prow[s],bt=(size_t)b*R+prow[t];
                rs[as_]-=c[s];rs[at]+=c[t];tot[a]+=c[t]-c[s];
                rs[bt]-=c[t];rs[bs]+=c[s];tot[b]+=c[s]-c[t];
                mx[a]=recompMx(a);mx[b]=recompMx(b);
                if(min(gcOf(a),gcOf(b))>=min(ga,gb)){
                    removeMem(s);removeMem(t);addMem(s,b);addMem(t,a);
                }else{
                    rs[as_]+=c[s];rs[at]-=c[t];tot[a]-=c[t]-c[s];
                    rs[bt]+=c[t];rs[bs]-=c[s];tot[b]-=c[s]-c[t];
                    mx[a]=oma;mx[b]=omb;
                }
            }
        }
    }
    string out;out.reserve((size_t)M*12);
    char buf[64];
    for(int i=0;i<M;i++){
        if(prow[i]<0||pool[i]<0){out+="x\n";}
        else{snprintf(buf,sizeof(buf),"%d %d %d\n",prow[i],pslot[i],pool[i]);out+=buf;}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
