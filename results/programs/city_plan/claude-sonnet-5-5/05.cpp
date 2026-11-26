#include <bits/stdc++.h>
using namespace std;
struct P{bool res;int h,w,v;vector<string>g;int cnt;};
int H,W,D,B;vector<P> ps;
typedef vector<array<int,3>> Sol;
Sol shelf(const vector<int>&pat){
    Sol s;vector<vector<char>> occ(H,vector<char>(W,0));
    int r=0;size_t idx=0;
    while(r<H){
        int c=0,rowh=0;int fails=0;
        while(c<W&&fails<(int)pat.size()+1){
            int b=pat[idx%pat.size()];idx++;
            P&p=ps[b];
            if(r+p.h>H||c+p.w>W){fails++;continue;}
            fails=0;
            for(int i=0;i<p.h;i++)for(int j=0;j<p.w;j++)if(p.g[i][j]=='#')occ[r+i][c+j]=1;
            s.push_back({b,r,c});
            c+=p.w;rowh=max(rowh,p.h);
        }
        if(rowh==0)break;
        r+=rowh;
    }
    return s;
}
long long score(const Sol&s){
    vector<vector<int>> id(H,vector<int>(W,-1));
    for(size_t k=0;k<s.size();k++){P&p=ps[s[k][0]];if(p.res)continue;
        for(int i=0;i<p.h;i++)for(int j=0;j<p.w;j++)if(p.g[i][j]=='#')id[s[k][1]+i][s[k][2]+j]=k;}
    long long t=0;
    for(auto&e:s){P&p=ps[e[0]];if(!p.res)continue;
        set<int> ty;
        for(int i=0;i<p.h;i++)for(int j=0;j<p.w;j++)if(p.g[i][j]=='#'){
            int R=e[1]+i,C=e[2]+j;
            for(int a=-D;a<=D;a++){int rr=R+a;if(rr<0||rr>=H)continue;int m=D-abs(a);
                for(int b=-m;b<=m;b++){int cc=C+b;if(cc<0||cc>=W)continue;int k=id[rr][cc];if(k>=0)ty.insert(ps[s[k][0]].v);}}
        }
        t+=(long long)p.v*ty.size();
    }
    return t;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&B);
    ps.resize(B);
    for(int i=0;i<B;i++){char t;P&p=ps[i];scanf(" %c %d %d %d",&t,&p.h,&p.w,&p.v);p.res=t=='R';p.g.resize(p.h);p.cnt=0;
        for(auto&l:p.g){char buf[100];scanf("%s",buf);l=buf;for(char ch:l)p.cnt+=ch=='#';}}
    vector<int> R;map<int,int> best;
    for(int i=0;i<B;i++){
        if(ps[i].res)R.push_back(i);
        else{auto it=best.find(ps[i].v);
            if(it==best.end()||ps[i].h*ps[i].w<ps[best[ps[i].v]].h*ps[best[ps[i].v]].w)best[ps[i].v]=i;}
    }
    sort(R.begin(),R.end(),[&](int a,int b){return (double)ps[a].v/(ps[a].h*ps[a].w)>(double)ps[b].v/(ps[b].h*ps[b].w);});
    vector<int> U;for(auto&k:best)U.push_back(k.second);
    sort(U.begin(),U.end(),[&](int a,int b){return ps[a].h*ps[a].w<ps[b].h*ps[b].w;});
    long long bs=-1;Sol bsol;
    auto t0=clock();
    for(size_t ri=0;ri<R.size()&&ri<3;ri++)
        for(size_t k=1;k<=U.size()&&k<=4;k++)
            for(int rep=1;rep<=3;rep++){
                if((double)(clock()-t0)/CLOCKS_PER_SEC>6)goto done;
                vector<int> pat;
                for(int q=0;q<rep;q++)pat.push_back(R[ri]);
                for(size_t j=0;j<k;j++)pat.push_back(U[j]);
                Sol s=shelf(pat);long long sc=score(s);
                if(sc>bs){bs=sc;bsol=s;}
            }
    done:
    if(bsol.empty()){bsol=shelf({R[0]});}
    printf("%d\n",(int)bsol.size());
    for(auto&e:bsol)printf("%d %d %d\n",e[0],e[1],e[2]);
}
