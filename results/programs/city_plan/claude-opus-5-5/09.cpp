#include <bits/stdc++.h>
using namespace std;

struct Proj{char t;int h,w,v;vector<pair<int,int>> cells;vector<pair<int,int>> reach;int area;};
int H,W,D,B; vector<Proj> pr;
chrono::steady_clock::time_point T_START;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T_START).count();}
const double TIME_GUARD=9.0;

struct RNG{unsigned long long s;RNG(unsigned long long x):s(x){}
 unsigned long long next(){s^=s<<13;s^=s>>7;s^=s<<17;return s;}
 int rnd(int n){return (int)(next()%(unsigned long long)n);}
 double d01(){return (next()>>11)*(1.0/9007199254740992.0);}};

void computeReach(Proj&p){
    int GH=p.h+2*D,GW=p.w+2*D; vector<int> dist(GH*GW,INT_MAX); deque<int> q;
    for(auto&c:p.cells){int id=(c.first+D)*GW+c.second+D;dist[id]=0;q.push_back(id);}
    while(!q.empty()){int id=q.front();q.pop_front();int x=id/GW,y=id%GW;int dd=dist[id];if(dd>=D)continue;
        int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};
        for(int k=0;k<4;k++){int nx=x+dx[k],ny=y+dy[k];if(nx<0||ny<0||nx>=GH||ny>=GW)continue;int nid=nx*GW+ny;
            if(dist[nid]>dd+1){dist[nid]=dd+1;q.push_back(nid);}}}
    for(int i=0;i<GH*GW;i++) if(dist[i]>0&&dist[i]<=D) p.reach.push_back({i/GW-D,i%GW-D});
}

struct Bd{int p,r,c;bool alive;bool isU;int val;vector<pair<int,int>> tc;};

