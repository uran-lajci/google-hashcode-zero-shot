#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
string readTok(){
    string s;int c=gc();
    while(c!=-1&&isspace(c))c=gc();
    while(c!=-1&&!isspace(c)){s.push_back((char)c);c=gc();}
    return s;
}

long long work=0;
int scv(const vector<int>&a,const vector<int>&b){
    work+=a.size()+b.size();
    size_t i=0,j=0;int c=0;
    while(i<a.size()&&j<b.size()){
        if(a[i]<b[j])i++;else if(a[i]>b[j])j++;else{c++;i++;j++;}
    }
    int x=(int)a.size()-c,y=(int)b.size()-c;
    return min(c,min(x,y));
}
vector<int> uni(const vector<int>&a,const vector<int>&b){
    vector<int> r;r.reserve(a.size()+b.size());
    set_union(a.begin(),a.end(),b.begin(),b.end(),back_inserter(r));
    return r;
}

int main(){
    int N=stoi(readTok());
    unordered_map<string,int> tmap;tmap.reserve(1<<20);
    vector<vector<int>> pt(N);vector<char> ori(N);
    for(int i=0;i<N;i++){
        string o=readTok();ori[i]=o[0];
        int m=stoi(readTok());
        for(int k=0;k<m;k++){
            string t=readTok();
            auto it=tmap.find(t);int id;
            if(it==tmap.end()){id=tmap.size();tmap.emplace(t,id);}else id=it->second;
            pt[i].push_back(id);
        }
        sort(pt[i].begin(),pt[i].end());
        pt[i].erase(unique(pt[i].begin(),pt[i].end()),pt[i].end());
    }
    int T=tmap.size();
    vector<array<int,2>> ph;
    for(int i=0;i<N;i++)if(ori[i]=='H')ph.push_back({i,-1});
    vector<int> vs;
    for(int i=0;i<N;i++)if(ori[i]=='V')vs.push_back(i);
    sort(vs.begin(),vs.end(),[&](int a,int b){if(pt[a].size()!=pt[b].size())return pt[a].size()>pt[b].size();return a<b;});
    if(vs.size()%2)vs.pop_back();
    {
        int m=vs.size(),h=m/2;
        vector<int> mark(T,-1);
        vector<int> pool;int nb=m-1; // fill from smallest
        for(int f=0;f<h;f++){
            while((int)pool.size()<100&&nb>=h)pool.push_back(vs[nb--]);
            int x=vs[f];
            for(int t:pt[x])mark[t]=f;
            int best=-1,bi=-1,bsz=-1;
            for(int k=0;k<(int)pool.size();k++){
                int y=pool[k];int c=0;
                for(int t:pt[y])if(mark[t]==f)c++;
                int sz=pt[y].size();
                if(bi<0||c<best||(c==best&&sz>bsz)){best=c;bi=k;bsz=sz;}
            }
            ph.push_back({x,pool[bi]});
            pool[bi]=pool.back();pool.pop_back();
        }
    }
    int S=ph.size();
    vector<vector<int>> tg(S);
    for(int s=0;s<S;s++){
        if(ph[s][1]<0)tg[s]=pt[ph[s][0]];
        else tg[s]=uni(pt[ph[s][0]],pt[ph[s][1]]);
    }
    // greedy
    vector<vector<int>> lists(T);
    for(int s=0;s<S;s++)for(int t:tg[s])lists[t].push_back(s);
    vector<int> bySize(S);iota(bySize.begin(),bySize.end(),0);
    sort(bySize.begin(),bySize.end(),[&](int a,int b){if(tg[a].size()!=tg[b].size())return tg[a].size()>tg[b].size();return a<b;});
    vector<char> used(S,0);
    vector<int> mark(T,-1),seen(S,-1);
    vector<int> seq;seq.reserve(S);
    long long stepBudget=max<long long>(2000,1500000000LL/max(1,S));
    int ptr=0;
    int cur=bySize[0];used[cur]=1;seq.push_back(cur);
    for(int step=1;step<S;step++){
        for(int t:tg[cur])mark[t]=step;
        int cs=tg[cur].size();int ub=cs/2;
        int best=-1,bs=-1;long long w=0;bool stop=false;
        for(int t:tg[cur]){
            auto&L=lists[t];
            for(size_t k=0;k<L.size();){
                int c=L[k];
                if(used[c]){L[k]=L.back();L.pop_back();continue;}
                k++;
                if(seen[c]==step)continue;
                seen[c]=step;
                int cm=0;for(int x:tg[c])if(mark[x]==step)cm++;
                w+=tg[c].size();
                int sc=min(cm,min(cs-cm,(int)tg[c].size()-cm));
                if(sc>best){best=sc;bs=c;}
                if(best>=ub||w>stepBudget){stop=true;break;}
            }
            if(stop)break;
        }
        if(bs<0){
            while(used[bySize[ptr]])ptr++;
            bs=bySize[ptr];
        }
        used[bs]=1;seq.push_back(bs);cur=bs;
    }
    // local search
    if(S>=3){
        vector<int> tr(S-1);
        for(int k=0;k+1<S;k++)tr[k]=scv(tg[seq[k]],tg[seq[k+1]]);
        bool hasV=false;for(int s=0;s<S;s++)if(ph[s][1]>=0){hasV=true;break;}
        mt19937 rng(12345);
        work=0;
        const long long BUDGET=1500000000LL;
        const int W=60;
        while(work<BUDGET){
            int mv=hasV?(rng()%2):0;
            if(mv==0){
                int i=rng()%S;int len=1+rng()%W;int j=i+len;
                if(j>=S){work+=5;continue;}
                int old=(i>0?tr[i-1]:0)+(j<S-1?tr[j]:0);
                int nw=(i>0?scv(tg[seq[i-1]],tg[seq[j]]):0)+(j<S-1?scv(tg[seq[i]],tg[seq[j+1]]):0);
                if(nw>=old){
                    int a1=(i>0?scv(tg[seq[i-1]],tg[seq[j]]):0);
                    int a2=(j<S-1?scv(tg[seq[i]],tg[seq[j+1]]):0);
                    reverse(seq.begin()+i,seq.begin()+j+1);
                    reverse(tr.begin()+i,tr.begin()+j);
                    if(i>0)tr[i-1]=a1;
                    if(j<S-1)tr[j]=a2;
                    work+=len;
                }
            }else{
                int p=rng()%S;int q=p+1+rng()%W;
                if(q>=S){work+=5;continue;}
                int sp=seq[p],sq=seq[q];
                if(ph[sp][1]<0||ph[sq][1]<0){work+=5;continue;}
                int a=rng()%2,b=rng()%2;
                vector<int> A=uni(pt[ph[sp][1-a]],pt[ph[sq][b]]);
                vector<int> B=uni(pt[ph[sq][1-b]],pt[ph[sp][a]]);
                work+=A.size()+B.size();
                int es[4]={p-1,p,q-1,q};int ne=0;int E[4];
                for(int e:es){if(e<0||e>S-2)continue;bool d=false;for(int z=0;z<ne;z++)if(E[z]==e)d=true;if(!d)E[ne++]=e;}
                auto tof=[&](int pos)->const vector<int>&{if(pos==p)return A;if(pos==q)return B;return tg[seq[pos]];};
                int old=0,nw=0;int nv[4];
                for(int z=0;z<ne;z++){old+=tr[E[z]];nv[z]=scv(tof(E[z]),tof(E[z]+1));nw+=nv[z];}
                if(nw>=old){
                    int pa=ph[sp][a],qb=ph[sq][b];
                    ph[sp][a]=qb;ph[sq][b]=pa;
                    tg[sp]=move(A);tg[sq]=move(B);
                    for(int z=0;z<ne;z++)tr[E[z]]=nv[z];
                }
            }
        }
    }
    string out;out.reserve(S*14+20);
    out+=to_string(S);out+='\n';
    for(int k=0;k<S;k++){
        int s=seq[k];
        out+=to_string(ph[s][0]);
        if(ph[s][1]>=0){out+=' ';out+=to_string(ph[s][1]);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
}
