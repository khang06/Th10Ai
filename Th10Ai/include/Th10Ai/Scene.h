#pragma once

#include "Th10Ai/Common.h"

#if RENDER
#include <opencv2/opencv.hpp>
#endif

#include "Th10Base/SharedMemory.h"
#include "Th10Base/SharedData.h"
#include "Th10Base/Player.h"
#include "Th10Base/Item.h"
#include "Th10Base/Enemy.h"
#include "Th10Base/Bullet.h"
#include "Th10Base/Laser.h"
#include "Th10Ai/Region.h"

namespace th
{
	// 场景坐标
	//     +-----------------------+-----------------------+
	//     |      |16                               |      |
	//     |------+----------------+----------------+------|------> X
	//     |  32  |(-192,0)        |(0,0)    (192,0)|      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |448   |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |                |                |      |
	//     |      |(-192,448)      |(0,448)(192,448)|      |
	//     |------+----------------+----------------+------|
	//     |      |               384               |      |
	//     +-----------------------+-----------------------+
	//                             |
	//                            \|/
	//                             Y
	class Scene
	{
	public:
		static constexpr vec2 SIZE = vec2(_F(384.0), _F(448.0));
		static constexpr vec2 BORDER_OFFSET = vec2(_F(32.0), _F(16.0));
		static constexpr vec2 ORIGIN_POINT_OFFSET = vec2(_F(192.0), _F(0.0));

		static bool IsInScene(const vec2& pos);
		static bool IsInPlayerRegion(const vec2& pos);
		static vec2 ToWindowPos(const vec2& scenePos);
		static vec2 ToScenePos(const vec2& windowPos);

		Scene();

		void clearAll();
		void splitEnemies(const std::vector<Enemy>& enemies, int32_t frame);
		void splitBullets(const std::vector<Bullet>& bullets, int32_t frame);
		void splitLasers(const std::vector<Laser>& lasers, int32_t frame);
		RegionCollideResult collideAll(const Player& player) const;
		RegionCollideResult collideAll(const Player& player, const Bullet& target) const;

#if RENDER
		void render(cv::Mat& mat, const Player& player);
#endif

	private:
		Region m_region;
	};
}
