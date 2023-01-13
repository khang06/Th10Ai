#include "Th10Ai/Scene.h"

namespace th
{
	bool Scene::IsInScene(const vec2& pos)
	{
		return pos.x >= _F(-192.0) && pos.x <= _F(192.0)
			&& pos.y >= _F(0.0) && pos.y <= _F(448.0);
	}

	bool Scene::IsInPlayerRegion(const vec2& pos)
	{
		return pos.x >= _F(-184.0) && pos.x <= _F(184.0)
			&& pos.y >= _F(32.0) && pos.y <= _F(432.0);
	}

	vec2 Scene::ToWindowPos(const vec2& scenePos)
	{
		return BORDER_OFFSET + ORIGIN_POINT_OFFSET + scenePos;
	}

	vec2 Scene::ToScenePos(const vec2& windowPos)
	{
		return windowPos - ORIGIN_POINT_OFFSET - BORDER_OFFSET;
	}

	Scene::Scene() :
		m_region(vec2(_F(0.0), _F(224.0)), SIZE)
	{
		m_region.split(6);
	}

	void Scene::clearAll()
	{
		m_region.clearAll();
	}

	void Scene::splitEnemies(const std::vector<Enemy>& enemies, int32_t frame)
	{
		m_region.splitEnemies(enemies, frame);
	}

	void Scene::splitBullets(const std::vector<Bullet>& bullets, int32_t frame)
	{
		m_region.splitBullets(bullets, frame);
	}

	void Scene::splitLasers(const std::vector<Laser>& lasers, int32_t frame)
	{
		m_region.splitLasers(lasers, frame);
	}

	RegionCollideResult Scene::collideAll(const Player& player) const
	{
		return m_region.collideAll(player);
	}

	RegionCollideResult Scene::collideAll(const Player& player, const Bullet& target) const
	{
		return m_region.collideAll(player, target);
	}

#if RENDER
	void Scene::render(cv::Mat& mat, const Player& player)
	{
		m_region.render(mat, player);
	}
#endif
}
