#include <bits/stdc++.h>
using namespace std;

static char*buf;size_t bl,bp=0;
static int rd(){while(bp<bl&&buf[bp]!='-'&&(buf[bp]<'0'||buf[bp]>'9'))bp++;bool n=false;if(bp<bl&&buf[bp]=='-'){n=true;bp++;}int x=0;while(bp<bl&&buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}return n?-x:x;}

int R,C,A,L,V,B,T,rs,cs;
vector<int> tr,tc;
vector<vector<int>> rowT; // targets by row
vector<int> dest; // (cell*A+a) -> cell or -1
vector<uint8_t> covered; // T*L
chrono::steady_clock::time_point st;
double el(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

template<class F> void targetsNear(int cell,F f){
    int r=cell/C,c=cell%C;
    for(int rr=max(0,r-V);rr<=min(R-1,r+V);rr++){
        int w2=V*V-(rr-r)*(rr-r);
        for(int j:rowT[rr]){int cd=abs(c-tc[j]);cd=min(cd,C-cd);if(cd*cd<=w2)f(j);}
    }
}
template<class F> void cellsCovering(int j,F f){
    int r=tr[j],c=tc[j];
    for(int rr=max(0,r-V);rr<=min(R-1,r+V);rr++){
        int w2=V*V-(rr-r)*(rr-r);int w=(int)sqrt((double)w2);
        while((w+1)*(w+1)<=w2)w++;while(w*w>w2)w--;
        if(2*w+1>=C){for(int cc=0;cc<C;cc++)f(rr*C+cc);}
        else for(int d=-w;d<=w;d++){int cc=((c+d)%C+C)%C;f(rr*C+cc);}
    }
}

int main(){
    st=chrono::steady_clock::now();
    {vector<char> v;size_t cap=1<<20;v.resize(cap);size_t n=0;while(true){if(n==cap){cap*=2;v.resize(cap);}size_t g=fread(v.data()+n,1,cap-n,stdin);if(g==0)break;n+=g;}buf=new char[n+1];memcpy(buf,v.data(),n);bl=n;}
    R=rd();C=rd();A=rd();L=rd();V=rd();B=rd();T=rd();rs=rd();cs=rd();
    tr.resize(L);tc.resize(L);rowT.assign(R,{});
    for(int i=0;i<L;i++){tr[i]=rd();tc[i]=rd();rowT[tr[i]].push_back(i);}
    long long RC=(long long)R*C;long long S=RC*A;
    dest.assign(S,-1);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int dr=rd(),dc=rd();int nr=r+dr;
        if(nr>=0&&nr<R){int nc=((c+dc)%C+C)%C;dest[(long long)(r*C+c)*A+a]=nr*C+nc;}
    }
    covered.assign((size_t)T*L,0);
    vector<vector<int8_t>> out(T,vector<int8_t>(B,0));
    int startCell=rs*C+cs;
    bool dpMode=(S<=30000000LL)&&((long long)T*S<=150000000LL);
    if(dpMode){
        vector<int> M(S);
        for(long long s=0;s<S;s++){int a=s%A;M[s]=dest[s]>=0?dest[s]*A+a:(int)S;}
        vector<int16_t> base(RC,0);
        for(int j=0;j<L;j++)cellsCovering(j,[&](int x){base[x]++;});
        vector<int16_t> gain((size_t)T*RC);
        for(int t=0;t<T;t++)memcpy(&gain[(size_t)t*RC],base.data(),RC*2);
        vector<int> F(S+1),W(S+1);
        vector<uint8_t> choice((size_t)T*S);
        vector<uint8_t> gc(T);
        vector<int> m(A);
        double last=0;
        for(int b=0;b<B;b++){
            double t0=el();
            if(t0+last*1.15>9.3)break;
            fill(F.begin(),F.end(),0);
            int G=0;
            for(int t=T-1;t>=0;t--){
                const int16_t*g=&gain[(size_t)t*RC];
                for(long long cell=0;cell<RC;cell++){int gv=g[cell];long long bs=cell*A;for(int a=0;a<A;a++)W[bs+a]=gv+F[bs+a];}
                W[S]=0;
                int lv=W[M[(long long)startCell*A]];
                if(lv>G){G=lv;gc[t]=1;}else gc[t]=0;
                if(t>0){
                    uint8_t*ch=&choice[(size_t)t*S];
                    for(long long cell=0;cell<RC;cell++){
                        long long bs=cell*A;
                        for(int a=0;a<A;a++)m[a]=W[M[bs+a]];
                        for(int a=0;a<A;a++){
                            int bv=m[a],bd=1;
                            if(a>0&&m[a-1]>bv){bv=m[a-1];bd=0;}
                            if(a+1<A&&m[a+1]>bv){bv=m[a+1];bd=2;}
                            F[bs+a]=bv;ch[bs+a]=bd;
                        }
                    }
                }
            }
            if(G<=0){last=el()-t0;break;}
            // reconstruct
            int state=-1;bool ground=true,lost=false;
            for(int t=0;t<T;t++){
                if(lost){out[t][b]=0;continue;}
                int x;
                if(ground){
                    if(!gc[t]){out[t][b]=0;continue;}
                    out[t][b]=1;ground=false;x=M[(long long)startCell*A];
                }else{
                    int d=(int)choice[(size_t)t*S+state]-1;out[t][b]=d;
                    int cell=state/A,a=state%A+d;x=M[(long long)cell*A+a];
                }
                if(x==S){lost=true;continue;}
                state=x;
                int cell=x/A;
                int16_t*g=&gain[(size_t)t*RC];
                targetsNear(cell,[&](int j){
                    uint8_t&cv=covered[(size_t)t*L+j];
                    if(!cv){cv=1;cellsCovering(j,[&](int y){g[y]--;});}
                });
            }
            last=el()-t0;
        }
    }else{
        auto gainAt=[&](int t,int cell){int cnt=0;targetsNear(cell,[&](int j){if(!covered[(size_t)t*L+j])cnt++;});return cnt;};
        function<int(int,int,int,int)> ev=[&](int t,int cell,int a,int depth)->int{
            if(t>=T||depth==0)return 0;
            int best=INT_MIN;
            for(int d=-1;d<=1;d++){int a2=a+d;if(a2<0||a2>=A)continue;
                int x=dest[(long long)cell*A+a2];int v;
                if(x<0)v=-1000000;else v=gainAt(t,x)+ev(t+1,x,a2,depth-1);
                best=max(best,v);}
            return best;
        };
        for(int b=0;b<B;b++){
            if(el()>9.0)break;
            int cell=startCell,a=-1;bool lost=false;
            for(int t=0;t<T;t++){
                if(lost){out[t][b]=0;continue;}
                int d;
                if(a<0){d=1;a=0;}
                else if(el()>9.3){d=0;}
                else{
                    int best=INT_MIN;d=0;
                    for(int dd=-1;dd<=1;dd++){int a2=a+dd;if(a2<0||a2>=A)continue;
                        int x=dest[(long long)cell*A+a2];int v;
                        if(x<0)v=-1000000;else v=gainAt(t,x)+ev(t+1,x,a2,2);
                        if(v>best){best=v;d=dd;}}
                    a+=d;
                }
                out[t][b]=d;
                int x=dest[(long long)cell*A+a];
                if(x<0){lost=true;continue;}
                cell=x;
                targetsNear(cell,[&](int j){covered[(size_t)t*L+j]=1;});
            }
        }
    }
    string s;s.reserve((size_t)T*B*3+T);
    for(int t=0;t<T;t++){for(int b=0;b<B;b++){if(b)s+=' ';int v=out[t][b];if(v<0)s+="-1";else s+=char('0'+v);}s+='\n';}
    fwrite(s.data(),1,s.size(),stdout);
}
