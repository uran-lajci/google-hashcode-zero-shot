#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } int s=1; if(c=='-'){s=-1;c=gc();} int x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return x*s; }

int B,L,D;
vector<int> S;
vector<int> N_,T_,M_;
vector<vector<int>> lb;
chrono::steady_clock::time_point T0;
double elapsed(){ return chrono::duration<double>(chrono::steady_clock::now()-T0).count(); }

vector<char> scanned;

struct Result{ ll score; vector<int> order; vector<int> owner; };

Result greedyRun(double alpha){
    fill(scanned.begin(),scanned.end(),0);
    vector<double> pw(L);
    for(int l=0;l<L;l++) pw[l]=pow((double)T_[l],alpha);
    auto val=[&](int l,ll t)->double{
        ll cap=(ll)(D-t-T_[l])*M_[l];
        if(cap<=0) return 0;
        ll s=0; ll c=0;
        for(int b:lb[l]){ if(c>=cap) break; if(!scanned[b]){ s+=S[b]; c++; } }
        return s/pw[l];
    };
    priority_queue<pair<double,int>> pq;
    for(int l=0;l<L;l++){ double v=val(l,0); if(v>0) pq.push({v,l}); }
    Result r; r.score=0; r.owner.assign(B,-1);
    ll t=0;
    while(!pq.empty()){
        auto [v,l]=pq.top(); pq.pop();
        if(t+T_[l]>=D) continue;
        double nv=val(l,t);
        if(nv<=0) continue;
        if(!pq.empty() && nv<pq.top().first){ pq.push({nv,l}); continue; }
        ll cap=(ll)(D-t-T_[l])*M_[l]; ll c=0;
        for(int b:lb[l]){ if(c>=cap) break; if(!scanned[b]){ scanned[b]=1; r.owner[b]=l; r.score+=S[b]; c++; } }
        r.order.push_back(l);
        t+=T_[l];
    }
    return r;
}

int main(){
    T0=chrono::steady_clock::now();
    B=readInt(); L=readInt(); D=readInt();
    S.resize(B); for(int i=0;i<B;i++) S[i]=readInt();
    N_.resize(L);T_.resize(L);M_.resize(L);lb.resize(L);
    for(int l=0;l<L;l++){
        N_[l]=readInt();T_[l]=readInt();M_[l]=readInt();
        lb[l].resize(N_[l]);
        for(int i=0;i<N_[l];i++) lb[l][i]=readInt();
        sort(lb[l].begin(),lb[l].end(),[&](int a,int b){ if(S[a]!=S[b]) return S[a]>S[b]; return a<b;});
    }
    scanned.assign(B,0);
    vector<double> alphas={1.0,0.75,1.25,0.5,1.5,2.0,0.9,1.1,0.6,0.8,1.2,1.4,0.4,0.95,1.05,0.85,1.15,0.3,1.75,0.7};
    Result best; best.score=-1;
    double lastDur=0;
    for(size_t i=0;i<alphas.size();i++){
        double st=elapsed();
        if(i>0 && st+lastDur*1.2>5.0) break;
        Result r=greedyRun(alphas[i]);
        lastDur=elapsed()-st;
        if(r.score>best.score) best=move(r);
    }
    // matching improvement
    vector<int> order=best.order;
    vector<int>& owner=best.owner;
    int A=order.size();
    vector<int> pos(L,-1);
    for(int i=0;i<A;i++) pos[order[i]]=i;
    vector<ll> cap(L,0), used(L,0);
    {
        ll t=0;
        for(int l:order){ cap[l]=min<ll>((ll)(D-t-T_[l])*M_[l],N_[l]); t+=T_[l]; }
    }
    for(int b=0;b<B;b++) if(owner[b]>=0) used[owner[b]]++;
    // book -> selected libs
    vector<int> deg(B+1,0);
    for(int l:order) for(int b:lb[l]) deg[b+1]++;
    for(int i=0;i<B;i++) deg[i+1]+=deg[i];
    vector<int> adj(deg[B]); vector<int> fillp(deg.begin(),deg.end()-1);
    for(int l:order) for(int b:lb[l]) adj[fillp[b]++]=l;
    vector<int> cand;
    for(int b=0;b<B;b++) if(owner[b]<0 && S[b]>0 && deg[b+1]>deg[b]) cand.push_back(b);
    sort(cand.begin(),cand.end(),[&](int a,int b){ if(S[a]!=S[b]) return S[a]>S[b]; return a<b;});
    vector<int> visB(B,0), visL(L,0), prevBook(B,-1);
    int stamp=0;
    vector<int> q; q.reserve(B);
    for(int b0:cand){
        if(elapsed()>9.0) break;
        stamp++;
        q.clear(); q.push_back(b0); visB[b0]=stamp; prevBook[b0]=-1;
        ll steps=0; const ll LIM=200000;
        bool done=false;
        for(size_t qi=0;qi<q.size() && !done && steps<LIM;qi++){
            int x=q[qi];
            for(int k=deg[x];k<deg[x+1] && !done;k++){
                int l=adj[k]; steps++;
                if(visL[l]==stamp) continue;
                visL[l]=stamp;
                if(used[l]<cap[l]){
                    // augment
                    used[l]++;
                    int cur=x, target=l;
                    while(true){
                        int o=owner[cur];
                        owner[cur]=target;
                        if(cur==b0) break;
                        target=o; cur=prevBook[cur];
                    }
                    done=true; break;
                }
                for(int y:lb[l]){
                    steps++;
                    if(owner[y]==l && visB[y]!=stamp){ visB[y]=stamp; prevBook[y]=x; q.push_back(y); }
                    if(steps>=LIM) break;
                }
                if(steps>=LIM) break;
            }
        }
    }
    // output
    vector<vector<int>> asg(L);
    for(int b=0;b<B;b++) if(owner[b]>=0) asg[owner[b]].push_back(b);
    vector<int> fin;
    for(int l:order) if(!asg[l].empty()) fin.push_back(l);
    string out;
    out.reserve(1<<24);
    out+=to_string(fin.size()); out+='\n';
    for(int l:fin){
        auto& v=asg[l];
        sort(v.begin(),v.end(),[&](int a,int b){ if(S[a]!=S[b]) return S[a]>S[b]; return a<b;});
        out+=to_string(l); out+=' '; out+=to_string(v.size()); out+='\n';
        for(size_t i=0;i<v.size();i++){ if(i) out+=' '; out+=to_string(v[i]); }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
