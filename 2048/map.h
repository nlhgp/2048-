#ifndef MAP_H
#define MAP_H

#include<vector>
#include<ctime>
#define pn printf("\n");
#define pd(x) printf("%d ",x);
#define sd(x) scanf("%d",&x);
#define pdn(x) printf("%d\n",x);
#define fi(x) for(i=0;i<x;++i)
#define pc(x) printf("%c ",x);
#define pcn(x) printf("%c\n",x);
#define sc(x) scanf("%c",&x);
#define fstl(act) for(auto it=act.begin();it!=act.end();++it)
#define fj(x) for(j=0;j<x;++j)
#define fk(x) for(k=0;k<x;++k)

//坐标类表示
typedef struct point
{
    int x;
    int y;
    bool operator==(const point p)
    {
        if(this->x == p.x && this->y == p.y) return true;
        return false;
    }
}point;


//移动方向
enum Direction { UP, DOWN, LEFT, RIGHT, NONE };


//道具类
class prop
{
public:
    prop()
    {
        srand((unsigned int)time(NULL));
        this->id = (rand()%3)*2+1;
    }
    void set_id(int num) {this->id=num;}
    int get_id() {return id;}
    point get_position() {return this->position;}
    void set_position(point p) {this->position = p;}
private:
    int id;//1消列 3炸一圈 5消行
    point position;
};


//游戏地图类
class map
{
public:
    map();
    map* copy();
    //基本操作
    bool left();
    bool right();
    bool up();
    bool down();
    //获取数组元素
    int num(int i,int j);
    void set_num(int i,int j,int num);
    //设置完成与失败
    bool finished() {return this->finish;}
    bool falsed() {return this->isfalse;}
    //获取分数
    long long get_score();
    int get_max_num();
    std::vector<point> get_zeros() {
        std::vector<point> v;
        point p;
        int i,j;
        fi(4)fj(4)if(m[i][j]==0)
        {
            p.x=i;p.y=j;
            v.push_back(p);
        }
        return v;
    }

    //新建方块
    void new_two();

    //新建小道具
    void new_prop();
    //进入道具模式
    void set_prop_mode() {this->prop_mode = true;}
    //是否为道具模式
    bool is_prop_mode() {return this->prop_mode;}
    //退出道具模式
    void exit_prop_mode() {this->prop_mode = false;}
    /*
     * 道具操作
     */
    void use_prop();
    void clear_line(point &p) {int n = p.x,i; fi(n) m[n][i] = 0;}
    void clear_column(point &p) {int n = p.y,i; fi(n) m[i][n] = 0;}
    void clear_nine(point &p)
    {
        int i,j;
        fi(3)fj(3)
        {
            if(p.x + i-1 >= 0 && p.y + j-1 >= 0)
            m[p.x + i-1][p.y + j-1] = 0;
        }
    }

    //初始化
    void init();

    //运算符重载，相当于重写equals方法
    bool operator==(map &m);

    //是否结束
    bool is_over();
    void print();
    void set_finish() {this->finish = true;}
    bool move(Direction dir) {//执行移动
        switch (dir) {
        case UP: return up();
        case DOWN: return down();
        case LEFT: return left();
        case RIGHT: return right();
        default: return false;
        }
    }
    void addTile(int x, int y, int value) { // 添加新方块
        m[x][y] = value;
    }
private:
    int m[4][4];
    long long score = 0;
    bool moved[4][4];
    bool finish = false;//是否通关
    bool prop_mode = false;//是否为道具模式
    bool isfalse = false;//是否嗝屁了
    std::vector<prop> props;
};

#endif // MAP_H

