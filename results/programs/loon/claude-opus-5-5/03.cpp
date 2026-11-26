#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static char*buf;size_t bp=0,bl=0;
int readInt(){while(bp<bl&&!(buf[bp]=='-'||(buf[bp]>='0'&&buf[bp]<='9')))bp++;bool n=false;if(bp<bl&&buf[bp]=='-'){n=true;bp++;}int x=0;while(bp<bl&&buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}return n?-x:x;}
int R,C,A,L,V,B,T,RC,startCell;
vector<int> dest;vector<int16_t> gainv;vector<char> covered;
vector<int> tcStart,tcells,clStart,clist;
auto T0=chrono::steady_clock::now();
double el(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}
void commit(const vector<int>&adj){
    int alt=0,pos=startCell;bool lost=false;
    for(int t=0;t<T;t++){alt+=adj[t];if(alt>0&&!lost){int d=dest[(size_t)(alt-1)*RC+pos];if(d<0){lost=true;continue;}pos=d;
        for(int k=clStart[pos];k<clStart[pos+1];k++){int tg=clist[k];char&cv=covered[(size_t)t*L+tg];if(!cv){cv=1;int16_t*g=&gainv[(size_t)t*RC];for(int j=tcStart[tg];j<tcStart[tg+1];j++)g[tcells[j]]--;}}}}
}
int main(){
    // read all
    {std::string s((istreambuf_iterator<char>(cin)),istreambuf_iterator<char>());buf=new char[s.size()+1];memcpy(buf,s.data(),s.size());bl=s.size();}
    R=readInt();C=readInt();A=readInt();L=readInt();V=readInt();B=readInt();T=readInt();
    int rs=readInt(),cs=readInt();RC=R*C;startCell=rs*C+cs;
    vector<int> tr(L),tcc(L);for(int i=0;i<L;i++){tr[i]=readInt();tcc[i]=readInt();}
    vector<vector<int>> plan(B,vector<int>(T,0));
    auto output=[&](){string o;o.reserve((size_t)T*B*3);for(int t=0;t<T;t++){for(int b=0;b<B;b++){if(b)o+=' ';o+=to_string(plan[b][t]);}o+='\n';}fwrite(o.data(),1,o.size(),stdout);};
    if((ll)A*RC>300000000LL||(ll)T*RC>500000000LL){output();return 0;}
    dest.assign((size_t)A*RC,-1);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){int dr=readInt(),dc=readInt();int nr=r+dr;if(nr<0||nr>=R)continue;int nc=((c+dc)%C+C)%C;dest[(size_t)a*RC+r*C+nc*0+0]=0;dest[(size_t)a*RC+r*C+c]=nr*C+nc;}
    // target cells
    tcStart.assign(L+1,0);
    for(int i=0;i<L;i++){tcStart[i]=tcells.size();int r=tr[i],c=tcc[i];
        for(int dr=-V;dr<=V;dr++){int r2=r+dr;if(r2<0||r2>=R)continue;
            if(C<=2*V+1){for(int c2=0;c2<C;c2++){int d=abs(c-c2);d=min(d,C-d);if(dr*dr+d*d<=V*V)tcells.push_back(r2*C+c2);}}
            else for(int dc=-V;dc<=V;dc++)if(dr*dr+dc*dc<=V*V)tcells.push_back(r2*C+((c+dc)%C+C)%C);}
    }
    tcStart[L]=tcells.size();
    clStart.assign(RC+1,0);for(int x:tcells)clStart[x+1]++;for(int i=0;i<RC;i++)clStart[i+1]+=clStart[i];
    clist.resize(tcells.size());{vector<int> p(clStart.begin(),clStart.end()-1);for(int i=0;i<L;i++)for(int j=tcStart[i];j<tcStart[i+1];j++)clist[p[tcells[j]]++]=i;}
    gainv.assign((size_t)T*RC,0);
    for(int c=0;c<RC;c++){int16_t v=clStart[c+1]-clStart[c];for(int t=0;t<T;t++)gainv[(size_t)t*RC+c]=v;}
    covered.assign((size_t)T*L,0);
    size_t S=(size_t)A*RC;
    bool dpOk=(ll)T*S<=450000000LL;
    vector<int> prevv,cur,nxt;
    if(dpOk){prevv.resize((size_t)T*S);cur.resize(S);nxt.resize(S);}
    double dpEst=(double)T*S*3*1.5e-9;
    vector<vector<int>> bases; // adjustments from launch
    {int nh=min(A,30);for(int i=0;i<nh;i++){int h=1+(ll)i*(A-1)/max(1,nh-1);if(nh==1)h=1;vector<int> a(T,0);for(int k=0;k<min(h,T);k++)a[k]=1;bases.push_back(a);}}
    const double LIM=9.0;
    for(int b=0;b<B;b++){
        if(dpOk&&el()+dpEst*1.2<LIM){
            double ts=el();
            int bestv=0,bt=-1,bi=-1;
            int sd=dest[startCell];
            for(int t=0;t<T;t++){
                fill(nxt.begin(),nxt.end(),-1);
                int*pv=&prevv[(size_t)t*S];
                const int16_t*g=&gainv[(size_t)t*RC];
                if(t>0){
                    for(int a=0;a<A;a++){const int*cr=&cur[(size_t)a*RC];
                        int lo=max(0,a-1),hi=min(A-1,a+1);
                        for(int c=0;c<RC;c++){int v=cr[c];if(v<0)continue;
                            if(v>bestv){bestv=v;bt=t-1;bi=a*RC+c;}
                            for(int a2=lo;a2<=hi;a2++){int d=dest[(size_t)a2*RC+c];if(d<0)continue;int nv=v+g[d];size_t idx=(size_t)a2*RC+d;if(nv>nxt[idx]){nxt[idx]=nv;pv[idx]=a*RC+c;}}
                        }}
                }
                if(sd>=0){int nv=g[sd];if(nv>nxt[sd]){nxt[sd]=nv;pv[sd]=-1;}}
                swap(cur,nxt);
            }
            for(size_t i=0;i<S;i++)if(cur[i]>bestv){bestv=cur[i];bt=T-1;bi=i;}
            vector<int> adj(T,0);
            if(bt>=0){vector<int> alt(T,0);int t=bt;int idx=bi;
                while(true){alt[t]=idx/RC+1;int p=prevv[(size_t)t*S+idx];if(p<0)break;idx=p;t--;}
                int prevAlt=0;for(int k=0;k<=bt;k++){adj[k]=alt[k]-prevAlt;prevAlt=alt[k];}
                int lt=-1;for(int k=0;k<T;k++)if(adj[k]){lt=k;break;}
                if(lt>=0){vector<int> bs(T,0);for(int k=lt;k<T;k++)bs[k-lt]=adj[k];bases.push_back(bs);}
            }
            plan[b]=adj;commit(adj);
            double tk=el()-ts;dpEst=max(dpEst*0.3,tk);
        } else {
            if(el()>9.6)continue;
            int nb=bases.size();
            vector<vector<int>> traj(nb,vector<int>(T,-1));
            for(int i=0;i<nb;i++){int alt=0,pos=startCell;for(int k=0;k<T;k++){alt+=bases[i][k];if(alt>0){pos=dest[(size_t)(alt-1)*RC+pos];if(pos<0)break;traj[i][k]=pos;}}}
            ll work=(ll)nb*T*T;int step=max<ll>(1,work/20000000);
            ll bv=-1;int bbi=0,bd=0;
            for(int i=0;i<nb;i++)for(int d=0;d<T;d+=step){ll s=0;for(int k=0;d+k<T;k++){int p=traj[i][k];if(p<0)break;s+=gainv[(size_t)(d+k)*RC+p];}if(s>bv){bv=s;bbi=i;bd=d;}}
            vector<int> adj(T,0);for(int k=0;bd+k<T;k++)adj[bd+k]=bases[bbi][k];
            plan[b]=adj;commit(adj);
        }
    }
    output();
}
