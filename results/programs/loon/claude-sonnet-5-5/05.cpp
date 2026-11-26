#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> TR,TC;
vector<vector<int>> bucket;
int BS,NBR,NBC;
vector<short> WA_,WB_;
struct S{int r,c,a,par,sc;signed char d;};
int main(){
    scanf("%d %d %d %d %d %d %d %d %d",&R,&C,&A,&L,&V,&B,&T,&rs,&cs);
    TR.resize(L);TC.resize(L);
    for(int i=0;i<L;i++)scanf("%d %d",&TR[i],&TC[i]);
    WA_.resize((size_t)A*R*C);WB_.resize((size_t)A*R*C);
    for(size_t i=0;i<(size_t)A*R*C;i++){int a,b;scanf("%d %d",&a,&b);WA_[i]=a;WB_[i]=b;}
    BS=V+1;NBR=(R+BS-1)/BS;NBC=(C+BS-1)/BS;
    bucket.assign(NBR*NBC,{});
    for(int i=0;i<L;i++)bucket[(TR[i]/BS)*NBC+TC[i]/BS].push_back(i);
    vector<char> cov((size_t)T*L,0);
    vector<vector<signed char>> out(T,vector<signed char>(B,0));
    auto start=chrono::steady_clock::now();
    const int K=20;
    long long V2=(long long)V*V;
    auto gain=[&](int r,int c,int t,bool mark)->int{
        int g=0;
        int br=r/BS,bc=c/BS;
        for(int dr=-1;dr<=1;dr++){
            int rr=br+dr;if(rr<0||rr>=NBR)continue;
            for(int dc=-1;dc<=1;dc++){
                int cc=((bc+dc)%NBC+NBC)%NBC;
                if(NBC<3&&dc!=0&&((bc+dc)%NBC+NBC)%NBC==bc)continue;
                if(NBC==2&&dc==-1&&((bc-1)%NBC+NBC)%NBC==((bc+1)%NBC))continue;
                for(int i:bucket[rr*NBC+cc]){
                    char&cv=cov[(size_t)t*L+i];
                    if(cv)continue;
                    int dR=r-TR[i];int d=abs(c-TC[i]);d=min(d,C-d);
                    if((long long)dR*dR+(long long)d*d<=V2){g++;if(mark)cv=1;}
                }
            }
        }
        return g;
    };
    for(int b=0;b<B;b++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
        if(el>7.0)break;
        vector<vector<S>> lay(T+1);
        lay[0].push_back({rs,cs,0,-1,0,0});
        for(int t=0;t<T;t++){
            vector<S> nx;
            auto&cur=lay[t];
            for(int i=0;i<(int)cur.size();i++){
                S&s=cur[i];
                for(int d=-1;d<=1;d++){
                    int na=s.a+d;
                    if(s.a==0){if(d<0)continue;if(d==0){nx.push_back({s.r,s.c,0,i,s.sc,0});continue;}}
                    if(na<1||na>A)continue;
                    size_t idx=((size_t)(na-1)*R+s.r)*C+s.c;
                    int nr=s.r+WA_[idx];
                    if(nr<0||nr>=R)continue;
                    int nc=((s.c+WB_[idx])%C+C)%C;
                    int g=gain(nr,nc,t,false);
                    nx.push_back({nr,nc,na,i,s.sc+g,(signed char)d});
                }
            }
            if((int)nx.size()>K){
                nth_element(nx.begin(),nx.begin()+K,nx.end(),[](const S&x,const S&y){return x.sc>y.sc;});
                nx.resize(K);
            }
            lay[t+1]=nx;
        }
        int bi=-1,bs=-1;
        for(int i=0;i<(int)lay[T].size();i++)if(lay[T][i].sc>bs){bs=lay[T][i].sc;bi=i;}
        if(bi<0||bs<=0)continue;
        int idx=bi;
        vector<int> ids(T+1);
        for(int t=T;t>=1;t--){
            S&s=lay[t][idx];
            out[t-1][b]=s.d;
            ids[t]=idx;
            idx=s.par;
        }
        for(int t=1;t<=T;t++){
            S&s=lay[t][ids[t]];
            gain(s.r,s.c,t-1,true);
        }
    }
    string o;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b)o+=' ';
            o+=to_string((int)out[t][b]);
        }
        o+='\n';
    }
    fputs(o.c_str(),stdout);
}
