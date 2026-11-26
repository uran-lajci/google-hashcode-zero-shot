#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
int main(){
    scanf("%d %d %d",&R,&C,&A);
    scanf("%d %d %d %d",&L,&V,&B,&T);
    scanf("%d %d",&rs,&cs);
    vector<int> tr(L),tc(L);
    vector<vector<int>> rowT(R);
    for(int i=0;i<L;i++){scanf("%d %d",&tr[i],&tc[i]);rowT[tr[i]].push_back(tc[i]);}
    size_t N=(size_t)A*R*C;
    vector<signed char> da(N),db(N);
    for(size_t i=0;i<N;i++){int a,b;scanf("%d %d",&a,&b);da[i]=a;db[i]=b;}
    // distance potential
    vector<int> dist((size_t)R*C,1e9);
    {
        vector<int> q;q.reserve((size_t)R*C);
        for(int i=0;i<L;i++){dist[(size_t)tr[i]*C+tc[i]]=0;q.push_back(tr[i]*C+tc[i]);}
        for(size_t h=0;h<q.size();h++){
            int r=q[h]/C,c=q[h]%C;int d=dist[q[h]];
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){
                if(!dr&&!dc)continue;
                int nr=r+dr;if(nr<0||nr>=R)continue;
                int nc=(c+dc+C)%C;
                if(dist[(size_t)nr*C+nc]>d+1){dist[(size_t)nr*C+nc]=d+1;q.push_back(nr*C+nc);}
            }
        }
    }
    vector<int> alt(B,0),pr(B,rs),pc(B,cs);
    vector<char> alive(B,1);
    vector<vector<int>> out(T,vector<int>(B,0));
    vector<int> mark((size_t)R*C,-1);
    // targets index map
    vector<int> tid((size_t)R*C,-1);
    for(int i=0;i<L;i++)tid[(size_t)tr[i]*C+tc[i]]=i;
    vector<int> covT(L,-1);
    auto t0=chrono::steady_clock::now();
    bool fast=false;
    int launchedCount=0;
    auto cnt=[&](int r,int c,int turn,bool apply){
        int n=0;
        for(int u=max(0,r-V);u<=min(R-1,r+V);u++){
            int dr=r-u;
            for(int v:rowT[u]){
                int d=abs(c-v);d=min(d,C-d);
                if(dr*dr+d*d<=V*V){
                    int id=tid[(size_t)u*C+v];
                    if(covT[id]!=turn){n++;if(apply)covT[id]=turn;}
                }
            }
        }
        return n;
    };
    for(int t=0;t<T;t++){
        if(!fast&&(t&7)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
            if(el>6.5)fast=true;
        }
        bool launchedThis=false;
        for(int b=0;b<B;b++){
            if(!alive[b])continue;
            int bestO=0;long long bestS=LLONG_MIN;
            for(int o=-1;o<=1;o++){
                int na=alt[b]+o;
                if(na<1&&alt[b]==0&&o==0){
                    // stay
                    long long s=-1000000;
                    // staying on ground: tiny score
                    if(bestS<s){bestS=s;bestO=0;}
                    continue;
                }
                if(alt[b]==0&&o==-1)continue;
                if(na<1||na>A)continue;
                if(alt[b]==0&&o==1&&launchedThis)continue;
                size_t idx=(size_t)(na-1)*R*C+(size_t)pr[b]*C+pc[b];
                int nr=pr[b]+da[idx];
                if(nr<0||nr>=R){
                    long long s=-100000000LL;
                    if(s>bestS){bestS=s;bestO=o;}
                    continue;
                }
                int nc=((pc[b]+db[idx])%C+C)%C;
                int d=dist[(size_t)nr*C+nc];
                long long s=0;
                if(!fast&&d<=V)s=(long long)cnt(nr,nc,t,false)*1000;
                s-=min(d,100000);
                if(alt[b]==0)s-=0;
                if(s>bestS){bestS=s;bestO=o;}
            }
            if(alt[b]==0){
                if(bestO==1){launchedThis=true;alt[b]=1;}
                else {continue;}
            } else alt[b]+=bestO;
            out[t][b]=bestO;
            size_t idx=(size_t)(alt[b]-1)*R*C+(size_t)pr[b]*C+pc[b];
            int nr=pr[b]+da[idx];
            if(nr<0||nr>=R){alive[b]=0;continue;}
            pc[b]=((pc[b]+db[idx])%C+C)%C;pr[b]=nr;
            if(!fast&&dist[(size_t)nr*C+pc[b]]<=V)cnt(nr,pc[b],t,true);
        }
    }
    string s;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b)s+=' ';
            s+=to_string(out[t][b]);
        }
        s+='\n';
    }
    fputs(s.c_str(),stdout);
}
