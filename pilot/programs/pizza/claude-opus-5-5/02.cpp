#include <bits/stdc++.h>
using namespace std;

int R,C,L,H;
vector<string> g;
vector<int> pm; // (R+1)*(C+1)
vector<int> owner;
vector<int> sr1,sc1,sr2,sc2; vector<char> alive;
vector<int> markv; int stampv=0;
long long score=0;
vector<pair<int,int>> shapes;
uint64_t rs=88172645463325252ULL;
inline uint64_t rnd(){rs^=rs<<13;rs^=rs>>7;rs^=rs<<17;return rs;}

inline int cntM(int r1,int c1,int r2,int c2){
    int W=C+1;
    return pm[(r2+1)*W+c2+1]-pm[r1*W+c2+1]-pm[(r2+1)*W+c1]+pm[r1*W+c1];
}
inline bool validRect(int r1,int c1,int r2,int c2){
    int a=(r2-r1+1)*(c2-c1+1); if(a>H) return false;
    int m=cntM(r1,c1,r2,c2); return m>=L && a-m>=L;
}
inline int areaOf(int id){return (sr2[id]-sr1[id]+1)*(sc2[id]-sc1[id]+1);}
int addSlice(int r1,int c1,int r2,int c2){
    int id=sr1.size();
    sr1.push_back(r1);sc1.push_back(c1);sr2.push_back(r2);sc2.push_back(c2);alive.push_back(1);markv.push_back(0);
    for(int r=r1;r<=r2;r++)for(int c=c1;c<=c2;c++)owner[r*C+c]=id;
    score+=areaOf(id);
    return id;
}
void removeSlice(int id){
    alive[id]=0;
    for(int r=sr1[id];r<=sr2[id];r++)for(int c=sc1[id];c<=sc2[id];c++)owner[r*C+c]=-1;
    score-=areaOf(id);
}
bool isFree(int r1,int c1,int r2,int c2){
    for(int r=r1;r<=r2;r++)for(int c=c1;c<=c2;c++)if(owner[r*C+c]>=0)return false;
    return true;
}
// place largest free valid slice containing (r,c)
bool bestFree(int r,int c){
    int ba=0,b1=0,b2=0,b3=0,b4=0;
    for(auto&s:shapes){
        int h=s.first,w=s.second,a=h*w; if(a<=ba)continue;
        for(int dr=0;dr<h;dr++){int r1=r-dr,r2=r1+h-1; if(r1<0||r2>=R)continue;
            for(int dc=0;dc<w;dc++){int c1=c-dc,c2=c1+w-1; if(c1<0||c2>=C)continue;
                if(a<=ba)break;
                if(!validRect(r1,c1,r2,c2))continue;
                if(!isFree(r1,c1,r2,c2))continue;
                ba=a;b1=r1;b2=c1;b3=r2;b4=c2;
            }}
    }
    if(ba>0){addSlice(b1,b2,b3,b4);return true;}
    return false;
}
void expandAll(){
    bool ch=true;
    while(ch){ch=false;
        for(size_t id=0;id<sr1.size();id++){ if(!alive[id])continue;
            bool g2=true;
            while(g2){g2=false;
                int h=sr2[id]-sr1[id]+1,w=sc2[id]-sc1[id]+1;
                // up
                if(sr1[id]>0 && (h+1)*w<=H && isFree(sr1[id]-1,sc1[id],sr1[id]-1,sc2[id])){
                    int r=sr1[id]-1; for(int c=sc1[id];c<=sc2[id];c++)owner[r*C+c]=id; sr1[id]=r; score+=w; g2=ch=true; continue;}
                if(sr2[id]<R-1 && (h+1)*w<=H && isFree(sr2[id]+1,sc1[id],sr2[id]+1,sc2[id])){
                    int r=sr2[id]+1; for(int c=sc1[id];c<=sc2[id];c++)owner[r*C+c]=id; sr2[id]=r; score+=w; g2=ch=true; continue;}
                if(sc1[id]>0 && h*(w+1)<=H && isFree(sr1[id],sc1[id]-1,sr2[id],sc1[id]-1)){
                    int c=sc1[id]-1; for(int r=sr1[id];r<=sr2[id];r++)owner[r*C+c]=id; sc1[id]=c; score+=h; g2=ch=true; continue;}
                if(sc2[id]<C-1 && h*(w+1)<=H && isFree(sr1[id],sc2[id]+1,sr2[id],sc2[id]+1)){
                    int c=sc2[id]+1; for(int r=sr1[id];r<=sr2[id];r++)owner[r*C+c]=id; sc2[id]=c; score+=h; g2=ch=true; continue;}
            }
        }
    }
}

