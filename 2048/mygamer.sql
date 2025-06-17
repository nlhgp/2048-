/*
 Navicat Premium Dump SQL

 Source Server         : 123456
 Source Server Type    : MySQL
 Source Server Version : 80039 (8.0.39)
 Source Host           : localhost:3306
 Source Schema         : mygamer

 Target Server Type    : MySQL
 Target Server Version : 80039 (8.0.39)
 File Encoding         : 65001

 Date: 09/06/2025 19:37:53
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for achievement_type
-- ----------------------------
DROP TABLE IF EXISTS `achievement_type`;
CREATE TABLE `achievement_type`  (
  `achievement_id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '成就名称',
  `description` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '成就描述',
  `condition_type` enum('score','wins','login','games') CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '条件类型',
  `condition_value` int NOT NULL COMMENT '达成条件值',
  `file_path` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '文件路径',
  PRIMARY KEY (`achievement_id`) USING BTREE,
  UNIQUE INDEX `name`(`name` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci COMMENT = '成就定义表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of achievement_type
-- ----------------------------
INSERT INTO `achievement_type` VALUES (1, '初出茅庐', '完成第一局游戏', 'games', 1, ':/achievement/first_game_lock.png');
INSERT INTO `achievement_type` VALUES (2, '2048大师', '达成2048方块', 'score', 2048, ':/achievement/2048_lock.png');

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user`  (
  `user_id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '用户名',
  `password_hash` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '加密密码',
  `email` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '邮箱',
  `total_score` int NOT NULL DEFAULT 0 COMMENT '累计得分',
  `highest_score` int NOT NULL DEFAULT 0 COMMENT '历史最高分',
  `created_at` datetime NULL DEFAULT CURRENT_TIMESTAMP COMMENT '注册时间',
  `run_time` int NULL DEFAULT 0,
  PRIMARY KEY (`user_id`) USING BTREE,
  UNIQUE INDEX `username`(`username` ASC) USING BTREE,
  UNIQUE INDEX `email`(`email` ASC) USING BTREE,
  INDEX `idx_highest_score`(`highest_score` DESC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci COMMENT = '用户信息表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES (1, 'nlhgp', '123456', NULL, 0, 0, '2025-04-18 14:42:24', 0);
INSERT INTO `user` VALUES (3, '2', '2', NULL, 2048, 2048, '2025-04-18 15:12:01', 17);

-- ----------------------------
-- Table structure for user_achievement
-- ----------------------------
DROP TABLE IF EXISTS `user_achievement`;
CREATE TABLE `user_achievement`  (
  `user_id` int NOT NULL,
  `achievement_id` int NOT NULL,
  `achieved_at` datetime NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`user_id`, `achievement_id`) USING BTREE,
  INDEX `achievement_id`(`achievement_id` ASC) USING BTREE,
  CONSTRAINT `user_achievement_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `user_achievement_ibfk_2` FOREIGN KEY (`achievement_id`) REFERENCES `achievement_type` (`achievement_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci COMMENT = '用户成就记录表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of user_achievement
-- ----------------------------

-- ----------------------------
-- Procedure structure for GetUserAchievements
-- ----------------------------
DROP PROCEDURE IF EXISTS `GetUserAchievements`;
delimiter ;;
CREATE PROCEDURE `GetUserAchievements`(IN input_username VARCHAR(50))
BEGIN
    DECLARE target_user_id INT;

    -- 步骤1：验证用户存在性并获取用户ID
    SELECT user_id INTO target_user_id
    FROM user
    WHERE username = input_username;

    -- 用户不存在处理
    IF target_user_id IS NULL THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = '用户不存在';
    ELSE
        -- 步骤2：关联查询成就信息
        SELECT 
            a.name AS '成就名称',
            a.description AS '成就描述',
            a.file_path AS '文件路径',
            CASE 
                WHEN ua.achievement_id IS NOT NULL THEN '已完成'
                ELSE '未完成'
            END AS '完成状态'
        FROM achievement_type a
        LEFT JOIN user_achievement ua 
            ON a.achievement_id = ua.achievement_id
            AND ua.user_id = target_user_id
        ORDER BY a.achievement_id;
    END IF;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for UpdateUserScoreSafe
-- ----------------------------
DROP PROCEDURE IF EXISTS `UpdateUserScoreSafe`;
delimiter ;;
CREATE PROCEDURE `UpdateUserScoreSafe`(IN p_username VARCHAR(50),
    IN p_new_score INT)
BEGIN
    DECLARE current_highest INT;
    
    START TRANSACTION;
    
    -- 获取并锁定记录
    SELECT highest_score INTO current_highest
    FROM user
    WHERE username = p_username
    FOR UPDATE;
		
		-- 游戏次数+1
    UPDATE user 
        SET run_time = run_time + 1
        WHERE username = p_username;

    -- 条件更新
    IF p_new_score > current_highest THEN
        UPDATE user 
        SET total_score = p_new_score,
            highest_score = p_new_score
        WHERE username = p_username;
    ELSE
        UPDATE user
        SET total_score = p_new_score
        WHERE username = p_username;
    END IF;
    COMMIT;
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
