#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<pair<int,int>> tg;
vector<vector<int>> cnt; // T+1 x L
vector<int> wr,wc;
inline int cd(int a,int b){int d=abs(a-b);return min(d,C-d);}
int main(){
    scanf("%d %d %d %d %d %d %d %d %d",&R,&C,&A,&L,&V,&B,&T,&rs,&cs);
    tg.resize(L);
    for(auto&p:tg)scanf("%d %d",&p.first,&p.second);
    sort(tg.begin(),tg.end());
    wr.assign((size_t)A*R*C,0);wc.assign((size_t)A*R*C,0);
    for(size_t i=0;i<(size_t)A*R*C;i++){scanf("%d %d",&wr[i],&wc[i]);}
    cnt.assign(T+1,vector<int>(L,0));
    vector<vector<signed char>> out(T,vector<signed char>(B,0));
    auto gain=[&](int r,int c,int t){
        int lo=lower_bound(tg.begin(),tg.end(),make_pair(r-V,-1))-tg.begin();
        int g=0;
        for(int i=lo;i<L&&tg[i].first<=r+V;i++){
            if(cnt[t][i])continue;
            int dr=tg[i].first-r,dc=cd(c,tg[i].second);
            if(dr*dr+dc*dc<=V*V)g++;
        }
        return g;
    };
    auto t0=chrono::steady_clock::now();
    const int W=3;
    struct S{int r,c,a;long long sc;int par;signed char d;};
    for(int b=0;b<B;b++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(el>8.0)break;
        vector<vector<S>> lay(T+1);
        lay[0].push_back({rs,cs,0,0,-1,0});
        for(int t=1;t<=T;t++){
            vector<S> cand;
            auto&prev=lay[t-1];
            for(int i=0;i<(int)prev.size();i++){
                S&s=prev[i];
                for(int d=-1;d<=1;d++){
                    int na=s.a+d;
                    if(na<0||na>A)continue;
                    if(s.a==0&&d==-1)continue;
                    if(s.a>0&&na<1)continue;
                    int nr=s.r,nc=s.c;
                    if(na>0){
                        size_t k=((size_t)(na-1)*R+s.r)*C+s.c;
                        nr=s.r+wr[k];
                        nc=((s.c+wc[k])%C+C)%C;
                        if(nr<0||nr>=R)continue;
                    }
                    long long sc=s.sc;
                    if(na>0)sc+=gain(nr,nc,t);
                    cand.push_back({nr,nc,na,sc,i,(signed char)d});
                }
            }
            sort(cand.begin(),cand.end(),[](const S&x,const S&y){return x.sc>y.sc;});
            vector<S>&nx=lay[t];
            for(auto&s:cand){
                bool dup=false;
                for(auto&q:nx)if(q.r==s.r&&q.c==s.c&&q.a==s.a){dup=true;break;}
                if(dup)continue;
                nx.push_back(s);
                if((int)nx.size()>=W)break;
            }
            if(nx.empty())break;
        }
        // find best final: use deepest layer reached
        int tt=T;
        while(tt>0&&lay[tt].empty())tt--;
        if(tt==0)continue;
        int idx=0;
        // path
        vector<pair<int,int>> pos(T+1,{-1,-1});
        int cur=idx;
        for(int t=tt;t>=1;t--){
            S&s=lay[t][cur];
            out[t-1][b]=s.d;
            if(s.a>0)pos[t]={s.r,s.c};
            cur=s.par;
        }
        for(int t=1;t<=tt;t++){
            if(pos[t].first<0)continue;
            int r=pos[t].first,c=pos[t].second;
            int lo=lower_bound(tg.begin(),tg.end(),make_pair(r-V,-1))-tg.begin();
            for(int i=lo;i<L&&tg[i].first<=r+V;i++){
                int dr=tg[i].first-r,dc=cd(c,tg[i].second);
                if(dr*dr+dc*dc<=V*V)cnt[t][i]++;
            }
        }
    }
    string s;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b)s+=' ';
            s+=to_string((int)out[t][b]);
        }
        s+='\n';
    }
    fputs(s.c_str(),stdout);
}
