#include "Th10Ai/Region.h"

#include "Th10Ai/Scene.h"

namespace th
{
	Region::Region(const vec2& pos, const vec2& size) :
		m_pos(pos),
		m_size(size),
		m_times(0)
	{
		updateAABB();

		m_enemies.reserve(200);
		m_bullets.reserve(BULLET_MAX_COUNT);
		m_lasers.reserve(200);
	}

	void Region::split(int_t times)
	{
		m_times = times;
		if (times <= 0)
			return;

		if (m_size.x > m_size.y)
		{
			m_first = std::make_unique<Region>(vec2(m_pos.x - m_size.x / _F(4.0), m_pos.y), vec2(m_size.x / _F(2.0), m_size.y));
			m_second = std::make_unique<Region>(vec2(m_pos.x + m_size.x / _F(4.0), m_pos.y), vec2(m_size.x / _F(2.0), m_size.y));
		}
		else
		{
			m_first = std::make_unique<Region>(vec2(m_pos.x, m_pos.y - m_size.y / _F(4.0)), vec2(m_size.x, m_size.y / _F(2.0)));
			m_second = std::make_unique<Region>(vec2(m_pos.x, m_pos.y + m_size.y / _F(4.0)), vec2(m_size.x, m_size.y / _F(2.0)));
		}

		m_first->split(times - 1);
		m_second->split(times - 1);
	}

	void Region::clearAll()
	{
		m_enemies.clear();
		m_bullets.clear();
		m_lasers.clear();

		if (m_first != nullptr)
			m_first->clearAll();
		if (m_second != nullptr)
			m_second->clearAll();
	}

	void Region::splitEnemies(const std::vector<Enemy>& enemies, int32_t frame)
	{
		// TODO: Do this without an unnecessary copy
		for (Enemy enemy : enemies)
		{
			enemy.advance(frame);
			//if (enemy.willCollideWith(*this))
			if (enemy.collide(*this))
				m_enemies.push_back(enemy);
		}
		if (m_enemies.empty())
			return;

		if (m_first != nullptr)
			m_first->splitEnemies(m_enemies, 0);
		if (m_second != nullptr)
			m_second->splitEnemies(m_enemies, 0);
	}

	void Region::splitBullets(const std::vector<Bullet>& bullets, int32_t frame)
	{
		// TODO: Do this without an unnecessary copy
		for (Bullet bullet : bullets)
		{
			bullet.advance(frame);
			//if (bullet.willCollideWith(*this))// || bullet.collide(*this))
			if (bullet.collide(*this))
				m_bullets.push_back(bullet);
		}
		if (m_bullets.empty())
			return;

		if (m_first != nullptr)
			m_first->splitBullets(m_bullets, 0);
		if (m_second != nullptr)
			m_second->splitBullets(m_bullets, 0);
	}

	void Region::splitLasers(const std::vector<Laser>& lasers, int32_t frame)
	{
		// TODO: Do this without an unnecessary copy
		for (Laser laser : lasers)
		{
			laser.advance(frame);
			//if (laser.willCollideWith(*this))
			if (laser.collide(*this))
				m_lasers.push_back(laser);
		}
		if (m_lasers.empty())
			return;

		if (m_first != nullptr)
			m_first->splitLasers(m_lasers, 0);
		if (m_second != nullptr)
			m_second->splitLasers(m_lasers, 0);
	}

	RegionCollideResult Region::collideAll(const Player& player) const
	{
		RegionCollideResult result = {};

		if (!player.collide(*this))
			return result;

		// 只检测叶子节点
		if (m_first == nullptr && m_second == nullptr)
		{
			for (const Enemy& enemy : m_enemies)
			{
				if (enemy.collide(player))
				{
					result.collided = true;
					break;
				}
			}

			if (!result.collided)
			{
				for (const Bullet& bullet : m_bullets)
				{
					if (bullet.collide(player))
					{
						result.collided = true;
						break;
					}
				}
			}

			if (!result.collided)
			{
				for (const Laser& laser : m_lasers)
				{
					if (laser.collide(player))
					{
						result.collided = true;
						break;
					}
				}
			}
		}

		if (m_first != nullptr)
		{
			RegionCollideResult firstResult = m_first->collideAll(player);
			if (firstResult.collided)
				result.collided = true;
		}

		if (m_second != nullptr)
		{
			RegionCollideResult secondResult = m_second->collideAll(player);
			if (secondResult.collided)
				result.collided = true;
		}

		return result;
	}

