#include <bits/stdc++.h>
#pragma GCC optimize("O3,unroll-loops")
using i64=long long;
using namespace std;
struct Net {
    
    int block=0; // 每个机房的节点数
    vector<int> node; // 每个机架接入的网络节点
    vector<i64> edges; // 在线链路存非负时延，离线时按位取反，按输入顺序编号
    vector<vector<pair<int,int>>> g; // 邻接表，每项为邻点和链路编号
    vector<unsigned char> online; // 节点是否通电
    array<array<i64,30>,30> route; // 机架间距离，-2 未算，-1 不可达
    vector<i64> best; // Dijkstra 复用的节点距离表
    vector<pair<i64,int>> heap; // Dijkstra 复用的小根堆
    vector<unsigned char> target; // 网络节点是否连接至少一个机架
    void init(int nodes,int d,int racks){
        block=nodes/d;node.resize(racks);g.resize(nodes);online.assign(nodes,1);
        best.resize(nodes);target.assign(nodes,0);invalidate();
    }
    void edge(int u,int v,i64 w){int i=edges.size();edges.push_back(w);g[u].emplace_back(v,i);g[v].emplace_back(u,i);}
    void invalidate(){for(auto& row:route)row.fill(-2);}
    void setDC(int d,bool up){int l=d*block;if(online[l]!=up){fill(online.begin()+l,online.begin()+l+block,up);invalidate();}}
    void setLink(int e,bool up){if((edges[e]>=0)!=up){edges[e]=~edges[e];invalidate();}}
    i64 dist(int a,int b){
        i64& cached=route[a][b];return cached==-2?cached=rebuild(a,b):cached;
    }
    i64 rebuild(int a,int b){
        int s=node[a],targetNode=node[b];if(!online[s]||!online[targetNode])return -1;if(s==targetNode)return 0;
        fill(target.begin(),target.end(),0);int left=0;
        for(int u:node)if(!target[u]){target[u]=1;left+=online[u];}
        fill(best.begin(),best.end(),-1);best[s]=0;heap.clear();heap.emplace_back(0,s);
        while(!heap.empty()){
            pop_heap(heap.begin(),heap.end());auto [ds,u]=heap.back();ds=-ds;heap.pop_back();if(ds!=best[u])continue;
            if(target[u]&&!--left)break;
            for(auto [v,e]:g[u])if(online[v]&&edges[e]>=0&&ds<=numeric_limits<i64>::max()-edges[e]){
                i64 dd=ds+edges[e];if(best[v]<0||dd<best[v]){best[v]=dd;heap.emplace_back(-dd,v);push_heap(heap.begin(),heap.end());}
            }
        }
        for(size_t r=0;r<node.size();++r)route[r][a]=route[a][r]=best[node[r]];
        return route[a][b];
    }
};

struct Cache {
    static const int S=400; // 循环链表的哨兵编号，也是镜像层数上限
    inline static long long size[S]{}; // 各镜像层的大小，所有缓存共用
    long long cap=0,used=0; // 容量与当前占用量
    array<short,S+1> prv{},nxt{}; // 层的前驱、后继；前驱为 -1 表示不存在
    Cache(){clear();}
    bool has(int l){return prv[l]>=0;}
    void clear(){prv.fill(-1);prv[S]=nxt[S]=S;used=0;}
    void unlink(int l){nxt[prv[l]]=nxt[l];prv[nxt[l]]=prv[l];prv[l]=-1;}
    void touch(int l){
        if(has(l)){if(nxt[l]==S)return;unlink(l);}
        else{
            if(size[l]>cap){if(nxt[S]!=S)clear();return;}
            while(used>cap-size[l]){int u=nxt[S];used-=size[u];unlink(u);}
            used+=size[l];
        }
        prv[l]=prv[S];nxt[l]=S;nxt[prv[S]]=l;prv[S]=l;
    }
};
struct Pull { long long reg,rack; }; // 仓库与机架的拉取量
inline Pull pull(Cache& m,Cache& r, vector<int>& layers){
    Pull q{};for(int l:layers){if(!m.has(l)){(r.has(l)?q.rack:q.reg)+=Cache::size[l];r.touch(l);}m.touch(l);}return q;
}
inline Pull preview( Cache& m, Cache& r, vector<int>& layers){
    for(int l:layers)if(!m.has(l)){Cache mc=m,rc=r;return pull(mc,rc,layers);}
    return {};
}

struct Logs {
    
