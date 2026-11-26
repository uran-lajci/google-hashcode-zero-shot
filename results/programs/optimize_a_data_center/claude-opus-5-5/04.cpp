#include <bits/stdc++.h>
using namespace std;

int R,S,U,P,M;
vector<int> Z,C;
vector<int> posR,posS,pl_,poolOf;
vector<long long> cap, tot, gcv;
long long ops=0;

long long gcOf(int p){
    long long mx=0; const long long* a=&cap[(size_t)p*R];
    for(int r=0;r<R;r++) if(a[r]>mx) mx=a[r];
    ops+=R;
    return tot[p]-mx;
}

int main(){
    auto T0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<vector<char>> bad(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[r][s]=1;}
    Z.resize(M);C.resize(M);
    for(int i=0;i<M;i++) scanf("%d %d",&Z[i],&C[i]);
    // segments
    vector<vector<pair<int,int>>> seg(R);
    vector<int> maxSeg(R,0);
    vector<long long> rowCap(R,0);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[r][s]){s++;continue;}
            int t=s; while(t<S&&!bad[r][t]) t++;
            seg[r].push_back({s,t-s}); maxSeg[r]=max(maxSeg[r],t-s); s=t;
        }
    }
    vector<int> ord(M); iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        long long l=(long long)C[a]*Z[b], rr=(long long)C[b]*Z[a];
        if(l!=rr) return l>rr;
        if(C[a]!=C[b]) return C[a]>C[b];
        return a<b;});
    posR.assign(M,-1);posS.assign(M,-1);poolOf.assign(M,-1);
    for(int id:ord){
        int z=Z[id];
        int best=-1;
        for(int r=0;r<R;r++) if(maxSeg[r]>=z){ if(best<0||rowCap[r]<rowCap[best]) best=r; }
        if(best<0) continue;
        int bi=-1;
        for(int k=0;k<(int)seg[best].size();k++){
            int L=seg[best][k].second;
            if(L>=z && (bi<0||L<seg[best][bi].second)) bi=k;
        }
        posR[id]=best; posS[id]=seg[best][bi].first;
        seg[best][bi].first+=z; seg[best][bi].second-=z;
        rowCap[best]+=C[id];
        int m=0; for(auto&p:seg[best]) m=max(m,p.second); maxSeg[best]=m;
    }
    vector<int> pl;
    for(int i=0;i<M;i++) if(posR[i]>=0) pl.push_back(i);
    sort(pl.begin(),pl.end(),[&](int a,int b){ if(C[a]!=C[b]) return C[a]>C[b]; return a<b;});
    cap.assign((size_t)P*R,0); tot.assign(P,0); gcv.assign(P,0);
    vector<long long> mxr(P,0);
    for(int id:pl){
        int bp=0;
        for(int p=1;p<P;p++){
            if(gcv[p]<gcv[bp]||(gcv[p]==gcv[bp]&&tot[p]<tot[bp])) bp=p;
        }
        poolOf[id]=bp;
        cap[(size_t)bp*R+posR[id]]+=C[id]; tot[bp]+=C[id];
        mxr[bp]=max(mxr[bp],cap[(size_t)bp*R+posR[id]]);
        gcv[bp]=tot[bp]-mxr[bp];
    }
    for(int p=0;p<P;p++) gcv[p]=gcOf(p);
    int N=pl.size();
    auto curMin=[&](){ long long m=LLONG_MAX; for(int p=0;p<P;p++) m=min(m,gcv[p]); return m;};
    // best storage
    vector<int> bPool=poolOf,bR=posR,bS=posS;
    long long bestScore=curMin();
    mt19937 rng(12345);
    const long long BUDGET=400000000LL;
    struct Ch{int p,r;long long d;};
    auto applyCh=[&](Ch*ch,int n,int sgn){
        for(int i=0;i<n;i++){cap[(size_t)ch[i].p*R+ch[i].r]+=sgn*ch[i].d; }
    };
    auto evalCh=[&](Ch*ch,int n,int pa,int pb,long long dta,long long dtb,long long &ga,long long &gb){
        applyCh(ch,n,1); tot[pa]+=dta; if(pb>=0) tot[pb]+=dtb;
        ga=gcOf(pa); gb=(pb>=0&&pb!=pa)?gcOf(pb):LLONG_MAX;
        applyCh(ch,n,-1); tot[pa]-=dta; if(pb>=0) tot[pb]-=dtb;
    };
    if(N>0 && P>1){
        int stuckCount=0;
        while(ops<BUDGET){
            if((ops&0)==0){
                double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
                if(el>9.0) break;
            }
            long long gmin=LLONG_MAX; vector<int> ws;
            for(int p=0;p<P;p++){ if(gcv[p]<gmin){gmin=gcv[p];ws.clear();} if(gcv[p]==gmin) ws.push_back(p);}
            int w=ws[rng()%ws.size()];
            vector<int> W; for(int id:pl) if(poolOf[id]==w) W.push_back(id);
            bool done=false;
            int off=rng()%N;
            for(int t=0;t<N&&!done;t++){
                int b=pl[(t+off)%N]; int q=poolOf[b]; int rb=posR[b]; long long cb=C[b];
                if(q!=w){
                    Ch ch[2]={{w,rb,cb},{q,rb,-cb}};
                    long long ga,gb; evalCh(ch,2,w,q,cb,-cb,ga,gb);
                    if(min(ga,gb)>gmin){
                        applyCh(ch,2,1); tot[w]+=cb; tot[q]-=cb; poolOf[b]=w;
                        gcv[w]=ga; gcv[q]=gb; done=true; break;
                    }
                }
                for(int a:W){
                    if(a==b) continue;
                    int ra=posR[a]; long long ca=C[a];
                    if(q!=w){
                        Ch ch[4]={{w,ra,-ca},{w,rb,cb},{q,rb,-cb},{q,ra,ca}};
                        long long ga,gb; evalCh(ch,4,w,q,cb-ca,ca-cb,ga,gb);
                        if(min(ga,gb)>gmin){
                            applyCh(ch,4,1); tot[w]+=cb-ca; tot[q]+=ca-cb;
                            poolOf[a]=q; poolOf[b]=w; gcv[w]=ga; gcv[q]=gb; done=true; break;
                        }
                    }
                    if(Z[a]==Z[b]&&ra!=rb){
                        Ch ch[4]={{w,ra,-ca},{w,rb,ca},{q,rb,-cb},{q,ra,cb}};
                        long long ga,gb; evalCh(ch,4,w,(q!=w?q:-1),0,0,ga,gb);
                        if(min(ga,gb)>gmin){
                            applyCh(ch,4,1);
                            swap(posR[a],posR[b]); swap(posS[a],posS[b]);
                            gcv[w]=ga; if(q!=w) gcv[q]=gb; done=true; break;
                        }
                    }
                }
            }
            if(done){
                long long m=curMin();
                if(m>bestScore){bestScore=m;bPool=poolOf;bR=posR;bS=posS;}
                continue;
            }
            // stuck: compare with best, perturb
            long long m=curMin();
            if(m>bestScore){bestScore=m;bPool=poolOf;bR=posR;bS=posS;}
            else if(m<bestScore){
                poolOf=bPool;posR=bR;posS=bS;
                fill(cap.begin(),cap.end(),0); fill(tot.begin(),tot.end(),0);
                for(int id:pl){cap[(size_t)poolOf[id]*R+posR[id]]+=C[id]; tot[poolOf[id]]+=C[id];}
                for(int p=0;p<P;p++) gcv[p]=gcOf(p);
            }
            stuckCount++;
            int K=1+rng()%3;
            for(int k=0;k<K;k++){
                int b=pl[rng()%N]; int q=poolOf[b]; int p=rng()%P; if(p==q) continue;
                cap[(size_t)q*R+posR[b]]-=C[b]; tot[q]-=C[b];
                cap[(size_t)p*R+posR[b]]+=C[b]; tot[p]+=C[b];
                poolOf[b]=p; gcv[p]=gcOf(p); gcv[q]=gcOf(q);
            }
            ops+=N;
        }
        long long m=curMin();
        if(m>bestScore){bestScore=m;bPool=poolOf;bR=posR;bS=posS;}
        poolOf=bPool;posR=bR;posS=bS;
    }
    string out; out.reserve(M*16);
    char buf[64];
    for(int i=0;i<M;i++){
        if(posR[i]<0) out+="x\n";
        else { snprintf(buf,sizeof buf,"%d %d %d\n",posR[i],posS[i],poolOf[i]); out+=buf; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
