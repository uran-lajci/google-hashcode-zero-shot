#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int T,D,W,G;
vector<ll> L,A;
ll acc(ll w){
    for(int i=0;i<W;i++) if(w<=L[i]) return A[i];
    return 0;
}
int main(){
    scanf("%d %d %d %d",&T,&D,&W,&G);
    L.resize(W);A.resize(W);
    for(int i=0;i<W;i++) scanf("%lld %lld",&L[i],&A[i]);
    vector<string> nm(G);
    vector<ll> s(G),w(G),c(G),r(G);
    char buf[64];
    for(int i=0;i<G;i++){
        scanf("%s %lld %lld %lld %lld",buf,&s[i],&w[i],&c[i],&r[i]);
        nm[i]=buf;
    }
    vector<string> out;
    ll D2=(ll)D*D;
    vector<char> near(G,0);
    for(int i=0;i<G;i++){
        if(c[i]*c[i]+r[i]*r[i]<=D2){
            near[i]=1;
            out.push_back("LoadGift "+nm[i]);
            out.push_back("DeliverGift "+nm[i]);
        }
    }
    // far gift
    ll bestS=0;int bg=-1;ll bax=0,bay=0,bk=0;
    for(int g=0;g<G;g++){
        if(near[g]||s[g]<=bestS) continue;
        ll A1=acc(w[g]+2),A2=acc(w[g]+1);
        bool found=false;
        for(ll k=1;k+1<=T&&!found;k++){
            ll m=k+1;
            ll q=c[g]/m; if(c[g]%m<0) q--;
            ll p=r[g]/k; if(r[g]%k<0) p--;
            for(ll ax=q;ax<=q+1&&!found;ax++){
                if(ax<-A1||ax>A1) continue;
                ll dx=ax*m-c[g];
                if(dx*dx>D2) continue;
                for(ll ay=p;ay<=p+1;ay++){
                    if(ay<-A2||ay>A2) continue;
                    ll dy=ay*k-r[g];
                    if(dx*dx+dy*dy<=D2){
                        found=true;bax=ax;bay=ay;bk=k;break;
                    }
                }
            }
        }
        if(found){bestS=s[g];bg=g;}
        // note: bax etc. may be overwritten only when found for better gift
    }
    if(bg>=0){
        out.push_back("LoadCarrots 2");
        out.push_back("LoadGift "+nm[bg]);
        out.push_back(string(bax>=0?"AccRight ":"AccLeft ")+to_string(llabs(bax)));
        out.push_back("Float 1");
        out.push_back(string(bay>=0?"AccUp ":"AccDown ")+to_string(llabs(bay)));
        out.push_back("Float "+to_string(bk));
        out.push_back("DeliverGift "+nm[bg]);
    }
    printf("%d\n",(int)out.size());
    for(auto&x:out) printf("%s\n",x.c_str());
}