struct Solver{
    int P,Q;bool wr,wc;int OFF;
    vector<int> rowMap,colMap,owner;
    vector<Bd> bs; vector<int> freeIds; vector<int> stamp; int cur=0;
    long long score=0,work=0;
    vector<int> res,uti;
    long long bestScore=-1; vector<array<int,3>> best;
    int cellIdx(int x,int y){int a=rowMap[x+OFF],b=colMap[y+OFF];if(a<0||b<0)return -1;return a*Q+b;}
    void init(){
        OFF=D+2;
        rowMap.assign(P+2*OFF+60,-1);colMap.assign(Q+2*OFF+60,-1);
        for(int i=0;i<(int)rowMap.size();i++){int x=i-OFF;if(wr)rowMap[i]=((x%P)+P)%P;else rowMap[i]=(x>=0&&x<P)?x:-1;}
        for(int i=0;i<(int)colMap.size();i++){int x=i-OFF;if(wc)colMap[i]=((x%Q)+Q)%Q;else colMap[i]=(x>=0&&x<Q)?x:-1;}
        owner.assign(P*Q,-1);
    }
    void reset(){owner.assign(P*Q,-1);bs.clear();freeIds.clear();stamp.clear();score=0;}
    void newStamp(){cur++;}
    int add(int p,int r,int c){
        int id;
        if(!freeIds.empty()){id=freeIds.back();freeIds.pop_back();}
        else{id=bs.size();bs.push_back(Bd());stamp.push_back(0);}
        Bd&b=bs[id];b.p=p;b.r=r;b.c=c;b.alive=true;b.isU=pr[p].t=='U';b.val=pr[p].v;b.tc.clear();
        for(auto&ce:pr[p].cells) owner[cellIdx(r+ce.first,c+ce.second)]=id;
        newStamp();
        auto&rc=pr[p].reach; work+=rc.size();
        if(!b.isU){
            for(auto&o:rc){int idx=cellIdx(r+o.first,c+o.second);if(idx<0)continue;int ow=owner[idx];
                if(ow<0||ow==id)continue;if(!bs[ow].isU)continue;if(stamp[ow]==cur)continue;stamp[ow]=cur;
                int t=bs[ow].val;bool f=false;for(auto&e:bs[id].tc)if(e.first==t){e.second++;f=true;break;}
                if(!f)bs[id].tc.push_back({t,1});}
            score+=(long long)bs[id].val*bs[id].tc.size();
        }else{
            int t=b.val;
            for(auto&o:rc){int idx=cellIdx(r+o.first,c+o.second);if(idx<0)continue;int ow=owner[idx];
                if(ow<0||ow==id)continue;if(bs[ow].isU)continue;if(stamp[ow]==cur)continue;stamp[ow]=cur;
                bool f=false;for(auto&e:bs[ow].tc)if(e.first==t){e.second++;f=true;break;}
                if(!f){bs[ow].tc.push_back({t,1});score+=bs[ow].val;}}
        }
        return id;
    }
    void remove(int id){
        Bd&b=bs[id];int p=b.p,r=b.r,c=b.c;
        for(auto&ce:pr[p].cells) owner[cellIdx(r+ce.first,c+ce.second)]=-1;
        if(!b.isU){score-=(long long)b.val*b.tc.size();b.tc.clear();}
        else{
            newStamp();int t=b.val;auto&rc=pr[p].reach;work+=rc.size();
            for(auto&o:rc){int idx=cellIdx(r+o.first,c+o.second);if(idx<0)continue;int ow=owner[idx];
                if(ow<0)continue;if(bs[ow].isU)continue;if(stamp[ow]==cur)continue;stamp[ow]=cur;
                auto&tc=bs[ow].tc;
                for(size_t k=0;k<tc.size();k++)if(tc[k].first==t){if(--tc[k].second==0){tc[k]=tc.back();tc.pop_back();score-=bs[ow].val;}break;}}
        }
        b.alive=false;freeIds.push_back(id);
    }
    void snapshot(){best.clear();for(auto&b:bs)if(b.alive)best.push_back({b.p,b.r,b.c});bestScore=score;}
    bool fits(int p,int r,int c){
        for(auto&ce:pr[p].cells)if(owner[cellIdx(r+ce.first,c+ce.second)]>=0)return false;return true;}
    void run(long long budget){
        RNG rng(1234567+P*1000+Q);
        double avg=0;for(int p:res)avg+=pr[p].v;avg/=res.size();
        double T0=avg*0.7,T1=avg*0.003;
        snapshot();
        long long it=0;vector<int> conf;vector<array<int,3>> saved;
        long long startWork=work;
        while(work-startWork<budget){
            if(((++it)&63)==0&&elapsed()>TIME_GUARD)break;
            double frac=(double)(work-startWork)/budget;
            int p=(rng.next()&1)?uti[rng.rnd(uti.size())]:res[rng.rnd(res.size())];
            int r=wr?rng.rnd(P):rng.rnd(P-pr[p].h+1);
            int c=wc?rng.rnd(Q):rng.rnd(Q-pr[p].w+1);
            newStamp();conf.clear();
            for(auto&ce:pr[p].cells){int ow=owner[cellIdx(r+ce.first,c+ce.second)];
                if(ow>=0&&stamp[ow]!=cur){stamp[ow]=cur;conf.push_back(ow);if(conf.size()>3)break;}}
            work+=pr[p].cells.size()+4;
            if(conf.size()>3)continue;
            long long old=score;saved.clear();
            for(int o:conf){saved.push_back({bs[o].p,bs[o].r,bs[o].c});remove(o);}
            int nid=add(p,r,c);
            long long delta=score-old;
            bool acc=delta>=0;
            if(!acc){double T=T0*pow(T1/T0,frac);acc=rng.d01()<exp(delta/T);}
            if(!acc){remove(nid);for(auto&s:saved)add(s[0],s[1],s[2]);}
            else if(score>bestScore)snapshot();
        }
        // rebuild best
        reset();for(auto&b:best)add(b[0],b[1],b[2]);
        // fill pass
        vector<int> order=res;order.insert(order.end(),uti.begin(),uti.end());
        for(int r=0;r<P;r++)for(int c=0;c<Q;c++){
            if(elapsed()>TIME_GUARD)break;
            for(int p:order){
                if(!wr&&r+pr[p].h>P)continue;if(!wc&&c+pr[p].w>Q)continue;
                if(!fits(p,r,c))continue;
                long long old=score;int id=add(p,r,c);
                if(score<=old)remove(id);else break;
            }
        }
        snapshot();
    }
};

