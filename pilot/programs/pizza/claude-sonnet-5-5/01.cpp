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
    vector<array<int,2>> sh;
    for(int h=1;h<=H;h++)for(int w=1;h*w<=H;w++)
        if(h*w>=2*L && h<=R && w<=C) sh.push_back({h,w});
    stable_sort(sh.begin(),sh.end(),[](const array<int,2>&a,const array<int,2>&b){return a[0]*a[1]>b[0]*b[1];});
    vector<vector<int>> run(R,vector<int>(C+1,0));
    for(int i=0;i<R;i++)for(int j=C-1;j>=0;j--)run[i][j]=run[i][j+1]+1;
    vector<array<int,4>> res;
    clock_t st=clock();
    bool stop=false;
    for(int r=0;r<R&&!stop;r++){
        for(int c=0;c<C;c++){
            if(run[r][c]==0)continue;
            if((c&63)==0 && (double)(clock()-st)/CLOCKS_PER_SEC>8.0){stop=true;break;}
            for(auto&s:sh){
                int h=s[0],w=s[1];
                if(r+h>R||c+w>C)continue;
                if(run[r][c]<w)continue;
                int area=h*w;
                int t=P[r+h][c+w]-P[r][c+w]-P[r+h][c]+P[r][c];
                if(t<L||area-t<L)continue;
                bool ok=true;
                for(int i=1;i<h;i++)if(run[r+i][c]<w){ok=false;break;}
                if(!ok)continue;
                res.push_back({r,c,r+h-1,c+w-1});
                for(int i=0;i<h;i++){
                    for(int j=c+w-1;j>=c;j--)run[r+i][j]=0;
                    for(int j=c-1;j>=0;j--){
                        if(run[r+i][j]==0)break;
                        run[r+i][j]=run[r+i][j+1]+1;
                    }
                }
                c+=w-1;
                break;
            }
        }
    }
    printf("%d\n",(int)res.size());
    for(auto&a:res)printf("%d %d %d %d\n",a[0],a[1],a[2],a[3]);
}