    struct ACNode {
        array<int,8> go{}; // 字母转移，构建后补全失败转移
        i64 sum=0; // 在该状态结尾的所有告警模式的权重和
        i64 peak=0; // 构建前为最高单模式权重，构建后为是否达到固定告警阈值
    };
    vector<ACNode> ac{1}; // 告警模式的 AC 自动机，0 是根
    array<array<uint64_t,256>,17> decode{}; // 低32位为加一后的四位符号流，高位为赫夫曼终态
    array<array<int,2>,17> child{}; // 赫夫曼树的左右子结点
    int slot=0; // 本 tick 对应的窗口桶
    vector<i64> value; // 每台机器当前窗口的告警值
    vector<vector<i64>> window; // 按 tick 取模记录每台机器的告警分
    void init(int machines,int w, array<i64,9>& freq){
        value.assign(machines,0);window.assign(w,vector<i64>(machines));
        using Q=pair<i64,int>;priority_queue<Q,vector<Q>,greater<Q>> q;
        for(int i=0;i<9;++i)q.emplace(freq[i],i);
        for(int i=9;i<17;++i){auto [a,l]=q.top();q.pop();auto [b,r]=q.top();q.pop();child[i]={l,r};q.emplace(a+b,i);}
        for(int u=9;u<17;++u)for(int h=0;h<256;++h){
            auto& s=decode[u][h];int a=u,n=0;
            for(int i=7;i>=0;--i){a=child[a][h>>i&1];if(a<9){s|=uint64_t(a+1)<<(4*n++);a=16;}}s|=uint64_t(a)<<32;
        }
    }
    void pattern( string& s,i64 w){
        int u=0;for(char c:s){int j=c-'a',v=ac[u].go[j];if(!v){v=ac.size();ac[u].go[j]=v;ac.emplace_back();}u=v;}ac[u].sum+=w;ac[u].peak=max(ac[u].peak,w);
    }
    void build(i64 threshold){
        vector<int> fail(ac.size()),order;order.reserve(ac.size());
        for(int v:ac[0].go)if(v)order.push_back(v);
        for(size_t i=0;i<order.size();++i){
            int u=order[i],f=fail[u];ac[u].sum+=ac[f].sum;ac[u].peak=ac[u].peak>=threshold||ac[f].peak;
            for(int j=0;j<8;++j){int& v=ac[u].go[j];if(v){fail[v]=ac[f].go[j];order.push_back(v);}else v=ac[f].go[j];}
        }
    }
    void tick(int t){slot=t%(int)window.size();auto& row=window[slot];for(size_t x=0;x<row.size();++x){value[x]-=row[x];row[x]=0;}}
    static int hex(char h){return h<='9'?h-'0':(h|32)-'a'+10;}
    i64 add(int x,int bits, string& h){
        int ht=16,u=0,i=0;i64 score=0;bool lastWeight=0;
        auto feed=[&](int a){if(a==8)u=0;else{u=ac[u].go[a];score+=ac[u].sum;lastWeight|=ac[u].peak;}};
        for(;bits>=8;bits-=8,i+=2){
            auto s=decode[ht][hex(h[i])*16+hex(h[i+1])];ht=s>>32;
            for(unsigned v=s;v;v>>=4)feed((v&15)-1);
        }
        for(int k=0;k<bits;++k){ht=child[ht][hex(h[i+k/4])>>(3-k%4)&1];if(ht<9){feed(ht);ht=16;}}
        window[slot][x]+=score;value[x]+=score;return lastWeight;
    }
};
#include <unistd.h>


using i128=__int128_t;
const int LIM=20000;
static i64 divup(i64 n,i64 d){return n<d?(n!=0):n/d+(n%d!=0);}

struct IO{
    char buf[1<<16]; int at,end; // 管道输入块与读写位置
    string out; // 当前 tick 的完整回复
    int ch(){
        if(at==end){
            end=::read(0,buf,sizeof buf);
            at=0;
        }
        return buf[at++];
    }
    int first(){int c;do c=ch();while(c<=32);return c;}
    void word(string& s){s.clear();int c=first();for(;c>32;c=ch())s+=c;}
    i64 num(){int c=first();bool neg=c=='-';if(neg)c=ch();uint64_t v=0;for(;c>32;c=ch())v=v*10+c-'0';return neg?-v:v;}
    template<class T>void put(T x){
        char s[48];int n=0;if(x<0){out+='-';x=-x;}
        do{s[n++]='0'+x%10;x/=10;}while(x);
        while(n)out+=s[--n];
    }
    void flush(){out+="END\n";fwrite(out.data(),1,out.size(),stdout);fflush(stdout);out.clear();}
} io;

struct Tenant{
    int parent=-1,seen; // 父租户、真实时长样本数
    i64 quota,used; // 配额、子树已用核数
    i128 bill,rate; // 仿射账单常数、子树每 tick 运行费
    double observed,bias=1.2; // 时长倍率样本和、本轮预测倍率
    vector<int> path; // 从本租户到根的路径
};
struct Machine{
    int rack,alert=-100,lastFail=-100; // 所在机架、最近高权重告警和故障的 tick
    i64 cpu,mem; // 当前空闲核数和内存
    i64 totalCPU,totalMEM; // 单机总容量，用于判断作业是否永久无处运行
    bool up=1; // 是否正常
    Cache cache; // 机器层缓存
    vector<int> run; // 运行中作业的下标
};
struct Rack{
    int dc; // 所在机房
    Cache cache; // 机架层缓存
};
struct Image{
    vector<int> layer; // 从底到顶的镜像层
    i64 bytes; // 镜像层大小之和
    array<int,3> sample{}; // 粗筛机器时查看的三层
};
struct Input{
    i64 bytes,serial; // 同源输入的总大小、最长串行传输耗时
    int rack; // 输入源机架
};

struct Job{
    int id,w,img,src=-1; // 外部编号、所属工作流、镜像、外部输入机架
    int pb,pe,sb,se,need; // 前驱和后继的半开区间、未完成前驱数
    int pos=-1,x=-1,start; // 就绪/运行表位置、机器、输出机架、启动 tick
    int retry=-1; // 未投入工作流上次放置失败时的环境版本
    unsigned state:2,fit:30; // 作业状态、已证在线可容纳的机架位集
    i64 cpu,mem,e,zin,zout; // 核数、内存、估计时长、输入大小、输出大小
    i64 tail,rough,es,prep; // 精确尾长、含准备的预测尾长、最早起点、实际准备时长
    vector<Input> delivered; // 已完成前驱按实际机架累计的字节数与最长串行传输期
    double future=-1,charge; // 计划中的后继关键路径、当前作业的预估剩余费用
    uint64_t planPred,knownPred; // 静态约简前驱、原前驱列表中正在运行的位集
};
struct Flow{
    i64 largest; // 未完成作业最大核数，负值表示完成事件后需要重算
    int owner,lo,hi,dl,grace,latest,finished; // 租户、作业区间、截止、宽限、最后可收益 tick、完成数
    int audited=-1,stamp; // 最后完成 tick、松弛缓存版本、预测版本
    i64 value,cp,cores; // 初始价值、审计关键路径、未完成作业的核数和
    double work,critical,burn,remain,cost,rank; // 剩余估计核时、临界路径、运行中预计核时、完工估时、预计费用、优先级
    bool started; // 是否已投入资源
    int rescueAt=-1; // 本流程最近一次尝试救急重选的 tick
    int anchor=-1,planned=-100,planChange=-1; // 计划锚定机架、计划tick、完成状态版本
    double planCost,planSpan; // 整个未完成部分的预计费用、关键路径
    int exactTick=-1,exactEnvironment=-1; // 最后完整恢复的精确规划对应 tick 与环境版本
};
struct WarmHint{
    int stamp=-1; // 估价对应的七 tick 规划周期
    int machine; // 同时考虑拉取费与准备耗时后选中的机器
    Pull pull; // 同一台机器预计从仓库和机架拉取的字节数
};
struct Route{
    i64 time=-1; i128 fee; // 传输耗时（负数不可达）、传输费
};
using Candidate=pair<double,int>; // 负优先级与作业下标，或机器粗排分与机器下标