int main(){
    T_START=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>H>>W>>D>>B;
    pr.resize(B);
    map<string,int> seen;vector<int> uniq;
    for(int i=0;i<B;i++){
        string t;cin>>t>>pr[i].h>>pr[i].w>>pr[i].v;pr[i].t=t[0];
        string plan;
        for(int r=0;r<pr[i].h;r++){string s;cin>>s;plan+=s;plan+='|';
            for(int c=0;c<pr[i].w;c++)if(s[c]=='#')pr[i].cells.push_back({r,c});}
        pr[i].area=pr[i].cells.size();
        string key=string(1,pr[i].t)+(pr[i].t=='U'?to_string(pr[i].v):string(""))+"#"+to_string(pr[i].w)+"#"+plan;
        auto itr=seen.find(key);
        if(itr==seen.end()){seen[key]=i;uniq.push_back(i);}
        else if(pr[i].t=='R'&&pr[i].v>pr[itr->second].v){
            for(auto&u:uniq)if(u==itr->second)u=i;itr->second=i;}
    }
    vector<int> resAll,utiAll;
    {
        vector<int> rs;for(int i:uniq)if(pr[i].t=='R')rs.push_back(i);
        sort(rs.begin(),rs.end(),[&](int a,int b){
            long long x=(long long)pr[a].v*pr[b].area,y=(long long)pr[b].v*pr[a].area;if(x!=y)return x>y;return a<b;});
        set<int> s;for(int k=0;k<(int)rs.size()&&k<20;k++)s.insert(rs[k]);
        vector<int> rs2=rs;sort(rs2.begin(),rs2.end(),[&](int a,int b){if(pr[a].area!=pr[b].area)return pr[a].area<pr[b].area;if(pr[a].v!=pr[b].v)return pr[a].v>pr[b].v;return a<b;});
        for(int k=0;k<(int)rs2.size()&&k<8;k++)s.insert(rs2[k]);
        resAll.assign(s.begin(),s.end());
        map<int,vector<int>> byType;for(int i:uniq)if(pr[i].t=='U')byType[pr[i].v].push_back(i);
        vector<pair<int,int>> tl;
        for(auto&kv:byType){auto&v=kv.second;sort(v.begin(),v.end(),[&](int a,int b){
            if(pr[a].area!=pr[b].area)return pr[a].area<pr[b].area;return pr[a].h*pr[a].w<pr[b].h*pr[b].w||(pr[a].h*pr[a].w==pr[b].h*pr[b].w&&a<b);});
            tl.push_back({pr[v[0]].area,kv.first});}
        sort(tl.begin(),tl.end());
        for(int k=0;k<(int)tl.size()&&k<160;k++){auto&v=byType[tl[k].second];
            for(int j=0;j<(int)v.size()&&j<2;j++)utiAll.push_back(v[j]);}
    }
    for(int p:resAll)computeReach(pr[p]);for(int p:utiAll)computeReach(pr[p]);

    // configs
    vector<array<int,4>> cfgs;
    int sizes[5]={20,32,50,72,100};
    for(int s:sizes){
        int P,Q;bool wr,wc;
        if(H<=s*1.5){P=H;wr=false;}else{P=s;wr=true;}
        if(W<=s*1.5){Q=W;wc=false;}else{Q=s;wc=true;}
        array<int,4> c={P,Q,wr,wc};
        if(find(cfgs.begin(),cfgs.end(),c)==cfgs.end())cfgs.push_back(c);
    }
    vector<array<int,4>> valid;vector<vector<int>> vres,vuti;
    for(auto&c:cfgs){
        vector<int> r,u;
        for(int p:resAll)if(pr[p].h<=c[0]&&pr[p].w<=c[1])r.push_back(p);
        for(int p:utiAll)if(pr[p].h<=c[0]&&pr[p].w<=c[1])u.push_back(p);
        if(r.empty()||u.empty())continue;
        valid.push_back(c);vres.push_back(r);vuti.push_back(u);
    }
    const long long TOTAL=1200000000LL;
    double bestEst=-1;vector<array<int,3>> bestList;int bP=H,bQ=W;
    for(size_t k=0;k<valid.size();k++){
        Solver s;s.P=valid[k][0];s.Q=valid[k][1];s.wr=valid[k][2];s.wc=valid[k][3];
        s.res=vres[k];s.uti=vuti[k];s.init();
        s.run(TOTAL/valid.size());
        double est=(double)s.bestScore*((double)H*W)/((double)s.P*s.Q);
        if(est>bestEst){bestEst=est;bestList=s.best;bP=s.P;bQ=s.Q;}
    }
    // replicate
    string out;vector<array<int,3>> fin;
    for(auto&b:bestList){
        int p=b[0];
        for(int R=b[1];R+pr[p].h<=H;R+=bP)
            for(int C=b[2];C+pr[p].w<=W;C+=bQ)fin.push_back({p,R,C});
    }
    out.reserve(fin.size()*16+16);
    out+=to_string(fin.size());out+='\n';
    for(auto&f:fin){out+=to_string(f[0]);out+=' ';out+=to_string(f[1]);out+=' ';out+=to_string(f[2]);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
