#include <bits/stdc++.h>
using namespace std;
struct Proj{string name;int D,S,B;vector<pair<int,int>> r;bool done=false;};
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    unordered_map<string,int> sid;
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    vector<string> cn(C);
    vector<unordered_map<int,int>> lv(C);
    char buf[64],b2[64];
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",buf,&n);cn[i]=buf;
        for(int j=0;j<n;j++){int l;scanf("%s %d",b2,&l);lv[i][gid(b2)]=l;}
    }
    vector<Proj> pr(P);
    for(int i=0;i<P;i++){
        int R;scanf("%s %d %d %d %d",buf,&pr[i].D,&pr[i].S,&pr[i].B,&R);
        pr[i].name=buf;
        for(int j=0;j<R;j++){int l;scanf("%s %d",b2,&l);pr[i].r.push_back({gid(b2),l});}
    }
    vector<int> freeT(C,0);
    vector<char> busy(C,0);
    auto getl=[&](int c,int s){auto it=lv[c].find(s);return it==lv[c].end()?0:it->second;};
    vector<int> order(P);
    iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){
        double x=(double)pr[a].S/((double)pr[a].D*pr[a].r.size());
        double y=(double)pr[b].S/((double)pr[b].D*pr[b].r.size());
        if(x!=y)return x>y;return a<b;});
    vector<int> pend=order;
    typedef tuple<int,int,vector<int>> Ev;
    // event: finish time, project, members
    auto cmp=[](const Ev&a,const Ev&b){return get<0>(a)>get<0>(b);};
    priority_queue<Ev,vector<Ev>,decltype(cmp)> pq(cmp);
    vector<pair<int,vector<int>>> out;
    int t=0;
    long long work=0;
    while(true){
        // try to start projects at time t
        vector<int> np;
        for(size_t pi=0;pi<pend.size();pi++){
            int p=pend[pi];
            Proj&pj=pr[p];
            if(work>60000000){np.push_back(p);continue;}
            int val=pj.S-max(0,t+pj.D-pj.B);
            if(val<=0)continue;
            int R=pj.r.size();
            vector<int> asg(R,-1);
            set<int> used;
            bool ok=true;
            // order roles by descending level
            vector<int> ro(R);iota(ro.begin(),ro.end(),0);
            sort(ro.begin(),ro.end(),[&](int a,int b){return pj.r[a].second>pj.r[b].second;});
            for(int k:ro){
                int s=pj.r[k].first,l=pj.r[k].second;
                int best=-1,bl=1<<30;
                for(int c=0;c<C;c++){
                    work++;
                    if(busy[c]||used.count(c))continue;
                    int cl=getl(c,s);
                    if(cl>=l&&cl<bl){bl=cl;best=c;if(cl==l)break;}
                }
                if(best<0){
                    // try mentored
                    int mx=0;
                    for(int q=0;q<R;q++)if(asg[q]>=0)mx=max(mx,getl(asg[q],s));
                    if(mx>=l){
                        for(int c=0;c<C;c++){
                            work++;
                            if(busy[c]||used.count(c))continue;
                            if(getl(c,s)==l-1){best=c;break;}
                        }
                    }
                }
                if(best<0){ok=false;break;}
                asg[k]=best;used.insert(best);
            }
            if(ok){
                // verify
                for(int k=0;k<R&&ok;k++){
                    int s=pj.r[k].first,l=pj.r[k].second;
                    int cl=getl(asg[k],s);
                    if(cl>=l)continue;
                    if(cl==l-1){
                        bool m=false;
                        for(int q=0;q<R;q++)if(q!=k&&getl(asg[q],s)>=l)m=true;
                        if(!m)ok=false;
                    } else ok=false;
                }
            }
            if(ok){
                for(int c:asg)busy[c]=1;
                pq.push(Ev(t+pj.D,p,asg));
                out.push_back({p,asg});
            } else np.push_back(p);
        }
        pend=np;
        if(pq.empty())break;
        int nt=get<0>(pq.top());
        t=nt;
        while(!pq.empty()&&get<0>(pq.top())==t){
            Ev e=pq.top();pq.pop();
            Proj&pj=pr[get<1>(e)];
            auto&m=get<2>(e);
            for(size_t k=0;k<m.size();k++){
                int s=pj.r[k].first,l=pj.r[k].second;
                int c=m[k];
                if(getl(c,s)<=l)lv[c][s]=getl(c,s)+1;
                busy[c]=0;
            }
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pr[o.first].name.c_str());
        for(size_t k=0;k<o.second.size();k++)
            printf("%s%c",cn[o.second[k]].c_str(),k+1==o.second.size()?'\n':' ');
    }
}