struct Solver{
    int T,D,N,R,M,K,L,I,P,W,E; // 初始化规模，含 tick 总数
    int t,commands,seen; // 当前 tick、指令数、完成样本数
    int rescues; // 本 tick 的救急重选次数，限制最坏规模规划开销
    int environment; // 会改变资源、缓存、估时或冷启动协作机会的环境版本
    bool invested; // 本 tick 是否已为共享镜像做过一次冷启动投资
    bool fitDirty; // 本 tick 有机器故障，需要在规划前清除容量正命中
    i64 Breg,Brack,Bnet,Ureg,Unet,maxWeight; // 带宽、费用、最大告警模式权重
    i64 minPrice; // 全场最低 CPU 单价
    double observed; // 全场真实时长倍率样本和
    vector<i64> price; // 机房 CPU 单价
    vector<Rack> racks; vector<Machine> machines; // 机架和机器状态
    vector<Tenant> tenants; vector<Image> images; // 租户和镜像
    vector<pair<int,i64>> undo; // 配额的撤销记录
    vector<Job> jobs; vector<Flow> flows; // 按工作流连续存放的作业、工作流
    vector<int> pred,succ,ready,live,touched; // 前驱、后继、就绪表、在线机器、待更新的工作流
    vector<i64> headroom,limit; // 本轮的租户路径剩余配额与总配额
    vector<Candidate> queue; // 候选优先级
    double stage[2][40],charges[2][40]; // 规划双缓冲：当前试算和胜出方案的阶段耗时、费用
    int scratch=0; // 正在写入的规划槽
    using Snapshot=array<pair<double,double>,40>; // 单条可调度工作流的后继尾长与作业费用缓冲区
    vector<vector<int>> rackMachines; // 各机架机器下标
    vector<double> committed; // 各租户子树已接纳工作流的剩余占核时
    array<i64,30> rackCPU{},rackMEM{}; // 每机架在本轮开始时的最大单机空闲资源
    array<double,30> rackDebt{},inverseCores; // 机架的剩余核时、有效在线核数倒数
    array<i64,30> rackCores{},maxCPU{},maxMEM{}; // 机架在线总核数、单机额定资源上界
    array<int,30> representative; // 每机架的代表缓存机器，按 tick 重建
    unordered_map<int,int> jid{65536}; // 开放寻址表将九位作业编号映射到连续下标
    vector<WarmHint> warmHints; // 按机架与镜像缓存本轮乐观暖度
    Net net; Logs logs; // 动态最短路和告警统计
    string op,token; // 协议关键字与临时字符串
    static bool fits( Machine& m, Job& j){return m.totalCPU>=j.cpu&&m.totalMEM>=j.mem;}