	RegionCollideResult Region::collideAll(const Player& player, const Bullet& target) const
	{
		RegionCollideResult result = {};

		std::cout << m_times << " collideAll size: " << m_bullets.size() << std::endl;
		if (target.willCollideWith(*this))
		{
			std::cout << m_times << " willCollideWith" << std::endl;
		}
		for (const Bullet& bullet : m_bullets)
		{
			if (bullet.id == target.id)
			{
				std::cout << m_times << " have target" << std::endl;
			}
		}

		if (!player.collide(*this))
		{
			return result;
		}

		// 只检测叶子节点
		if (m_first == nullptr && m_second == nullptr)
		{
			//for (Enemy enemy : m_enemies)
			//{
			//	enemy.advance(frame);
			//	if (enemy.collide(player))
			//	{
			//		result.collided = true;
			//		break;
			//	}
			//}

			//if (!result.collided)
			//{
			//	std::cout << m_times << " collide Bullet size: " << m_bullets.size() << std::endl;
			//	for (const Bullet& bullet : m_bullets)
			//	{
			//		if (bullet.id == target.id)
			//			std::cout << m_times << " collide Bullet true" << std::endl;
			//		else
			//			std::cout << m_times << " collide Bullet false" << std::endl;
			//	}
			//}

			//if (!result.collided)
			//{
			//	for (Laser laser : m_lasers)
			//	{
			//		laser.advance(frame);
			//		if (laser.collide(player))
			//		{
			//			result.collided = true;
			//			break;
			//		}
			//	}
			//}
		}

		if (m_first != nullptr)
		{
			RegionCollideResult firstResult = m_first->collideAll(player, target);
			if (firstResult.collided)
				result.collided = true;
		}

		if (m_second != nullptr)
		{
			RegionCollideResult secondResult = m_second->collideAll(player, target);
			if (secondResult.collided)
				result.collided = true;
		}

		return result;
	}

#if RENDER
	void Region::render(cv::Mat& mat, const Player& player)
	{
		if (m_first == nullptr && m_second == nullptr)
		{
			cv::Scalar black(0, 0, 0);
			cv::Scalar white(255, 255, 255);
			cv::Scalar red(0, 0, 255);
			cv::Scalar green(0, 255, 0);
			cv::Scalar blue(255, 0, 0);
			cv::Scalar yellow(0, 255, 255);

			{
				vec2 windowPos = Scene::ToWindowPos(getTopLeft());
				cv::Rect rect(int_t(windowPos.x), int_t(windowPos.y), int_t(m_size.x), int_t(m_size.y));
				cv::rectangle(mat, rect, yellow);
			}

			if (collide(player))
			{
				for (const Bullet& bullet : m_bullets)
				{
					/*
					vec2 footPoint = bullet.getFootPoint(player);
					vec2 wp1 = Scene::ToWindowPos(player.pos);
					vec2 wp2 = Scene::ToWindowPos(bullet.pos);
					vec2 wp3 = Scene::ToWindowPos(footPoint);
					cv::line(mat, cv::Point(int_t(wp1.x), int_t(wp1.y)), cv::Point(int_t(wp3.x), int_t(wp3.y)), green);
					cv::line(mat, cv::Point(int_t(wp2.x), int_t(wp2.y)), cv::Point(int_t(wp3.x), int_t(wp3.y)), green);
					*/
					vec2 p1 = Scene::ToWindowPos(bullet.topLeft);
					vec2 p2 = Scene::ToWindowPos(bullet.topRight);
					vec2 p3 = Scene::ToWindowPos(bullet.bottomLeft);
					vec2 p4 = Scene::ToWindowPos(bullet.bottomRight);
					cv::line(mat, cv::Point(int_t(p1.x), int_t(p1.y)), cv::Point(int_t(p2.x), int_t(p2.y)), red);
					cv::line(mat, cv::Point(int_t(p2.x), int_t(p2.y)), cv::Point(int_t(p3.x), int_t(p3.y)), red);
					cv::line(mat, cv::Point(int_t(p3.x), int_t(p3.y)), cv::Point(int_t(p4.x), int_t(p4.y)), red);
					cv::line(mat, cv::Point(int_t(p4.x), int_t(p4.y)), cv::Point(int_t(p1.x), int_t(p1.y)), red);
				}

				for (const Laser& laser : m_lasers)
				{
					vec2 p1 = Scene::ToWindowPos(laser.topLeft);
					vec2 p2 = Scene::ToWindowPos(laser.topRight);
					vec2 p3 = Scene::ToWindowPos(laser.bottomLeft);
					vec2 p4 = Scene::ToWindowPos(laser.bottomRight);
					cv::line(mat, cv::Point(int_t(p1.x), int_t(p1.y)), cv::Point(int_t(p2.x), int_t(p2.y)), red);
					cv::line(mat, cv::Point(int_t(p2.x), int_t(p2.y)), cv::Point(int_t(p3.x), int_t(p3.y)), red);
					cv::line(mat, cv::Point(int_t(p3.x), int_t(p3.y)), cv::Point(int_t(p4.x), int_t(p4.y)), red);
					cv::line(mat, cv::Point(int_t(p4.x), int_t(p4.y)), cv::Point(int_t(p1.x), int_t(p1.y)), red);

					//vec2 o = Scene::ToWindowPos(vec2(0, 0));
					//vec2 axisX = o + laser.axisX * 100;
					//vec2 axisY = o + laser.axisY * 100;
					//cv::line(mat, cv::Point(int_t(o.x), int_t(o.y)), cv::Point(int_t(axisX.x), int_t(axisX.y)), green);
					//cv::line(mat, cv::Point(int_t(o.x), int_t(o.y)), cv::Point(int_t(axisY.x), int_t(axisY.y)), green);

					//break;
				}
			}
		}

		if (m_first != nullptr)
			m_first->render(mat, player);
		if (m_second != nullptr)
			m_second->render(mat, player);
	}
#endif

	vec2 Region::getTopLeft() const
	{
		return vec2(m_pos.x - m_size.x / _F(2.0), m_pos.y - m_size.y / _F(2.0));
	}

	vec2 Region::getTopRight() const
	{
		return vec2(m_pos.x + m_size.x / _F(2.0), m_pos.y - m_size.y / _F(2.0));
	}

	vec2 Region::getBottomLeft() const
	{
		return vec2(m_pos.x - m_size.x / _F(2.0), m_pos.y + m_size.y / _F(2.0));
	}

	vec2 Region::getBottomRight() const
	{
		return vec2(m_pos.x + m_size.x / _F(2.0), m_pos.y + m_size.y / _F(2.0));
	}

	void Region::updateAABB()
	{
		topLeft = getTopLeft();
		topRight = getTopRight();
		bottomLeft = getBottomLeft();
		bottomRight = getBottomRight();
	}
}
