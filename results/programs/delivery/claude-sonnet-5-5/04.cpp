#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int dc(int r1,int c1,int r2,int c2){
    ll d2=(ll)(r1-r2)*(r1-r2)+(ll)(c1-c2)*(c1-c2);
    ll s=(ll)sqrt((double)d2);
    while(s*s<d2)s++;
    while(s>0&&(s-1)*(s-1)>=d2)s--;
    return (int)s;
}
int main(){
    int R,Cc,D,T,ML;
    scanf("%d %d %d %d %d",&R,&Cc,&D,&T,&ML);
    int P;scanf("%d",&P);
    vector<int> wt(P);
    for(auto&x:wt)scanf("%d",&x);
    int W;scanf("%d",&W);
    vector<int> wr(W),wc(W);
    vector<vector<int>> st(P,vector<int>());
    vector<vector<int>> stock(W,vector<int>(P));
    for(int w=0;w<W;w++){
        scanf("%d %d",&wr[w],&wc[w]);
        for(int p=0;p<P;p++){scanf("%d",&stock[w][p]);if(stock[w][p]>0)st[p].push_back(w);}
    }
    int C;scanf("%d",&C);
    vector<int> orr(C),occ(C);
    vector<vector<int>> items(C);
    vector<ll> tw(C,0);
    for(int i=0;i<C;i++){
        int L;scanf("%d %d",&orr[i],&occ[i]);
        scanf("%d",&L);
        items[i].resize(L);
        for(auto&x:items[i]){scanf("%d",&x);tw[i]+=wt[x];}
    }
    vector<int> ord(C);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return tw[a]<tw[b];});
    vector<int> dt(D,0),dr(D,wr[0]),dcol(D,wc[0]);
    vector<string> out;
    for(int o:ord){
        vector<int> rem=items[o];
        // rollback records
        vector<array<int,4>> dsave; // id,time,r,c
        vector<array<int,3>> ssave; // w,p,count
        size_t outsz=out.size();
        bool ok=true;
        int orow=orr[o],ocol=occ[o];
        while(!rem.empty()&&ok){
            // build trip
            int cap=ML;
            vector<int> left;
            map<pair<int,int>,int> load; // (w,p)->cnt
            for(int p:rem){
                if(wt[p]>cap){left.push_back(p);continue;}
                int best=-1,bd=INT_MAX;
                auto&lst=st[p];
                for(int w:lst){
                    if(stock[w][p]>0){
                        int d=dc(wr[w],wc[w],orow,ocol);
                        if(d<bd){bd=d;best=w;}
                    }
                }
                if(best<0){ok=false;break;}
                stock[best][p]--;
                ssave.push_back({best,p,1});
                load[{best,p}]++;
                cap-=wt[p];
            }
            if(!ok)break;
            // drone
            int d=0;
            for(int i=1;i<D;i++)if(dt[i]<dt[d])d=i;
            vector<array<int,3>> ld; // w,p,cnt
            for(auto&kv:load)ld.push_back({kv.first.first,kv.first.second,kv.second});
            sort(ld.begin(),ld.end(),[&](const array<int,3>&a,const array<int,3>&b){
                int da=dc(wr[a[0]],wc[a[0]],orow,ocol),db=dc(wr[b[0]],wc[b[0]],orow,ocol);
                if(da!=db)return da>db;
                if(a[0]!=b[0])return a[0]<b[0];
                return a[1]<b[1];
            });
            dsave.push_back({d,dt[d],dr[d],dcol[d]});
            ll t=dt[d];int r=dr[d],c=dcol[d];
            map<int,int> dl;
            for(auto&l:ld){
                t+=dc(r,c,wr[l[0]],wc[l[0]])+1;
                r=wr[l[0]];c=wc[l[0]];
                out.push_back(to_string(d)+" L "+to_string(l[0])+" "+to_string(l[1])+" "+to_string(l[2]));
                dl[l[1]]+=l[2];
            }
            for(auto&kv:dl){
                t+=dc(r,c,orow,ocol)+1;
                r=orow;c=ocol;
                out.push_back(to_string(d)+" D "+to_string(o)+" "+to_string(kv.first)+" "+to_string(kv.second));
            }
            if(t>T){ok=false;break;}
            dt[d]=(int)t;dr[d]=r;dcol[d]=c;
            rem=left;
        }
        if(!ok){
            for(int i=(int)dsave.size()-1;i>=0;i--){
                auto&a=dsave[i];
                dt[a[0]]=a[1];dr[a[0]]=a[2];dcol[a[0]]=a[3];
            }
            for(auto&s:ssave)stock[s[0]][s[1]]+=s[2];
            out.resize(outsz);
        }
    }
    string res=to_string(out.size())+"\n";
    for(auto&s:out){res+=s;res+="\n";}
    fputs(res.c_str(),stdout);
}