    void init(){
        T=io.num();
        D=io.num();N=io.num();R=io.num();M=io.num();K=io.num();L=io.num();I=io.num();P=io.num();W=io.num();E=io.num();
        Breg=io.num();Brack=io.num();Bnet=io.num();Ureg=io.num();Unet=io.num();
        price.resize(D);for(auto& p:price)p=io.num();minPrice=*min_element(price.begin(),price.end());
        net.init(N,D,R);racks.resize(R);machines.resize(M);tenants.resize(K);images.resize(I);
        for(int r=0;r<R;++r){racks[r].dc=io.num();net.node[r]=io.num();racks[r].cache.cap=io.num();}
        for(auto& x:machines){x.rack=io.num();x.cpu=x.totalCPU=io.num();x.mem=x.totalMEM=io.num();x.cache.cap=io.num();}
        for(int e=0;e<E;++e){int u=io.num(),v=io.num();i64 w=io.num();net.edge(u,v,w);}
        for(auto& a:tenants){a.parent=io.num();a.quota=io.num();}
        for(int a=0;a<K;++a)for(int u=a;u>=0;u=tenants[u].parent)tenants[a].path.push_back(u);
        for(int l=0;l<L;++l)Cache::size[l]=io.num();
        for(auto& a:images){
            a.layer.resize(io.num());int mx=0;
            for(int q=0;q<(int)a.layer.size();++q){int l=a.layer[q]=io.num();a.bytes+=Cache::size[l];if(Cache::size[l]>Cache::size[a.layer[mx]])mx=q;}
            int n=a.layer.size();if(n)a.sample={a.layer[0],a.layer[mx==0?n/2:mx],a.layer[n-1]};
        }
        array<i64,9> freq;for(auto& f:freq)f=io.num();logs.init(M,W,freq);
        for(int i=0;i<P;++i){io.word(token);i64 w=io.num();logs.pattern(token,w);maxWeight=max(maxWeight,w);}logs.build(max<i64>(8,maxWeight/6));
        jobs.reserve(60000);
        warmHints.resize(R*I);headroom.resize(K);limit.resize(K);committed.resize(K);
        rackMachines.resize(R);for(int x=0;x<M;++x)rackMachines[machines[x].rack].push_back(x);
        
    }
    void pushReady(int k){jobs[k].pos=ready.size();ready.push_back(k);}
    void popReady(int k){int i=jobs[k].pos;ready[i]=ready.back();jobs[ready[i]].pos=i;ready.pop_back();jobs[k].pos=-1;}
    void arrive(){
        io.num();Flow f{};f.owner=io.num();f.value=io.num();f.dl=io.num();int n=io.num();
        f.grace=max(5,(f.dl-t)/3);f.latest=min<i64>(T,(i64)f.dl+f.grace-1);
        f.lo=jobs.size();f.hi=f.lo+n;int w=flows.size();jobs.resize(f.hi);
        for(int k=f.lo;k<f.hi;++k){
            auto& j=jobs[k];j.id=io.num();jid[j.id]=k;j.w=w;j.cpu=io.num();j.mem=io.num();j.e=io.num();
            j.img=io.num();j.src=io.num();j.zin=io.num();j.zout=io.num();j.need=io.num();j.pb=pred.size();
            for(int q=0;q<j.need;++q){int p=jid[io.num()];pred.push_back(p);++jobs[p].se;}
            j.pe=pred.size();f.work+=(double)j.cpu*j.e;f.cores+=j.cpu;f.largest=max(f.largest,j.cpu);if(!j.need)pushReady(k);
            if(none_of(machines.begin(),machines.end(),[&]( Machine& x){return fits(x,j);} ))f.latest=0;
        }
        int total=succ.size();
        for(int k=f.lo;k<f.hi;++k){auto& j=jobs[k];total+=j.se;j.sb=j.se=total;}
        succ.resize(total);
        for(int k=f.hi;k-->f.lo;){
            auto& j=jobs[k];j.tail+=j.e;j.rough+=j.e+1;
            for(int q=j.pb;q<j.pe;++q){auto& p=jobs[pred[q]];int a=--p.sb;succ[a]=k;p.tail=max(p.tail,j.tail);p.rough=max(p.rough,j.rough);}
        }
        if(n<=40){
            uint64_t ancestors[40]{};
            for(int k=f.lo;k<f.hi;++k){
                auto& j=jobs[k];uint64_t mask=0;
                for(int q=j.pb;q<j.pe;++q){int p=pred[q]-f.lo;mask|=1ULL<<p;ancestors[k-f.lo]|=ancestors[p]|(1ULL<<p);}
                while(mask){int p=63-__builtin_clzll(mask);j.planPred|=1ULL<<p;mask&=~(ancestors[p]|(1ULL<<p));}
            }
        }
        flows.push_back(f);
    }
    i64 head(int o){
        i64 ans=LLONG_MAX;for(int u:tenants[o].path)ans=min(ans,tenants[u].quota-tenants[u].used);return ans;
    }
    void account( Job& j,int x,int sign,i128 immediate=0){
        i128 r=(i128)j.cpu*price[racks[machines[x].rack].dc]*sign;
        for(int u:tenants[flows[j.w].owner].path){
            auto& a=tenants[u];a.used+=j.cpu*sign;a.rate+=r;a.bill+=immediate-r*t;
        }
    }
    void mark(int k){
        for(int q=jobs[k].sb;q<jobs[k].se;++q){auto& a=jobs[succ[q]];if(a.pe-a.pb<=40)a.knownPred^=1ULL<<(find(pred.begin()+a.pb,pred.begin()+a.pe,k)-pred.begin()-a.pb);}
    }
    void release(int k,bool done){
        mark(k);
        auto& j=jobs[k];auto& x=machines[j.x];auto& f=flows[j.w];
        account(j,j.x,-1);x.cpu+=j.cpu;x.mem+=j.mem;
        x.run[j.pos]=x.run.back();jobs[x.run[j.pos]].pos=j.pos;x.run.pop_back();
        j.pos=j.x=-1;
        if(!done){j.state=0;pushReady(k);return;}
        j.state=2;++f.finished;f.work-=(double)j.cpu*j.e;f.cores-=j.cpu;if(j.cpu==f.largest)f.largest=-1;
        if(j.e){
            double ratio=clamp((t-j.start-j.prep)/(double)j.e,.25,4.);
            auto& a=tenants[f.owner];a.observed+=ratio;++a.seen;observed+=ratio;++seen;
        }
        i64 serial=divup(j.zout,Bnet);
        for(int q=j.sb;q<j.se;++q){
            int v=succ[q];auto& next=jobs[v];
            {
                auto& inputs=next.delivered;
                auto it=find_if(inputs.begin(),inputs.end(),[&]( Input& a){return a.rack==x.rack;});
                if(it==inputs.end())inputs.push_back({j.zout,serial,x.rack});
                else{it->bytes+=j.zout;it->serial=max(it->serial,serial);}
            }
            if(!--next.need)pushReady(v);
        }
        vector<Input>().swap(j.delivered);
    }
    i64 slack(int k){
        auto& j=jobs[k];if(j.state==2)return 0;auto& f=flows[j.w];
        if(f.audited!=f.finished){
            f.audited=f.finished;f.cp=0;
            for(int u=f.lo;u<f.hi;++u){
                auto& a=jobs[u];if(a.state==2)continue;a.es=0;
                for(int q=a.pb;q<a.pe;++q){auto& p=jobs[pred[q]];if(p.state!=2)a.es=max(a.es,p.es+p.e);}
                f.cp=max(f.cp,a.tail);
            }
        }
        return f.cp-j.es-j.tail;
    }
    void event(){
        io.word(op);
        if(op!="LOG")++environment;
        if(op=="WF"){arrive();return;}
        int a=io.num();
        if(op=="DONE")release(jid[a],1);
        else if(op=="FAIL"||op=="UP"){
            auto& x=machines[a];x.up=op=="UP";x.cache.clear();
            if(!x.up){fitDirty=1;x.lastFail=t;x.alert=-100;while(!x.run.empty())release(x.run.back(),0);}
        }else if(op=="DCDOWN"||op=="DCUP"){net.setDC(a,op=="DCUP");}
        else if(op=="LDOWN"||op=="LUP"){net.setLink(a,op=="LUP");}
        else if(op=="QUOTA"){undo.emplace_back(a,tenants[a].quota);tenants[a].quota=io.num();}
        else if(op=="UNDO"){while(a--){auto [u,q]=undo.back();undo.pop_back();tenants[u].quota=q;}}
        else{
            int b=io.num();io.word(token);
            if(logs.add(a,b,token))machines[a].alert=t;
        }
    }
    void answers(int n){
        io.out+="ANS";
        while(n--){
            io.word(op);int a=io.num();i128 x=0;
            if(op=="BILL")x=tenants[a].bill+tenants[a].rate*t;
            else if(op=="HEAD")x=head(a);
            else if(op=="ROUTE")x=net.dist(a,io.num());
            else if(op=="ALARM")x=logs.value[a];
            else if(op=="SLACK")x=slack(jid[a]);
            else if(op=="CACHE")x=machines[a].cache.used;
            else x=racks[a].cache.used;
            io.out+=' ';io.put(x);
        }
        io.out+='\n';
    }
    void command(const char* name,int id,int x=-1){
        io.out+=name;io.put(id);if(x>=0){io.out+=' ';io.put(x);}io.out+='\n';++commands;
    }
    double reward( Flow& f,double time){
        if(time>T)return 0;
        return (double)f.value*clamp((f.dl+double(f.grace)-time)/f.grace,0.,1.);
    }
    double payoff( Flow& f,double length){
        return .72*reward(f,t+length)+.28*reward(f,t+max(1.,.77*length));
    }
    static double after( Job& j,double bias){return j.future>=0?j.future:(j.tail-j.e)*bias+(j.rough-j.tail-1);}
    double duration( Job& j){
        double d=max(1.,j.e*tenants[flows[j.w].owner].bias);
        if(j.state!=1)return d;
        double elapsed=t-j.start-j.prep;
        return elapsed<0?d-elapsed:max({1.,d-elapsed,.25*d});
    }
    double debt( Flow& f){return f.work*tenants[f.owner].bias+f.cores;}
    i64 prepare(Pull p){return divup(p.reg,Breg)+divup(p.rack,Brack);}
    void exchange( Flow& f,Snapshot& s){
        for(int k=f.lo;k<f.hi;++k){swap(jobs[k].future,s[k-f.lo].first);swap(jobs[k].charge,s[k-f.lo].second);}
    }
    void planTails( Flow& f){
            for(int k=f.lo;k<f.hi;++k)jobs[k].future=0;
            for(int k=f.hi;k-->f.lo;){
                auto& j=jobs[k];j.charge=charges[scratch][k-f.lo];
                for(uint64_t mask=j.planPred;mask;mask&=mask-1){auto& p=jobs[f.lo+__builtin_ctzll(mask)];p.future=max(p.future,stage[scratch][k-f.lo]+j.future);}
            }
    }
    bool plan(Flow& f,int r,bool needTails=1,int machine=-1){
        if(machine<0)machine=representative[r];
        if(machine<0||!net.online[net.node[r]])return 0;
        Cache mc=machines[machine].cache,rc=racks[r].cache;
        double finish[40];
        double span=0,cost=0,work=0,unit=price[racks[r].dc];int lastImage=-1;
        for(int k=f.lo;k<f.hi;++k){
            int at=k-f.lo;auto& j=jobs[k];double& phase=stage[scratch][at];
            finish[at]=phase=charges[scratch][at]=0;if(j.state==2)continue;
            double d=duration(j),prep=0,charge=0,begin=0;
            if(j.state==1){
                finish[at]=phase=d;charge=d*j.cpu*price[racks[machines[j.x].rack].dc];
            }else{
                if(!(j.fit>>r&1)){if(none_of(rackMachines[r].begin(),rackMachines[r].end(),[&](int x){return machines[x].up&&fits(machines[x],j);} ))return 0;j.fit|=1u<<r;}
                double transfer=0;
                auto input=[&](int source,i64 z){
                    if(source==r)return 1;
                    i64 distance=net.dist(source,r);if(distance<0)return 0;
                    prep=max(prep,(double)(distance+divup(z,Bnet)));transfer+=(double)z*distance*Unet;return 1;
                };
                if(j.src>=0&&!input(j.src,j.zin))return 0;
                for( Input& in:j.delivered)if(in.rack!=r){
                    i64 distance=net.dist(in.rack,r);if(distance<0)return 0;
                    prep=max(prep,(double)(distance+in.serial));transfer+=(double)in.bytes*distance*Unet;
                }
                for(uint64_t mask=j.planPred;mask;mask&=mask-1)begin=max(begin,finish[__builtin_ctzll(mask)]);
                for(uint64_t mask=j.knownPred;mask;mask&=mask-1){
                    auto& p=jobs[pred[j.pb+__builtin_ctzll(mask)]];
                    if(!input(machines[p.x].rack,p.zout))return 0;
                }
                Pull p{};if(j.img!=lastImage)p=pull(mc,rc,images[j.img].layer);
                lastImage=images[j.img].bytes<=mc.cap?j.img:-1;
                if(p.reg||p.rack)prep=max(prep,(double)prepare(p));
                charge=transfer+(double)p.reg*Ureg+(prep+d)*j.cpu*unit;
                phase=prep+d;finish[at]=begin+phase;
            }
            charges[scratch][at]=charge;cost+=charge;work+=phase*j.cpu;span=max(span,finish[at]);
        }
        span=max(span,work/max<i64>(1,min(limit[f.owner],rackCores[r])));
        double own=f.started&&r==f.anchor?debt(f):0;
        span+=.25*max(0.,rackDebt[r]-own)/max<i64>(1,rackCores[r]);
        f.planCost=cost;f.planSpan=span;if(needTails)planTails(f);
        return 1;
    }
    void forecast(Flow& f){
        if(f.planned>t-7&&f.planChange==f.finished&&f.anchor>=0)return;
        i128 bytes[30]{};unsigned coarseSources=0;
        for(int k=f.lo;k<f.hi;++k){auto& j=jobs[k];if(j.state==2)continue;
            if(j.src>=0){coarseSources|=1u<<j.src;bytes[j.src]+=j.zin;}
            
            for( Input& in:j.delivered){bytes[in.rack]+=in.bytes;coarseSources|=1u<<in.rack;}
        }
        double total[30];
        for(unsigned mask=coarseSources;mask;mask&=mask-1){int source=__builtin_ctz(mask);total[source]=(double)bytes[source];}
        int best=-1,oldAnchor=f.anchor;double merit=-INFINITY,bestCost=0,bestSpan=0;
        double delayUnit=.25*f.value/max(5,f.latest-t);
        array<pair<double,int>,30> candidates;int count=0;
        int probe=f.lo;while(probe<f.hi&&jobs[probe].state!=0)++probe;
        for(int r=0;r<R;++r){
            if(probe<f.hi&&(maxCPU[r]<jobs[probe].cpu||maxMEM[r]<jobs[probe].mem))continue;
            double key=price[racks[r].dc]*f.work*tenants[f.owner].bias;
            if(representative[r]<0||!net.online[net.node[r]])continue;
            double transfer=0;
            for(unsigned mask=coarseSources&~(1u<<r);mask;mask&=mask-1){
                int source=__builtin_ctz(mask);i64 d=net.dist(source,r);if(d<0){transfer=-1;break;}
                transfer+=total[source]*d*Unet;
            }
            if(transfer<0)key=1e50;else key+=transfer;
            if(r==f.anchor)key*=.92;
            if(probe<f.hi){
                auto& j=jobs[probe];auto& hint=warmHints[r*I+j.img];
                if(hint.stamp!=t/7){
                    hint.stamp=t/7;hint.machine=-1;
                    double bestPull=INFINITY,unit=(double)j.cpu*price[racks[r].dc];
                    auto& rc=racks[r].cache;
                    for(int x:rackMachines[r]){
                        auto& m=machines[x];if(!m.up||m.cpu<j.cpu||m.mem<j.mem)continue;
                        Pull p{};for(int l:images[j.img].sample)if(!m.cache.has(l))(rc.has(l)?p.rack:p.reg)+=Cache::size[l];
                        double score=(double)p.reg*Ureg+prepare(p)*unit;
                        if(score<bestPull){bestPull=score;hint.machine=x;if(!score)break;}
                    }
                    hint.pull=exact(hint.machine<0?representative[r]:hint.machine,j);
                }
                key+=(double)hint.pull.reg*Ureg+(double)prepare(hint.pull)*j.cpu*price[racks[r].dc];
            }
            if(!f.started)key+=min(.1*key,delayUnit*rackDebt[r]*inverseCores[r]);
            candidates[count++]={key,r};
        }
        partial_sort(candidates.begin(),candidates.begin()+min(4,count),candidates.begin()+count);
        for(int i=0;i<min(4,count);++i){
            int r=candidates[i].second;
            int machine=-1;
            if(!f.started&&probe<f.hi){
                auto& j=jobs[probe];int x=warmHints[r*I+j.img].machine;
                if(x>=0&&machines[x].up&&machines[x].cpu>=j.cpu&&machines[x].mem>=j.mem)machine=x;
            }
            if(!plan(f,r,0,machine))continue;
            double key=payoff(f,f.planSpan)-f.planCost;
            if(r==f.anchor)key+=.015*f.value;
            if(key>merit){
                merit=key;best=r;bestCost=f.planCost;bestSpan=f.planSpan;
                scratch^=1;
            }
        }
        if(f.started&&best!=oldAnchor){
            double own=debt(f);
            if(oldAnchor>=0)rackDebt[oldAnchor]-=own;
            if(best>=0)rackDebt[best]+=own;
        }
        f.anchor=best;f.planned=t;f.planChange=f.finished;
        if(best>=0){
            f.planCost=bestCost;f.planSpan=bestSpan;
            scratch^=1;planTails(f);
            f.exactTick=t;f.exactEnvironment=environment;
        }
        else{f.planSpan=1e9;f.planCost=1e30;}
    }
    void touch(Flow& f,int w){
        if(f.stamp==t)return;
        f.stamp=t;f.critical=f.burn=0;f.rank=0;touched.push_back(w);
    }
    Pull exact(int x, Job& j){
        auto& m=machines[x];return preview(m.cache,racks[m.rack].cache,images[j.img].layer);
    }
    bool sharedWarmth(int k){
        auto& j=jobs[k];auto& a=images[j.img];
        // 仅给另一条可调度工作流也能复用的冷镜像留一次入场机会。
        for(int i=0;i<min(64,(int)queue.size());++i){
            auto& q=jobs[queue[i].second];
            if(q.w==j.w||q.state!=0||head(flows[q.w].owner)<q.cpu)continue;
            auto& b=images[q.img];if(!b.bytes)continue;
            for(int l:a.sample)for(int r:b.sample)
                if(l==r&&Cache::size[l]>=max<i64>(1,min(a.bytes,b.bytes)/4))return 1;
        }
        return 0;
    }
    double waitJob( Job& j,int r){
        double best=1e9;
        for(int x:rackMachines[r]){
            auto& m=machines[x];
            if(!m.up||!fits(m,j))continue;
            if(m.cpu>=j.cpu&&m.mem>=j.mem)return 0;
            vector<pair<double,int>> releases;releases.reserve(m.run.size());
            for(int k:m.run)releases.emplace_back(duration(jobs[k]),k);
            sort(releases.begin(),releases.end());i64 cpu=m.cpu,mem=m.mem;
            for(auto [when,k]:releases){cpu+=jobs[k].cpu;mem+=jobs[k].mem;if(cpu>=j.cpu&&mem>=j.mem){best=min(best,when);break;}}
        }
        return best;
    }
    bool rescue(int k){
        auto& j=jobs[k];auto& f=flows[j.w];
        if(!f.started||f.anchor<0||f.rescueAt==t||rescues>=8)return 0;
        f.rescueAt=t;
        auto room=[&](int r){
            if(!net.online[net.node[r]])return 0;
            for(int x:rackMachines[r])if(machines[x].up&&machines[x].cpu>=j.cpu&&machines[x].mem>=j.mem)return 1;
            return 0;
        };
        if(room(f.anchor))return 0;
        ++rescues;
        Flow saved=f;Snapshot prior{};exchange(f,prior);
        int old=f.anchor,best=-1;double merit=max(0.,payoff(f,f.planSpan+.5*waitJob(j,old))-f.planCost)+.005*f.value;
        for(int r=0;r<R;++r)if(r!=old&&room(r)&&plan(f,r)){
            double key=payoff(f,f.planSpan)-f.planCost;
            if(key>merit){merit=key;best=r;}
        }
        double own=debt(f);
        if(best>=0){
            rackDebt[old]-=own;rackDebt[best]+=own;f.anchor=best;f.exactTick=f.exactEnvironment=-1;
            if(place(k)){f.planned=t;f.planChange=f.finished;return 1;}
            rackDebt[old]+=own;rackDebt[best]-=own;
        }
        // 试算可能覆盖作业预测；未真正启动时完整恢复原锚定规划。
        f=saved;
        exchange(f,prior);
        return 0;
    }
    bool place(int k){
        auto& j=jobs[k];auto& f=flows[j.w];
        if(f.anchor>=0&&((f.exactTick==t&&f.exactEnvironment==environment)||plan(f,f.anchor))){
            f.exactTick=t;f.exactEnvironment=environment;f.remain=f.planSpan;f.cost=f.planCost;
        }
         double bias=tenants[f.owner].bias,dur=max(1.,j.e*bias);
         double tail=after(j,bias),later=max(0.,f.cost-j.charge);
        auto& img=images[j.img];
        Route route[30];unsigned possible=0;
        int end=f.anchor<0?R:f.anchor+1;
        for(int r=max(0,f.anchor);r<end;++r){
            if(rackCPU[r]<j.cpu||rackMEM[r]<j.mem)continue;
            i64 delay=0;i128 fee=0;
            for( auto& in:j.delivered)if(in.rack!=r){
                i64 d=net.dist(in.rack,r);if(d<0){delay=-1;break;}
                delay=max(delay,d+in.serial);fee+=(i128)in.bytes*d*Unet;
            }
            if(delay>=0&&j.src>=0&&j.src!=r){i64 d=net.dist(j.src,r);if(d<0)delay=-1;else{delay=max(delay,d+divup(j.zin,Bnet));fee+=(i128)j.zin*d*Unet;}}
            if(delay<0)continue;
            possible|=1u<<r;route[r]={delay,fee};
        }
        if(!possible)return 0;
         double horizon=f.latest-t,loss=(double)f.value/max(5,f.grace);
         double delayPrice=.09*f.value/max(4.,horizon)+.48*loss*clamp(1.+(t+f.remain-f.dl)/6.,0.,1.);
        auto risk=[&]( Machine& x){
            return max(0,7-(t-x.alert))*(dur+3.)*j.cpu*(.22+.07*minPrice)
                 + max(0,5-(t-x.lastFail))*.15*j.cpu;
        };
        int best=-1,warm=-1;i64 bestPrep=0;i128 bestFee=0;double bestKey=INFINITY;bool bestInvestment=0;
        vector<int>& choices=f.anchor>=0?rackMachines[f.anchor]:live;
        for(int id:choices){
            auto& x=machines[id];int r=x.rack;
            if(!x.up||!(possible>>r&1)||x.cpu<j.cpu||x.mem<j.mem)continue;
            Pull p=exact(id,j);
            i64 prep=max(route[r].time,prepare(p));
            if(prep>=horizon)continue;
            i128 fee=route[r].fee+(i128)p.reg*Ureg;
            double run=(double)j.cpu*price[racks[r].dc]*(prep+dur),cost=(double)fee+run;
            double key=cost+delayPrice*prep+risk(x)+.15*(x.cpu>j.cpu)+.07*(x.mem>j.mem);
            if(key>=bestKey)continue;
            double span=max(f.remain,prep+dur+tail);
            double opt=max(.77*f.remain,prep+max(1.,.72*dur)+.77*tail);
            double income=.72*reward(f,t+span)+.28*reward(f,t+opt);
            bool investment=0;
            if((cost>income||cost+later>income*1.03)&&!f.started&&!invested&&p.reg){
                if(warm<0)warm=sharedWarmth(k);
                if(warm){double allowance=.45*payoff(f,f.critical);investment=allowance>income;income=max(income,allowance);}
            }
            if(cost>income||cost+later>income*(f.started?1.12:1.03))continue;
            key+=(f.value-income)*.76;
            if(key<bestKey){bestKey=key;best=id;bestPrep=prep;bestFee=fee;bestInvestment=investment;}
        }
        if(best<0)return 0;
        auto& x=machines[best];pull(x.cache,racks[x.rack].cache,img.layer);
        ++environment;
        invested|=bestInvestment;
        if(!f.started&&f.anchor>=0)rackDebt[f.anchor]+=debt(f);
        popReady(k);j.pos=x.run.size();x.run.push_back(k);j.state=1;j.x=best;j.start=t;j.prep=bestPrep;
        mark(k);x.cpu-=j.cpu;x.mem-=j.mem;f.started=1;account(j,best,1,bestFee);command("RUN ",j.id,best);return 1;
    }
    void schedule(){
        live.clear();touched.clear();queue.clear();rackCPU.fill(0);rackMEM.fill(0);commands=0;invested=0;
        i64 onlineCPU=0,freeCPU=0;
        rescues=0;committed.assign(K,0);rackDebt.fill(0);rackCores.fill(0);
        maxCPU.fill(0);maxMEM.fill(0);representative.fill(-1);
        double quality[30];
        double global=(observed+9.6)/(seen+8.);
        for(int o=0;o<K;++o){
            auto& a=tenants[o];a.bias=(a.observed+4*global)/(a.seen+4.);
            limit[o]=LLONG_MAX;for(int p:a.path)limit[o]=min(limit[o],tenants[p].quota);
        }
        for( Flow& f:flows)if(f.started&&f.work>0&&t<f.latest){
            double work=debt(f);
            for(int p:tenants[f.owner].path)committed[p]+=work;
            if(f.anchor>=0)rackDebt[f.anchor]+=work;
        }
        for(int id=0;id<M;++id){
            auto& x=machines[id];if(!x.up)continue;
            int rack=x.rack;
            rackCores[rack]+=x.totalCPU;
            maxCPU[rack]=max(maxCPU[rack],x.totalCPU);maxMEM[rack]=max(maxMEM[rack],x.totalMEM);
            double score=x.cache.cap+3*x.totalCPU+x.totalMEM;
            if(representative[rack]<0||score>quality[rack]){quality[rack]=score;representative[rack]=id;}
            for(size_t i=0;i<x.run.size();){
                int k=x.run[i];auto& j=jobs[k];auto& f=flows[j.w];
                if(t>=f.latest&&commands<LIM){command("KILL ",j.id);release(k,0);++environment;continue;}
                ++i;onlineCPU+=j.cpu;
                 double bias=tenants[f.owner].bias,dur=max(1.,j.e*bias);
                double left=duration(j);
                touch(f,j.w);f.critical=max(f.critical,left+after(j,bias));
                f.burn+=j.cpu*max(0.,dur+1-left);
            }
            onlineCPU+=x.cpu;freeCPU+=x.cpu;live.push_back(id);rackCPU[x.rack]=max(rackCPU[x.rack],x.cpu);rackMEM[x.rack]=max(rackMEM[x.rack],x.mem);
        }
        if(t==T||live.empty()||commands==LIM)return;
        for(int r=0;r<R;++r)inverseCores[r]=1./max<i64>(1,rackCores[r]);
        for(int o=0;o<K;++o)headroom[o]=head(o);
        vector<i64> quickest(I,-1); // 本轮各镜像在全部在线机器上的最小拉取时间，按需计算
        // 回收已经没有收益的就绪项，并从就绪与运行前沿估计剩余关键路径。
        for(size_t i=0;i<ready.size();){
            int k=ready[i];auto& j=jobs[k];auto& f=flows[j.w];
            if(t>=f.latest){popReady(k);vector<Input>().swap(j.delivered);continue;}
            ++i;
            if(!f.started&&j.retry==environment)continue;
            if(j.cpu>min(freeCPU,headroom[f.owner])||f.largest>limit[f.owner])continue;
            // 单作业仅准备镜像就无法获益时，不再进入流程规划和候选队列。
            if(!f.started&&f.hi-f.lo==1){
                i64& prep=quickest[j.img];
                if(prep<0){prep=LLONG_MAX;for(int x:live){Pull p=exact(x,j);prep=min(prep,prepare(p));}}
                if(prep>=f.latest-t){j.retry=environment;continue;}
            }
            touch(f,j.w);f.critical=max(f.critical,j.tail*tenants[f.owner].bias+j.rough-j.tail);
        }
        if(touched.empty())return;
        for(int w:touched){
            auto& f=flows[w];double bias=tenants[f.owner].bias;
            if(f.largest<0){f.largest=0;for(int k=f.lo;k<f.hi;++k)if(jobs[k].state!=2)f.largest=max(f.largest,jobs[k].cpu);}
            // 配额未来可以恢复，因此只暂停整条工作流的新增投入，不永久放弃。
            if(f.largest>limit[f.owner]||f.hi-f.lo>40)continue;
            double work=max(0.,f.work*bias+f.cores-f.burn);
            forecast(f);
            f.remain=max(1.,f.planSpan-(t-f.planned));f.cost=f.planCost;
            if(!f.started){
                double ahead=0;for(int p:tenants[f.owner].path)ahead=max(ahead,committed[p]/max<i64>(1,min(tenants[p].quota,onlineCPU)));
                f.remain+=.45*ahead*max(0.,1.-headroom[f.owner]*f.remain/max(1.,work));
            }
            double profit=payoff(f,f.remain)-f.cost;
            if(!f.started)profit=max(profit,.15*(payoff(f,f.critical)-max(0.,f.work*bias-f.burn)*minPrice));
            if(profit<=0)continue;
            if(!f.started&&f.hi-f.lo>12&&(f.cost>.7*f.value||t+1.25*f.remain>min(T,f.dl)))continue;
            double slack=max(0.,f.dl-t-f.remain),urgent=1.+2./(3.+slack);
            double progress=f.started?1.8+1.2*f.finished/max(1,f.hi-f.lo):1.;
            f.rank=profit*urgent*progress/(18.+.8*work+3.*f.remain);
        }
        for(int k:ready){
            auto& j=jobs[k];auto& f=flows[j.w];
            if(f.stamp!=t||f.rank<=0||(!f.started&&j.retry==environment)||j.cpu>min(freeCPU,headroom[f.owner]))continue;
            double lead=(j.tail*tenants[f.owner].bias+j.rough-j.tail)/max(1.,f.critical);
            queue.push_back({-f.rank*(.78+.22*lead),k});
        }
        int misses=max(144,M),chunk=max(400,4*M),n=queue.size();
        for(int lo=0;lo<n&&misses>0&&commands<LIM;lo+=chunk){
            int hi=min(n,lo+chunk);
            if(hi<n)nth_element(queue.begin()+lo,queue.begin()+hi,queue.end());
            sort(queue.begin()+lo,queue.begin()+hi);
            for(int i=lo;i<hi&&misses>0&&commands<LIM;++i){
                int k=queue[i].second;auto& j=jobs[k]; int owner=flows[j.w].owner;
                if(j.cpu>freeCPU||head(owner)<j.cpu)continue;
                if(place(k)||rescue(k))freeCPU-=j.cpu;else{--misses;if(!flows[j.w].started)jobs[k].retry=environment;}
            }
            if(hi<n&&misses>0&&commands<LIM){
                for(int o=0;o<K;++o)headroom[o]=head(o);
                queue.erase(remove_if(queue.begin()+hi,queue.end(),[&]( Candidate& c){
                    auto& j=jobs[c.second];return j.cpu>min(freeCPU,headroom[flows[j.w].owner]);
                }),queue.end());n=queue.size();
            }
        }
    }
    void run(){
        init();
        for(int tick=1;tick<=T;++tick){
            io.word(op);
            t=io.num();int ne=io.num(),nq=io.num();logs.tick(t);
            while(ne--)event();
            if(fitDirty){for(auto& j:jobs)j.fit=0;fitDirty=0;}
            answers(nq);schedule();io.flush();
        }
    }
};

int main(){
#ifdef _WIN32
    _setmode(0,32768); // Windows 文本协议输入改用二进制模式，避免逐字节转换
#endif

    Solver solver{};solver.run();
}
