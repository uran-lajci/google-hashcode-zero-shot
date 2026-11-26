#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> W; // [alt][r][c] packed dr,dc
int main(){
    scanf("%d %d %d %d %d %d %d %d %d",&R,&C,&A,&L,&V,&B,&T,&rs,&cs);
    vector<int> tr(L),tc(L);
    vector<vector<int>> byRow(R);
    for(int i=0;i<L;i++){scanf("%d %d",&tr[i],&tc[i]);byRow[tr[i]].push_back(i);}
    vector<signed char> da((size_t)A*R*C),db((size_t)A*R*C);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int x,y;scanf("%d %d",&x,&y);
        size_t id=((size_t)a*R+r)*C+c;da[id]=x;db[id]=y;
    }
    vector<vector<char>> cov(T+1,vector<char>(L,0));
    vector<vector<signed char>> out(T,vector<signed char>(B,0));
    auto gain=[&](int t,int r,int c){
        int g=0;
        for(int rr=max(0,r-V);rr<=min(R-1,r+V);rr++){
            for(int i:byRow[rr]){
                if(cov[t][i])continue;
                int d=abs(c-tc[i]);d=min(d,C-d);
                int dr=rr-r;
                if(dr*dr+d*d<=V*V)g++;
            }
        }
        return g;
    };
    struct S{int key;long long sc;int par;signed char act;};
    const int K=4;
    clock_t t0=clock();
    for(int b=0;b<B;b++){
        if((double)(clock()-t0)/CLOCKS_PER_SEC>8.0)break;
        vector<vector<S>> lay(T+1);
        lay[0].push_back({-1,0,-1,0});
        for(int t=1;t<=T;t++){
            unordered_map<int,int> mp;
            vector<S> cand;
            auto add=[&](int key,long long sc,int par,int act){
                auto it=mp.find(key);
                if(it==mp.end()){mp[key]=cand.size();cand.push_back({key,sc,par,(signed char)act});}
                else if(cand[it->second].sc<sc){cand[it->second]={key,sc,par,(signed char)act};}
            };
            for(int i=0;i<(int)lay[t-1].size();i++){
                S&s=lay[t-1][i];
                if(s.key==-1){
                    add(-1,s.sc,i,0);
                    int a=0;
                    size_t id=((size_t)a*R+rs)*C+cs;
                    int nr=rs+da[id];
                    if(nr>=0&&nr<R){
                        int nc=((cs+db[id])%C+C)%C;
                        add((a*R+nr)*C+nc,s.sc+gain(t,nr,nc),i,1);
                    }
                } else {
                    int a=s.key/(R*C),rem=s.key%(R*C),r=rem/C,c=rem%C;
                    for(int d=-1;d<=1;d++){
                        int na=a+d;
                        if(na<0||na>=A)continue;
                        size_t id=((size_t)na*R+r)*C+c;
                        int nr=r+da[id];
                        if(nr<0||nr>=R)continue;
                        int nc=((c+db[id])%C+C)%C;
                        add((na*R+nr)*C+nc,s.sc+gain(t,nr,nc),i,d);
                    }
                }
            }
            sort(cand.begin(),cand.end(),[](const S&x,const S&y){return x.sc>y.sc;});
            bool hasU=false;
            vector<S> nl;
            for(auto&s:cand){
                if((int)nl.size()<K){nl.push_back(s);if(s.key==-1)hasU=true;}
            }
            if(!hasU){
                for(auto&s:cand)if(s.key==-1){nl.push_back(s);break;}
            }
            lay[t]=nl;
        }
        int best=0;
        for(int i=0;i<(int)lay[T].size();i++)if(lay[T][i].sc>lay[T][best].sc)best=i;
        if(lay[T].empty()||lay[T][best].sc<=0)break;
        // backtrack
        vector<int> keys(T+1);
        int cur=best;
        for(int t=T;t>=1;t--){
            out[t-1][b]=lay[t][cur].act;
            keys[t]=lay[t][cur].key;
            cur=lay[t][cur].par;
        }
        for(int t=1;t<=T;t++){
            int k=keys[t];
            if(k<0)continue;
            int rem=k%(R*C),r=rem/C,c=rem%C;
            for(int rr=max(0,r-V);rr<=min(R-1,r+V);rr++)
                for(int i:byRow[rr]){
                    int d=abs(c-tc[i]);d=min(d,C-d);
                    int dr=rr-r;
                    if(dr*dr+d*d<=V*V)cov[t][i]=1;
                }
        }
    }
    string s;
    for(int t=0;t<T;t++){
        s.clear();
        for(int b=0;b<B;b++){
            if(b)s+=' ';
            s+=to_string((int)out[t][b]);
        }
        s+='\n';
        fputs(s.c_str(),stdout);
    }
}
