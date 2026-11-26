#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    vector<int> mx(M),my(M);
    vector<char> mountG((size_t)W*H,0);
    for(int i=0;i<M;i++){scanf("%d %d",&mx[i],&my[i]);mountG[(size_t)my[i]*W+mx[i]]=1;}
    vector<int> S(T);
    vector<vector<int>> px(T),py(T);
    vector<long long> len(T,0);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&S[i],&p);
        px[i].resize(p);py[i].resize(p);
        for(int j=0;j<p;j++){scanf("%d %d",&px[i][j],&py[i][j]);
            if(j)len[i]+=abs(px[i][j]-px[i][j-1])+abs(py[i][j]-py[i][j-1]);}
    }
    vector<char> used(T,0),mused(M,0);
    vector<int> banStamp(T,-1);
    vector<vector<pair<int,int>>> occ((size_t)W*H);
    vector<char> own((size_t)W*H,0);
    const int INF=1e9;
    struct Out{int x,y;vector<int> tasks;string ins;};
    vector<Out> outs;
    const int DX[4]={1,-1,0,0},DY[4]={0,0,1,-1};
    const char DC[4]={'R','L','U','D'};
    for(int arm=0;arm<R;arm++){
        // choose mount & task
        double best=-1;int bm=-1;
        for(int m=0;m<M;m++){
            if(mused[m])continue;
            for(int t=0;t<T;t++){
                if(used[t])continue;
                long long c=abs(mx[m]-px[t][0])+abs(my[m]-py[t][0])+len[t];
                if(c>L)continue;
                double v=(double)S[t]/(c+1);
                if(v>best){best=v;bm=m;}
            }
        }
        if(bm<0)break;
        mused[bm]=1;
        int hx=mx[bm],hy=my[bm];
        int base=hy*W+hx;
        vector<int> stc{base},ste{0};
        own[base]=1;
        string moves;
        vector<int> done;int lastK=0;
        int cur=-1,idx=0,bestD=INF,lastProg=0;
        for(int t=0;;t++){
            while(true){
                if(cur>=0&&t-lastProg>400){banStamp[cur]=arm;cur=-1;}
                if(cur<0){
                    double bv=-1;int bt=-1;
                    for(int k=0;k<T;k++){
                        if(used[k]||banStamp[k]==arm)continue;
                        long long c=abs(hx-px[k][0])+abs(hy-py[k][0])+len[k];
                        if(c>L-t)continue;
                        double v=(double)S[k]/(c+1);
                        if(v>bv){bv=v;bt=k;}
                    }
                    if(bt<0)break;
                    cur=bt;idx=0;bestD=INF;lastProg=t;
                }
                int P=px[cur].size();
                while(idx<P&&px[cur][idx]==hx&&py[cur][idx]==hy){idx++;lastProg=t;bestD=INF;}
                if(idx==P){
                    used[cur]=1;done.push_back(cur);lastK=t;cur=-1;continue;
                }
                break;
            }
            if(cur<0||t==L)break;
            int tx=px[cur][idx],ty=py[cur][idx];
            int dx=tx-hx,dy=ty-hy;
            int cd=abs(dx)+abs(dy);
            if(cd<bestD){bestD=cd;lastProg=t;}
            int sc=t-lastProg;
            bool legal[4],retr[4];int nd[4];
            for(int d=0;d<4;d++){
                legal[d]=false;retr[d]=false;
                int nx=hx+DX[d],ny=hy+DY[d];
                if(nx<0||ny<0||nx>=W||ny>=H)continue;
                int n=ny*W+nx;
                nd[d]=abs(tx-nx)+abs(ty-ny);
                if(stc.size()>=2&&n==stc[stc.size()-2]){legal[d]=true;retr[d]=true;continue;}
                if(mountG[n]||own[n])continue;
                bool bl=false;
                for(auto&pr:occ[n])if(pr.first<=t+1&&t+1<=pr.second){bl=true;break;}
                if(bl)continue;
                legal[d]=true;
            }
            int order[4];
            if(abs(dx)>=abs(dy)){order[0]=dx>0?0:1;order[1]=dy>0?2:3;order[2]=dx>0?1:0;order[3]=dy>0?3:2;}
            else{order[0]=dy>0?2:3;order[1]=dx>0?0:1;order[2]=dy>0?3:2;order[3]=dx>0?1:0;}
            int ch=-1;
            for(int i=0;i<4;i++){int d=order[i];if(legal[d]&&nd[d]<cd){ch=d;break;}}
            if(ch<0&&sc%8>=4){
                int bd=INF;int st=(sc/8)%4;
                for(int i=0;i<4;i++){
                    int d=(i+st)%4;
                    if(legal[d]&&!retr[d]&&nd[d]<bd){bd=nd[d];ch=d;}
                }
                if(ch<0){for(int d=0;d<4;d++)if(legal[d]&&retr[d]){ch=d;break;}}
            }
            if(ch<0){moves.push_back('W');continue;}
            moves.push_back(DC[ch]);
            int nx=hx+DX[ch],ny=hy+DY[ch];
            int n=ny*W+nx;
            if(retr[ch]){
                int top=stc.back();own[top]=0;stc.pop_back();ste.pop_back();
            } else {
                stc.push_back(n);ste.push_back(t+1);own[n]=1;
            }
            hx=nx;hy=ny;
        }
        for(int c:stc)own[c]=0;
        if(done.empty())continue;
        moves.resize(lastK);
        // replay for occupancy
        vector<int> sc2{base},se2{0};
        for(int t=0;t<lastK;t++){
            char c=moves[t];
            if(c=='W')continue;
            int d=0;while(DC[d]!=c)d++;
            int cc=sc2.back();
            int cx=cc%W,cy=cc/W;
            int n=(cy+DY[d])*W+cx+DX[d];
            if(sc2.size()>=2&&n==sc2[sc2.size()-2]){
                occ[sc2.back()].push_back({se2.back(),t});
                sc2.pop_back();se2.pop_back();
            } else {sc2.push_back(n);se2.push_back(t+1);}
        }
        for(size_t i=1;i<sc2.size();i++)occ[sc2[i]].push_back({se2[i],INF});
        Out o;o.x=mx[bm];o.y=my[bm];o.tasks=done;o.ins=moves;
        outs.push_back(o);
    }
    if(outs.empty()){
        printf("1\n%d %d 1 1\n0\nW\n",mx[0],my[0]);
        return 0;
    }
    printf("%d\n",(int)outs.size());
    for(auto&o:outs){
        printf("%d %d %d %d\n",o.x,o.y,(int)o.tasks.size(),(int)o.ins.size());
        for(size_t i=0;i<o.tasks.size();i++)printf(i?" %d":"%d",o.tasks[i]);
        printf("\n");
        for(size_t i=0;i<o.ins.size();i++){if(i)putchar(' ');putchar(o.ins[i]);}
        printf("\n");
    }
}
