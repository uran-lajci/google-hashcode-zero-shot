#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64;

int H,W,R,S,WD;
vector<string> g;
vector<u64> mask_;
vector<char> covered;
vector<int> dist_;

int gainOf(int c){
    int a=c/W,b=c%W;int cnt=0;
    const u64* m=&mask_[(size_t)c*WD];
    for(int w=0;w<WD;w++){u64 x=m[w];while(x){int t=__builtin_ctzll(x);x&=x-1;int bit=w*64+t;int dx=bit/S-R,dy=bit%S-R;int cc=(a+dx)*W+(b+dy);if(!covered[cc])cnt++;}}
    return cnt;
}
void applyCov(int c){
    int a=c/W,b=c%W;
    const u64* m=&mask_[(size_t)c*WD];
    for(int w=0;w<WD;w++){u64 x=m[w];while(x){int t=__builtin_ctzll(x);x&=x-1;int bit=w*64+t;int dx=bit/S-R,dy=bit%S-R;covered[(a+dx)*W+(b+dy)]=1;}}
}
vector<int> bq;
void bfs(){
    size_t h=0;
    while(h<bq.size()){int u=bq[h++];int r=u/W,c=u%W;int du=dist_[u]+1;
        for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){if(!dr&&!dc)continue;int nr=r+dr,nc=c+dc;if(nr<0||nr>=H||nc<0||nc>=W)continue;int v=nr*W+nc;if(dist_[v]>du){dist_[v]=du;bq.push_back(v);}}}
    bq.clear();
}
int main(){
    auto T0=chrono::steady_clock::now();
    long long Pb,Pr,B;int br,bc;
    cin>>H>>W>>R>>Pb>>Pr>>B>>br>>bc;
    g.resize(H);for(int i=0;i<H;i++){cin>>g[i];if((int)g[i].size()<W)g[i].resize(W,'-');}
    S=2*R+1;WD=(S*S+63)/64;
    mask_.assign((size_t)H*W*WD,0);
    static bool ok[11][11];
    for(int a=0;a<H;a++)for(int b=0;b<W;b++){
        if(g[a][b]=='#')continue;
        u64* m=&mask_[((size_t)a*W+b)*WD];
        for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
            for(int i=0;i<=R;i++)for(int j=0;j<=R;j++){
                int x=a+sx*i,y=b+sy*j;bool v=false;
                if(x>=0&&x<H&&y>=0&&y<W&&g[x][y]!='#'){v=(i==0||ok[i-1][j])&&(j==0||ok[i][j-1]);}
                ok[i][j]=v;
                if(v&&g[x][y]=='.'){int bit=(sx*i+R)*S+(sy*j+R);m[bit>>6]|=1ULL<<(bit&63);}
            }
        }
    }
    covered.assign((size_t)H*W,0);
    dist_.assign((size_t)H*W,INT_MAX);
    int start=br*W+bc;dist_[start]=0;bq.push_back(start);bfs();
    priority_queue<pair<double,int>> pq;
    for(int c=0;c<H*W;c++){
        if(g[c/W][c%W]=='#')continue;
        int gn=gainOf(c);if(gn<=0)continue;
        double key=1000.0*gn/(Pr+Pb*(double)dist_[c]);
        pq.push({key,c});
    }
    long long rem=B;
    vector<int> backbone,routers;
    vector<char> isRouter((size_t)H*W,0);
    long long iter=0;
    while(!pq.empty()){
        if((++iter&255)==0){double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();if(el>9.0)break;}
        auto [k,c]=pq.top();pq.pop();
        if(isRouter[c])continue;
        int gn=gainOf(c);if(gn<=0)continue;
        long long cost=Pr+Pb*(long long)dist_[c];
        if(cost>rem)continue;
        if(1000LL*gn<=cost)continue;
        double nk=1000.0*gn/(double)cost;
        if(!pq.empty()&&nk<pq.top().first-1e-9){pq.push({nk,c});continue;}
        // accept
        vector<int> path;int p=c;
        while(dist_[p]>0){
            path.push_back(p);
            int r=p/W,cc=p%W;int d=dist_[p];int nxt=-1;
            for(int dr=-1;dr<=1&&nxt<0;dr++)for(int dc=-1;dc<=1;dc++){if(!dr&&!dc)continue;int nr=r+dr,nc=cc+dc;if(nr<0||nr>=H||nc<0||nc>=W)continue;int v=nr*W+nc;if(dist_[v]==d-1){nxt=v;break;}}
            p=nxt;
        }
        for(int i=(int)path.size()-1;i>=0;i--){backbone.push_back(path[i]);dist_[path[i]]=0;bq.push_back(path[i]);}
        bfs();
        rem-=Pr+Pb*(long long)path.size();
        routers.push_back(c);isRouter[c]=1;
        applyCov(c);
    }
    string out;
    out+=to_string(backbone.size())+"\n";
    for(int c:backbone)out+=to_string(c/W)+" "+to_string(c%W)+"\n";
    out+=to_string(routers.size())+"\n";
    for(int c:routers)out+=to_string(c/W)+" "+to_string(c%W)+"\n";
    fwrite(out.data(),1,out.size(),stdout);
}
