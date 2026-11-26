#include <bits/stdc++.h>
using namespace std;

int C,P;
vector<string> cname,pname;
vector<unordered_map<int,int>> lv;
vector<vector<int>> holders;
vector<int> D,S,B;
vector<vector<pair<int,int>>> roles;
vector<int> busyUntil;
vector<char> isFree;

int getLv(int c,int s){auto it=lv[c].find(s);return it==lv[c].end()?0:it->second;}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    auto st=chrono::steady_clock::now();
    cin>>C>>P;
    unordered_map<string,int> sid;
    auto gs=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int id=sid.size();sid[s]=id;holders.push_back({});return id;};
    cname.resize(C);lv.resize(C);
    for(int i=0;i<C;i++){int n;cin>>cname[i]>>n;for(int j=0;j<n;j++){string s;int l;cin>>s>>l;int id=gs(s);if(l>0){if(!lv[i].count(id))holders[id].push_back(i);lv[i][id]=l;}}}
    pname.resize(P);D.resize(P);S.resize(P);B.resize(P);roles.resize(P);
    for(int i=0;i<P;i++){int r;cin>>pname[i]>>D[i]>>S[i]>>B[i]>>r;roles[i].resize(r);for(int j=0;j<r;j++){string s;int l;cin>>s>>l;roles[i][j]={gs(s),l};}}
    vector<int> order(P);iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){double ka=(double)S[a]/((double)D[a]*roles[a].size());double kb=(double)S[b]/((double)D[b]*roles[b].size());if(ka!=kb)return ka>kb;if(B[a]!=B[b])return B[a]<B[b];return a<b;});
    isFree.assign(C,1);int freeCount=C;
    vector<int> stamp(C,-1);int curStamp=0;
    vector<pair<int,vector<int>>> out;
    // running: pq of (endtime, index into runs)
    struct Run{int p;vector<int> team;vector<char> learn;};
    vector<Run> runs;
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    long long t=0;bool timeUp=false;
    vector<int> team;
    while(true){
        // try start projects
        if(!timeUp){
            vector<int> nord;nord.reserve(order.size());
            int cnt=0;
            for(size_t oi=0;oi<order.size();oi++){
                int p=order[oi];
                if(timeUp){nord.push_back(p);continue;}
                if((++cnt&63)==0){double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();if(el>8.5){timeUp=true;nord.push_back(p);continue;}}
                long long sc=S[p]-max(0LL,t+D[p]-B[p]);
                if(sc<=0)continue; // drop
                int R=roles[p].size();
                if(R>freeCount){nord.push_back(p);continue;}
                curStamp++;
                team.assign(R,-1);
                vector<int> ro(R);iota(ro.begin(),ro.end(),0);
                sort(ro.begin(),ro.end(),[&](int a,int b){return roles[p][a].second>roles[p][b].second;});
                for(int r:ro){
                    int s=roles[p][r].first,req=roles[p][r].second;
                    int best=-1,bl=INT_MAX,bsz=INT_MAX;
                    for(int c:holders[s]){
                        if(!isFree[c]||stamp[c]==curStamp)continue;
                        int l=getLv(c,s);if(l<req)continue;
                        int sz=lv[c].size();
                        if(l<bl||(l==bl&&sz<bsz)){bl=l;bsz=sz;best=c;}
                    }
                    if(best>=0){team[r]=best;stamp[best]=curStamp;}
                }
                bool progress=true;
                while(progress){
                    progress=false;
                    for(int r:ro){
                        if(team[r]>=0)continue;
                        int s=roles[p][r].first,req=roles[p][r].second;
                        bool mentor=false;
                        for(int k=0;k<R;k++)if(team[k]>=0&&getLv(team[k],s)>=req){mentor=true;break;}
                        if(!mentor)continue;
                        int best=-1,bsz=INT_MAX;
                        if(req-1>0){
                            for(int c:holders[s]){
                                if(!isFree[c]||stamp[c]==curStamp)continue;
                                if(getLv(c,s)!=req-1)continue;
                                int sz=lv[c].size();if(sz<bsz){bsz=sz;best=c;}
                            }
                        }else{
                            for(int c=0;c<C;c++){
                                if(!isFree[c]||stamp[c]==curStamp)continue;
                                if(getLv(c,s)!=0)continue;
                                int sz=lv[c].size();if(sz<bsz){bsz=sz;best=c;}
                            }
                        }
                        if(best>=0){team[r]=best;stamp[best]=curStamp;progress=true;}
                    }
                }
                bool ok=true;for(int k=0;k<R;k++)if(team[k]<0){ok=false;break;}
                if(!ok){nord.push_back(p);continue;}
                Run rn;rn.p=p;rn.team=team;rn.learn.resize(R);
                for(int k=0;k<R;k++){int c=team[k];rn.learn[k]=getLv(c,roles[p][k].first)<=roles[p][k].second;isFree[c]=0;freeCount--;}
                runs.push_back(rn);
                pq.push({t+D[p],(int)runs.size()-1});
                out.push_back({p,team});
            }
            order.swap(nord);
        }
        if(pq.empty())break;
        t=pq.top().first;
        while(!pq.empty()&&pq.top().first==t){
            Run&rn=runs[pq.top().second];pq.pop();
            int p=rn.p;
            for(size_t k=0;k<rn.team.size();k++){
                int c=rn.team[k];
                if(rn.learn[k]){
                    int s=roles[p][k].first;
                    auto it=lv[c].find(s);
                    if(it==lv[c].end()){lv[c][s]=1;holders[s].push_back(c);}
                    else it->second++;
                }
                isFree[c]=1;freeCount++;
            }
        }
        if(timeUp){ // drain remaining without starting new
            while(!pq.empty())pq.pop();
            break;
        }
    }
    string o;o+=to_string(out.size())+"\n";
    for(auto&pr:out){o+=pname[pr.first]+"\n";for(size_t k=0;k<pr.second.size();k++){if(k)o+=' ';o+=cname[pr.second[k]];}o+="\n";}
    fwrite(o.data(),1,o.size(),stdout);
}
