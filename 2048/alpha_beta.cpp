#include "alpha_beta.h"
#include<QDebug>

alpha_beta::alpha_beta(map &m,QObject *parent)
    : QObject{parent}
{
    m_map = m.copy();
    // int i,j;
    // fi(4)fj(4) qDebug() << m_map->num(i,j);
}



//获取最大值位置
point alpha_beta::get_max_post()
{
    int i,j;
    point p; p.x = 0; p.y = 0;
    fi(4) fj(4) if(m_map->num(i,j) > m_map->num(p.x,p.y))
    {
        p.x = i;
        p.y = j;
    }
    return p;
}

//获取0的个数
int alpha_beta::get_zero_num()
{
    int i,j,count = 0;
    fi(4) fj(4) if(m_map->num(i,j) == 0) ++count;
    return count;
}

//获取横向单调性评分
float alpha_beta::calculateRowMonotonicity() {
    float rowScore = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            int current = m_map->num(y,x);
            int next = m_map->num(y,x+1);
            float score_current = current > 0 ? log2(current) : 0.0;
            float score_next = next > 0 ? log2(next) : 0.0;
            // 计算相邻方块的差值
            if (current > next) {
                rowScore += score_current - score_next;
            } else {
                rowScore += score_next - score_current;
            }
        }
    }
    return -rowScore; // 越小越好（取负值）
}

//获取纵向单调性评分
float alpha_beta::calculateColMonotonicity() {
    float colScore = 0;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 3; y++) {
            int current = m_map->num(y,x);
            int next = m_map->num(y+1,x);
            float score_current = current > 0 ? log2(current) : 0.0;
            float score_next = next > 0 ? log2(next) : 0.0;
            // 计算相邻方块的差值
            if (current > next) {
                colScore += score_current - score_next;
            } else {
                colScore += score_next - score_current;
            }
        }
    }
    return -colScore; // 越小越好（取负值）
}

//获取单调性评分
float alpha_beta::calculateMonotonicity() {
    float score = calculateRowMonotonicity() + calculateColMonotonicity();
    return score;
}

//获取平滑度评分
float alpha_beta::calculateSmoothness() {
    float smoothness = 0;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int current = m_map->num(y,x);
            if (current == 0) continue;

            // 检查右侧相邻块
            if (x < 3) {
                int right = m_map->num(y,x+1);
                if (right != 0) {
                    smoothness += abs(log2(current) - log2(right));
                }
            }

            // 检查下方相邻块
            if (y < 3) {
                int down = m_map->num(y+1,x);
                if (down != 0) {
                    smoothness += abs(log2(current) - log2(down));
                }
            }
        }
    }
    return -smoothness; // 返回负值使得差异越小得分越高
}

//检测位置可行性
std::vector<Direction> alpha_beta::generatePossibleMoves() {
    std::vector<Direction> moves;
    // 检查每个方向是否可行
    for (int dir = UP; dir <= RIGHT; dir++) {
        map *temp = this->m_map->copy();
        temp->move(static_cast<Direction>(dir));
        if (!(*temp == *(this->m_map))) {
            moves.push_back(static_cast<Direction>(dir));
        }
    }
    return moves;
}

//当前棋盘分数
float alpha_beta::current_score()
{
    float score = 0;
    int max = get_max();//最高分数值
    score += max==0 ? log2(max) * 0.1 : 0;

    return score;
}

//总得分
float alpha_beta::evaluate() {
    float score = 0;

    // 1. 当前分数
    score += current_score() * SCORE_WEIGHT;

    // 2. 空格数量
    score += get_zero_num() * EMPTY_CELL_WEIGHT;

    // 3. 单调性（相邻方块值是否有序）
    score += calculateMonotonicity() * MONOTONICITY_WEIGHT;

    // 4. 平滑性（相邻方块值是否接近）
    score += calculateSmoothness() * SMOOTHNESS_WEIGHT;

    // 5. 最大方块位置（是否在角落）
    score += calculateCornerMaxTile() * CORNER_WEIGHT;

    return score;
}

