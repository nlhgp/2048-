#include "map.h"
#include<ctime>
#include<stdlib.h>

map::map()
{
    int i,j;
    srand((unsigned int)time(NULL));
    fi(4)
    {
        fj(4)
        {
            m[i][j] = 0;
            moved[i][j] = false;
        }
    }
}

//拷贝
map* map::copy()
{
    map *mp = new map;
    int i,j;
    fi(4)fj(4) mp->set_num(i,j,this->num(i,j));
    mp->score = this->get_score();
    return mp;
}

bool map::left()
{
    map *map = this->copy();
    int i,j,k,l;
    fi(4)
    {
        fj(4)//遍历到第i行第j个
        {
            l=j;
            for(k = j-1;k>=0 && m[i][k] == 0;--k);
            if(k != j-1)
            {
                m[i][k+1] = m[i][j];
                m[i][j] = 0;
                l = k + 1;
            }
            //从j开始往前找一样的
            while(k >= 0)
            {
                if(m[i][l]==m[i][k] && !(m[i][l]%2))//相等
                {
                    if(m[i][l] == (0x7fffffff/2) + 1)//通关了
                    {
                        this->finish = true;
                        return false;
                    }
                    else if(!moved[i][l])
                    {
                        m[i][k] += m[i][l];
                        m[i][l] = 0;
                        moved[i][k]=true;
                        l = k;
                    }
                }
                else if(m[i][k] == 0 || m[i][k] % 2)//不相等但前边为0
                {
                    m[i][k] = m[i][l];
                    m[i][l] = 0;
                    l = k;
                }
                else break;//不相等前边也不是0
                --k;
            }
        }
    }
    fi(4) fj(4) moved[i][j]=false;
    if(*map == (*this)) return false;
    return true;
}

bool map::right()
{
    map *map = this->copy();
    int i,j,k,l;
    fi(4)
    {
        for(j=3;j>=0;--j)//遍历到第i行第j个
        {
            l=j;
            for(k=j+1 ; k<4 && m[i][k] == 0 ; ++k);
            if(k!=j+1)
            {
                m[i][k-1] = m[i][j];
                m[i][j] = 0;
                l = k-1;
            }
            //从j开始往前找一样的
            while(k<4)
            {
                if(m[i][l] == m[i][k] && !(m[i][l]%2))//相等
                {
                    if(m[i][l] == 0x7fffffff/2 + 1)//通关了
                    {
                        this->finish = true;
                        return false;
                    }
                    else if(!moved[i][l])//相等但是不为满值
                    {
                        m[i][k] += m[i][l];
                        m[i][l] = 0;
                        moved[i][k]=true;
                        l = k;
                    }
                }
                else if(m[i][k] == 0 || m[i][k] % 2)//不相等且为0或者有道具
                {
                    m[i][k] = m[i][l];
                    m[i][l] = 0;
                    l = k;
                }
                else break;
                ++k;
            }
        }
    }
    fi(4) fj(4) moved[i][j]=false;
    if(*map == (*this)) return false;
    return true;
}

bool map::up()
{
    map *map = this->copy();
    int i,j,k,l;
    fj(4)
    {
        fi(4)//遍历列
        {
            l = i;
            for(k=i-1 ; k >= 0 && m[k][j] == 0 ; --k);
            if(k!=i-1)
            {
                m[k+1][j] = m[i][j];
                m[i][j] = 0;
                l = k + 1;
            }
            while(k >= 0)
            {
                if(m[l][j]==m[k][j]  && !(m[l][j]%2))
                {
                    if(m[k][j]==0x7fffffff/2 + 1)//通关了
                    {
                        this->finish = true;
                        return false;
                    }
                    else if(!moved[l][j])
                    {
                        m[k][j] += m[l][j];
                        m[l][j] = 0;
                        moved[l][j]=true;
                        l = k;
                    }
                }
                else if(m[k][j] == 0 || m[i][k] % 2)
                {
                    m[k][j] = m[l][j];
                    m[k][j] = 0;
                    l = k;
                }
                else break;
                --k;
            }
        }
        fi(4)moved[i][j]=false;
    }
    if(*map == (*this)) return false;
    return true;
}

