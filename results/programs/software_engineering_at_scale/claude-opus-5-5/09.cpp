#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline string readTok(){ int c=gc(); while(c==' '||c=='\n'||c=='\r') c=gc(); string s; while(c!=-1&&c!=' '&&c!='\n'&&c!='\r'){ s.push_back((char)c); c=gc(); } return s; }
inline long long readInt(){ int c=gc(); while(c==' '||c=='\n'||c=='\r') c=gc(); long long x=0; bool neg=false; if(c=='-'){neg=true;c=gc();} while(c>='0'&&c<='9'){ x=x*10+(c-'0'); c=gc(); } return neg?-x:x; }

int L,G,S,B,F,N;
vector<string> fname;
vector<int> FD; vector<long long> FU;
vector<vector<int>> fbins; // sorted by R desc
vector<int> R;
vector<double> fwork;

vector<vector<pair<int,int>>> outTasks;

long long simulate(const vector<int>& order, bool record){
    if(record){ outTasks.assign(G,{}); }
    vector<priority_queue<int,vector<int>,greater<int>>> act(B);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
    for(int i=0;i<G;i++) pq.push({0,i});
    long long score=0;
    size_t oi=0; int cur=-1; size_t ti=0; int curEnd=0; bool curFailed=false;
    auto Cof=[&](int b,int t){ auto &h=act[b]; while(!h.empty()&&h.top()<=t) h.pop(); return (int)h.size(); };
    auto finalize=[&](){
        if(cur>=0 && !curFailed && ti>=fbins[cur].size() && curEnd<L) score+=FU[cur]*(long long)(L-curEnd);
        cur=-1;
    };
    bool exhausted=false;
    while(!pq.empty()){
        auto [t,e]=pq.top(); pq.pop();
        if(t>=L||exhausted) continue;
        bool assigned=false;
        while(true){
            if(cur>=0 && !curFailed && ti<fbins[cur].size()){
                int b=fbins[cur][ti];
                int dur=FD[cur]+R[b]+Cof(b,t);
                int end=t+dur;
                if(end>=L){ curFailed=true; continue; }
                ti++;
                curEnd=max(curEnd,end);
                act[b].push(end);
                if(record) outTasks[e].push_back({cur,b});
                pq.push({end,e});
                assigned=true;
                break;
            }
            finalize();
            // find next feasible feature
            bool found=false;
            while(oi<order.size()){
                int f=order[oi++];
                int b=fbins[f][0];
                int est=t+FD[f]+R[b]+Cof(b,t);
                if(est<L){ cur=f; ti=0; curEnd=0; curFailed=false; found=true; break; }
            }
            if(!found){ exhausted=true; break; }
        }
        (void)assigned;
    }
    finalize();
    return score;
}

int main(){
    L=readInt(); G=readInt(); S=readInt(); B=readInt(); F=readInt(); N=readInt();
    unordered_map<string,int> sid; sid.reserve(S*2+10);
    vector<int> sbin(S);
    R.assign(B,0);
    for(int i=0;i<S;i++){ string s=readTok(); int b=readInt(); sid[s]=i; sbin[i]=b; R[b]++; }
    fname.resize(F); FD.resize(F); FU.resize(F); fbins.resize(F); fwork.resize(F);
    vector<int> mark(B,-1);
    for(int i=0;i<F;i++){
        fname[i]=readTok(); int m=readInt(); FD[i]=readInt(); FU[i]=readInt();
        for(int j=0;j<m;j++){ string s=readTok(); int b=sbin[sid[s]]; if(mark[b]!=i){ mark[b]=i; fbins[i].push_back(b);} }
        sort(fbins[i].begin(),fbins[i].end(),[&](int a,int b){ return R[a]>R[b]; });
        double w=0; for(int b:fbins[i]) w+=FD[i]+R[b];
        fwork[i]=w;
    }
    vector<double> exps={0.5,0.75,1.0,1.25,1.5,2.0,3.0,0.0,1.0};
    long long best=-1; vector<int> bestOrder;
    for(size_t k=0;k<exps.size();k++){
        double a=exps[k];
        vector<double> key(F);
        for(int i=0;i<F;i++){
            if(k==8){ // by max duration
                double md=FD[i]+R[fbins[i][0]];
                key[i]=(double)FU[i]/ (md*sqrt(fwork[i]));
            } else key[i]=(double)FU[i]/pow(fwork[i],a);
        }
        vector<int> order(F); iota(order.begin(),order.end(),0);
        stable_sort(order.begin(),order.end(),[&](int x,int y){ return key[x]>key[y]; });
        long long sc=simulate(order,false);
        if(sc>best){ best=sc; bestOrder=order; }
    }
    simulate(bestOrder,true);
    string out;
    int E=0; for(int i=0;i<G;i++) if(!outTasks[i].empty()) E++;
    out+=to_string(E)+"\n";
    for(int i=0;i<G;i++){
        if(outTasks[i].empty()) continue;
        out+=to_string(outTasks[i].size())+"\n";
        for(auto &p:outTasks[i]){
            out+="impl "; out+=fname[p.first]; out+=' '; out+=to_string(p.second); out+='\n';
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