// 阿尔法贝塔剪枝算法
// 递归搜索函数
SearchResult alpha_beta::alphaBetaSearch(int depth, float alpha, float beta, bool isPlayerTurn)
{
    if (depth == 0 || m_map->falsed() || m_map->finished()) {
        return {NONE, evaluate()};
    }
    if (isPlayerTurn) {
        // 玩家回合（选择最佳移动）
        SearchResult best = {NONE, -INFINITY};
        auto moves = generatePossibleMoves();

        for (Direction move : moves) {
            map *child = this->m_map->copy();
            child->move(move);

            auto result = alphaBetaSearch(depth-1, alpha, beta, false);
            if (result.value > best.value) {
                best.value = result.value;
                best.bestMove = move;
            }

            alpha = max(alpha, best.value);
            if (beta <= alpha) break;  // 剪枝
        }
        return best;
    } else {
        // "对手"回合（随机添加方块）
        SearchResult worst = {NONE, INFINITY};
        auto emptyCells = m_map->get_zeros();

        // 考虑所有可能的随机方块添加情况
        for (auto& cell : emptyCells) {
            // 尝试添加2
            map *child2 = m_map->copy();
            child2->addTile(cell.x, cell.y, 2);
            auto result2 = alphaBetaSearch(depth-1, alpha, beta, true);
            worst.value = min(worst.value, result2.value);

            // 尝试添加4
            map *child4 = m_map->copy();
            child4->addTile(cell.x, cell.y, 4);
            auto result4 = alphaBetaSearch(depth-1, alpha, beta, true);
            worst.value = min(worst.value, result4.value);

            beta = min(beta, worst.value);
            if (beta <= alpha) break;  // 剪枝
        }
        return worst;
    }
}

// 获取最佳移动方向
Direction alpha_beta::getBestMove()
{
    // 使用迭代加深
    Direction bestMove = NONE;
    for (int depth = 2; depth <= MAX_DEPTH; depth += 2) {
        auto result = alphaBetaSearch(depth, -INFINITY, INFINITY, true);
        bestMove = result.bestMove;
        if (result.value > WIN_THRESHOLD) break;  // 足够好的移动
    }
    return bestMove;
}

//角落
float alpha_beta::cornerBonus() {
    const point corners[4] = {{0,0}, {0,3}, {3,0}, {3,3}};
    point maxPos = get_max_post();

    // 检查是否在任意角落
    for (auto& corner : corners) {
        if (maxPos == corner) {
            return CORNER_BONUS; // 典型值: 50-100
        }
    }
    return 0;
}

//边缘
float alpha_beta::edgePenalty() {
    point maxPos = get_max_post();
    int edgeDistance = 0;

    // 计算到最近边缘的距离
    edgeDistance += (maxPos.x == 0 || maxPos.x == 3) ? 0 : 1;
    edgeDistance += (maxPos.y == 0 || maxPos.y == 3) ? 0 : 1;

    return edgeDistance * EDGE_PENALTY; // 典型值: -20/单位
}

float alpha_beta::neighborScore() {
    point maxPos = get_max_post();
    int maxValue = m_map->num(maxPos.y,maxPos.x);
    float score = 0;

    // 检查四个方向的相邻块
    const int dirs[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    for (auto& dir : dirs) {
        int nx = maxPos.x + dir[0];
        int ny = maxPos.y + dir[1];
        if (nx >= 0 && nx < 4 && ny >= 0 && ny < 4) {
            int neighborValue = m_map->num(ny,nx);
            if (neighborValue == maxValue) {
                score += SAME_TILE_BONUS; // 可合并奖励
            } else if (neighborValue >= maxValue/2) {
                score += HALF_VALUE_BONUS; // 潜在合并奖励
            }
        }
    }
    return score;
}

//动态调整权重
float alpha_beta::getMaxTileWeight() {
    int maxTile = get_max();
    // 随着方块增大，位置权重增加
    if (maxTile >= 1024) return 2.0f;
    if (maxTile >= 256) return 1.5f;
    return 1.0f;
}

//防御性评分
float alpha_beta::defenseScore() {
    point maxPos = get_max_post();
    int defenseLayers = 0;

    // 检查外围保护层
    for (int d = 1; d <= 2; d++) {
        if (maxPos.x - d >= 0 &&
            m_map->num(maxPos.y,maxPos.x - d) > 0) defenseLayers++;
        if (maxPos.x + d < 4 &&
            m_map->num(maxPos.y,maxPos.x + d) > 0) defenseLayers++;
    }

    return defenseLayers * DEFENSE_WEIGHT;
}

//最大方块位置评分
float alpha_beta::calculateCornerMaxTile() {
    float score = 0;
    // 基础位置评分
    score += cornerBonus();
    score += edgePenalty();
    // 相邻块协同性
    score += neighborScore();
    // 动态调整
    score *= getMaxTileWeight();
    // 防御性评分
    score += defenseScore();
    return score;
}
