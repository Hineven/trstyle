#ifndef _H_PRETREATMENT
#define _H_PRETREATMENT

#include "heads.hpp"
#include "data.hpp"
#include "stylization.hpp"
#define x first
#define y second
#define D double 
#define sqr(x)  ((x)*(x))
#define dis(a) (sqr(p[a.x].x-p[a.y].x)+sqr(p[a.x].y-p[a.y].y))

namespace Init{
    constexpr double eps =  1e-10;
    constexpr int N=20000;
    struct point
    {
        D x,y;
        point(){}
        point(D x,D y):x(x),y(y){}
        point operator+(point z){return point(x+z.x,y+z.y);}
        point operator-(point z){return point(x-z.x,y-z.y);}
        point operator*(D z){return point(x*z,y*z);}
        point operator/(D z){return point(x/z,y/z);}
        D operator*(point z){return x*z.x+y*z.y;}
        D operator^(point z){return x*z.y-y*z.x;}
        bool operator<(point z)const{return abs(x-z.x)<eps?y<z.y:x<z.x;}
    };
    int n_init;
    point p[N];
    D swp,curx;
    struct pr
    {
        int x,y;
        pr(){};
        pr(int x,int y):x(x),y(y){}
        D get_y(D swp)const
        {
            if(y==-1)return curx;
            point u=(p[y]+p[x])/2,v=p[y]-p[x];
            v=point(v.y,-v.x);
            D a=v*v-v.x*v.x,b=2*((u-p[x])*v-(u.x-swp)*v.x),c=(u-p[x])*(u-p[x])-(u.x-swp)*(u.x-swp);
            if(abs(a)<eps)return u.y-c/b*v.y;
            D delta=sqrt(std::max((D)0,b*b-4*a*c));
            return u.y+(-b+delta)/(2*a)*v.y;
        }
        bool operator<(pr y)const
        {
            D xx=get_y(swp),yy=y.get_y(swp);
            if(abs(xx-yy)<eps&&x!=-1&&y.x!=-1)return p[x].x<p[y.x].x;
            return xx+eps<yy;
        }
    };
    struct ev
    {
        D T;
        int x,y,z;
        ev(){}
        ev(D T,int x,int y,int z):T(T),x(x),y(y),z(z){}
        bool operator <(ev i)const{return T>i.T;}
    };
    std::priority_queue<ev>event;
    inline void add(int x,int y,int z)
    {
        point a=p[x],b=p[y],c=p[z];
        point v=c-b,w=a-b;
        D d=w^v;
        if(d<eps)return;
        v=point(v.y,-v.x),w=point(w.y,-w.x);
        point P=(b+c)/2,Q=(a+b)/2;
        D t=(w^(Q-P))/d;
        D aa=P.x+v.x*t,bb=P.y+v.y*t;
        t=aa+sqrt(sqr(aa-a.x)+sqr(bb-a.y));
        event.push(ev(t,x,y,z));
    }
    std::set<pr> All;
    inline bool cmp(int a,int b){return p[a]<p[b];}
    int pos[N];
    bool cmp2(std::pair<int,int> a,std::pair<int,int> b){return dis(a)<dis(b);}

    std::pair<int,int> pp[N];
    int tag[N];
    //std::vector< int > G[N];

    struct Triangle {
        short v[3], t;
    };

