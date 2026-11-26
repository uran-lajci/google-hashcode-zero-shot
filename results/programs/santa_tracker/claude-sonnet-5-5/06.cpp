#include <bits/stdc++.h>
using namespace std;
int main(){
    long long T,D,W,G;
    cin>>T>>D>>W>>G;
    for(int i=0;i<W;i++){long long a,b;cin>>a>>b;}
    vector<string> acts;
    for(int i=0;i<G;i++){
        string n;long long s,w,c,r;
        cin>>n>>s>>w>>c>>r;
        if(c*c+r*r<=D*D){
            acts.push_back("LoadGift "+n);
            acts.push_back("DeliverGift "+n);
        }
    }
    printf("%d\n",(int)acts.size());
    for(auto&a:acts)printf("%s\n",a.c_str());
}