bool map::down()
{
    map *map = this->copy();
    int i,j,k,l;
    fj(4)
    {
        for(i=3;i>=0;--i)//遍历到第i行第j个
        {
            l=i;
            for(k=i+1;k < 4 && m[k][j] == 0;++k);
            if(k!=i+1)
            {
                m[k-1][j] = m[i][j];
                m[i][j] = 0;
                l = k - 1;
            }
            while(k < 4)
            {
                if(m[l][j]==m[k][j] && !(m[l][j]%2))
                {
                    if(m[k][j]==0x7fffffff/2 + 1)//通关了
                    {
                        this->finish = true;
                        return false;
                    }
                    else if(!moved[l][j])//相对且不为满值
                    {
                        m[k][j] += m[l][j];
                        moved[l][j]=true;
                        m[l][j] = 0;
                        l = k;
                    }
                }
                else if(m[k][j] == 0 || m[i][k] % 2)//不相等前值为0
                {
                    m[k][j] = m[l][j];
                    m[l][j] = 0;
                    l = k;
                }
                else break;
                ++k;
            }
        }
        fi(4) moved[i][j]=false;
    }
    if(*map == (*this)) return false;
    return true;
}

int map::num(int i,int j)
{
    return m[i][j];
}

//新方块
void map::new_two()
{
    std::vector<point> v;
    point p;
    int i,j;
    fi(4)fj(4)if(m[i][j]==0)
    {
        p.x=i;p.y=j;
        v.push_back(p);
    }
    if(v.size()!=0)
    {
        p=v[rand()%v.size()];
        if(rand()%10 < 8)
        {
            m[p.x][p.y] = 2;
            score += 2;
        }
        else
        {
            m[p.x][p.y] = 4;
            score += 4;
        }
    }
    if(this->is_over())
    {
        this->isfalse = true;
        return;
    }
}

//新道具
void map::new_prop()
{
    std::vector<point> v;
    point p;
    prop prop;
    int i,j;
    fi(4)fj(4)if(m[i][j]==0)
    {
        p.x=i;p.y=j;
        v.push_back(p);
    }
    if(v.size()!=0)
    {
        p=v[rand()%v.size()];
        prop.set_position(p);
        m[p.x][p.y] = prop.get_id();
        props.push_back(prop);
    }
}

//重载判断相等运算符
bool map::operator==(map &m)
{
    int i,j;
    fi(4)fj(4) if(this->num(i,j) != m.num(i,j)) return false;
    return true;
}

//判断是否结束
bool map::is_over()
{
    map m = *this;
    m.left();
    if(!(m==(*this))) return false;
    m.right();
    if(!(m==(*this))) return false;
    m.up();
    if(!(m==(*this))) return false;
    m.down();
    if(!(m==(*this))) return false;
    return true;
}

//输出
void map::print()
{
    int i,j;
    fi(4)
    {
        fj(4) pd(m[i][j]);
        pn;
    }
}

//设置方块值
void map::set_num(int i,int j,int num)
{
    m[i][j] = num;
}

//获取分数
long long map::get_score()
{
    return score;
    // long long score = 0;
    // int i,j;
    // fi(4) fj(4) score += m[i][j];
    // return score;
}

//获取最高分数
int map::get_max_num()
{
    int i,j,max=0;
    fi(4)fj(4) if(max < m[i][j]) max=m[i][j];
    return max;
}

//初始化
void map::init()
{
    int i,j;
    fi(4)
    {
        fj(4)
        {
            m[i][j] = 0;
            moved[i][j] = false;
        }
    }
    new_two();
    new_two();
    this->isfalse = false;
    this->finish = false;
}

//使用道具
void map::use_prop()
{
    for(auto it = props.begin(); it != props.end(); /* 不在这里递增 */)
    {
        point g = it->get_position();
        if(m[g.x][g.y] != 0)
        {
            switch(it->get_id())
            {
            case 1: clear_column(g); break;
            case 3: clear_nine(g); break;
            case 5: clear_line(g); break;
            default: printf("error\n");
            }
            it = props.erase(it); // erase 返回下一个有效迭代器
        }
        else
        {
            ++it; // 只有不删除时才递增
        }
    }
}