int main(){
    if(!(cin>>R>>C>>L>>H)){printf("0\n");return 0;}
    g.resize(R); for(int i=0;i<R;i++)cin>>g[i];
    if(H<2*L){printf("0\n");return 0;}
    pm.assign((R+1)*(C+1),0);
    for(int r=0;r<R;r++)for(int c=0;c<C;c++)
        pm[(r+1)*(C+1)+c+1]=pm[r*(C+1)+c+1]+pm[(r+1)*(C+1)+c]-pm[r*(C+1)+c]+(g[r][c]=='M');
    for(int h=1;h<=H&&h<=R;h++)for(int w=1;h*w<=H&&w<=C;w++)if(h*w>=2*L)shapes.push_back({h,w});
    owner.assign(R*C,-1);
    if(shapes.empty()){printf("0\n");return 0;}
    // initial greedy: smallest valid free slice with top-left at cell
    {
        vector<pair<int,int>> sh=shapes;
        sort(sh.begin(),sh.end(),[](auto&a,auto&b){return a.first*a.second<b.first*b.second;});
        for(int r=0;r<R;r++)for(int c=0;c<C;c++){
            if(owner[r*C+c]>=0)continue;
            for(auto&s:sh){int r2=r+s.first-1,c2=c+s.second-1;
                if(r2>=R||c2>=C)continue;
                if(!validRect(r,c,r2,c2))continue;
                if(!isFree(r,c,r2,c2))continue;
                addSlice(r,c,r2,c2);break;}
        }
    }
    expandAll();
    // local search
    long long work=0; const long long LIMIT=2000000000LL;
    long long N=(long long)R*C;
    vector<int> removed;
    while(work<LIMIT && score<N){
        int cell=rnd()%N; work++;
        if(owner[cell]>=0)continue;
        int r=cell/C,c=cell%C;
        int bestD=INT_MIN,cnt=0,b1=0,b2=0,b3=0,b4=0;
        for(auto&s:shapes){
            int h=s.first,w=s.second,a=h*w;
            if(a<bestD)continue;
            for(int dr=0;dr<h;dr++){int r1=r-dr,r2=r1+h-1; if(r1<0||r2>=R)continue;
                for(int dc=0;dc<w;dc++){int c1=c-dc,c2=c1+w-1; if(c1<0||c2>=C)continue;
                    work++;
                    if(!validRect(r1,c1,r2,c2))continue;
                    ++stampv; int sum=0; bool bad=false;
                    for(int rr=r1;rr<=r2&&!bad;rr++){const int base=rr*C;
                        for(int cc=c1;cc<=c2;cc++){int o=owner[base+cc];
                            if(o>=0&&markv[o]!=stampv){markv[o]=stampv;sum+=areaOf(o); if(a-sum<bestD){bad=true;break;}}}}
                    work+=a;
                    if(bad)continue;
                    int d=a-sum;
                    if(d>bestD){bestD=d;cnt=1;b1=r1;b2=c1;b3=r2;b4=c2;}
                    else if(d==bestD){cnt++; if(rnd()%cnt==0){b1=r1;b2=c1;b3=r2;b4=c2;}}
                }}
        }
        if(cnt==0||bestD<0)continue;
        removed.clear(); ++stampv;
        for(int rr=b1;rr<=b3;rr++)for(int cc=b2;cc<=b4;cc++){int o=owner[rr*C+cc];
            if(o>=0&&markv[o]!=stampv){markv[o]=stampv;removed.push_back(o);}}
        for(int id:removed)removeSlice(id);
        addSlice(b1,b2,b3,b4);
        for(int id:removed){
            for(int rr=sr1[id];rr<=sr2[id];rr++)for(int cc=sc1[id];cc<=sc2[id];cc++){
                if(owner[rr*C+cc]<0){bestFree(rr,cc); work+=200;}
            }
        }
    }
    expandAll();
    // output
    vector<int> ids;
    for(size_t i=0;i<sr1.size();i++)if(alive[i])ids.push_back(i);
    string out; out.reserve(ids.size()*20+20);
    out+=to_string(ids.size()); out+='\n';
    for(int id:ids){
        out+=to_string(sr1[id]);out+=' ';out+=to_string(sc1[id]);out+=' ';
        out+=to_string(sr2[id]);out+=' ';out+=to_string(sc2[id]);out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