    Vector2 verts[N];
    Triangle trigs[N];
    int num;
    void InitializeMain(int _n_init){
        srand(time(0));
        num=0;
        verts[0] = {0, 0};
        verts[1] = {(Float)Data::image_width, 0};
        verts[2] = {(Float)Data::image_width, (Float)Data::image_height};
        verts[3] = {0, (Float)Data::image_height};
        p[0].x=0;p[0].y=0;
        p[1].x=Data::image_width;p[1].y=0;
        p[2].x=Data::image_width;p[2].y=Data::image_height;
        p[3].x=0;p[3].y=Data::image_height;
        std::cerr<<Data::image_width<<" "<<Data::image_height<<std::endl;
        n_init=_n_init;
        int xxx,yyy;
        /*n_init = 5;
        xxx=Data::image_width / 2;
        yyy=Data::image_height / 2;
        verts[4]={(Float)xxx,(Float)yyy};
    //std::cerr<<i<<" "<<n_init<<std::endl;
        p[4].x=xxx;p[4].y=yyy;*/
        for(int i=4;i<n_init;i++){//随机撒点
            xxx=(std::rand()%(Data::image_width));
            yyy=(std::rand()%(Data::image_height));
            verts[i]={(Float)xxx,(Float)yyy};
    //std::cerr<<i<<" "<<n_init<<std::endl; 
            p[i].x=xxx;
            p[i].y=yyy;
        }
        for(int i=0;i<n_init;i++)pp[i]=std::make_pair((int)p[i].x,(int)p[i].y); 
        std::sort(pp,pp+n_init);
	    int uk=unique(pp,pp+n_init)-pp;
	    for(int i=0;i<n_init;i++){
            tag[i]=lower_bound(pp,pp+uk,std::make_pair((int)p[i].x,(int)p[i].y))-pp;
        }
	    n_init=uk;
	    for(int i=0;i<n_init;i++){
            p[i].x=pp[i].x,p[i].y=pp[i].y;
            verts[i]={(Float)pp[i].x,(Float)pp[i].y};
            pos[i]=i;
            //std::cerr<<pp[i].x<<" "<<pp[i].y<<std::endl;
        }
        std::sort(pos,pos+n_init,cmp);
        int i=1;
        D lst=p[pos[0]].x;
        swp=lst+1;
        while(i<n_init&&abs(p[pos[i]].x-lst)<eps)
        {
            All.insert(pr(pos[i-1],pos[i]));
            //G[pos[i-1]].push_back(pos[i]);G[pos[i]].push_back(pos[i-1]);
            i++;
        }
        int s=i;
        //std::cerr<<i<<std::endl;
        for(;i <= n_init;i++){
		    while(!event.empty()){//处理所有的圆事件
                if((i^n_init) && event.top().T>p[pos[i]].x+eps)break;
                ev x=event.top();
                event.pop();
                swp=(x.T+lst)/2;
                std::set<pr>::iterator tmp=All.find(pr(x.y,x.z));
                std::set<pr>::iterator tmp0=tmp;
                if(tmp==All.end()||tmp==All.begin()||((--tmp0)->x^x.x))continue;
                std::set<pr>::iterator tmp1=tmp0;
                if(tmp0!=All.begin()){
                    --tmp0;
                    add(tmp0->x,x.x,x.z);
                }
                if(++(tmp0=tmp)!=All.end()){
                    add(x.x,x.z,tmp0->y);
                }
                All.erase(tmp1,tmp0);
                All.insert(pr(x.x,x.z));
                //G[x.x].push_back(x.z);G[x.z].push_back(x.x);
                trigs[num].v[0]=x.x;
                trigs[num].v[1]=x.y;
                trigs[num++].v[2]=x.z;
                //std::cerr<<num<<" "<<x.x<<" "<<x.y<<" "<<x.z<<std::endl;
            }
            if(i==n_init)break;
            lst=swp=p[pos[i]].x;
            curx=p[pos[i]].y;
            std::set<pr>::iterator tmp=All.lower_bound(pr(-1,-1));//最左边的
            if(tmp!=All.end()&& abs(tmp->get_y(swp)-curx)<eps){
                //std::cerr<<"_____1_____"<<std::endl;
                int d=tmp->x,u=tmp->y;
                std::set<pr>::iterator tmp0=tmp;
                if(++tmp0!=All.end()){
                    add(pos[i],u,tmp0->y);
                }
                if(tmp!=All.begin()){
                    --(tmp0=tmp);
                    add(tmp0->x,d,pos[i]);
                }
                //add(d,pos[i],u);
                All.erase(tmp);
                //G[d].push_back(pos[i]);G[pos[i]].push_back(d);
                //G[pos[i]].push_back(u);G[u].push_back(pos[i]);
                All.insert(pr(d,pos[i]));
                All.insert(pr(pos[i],u));
                trigs[num].v[0]=d;
                trigs[num].v[1]=pos[i];
                trigs[num++].v[2]=u;
                //std::cerr<<num<<" "<<d<<" "<<pos[i]<<" "<<u<<std::endl;
            }
            else
            {
                //std::cerr<<"_____2_____"<<std::endl;
                int Mid=tmp==All.end()?pos[s-1]:tmp->x;
                if(tmp!=All.end())add(pos[i],Mid,tmp->y);
                if(tmp!=All.begin()){
                    std::set<pr>::iterator tmp0=tmp;
                    add((--tmp0)->x,Mid,pos[i]);
                }
                //G[pos[i]].push_back(Mid);G[Mid].push_back(pos[i]);
                All.insert(pr(pos[i],Mid));
                All.insert(pr(Mid,pos[i]));
            }
        }
        std::cerr<<num<<std::endl;
        //Stylization::initializeWith(verts, n, trigs, num);
    }

}

#endif