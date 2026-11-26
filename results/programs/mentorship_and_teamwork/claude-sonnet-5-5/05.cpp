#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(0);
    int C,P;cin>>C>>P;
    unordered_map<string,int> sid;
    vector<string> cn(C);
    vector<unordered_map<int,int>> sk(C);
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    for(int i=0;i<C;i++){int n;cin>>cn[i]>>n;while(n--){string s;int l;cin>>s>>l;sk[i][gid(s)]=l;}}
    struct Pr{string name;int d,s,b;vector<pair<int,int>> r;};
    vector<Pr> pr(P);
    for(auto&p:pr){int n;cin>>p.name>>p.d>>p.s>>p.b>>n;p.r.resize(n);for(auto&x:p.r){string s;cin>>s>>x.second;x.first=gid(s);}}
    int S=sid.size();
    vector<vector<int>> bySkill(S);
    for(int i=0;i<C;i++)for(auto&kv:sk[i])bySkill[kv.first].push_back(i);
    auto lvl=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
    vector<int> order(P);
    iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){
        double pa=(double)pr[a].s/pr[a].d/pr[a].r.size(),pb=(double)pr[b].s/pr[b].d/pr[b].r.size();
        if(pa!=pb)return pa>pb;return a<b;});
    vector<char> busy(C,0);
    set<int> freeSet;for(int i=0;i<C;i++)freeSet.insert(i);
    vector<int> pending=order;
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> ev;
    struct Run{vector<int> mem;vector<int> role;int pj;};
    vector<Run> runs;
    vector<pair<int,vector<int>>> out;
    auto st=chrono::steady_clock::now();
    int t=0;
    while(true){
        if(chrono::duration<double>(chrono::steady_clock::now()-st).count()>8.0)break;
        bool any=true;
        while(any){
            any=false;
            vector<int> np;
            for(size_t qi=0;qi<pending.size();qi++){
                int pj=pending[qi];
                if(chrono::duration<double>(chrono::steady_clock::now()-st).count()>8.0){np.push_back(pj);continue;}
                Pr&p=pr[pj];
                if(t+p.d>p.b+p.s){continue;}
                int R=p.r.size();
                vector<int> idx(R);iota(idx.begin(),idx.end(),0);
                sort(idx.begin(),idx.end(),[&](int a,int b){return p.r[a].second>p.r[b].second;});
                vector<int> as(R,-1);
                vector<int> used;
                bool ok=true;
                auto isUsed=[&](int c){for(int u:used)if(u==c)return true;return false;};
                for(int ri:idx){
                    int sKill=p.r[ri].first,rq=p.r[ri].second;
                    int best=-1,bl=1000;
                    for(int c:bySkill[sKill]){
                        if(busy[c]||isUsed(c))continue;
                        int l=lvl(c,sKill);
                        if(l>=rq&&l<bl){bl=l;best=c;}
                    }
                    if(best<0){
                        // mentored
                        bool has=false;
                        for(int u:used)if(lvl(u,sKill)>=rq){has=true;break;}
                        if(has){
                            int cnt=0;
                            if(rq-1>=1){
                                for(int c:bySkill[sKill]){
                                    if(busy[c]||isUsed(c))continue;
                                    if(lvl(c,sKill)==rq-1){best=c;break;}
                                }
                            }else{
                                for(int c:freeSet){
                                    if(++cnt>200)break;
                                    if(!isUsed(c)){best=c;break;}
                                }
                            }
                        }
                    }
                    if(best<0){ok=false;break;}
                    as[ri]=best;used.push_back(best);
                }
                if(!ok){np.push_back(pj);continue;}
                // verify
                for(int ri=0;ri<R&&ok;ri++){
                    int c=as[ri],s=p.r[ri].first,rq=p.r[ri].second;
                    int l=lvl(c,s);
                    if(l>=rq)continue;
                    bool m=false;
                    for(int rj=0;rj<R;rj++)if(rj!=ri&&lvl(as[rj],s)>=rq){m=true;break;}
                    if(!m||l<rq-1)ok=false;
                }
                if(!ok){np.push_back(pj);continue;}
                for(int c:as){busy[c]=1;freeSet.erase(c);}
                runs.push_back({as,{},pj});
                ev.push({t+p.d,(int)runs.size()-1});
                out.push_back({pj,as});
                any=true;
            }
            pending=np;
        }
        if(ev.empty())break;
        t=ev.top().first;
        while(!ev.empty()&&ev.top().first==t){
            Run&r=runs[ev.top().second];ev.pop();
            Pr&p=pr[r.pj];
            for(size_t i=0;i<p.r.size();i++){
                int c=r.mem[i];
                int s=p.r[i].first;
                if(lvl(c,s)<=p.r[i].second){
                    if(sk[c].find(s)==sk[c].end()){sk[c][s]=1;bySkill[s].push_back(c);}
                    else sk[c][s]++;
                }
                busy[c]=0;freeSet.insert(c);
            }
        }
    }
    cout<<out.size()<<"\n";
    for(auto&o:out){
        cout<<pr[o.first].name<<"\n";
        for(size_t i=0;i<o.second.size();i++){if(i)cout<<" ";cout<<cn[o.second[i]];}
        cout<<"\n";
    }
}
