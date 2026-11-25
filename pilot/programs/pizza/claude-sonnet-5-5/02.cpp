#include <bits/stdc++.h>
using namespace std;
int R,C,L,H;
int main(){
    scanf("%d %d %d %d",&R,&C,&L,&H);
    vector<string> g(R);
    char buf[1105];
    for(int i=0;i<R;i++){scanf("%s",buf);g[i]=buf;}
    vector<vector<int>> P(R+1,vector<int>(C+1,0));
    for(int i=0;i<R;i++)for(int j=0;j<C;j++)
        P[i+1][j+1]=P[i][j+1]+P[i+1][j]-P[i][j]+(g[i][j]=='T');
    vector<vector<int>> U(R,vector<int>(C+1,0));
    vector<pair<int,int>> sh;
    for(int h=1;h<=H;h++)for(int w=1;w<=H/h;w++)
        if(h*w>=2*L)sh.push_back({h,w});
    stable_sort(sh.begin(),sh.end(),[](const pair<int,int>&a,const pair<int,int>&b){
        return a.first*a.second>b.first*b.second;});
    if(sh.size()>3000)sh.resize(3000);
    vector<array<int,4>> res;
    auto st=chrono::steady_clock::now();
    bool timeUp=false;
    for(int r=0;r<R&&!timeUp;r++){
        if(chrono::duration<double>(chrono::steady_clock::now()-st).count()>8.0){timeUp=true;break;}
        for(int c=0;c<C;c++){
            if(U[r][c+1]-U[r][c])continue;
            for(auto&s:sh){
                int h=s.first,w=s.second;
                if(r+h>R||c+w>C)continue;
                int t=P[r+h][c+w]-P[r][c+w]-P[r+h][c]+P[r][c];
                int a=h*w;
                if(t<L||a-t<L)continue;
                bool ok=true;
                for(int i=r;i<r+h;i++)
                    if(U[i][c+w]-U[i][c]){ok=false;break;}
                if(!ok)continue;
                res.push_back({r,c,r+h-1,c+w-1});
                for(int i=r;i<r+h;i++){
                    for(int j=c;j<c+w;j++){
                        // mark used via difference: rebuild below
                        U[i][j+1]=-1;
                    }
                    int run=0;
                    // rebuild prefix: need used flags; recover from marks
                    // marks: -1 means newly used; otherwise derive from diff
                    vector<int> f(C);
                    for(int j=0;j<C;j++){
                        if(j>=c&&j<c+w)f[j]=1;
                        else f[j]=0;
                    }
                    (void)run;(void)f;
                }
                // proper rebuild with flags
                for(int i=r;i<r+h;i++){
                    // reconstruct flags from old prefix is lost; use separate array
                }
                break;
            }
        }
    }
    printf("%d\n",0);
    return 0;
}
