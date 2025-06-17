#ifndef ALPHA_BETA_H
#define ALPHA_BETA_H

#include <QObject>
#include "map.h"
// ================= 基础评估权重 =================
#define SCORE_WEIGHT        0.8f    // 原始分数权重（建议0.7-1.0）
#define EMPTY_CELL_WEIGHT   2.7f    // 空格权重（关键参数，建议2.5-3.0）

// ================= 棋盘结构权重 =================
#define MONOTONICITY_WEIGHT 1.2f    // 单调性权重（与平滑性保持比例）
#define SMOOTHNESS_WEIGHT   0.9f    // 平滑性权重（推荐单调性的0.7-0.8倍）

// ================= 最大块专项权重 ===============
#define CORNER_WEIGHT       0.3f    // 角落位置基础权重
#define CORNER_BONUS        50.0f   // 位于角落的绝对奖励
#define EDGE_PENALTY        -20.0f  // 位于边缘的惩罚（每格）

// ================= 合并潜力权重 =================
#define SAME_TILE_BONUS     3.0f    // 相同相邻块奖励（可立即合并）
#define HALF_VALUE_BONUS    1.2f    // 半值相邻块奖励（潜在合并）

// ================= 防御策略权重 =================
#define DEFENSE_WEIGHT      1.0f    // 保护层权重（每层）
#define ISOLATION_PENALTY   -5.0f   // 大块孤立惩罚（新增）

// ================= 搜索控制参数 =================
#define MAX_DEPTH           6       // 最大搜索深度（性能敏感）
#define WIN_THRESHOLD       4096    // 胜利判定阈值
#define BRANCH_LIMIT        8       // 每层最大分支数（防爆栈）

// ================= 动态调整系数 =================
#define EARLY_GAME_FACTOR   1.3f    // 前期空格权重乘数
#define LATE_GAME_FACTOR    1.5f    // 后期单调性权重乘数

#define max(a,b) (a>b)? a:b
#define min(a,b) (a<b)? a:b

//分数权重
struct SearchResult {
    Direction bestMove;
    float value;
};

//基于阿尔法贝塔剪枝算法的2048自动运算系统
class alpha_beta : public QObject
{
    Q_OBJECT
public:
    alpha_beta(map &m,QObject *parent = nullptr);
    void set_map(map *m) {free(this->m_map);this->m_map = m->copy();}//设置地图
    point get_max_post();//获取最大值位置
    int get_zero_num();//获取0的个数
    float calculateRowMonotonicity();//获取横向单调性评分
    float calculateColMonotonicity();//获取纵向单调性评分
    float calculateMonotonicity();//获取单调性评分
    float calculateSmoothness();//获取平滑度评分
    std::vector<Direction> generatePossibleMoves();//生成可能的移动方向
    float evaluate();//评估函数
    /*
    * alpha_beta剪枝算法
    * 核心代码部分
    */
    SearchResult alphaBetaSearch(int depth, float alpha, float beta, bool isPlayerTurn);
    Direction getBestMove();// 获取最佳移动方向
    //当前分数
    float current_score();
    //角落位置奖励
    float cornerBonus();
    //边缘位置评分
    float edgePenalty();
    //相邻方块协调性
    float neighborScore();
    //动态调整权重
    float getMaxTileWeight();
    //获取最大值
    int get_max(){int max=0,i,j;fi(4)fj(4)if(m_map->num(i,j)>max)max=m_map->num(i,j);return max;}
    //防御性评分
    float defenseScore();
    //最大方块位置，综合评分
    float calculateCornerMaxTile();

private:
    map *m_map = NULL;

signals:
};


#endif // ALPHA_BETA_H
