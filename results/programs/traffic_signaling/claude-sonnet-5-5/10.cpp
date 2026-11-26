#include <bits/stdc++.h>
using namespace std;

int D,I,S,V,F;
vector<int> Eend, Ls;
vector<vector<int>> path;
vector<int> cyc, offs, dur_, nf;
vector<int> head, nxt, posv;
long long work=0;

long long sim(){
    fill(nf.begin(),nf.end(),0);
    fill(head.begin(),head.end(),-1);
    for(int c=V-1;c>=0;c--){posv[c]=0;nxt[c]=head[0];head[0]=c;}
    long long score=0;
    work+=D;
    for(int t=0;t<D;t++){
        int c=head[t];
        while(c!=-1){
            int nx=nxt[c];
            work++;
            int s=path[c][posv[c]];
            if(cyc[s]){
                int t0=max(t,nf[s]);
                int C=cyc[s],r=t0%C,o=offs[s],d=dur_[s];
                int g;
                if(r>=o&&r<o+d)g=t0;
                else if(r<o)g=t0+o-r;
                else g=t0+C-r+o;
                nf[s]=g+1;
                if(g<D){
                    int p=++posv[c];
                    int ns=path[c][p];
                    int arr=g+Ls[ns];
                    if(p==(int)path[c].size()-1){
                        if(arr<=D)score+=F+D-arr;
                    } else if(arr<D){
                        nxt[c]=head[arr];head[arr]=c;
                    }
                }
            }
            c=nx;
        }
    }
    return score;
}

int main(){
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    Eend.resize(S);Ls.resize(S);
    unordered_map<string,int> id;
    vector<string> names(S);
    for(int i=0;i<S;i++){
        int b,e,l;char buf[64];
        scanf("%d %d %s %d",&b,&e,buf,&l);
        names[i]=buf;id[names[i]]=i;Eend[i]=e;Ls[i]=l;
    }
    path.resize(V);
    vector<int> cnt(S,0);
    for(int v=0;v<V;v++){
        int p;scanf("%d",&p);
        path[v].resize(p);
        for(int j=0;j<p;j++){
            char buf[64];scanf("%s",buf);
            path[v][j]=id[buf];
            if(j<p-1)cnt[path[v][j]]++;
        }
    }
    cyc.assign(S,0);offs.assign(S,0);dur_.assign(S,0);nf.assign(S,0);
    head.assign(D+2,-1);nxt.assign(V,-1);posv.assign(V,0);
    vector<vector<int>> ord(I),dd(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)ord[Eend[s]].push_back(s);
    vector<int> act;
    auto upd=[&](int i){
        int C=0;
        for(int d:dd[i])C+=d;
        int o=0;
        for(size_t k=0;k<ord[i].size();k++){
            int s=ord[i][k];
            cyc[s]=C;offs[s]=o;dur_[s]=dd[i][k];o+=dd[i][k];
        }
    };
    for(int i=0;i<I;i++){
        if(ord[i].empty())continue;
        sort(ord[i].begin(),ord[i].end(),[&](int a,int b){
            if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];return a<b;});
        dd[i].assign(ord[i].size(),1);
        upd(i);
        if(ord[i].size()>1)act.push_back(i);
    }
    long long best=sim();
    mt19937 rng(12345);
    auto start=chrono::steady_clock::now();
    long long budget=350000000LL;
    int iter=0;
    while(!act.empty()&&work<budget){
        if((++iter&15)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
            if(el>8.5)break;
        }
        int i=act[rng()%act.size()];
        int n=ord[i].size();
        auto so=ord[i];auto sd=dd[i];
        if(rng()&1){
            int a=rng()%n,b=rng()%n;
            if(a==b)continue;
            swap(ord[i][a],ord[i][b]);
            swap(dd[i][a],dd[i][b]);
        } else {
            int a=rng()%n;
            if(rng()&1)dd[i][a]++;
            else{ if(dd[i][a]<=1)continue; dd[i][a]--;}
            if(dd[i][a]>D)continue;
        }
        upd(i);
        long long sc=sim();
        if(sc>=best)best=sc;
        else{ord[i]=so;dd[i]=sd;upd(i);}
    }
    int A=0;
    for(int i=0;i<I;i++)if(!ord[i].empty())A++;
    printf("%d\n",A);
    for(int i=0;i<I;i++){
        if(ord[i].empty())continue;
        printf("%d\n%d\n",i,(int)ord[i].size());
        for(size_t k=0;k<ord[i].size();k++)
            printf("%s %d\n",names[ord[i][k]].c_str(),dd[i][k]);
    }
}
