#include <bits/stdc++.h>
using namespace std;

int R,S,U,P,M;
vector<int> Z,C,RowOf,SlotOf,PoolOf;
vector<long long> pc; // P*R
vector<long long> tot, mx;
vector<vector<int>> members;
vector<int> posInPool;

inline long long gcOf(int p){return tot[p]-mx[p];}
void recomputeMx(int p){long long m=0;for(int r=0;r<R;r++)m=max(m,pc[(size_t)p*R+r]);mx[p]=m;}
// evaluate gc of pool p after deltas at rows
long long evalAfter(int p,int r1,long long d1,int r2,long long d2){
    long long t=tot[p]+d1+d2; long long m=0;
    for(int r=0;r<R;r++){long long v=pc[(size_t)p*R+r];if(r==r1)v+=d1;if(r==r2)v+=d2;if(v>m)m=v;}
    return t-m;
}
void addToPool(int s,int p){PoolOf[s]=p;posInPool[s]=members[p].size();members[p].push_back(s);}
void removeFromPool(int s){int p=PoolOf[s];int i=posInPool[s];int l=members[p].back();members[p][i]=l;posInPool[l]=i;members[p].pop_back();PoolOf[s]=-1;}

int main(){
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5)return 0;
    vector<vector<char>> bad(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int a,b;scanf("%d %d",&a,&b);bad[a][b]=1;}
    Z.resize(M);C.resize(M);RowOf.assign(M,-1);SlotOf.assign(M,-1);PoolOf.assign(M,-1);posInPool.assign(M,-1);
    for(int i=0;i<M;i++)scanf("%d %d",&Z[i],&C[i]);
    vector<vector<pair<int,int>>> segs(R);
    vector<int> maxf(R,0);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){if(bad[r][s]){s++;continue;}int e=s;while(e<S&&!bad[r][e])e++;segs[r].push_back({s,e-s});maxf[r]=max(maxf[r],e-s);s=e;}
    }
    vector<int> order(M);iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){
        long long l=(long long)C[a]*Z[b],rr=(long long)C[b]*Z[a];
        if(l!=rr)return l>rr; if(C[a]!=C[b])return C[a]>C[b]; return a<b;});
    vector<long long> rowcap(R,0);
    set<pair<long long,int>> rs;
    for(int r=0;r<R;r++)if(maxf[r]>0)rs.insert({0,r});
    int failSize=INT_MAX;
    vector<int> placed;
    for(int s:order){
        int z=Z[s]; if(z>=failSize)continue;
        int found=-1;
        for(auto&pr:rs){if(maxf[pr.second]>=z){found=pr.second;break;}}
        if(found<0){failSize=z;continue;}
        int r=found;
        int bi=-1;
        for(int i=0;i<(int)segs[r].size();i++){if(segs[r][i].second>=z&&(bi<0||segs[r][i].second<segs[r][bi].second))bi=i;}
        RowOf[s]=r;SlotOf[s]=segs[r][bi].first;
        segs[r][bi].first+=z;segs[r][bi].second-=z;
        if(segs[r][bi].second==0)segs[r].erase(segs[r].begin()+bi);
        int m=0;for(auto&sg:segs[r])m=max(m,sg.second);maxf[r]=m;
        rs.erase({rowcap[r],r});rowcap[r]+=C[s];
        if(maxf[r]>0)rs.insert({rowcap[r],r});
        placed.push_back(s);
    }
    // pool assignment
    pc.assign((size_t)P*R,0);tot.assign(P,0);mx.assign(P,0);members.assign(P,{});
    vector<int> ps=placed;
    sort(ps.begin(),ps.end(),[&](int a,int b){if(C[a]!=C[b])return C[a]>C[b];return a<b;});
    set<pair<long long,int>> pq;
    for(int p=0;p<P;p++)pq.insert({0,p});
    for(int s:ps){
        auto it=pq.begin();int p=it->second;pq.erase(it);
        addToPool(s,p);
        size_t idx=(size_t)p*R+RowOf[s];
        pc[idx]+=C[s];tot[p]+=C[s];mx[p]=max(mx[p],pc[idx]);
        pq.insert({gcOf(p),p});
    }
    // local search
    int NP=placed.size();
    if(NP>0){
        int maxZ=0;for(int s:placed)maxZ=max(maxZ,Z[s]);
        vector<vector<int>> bySize(maxZ+1);
        for(int s:placed)bySize[Z[s]].push_back(s);
        mt19937_64 rng(12345);
        long long iters=600000000LL/(2LL*R+P+20);
        for(long long it=0;it<iters;it++){
            // find worst pool
            long long best=LLONG_MAX;int w=-1;int cnt=0;
            for(int p=0;p<P;p++){long long g=gcOf(p);
                if(g<best){best=g;w=p;cnt=1;}
                else if(g==best){cnt++;if(rng()%cnt==0)w=p;}}
            long long gw=best;
            int type=rng()%3;
            if(members[w].empty())type=0;
            if(type==0){
                int s=placed[rng()%NP];int q=PoolOf[s];if(q==w)continue;
                int r=RowOf[s];
                long long nw=evalAfter(w,r,C[s],-1,0);if(nw<=gw)continue;
                long long nq=evalAfter(q,r,-C[s],-1,0);if(nq<=gw)continue;
                removeFromPool(s);addToPool(s,w);
                pc[(size_t)w*R+r]+=C[s];tot[w]+=C[s];
                pc[(size_t)q*R+r]-=C[s];tot[q]-=C[s];
                recomputeMx(w);recomputeMx(q);
            }else if(type==1){
                int s=members[w][rng()%members[w].size()];
                int t=placed[rng()%NP];int q=PoolOf[t];if(q==w)continue;
                int rsw=RowOf[s],rt=RowOf[t];
                long long nw=evalAfter(w,rsw,-C[s],rt,C[t]);if(nw<=gw)continue;
                long long nq=evalAfter(q,rt,-C[t],rsw,C[s]);if(nq<=gw)continue;
                removeFromPool(s);removeFromPool(t);addToPool(s,q);addToPool(t,w);
                pc[(size_t)w*R+rsw]-=C[s];pc[(size_t)w*R+rt]+=C[t];tot[w]+=C[t]-C[s];
                pc[(size_t)q*R+rt]-=C[t];pc[(size_t)q*R+rsw]+=C[s];tot[q]+=C[s]-C[t];
                recomputeMx(w);recomputeMx(q);
            }else{
                int s=members[w][rng()%members[w].size()];
                auto&lst=bySize[Z[s]];
                int t=lst[rng()%lst.size()];
                if(RowOf[t]==RowOf[s])continue;
                int q=PoolOf[t];int r1=RowOf[s],r2=RowOf[t];
                if(q==w){
                    long long nw=evalAfter(w,r1,(long long)C[t]-C[s],r2,(long long)C[s]-C[t]);
                    if(nw<=gw)continue;
                    pc[(size_t)w*R+r1]+=C[t]-C[s];pc[(size_t)w*R+r2]+=C[s]-C[t];
                    recomputeMx(w);
                }else{
                    long long nw=evalAfter(w,r1,-C[s],r2,C[s]);if(nw<=gw)continue;
                    long long nq=evalAfter(q,r2,-C[t],r1,C[t]);if(nq<=gw)continue;
                    pc[(size_t)w*R+r1]-=C[s];pc[(size_t)w*R+r2]+=C[s];
                    pc[(size_t)q*R+r2]-=C[t];pc[(size_t)q*R+r1]+=C[t];
                    recomputeMx(w);recomputeMx(q);
                }
                swap(RowOf[s],RowOf[t]);swap(SlotOf[s],SlotOf[t]);
            }
        }
    }
    string out;out.reserve((size_t)M*14);
    char buf[64];
    for(int i=0;i<M;i++){
        if(RowOf[i]<0||PoolOf[i]<0){out+="x\n";}
        else{int n=snprintf(buf,sizeof(buf),"%d %d %d\n",RowOf[i],SlotOf[i],PoolOf[i]);out.append(buf,n);}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
