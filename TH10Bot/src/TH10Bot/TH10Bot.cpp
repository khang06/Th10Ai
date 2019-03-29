#include "TH10Bot/Common.h"
#include "TH10Bot/TH10Bot.h"

#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#define PLAY 1

namespace th
{
	TH10Bot::TH10Bot() :
		m_process(Process::FindByName("th10.exe")),
		m_window(Window::FindByClassName("BASE")),
		m_sync(),
		m_graphCap(m_process, GHT_D3D9FRAMESYNC),
		m_reader(m_process),
		m_active(false),
		m_keyUp(VK_UP), m_keyDown(VK_DOWN), m_keyLeft(VK_LEFT), m_keyRight(VK_RIGHT),
		m_keyShift(VK_SHIFT), m_keyZ('Z'), m_keyX('X'),
		m_itemId(-1),
		m_enemyId(-1),
		m_bombCooldown(0),
		m_talkCooldown(0),
		m_shootCooldown(0),
		m_collectCooldown(0)
	{
		m_items.reserve(2000);
		m_enemies.reserve(2000);
		m_bullets.reserve(2000);
		m_lasers.reserve(2000);

		m_focusBullets.reserve(500);
		m_focusLasers.reserve(300);

		m_path.reserve(100);

		srand((unsigned int)time(nullptr));
	}

	TH10Bot::~TH10Bot()
	{
	}

	void TH10Bot::start()
	{
		if (!m_active)
		{
			m_active = true;
			std::cout << "开启Bot。" << std::endl;
		}
	}

	void TH10Bot::stop()
	{
		if (m_active)
		{
			m_active = false;
			m_keyUp.release(), m_keyDown.release(), m_keyLeft.release(), m_keyRight.release(),
				m_keyShift.release(), m_keyZ.release(), m_keyX.release();
			std::cout << "停止Bot。" << std::endl;
		}
	}

	void TH10Bot::update()
	{
		if (!m_active)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(17));
			return;
		}
#if PLAY
		if (!m_sync.waitForPresent())
		{
			std::cout << "等待帧同步超时。" << std::endl;
			return;
		}
#else
		Rect rect = m_window.getClientRect();
		if (!m_capturer.capture(m_image, rect))
		{
			std::cout << "抓图失败：桌面没有变化导致超时，或者窗口位置超出桌面范围。" << std::endl;
			return;
		}
#endif
		m_clock.update();

		std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

		m_reader.getPlayer(m_player);
		m_reader.getItems(m_items);
		m_reader.getEnemies(m_enemies);
		m_reader.getBullets(m_bullets);
		m_reader.getLasers(m_lasers);

		//std::cout << m_player.x << " " << m_player.y << std::endl;
		//m_keyShift.press();
		//static bool b = true;
		//if (b)
		//{
		//	m_keyRight.press();
		//	m_keyUp.press();
		//	b = false;
		//}
		//else
		//{
		//	m_keyRight.release();
		//	m_keyUp.release();
		//	b = true;
		//}
		//return;

		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
		time_t e1 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
		//std::cout << "e1: " << e1 << std::endl;

		// 裁剪弹幕
		m_focusBullets.clear();
		for (uint_t i = 0; i < m_bullets.size(); ++i)
		{
			const Bullet& bullet = m_bullets[i];

			float_t distance = bullet.getDistance(m_player);
			FootPoint footPoint = bullet.getFootPoint(m_player);
			if (distance < 100.0f	// 在附近的
				|| (m_player.getDistance(Pointf(footPoint.x, footPoint.y)) < 100.0f
					&& footPoint.frame >= 0.0f && footPoint.frame <= 180.0f))	// 3秒内可能碰撞的
			{
				BulletView view;
				view.index = i;
				view.dir = bullet.getDirection();
				view.footPoint = footPoint;
				m_focusBullets.push_back(view);
			}
		}
		//m_focusLasers.clear();
		//for (uint_t i = 0; i < m_lasers.size(); ++i)
		//{
		//	const Laser& laser = m_lasers[i];

		//	LaserLv1 lv1;
		//	lv1.index = i;
		//	lv1.distance = laser.distance(m_player);
		//	lv1.footFrame = laser.footFrame(m_player.getPos());
		//	lv1.footPoint = laser.footPoint(lv1.footFrame);
		//	lv1.angle = laser.angle(m_player);
		//	lv1.dir = laser.direction();
		//	if (lv1.distance < 100.0f	// 在附近的
		//		|| (m_player.distance(lv1.footPoint) < 100.0f && lv1.angle > 0.0f && lv1.angle < 90.0f))	// 将来可能碰撞的
		//	{
		//		m_focusLasers.push_back(lv1);
		//	}
		//}

		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		time_t e2 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		//std::cout << "e2: " << e2 << std::endl;
#if PLAY
		handleBomb();
		if (!handleTalk())
			handleShoot();
		handleMove();

		std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
		time_t e3 = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t0).count();
		//if (e3 > 10)
		//	std::cout << "e3: " << e3 << std::endl;
#else
		cv::Scalar red(0, 0, 255);
		cv::Scalar green(0, 255, 0);
		cv::Scalar blue(255, 0, 0);
		cv::Scalar orange(0, 97, 255);
		cv::Scalar yellow(0, 255, 255);

		Pointi windowPos1 = Scene::ToWindowPos(m_player.getTopLeft());
		cv::Rect rect1(windowPos1.x, windowPos1.y, int_t(m_player.width), int_t(m_player.height));
		cv::rectangle(m_image.m_data, rect1, green, -1);
		Pointi windowPos11 = Scene::ToWindowPos(m_player.getPosition());
		cv::Point center11(windowPos11.x, windowPos11.y);
		cv::circle(m_image.m_data, center11, int_t(100.0f), green);

		//for (const Item& item : m_items)
		//{
		//	Pointi windowPos = Scene::ToWindowPos(item.getLeftTop());
		//	cv::Rect rect(windowPos.x, windowPos.y, int_t(item.width), int_t(item.height));
		//	cv::rectangle(m_image.m_data, rect, blue, -1);
		//}

		//for (const Enemy& enemy : m_enemies)
		//{
		//	Pointi windowPos = Scene::ToWindowPos(enemy.getTopLeft());
		//	cv::Rect rect(windowPos.x, windowPos.y, int_t(enemy.width), int_t(enemy.height));
		//	cv::rectangle(m_image.m_data, rect, red);
		//}
		//for (const BulletLv1& it : m_nearbyList)
		//{
		//	const Bullet& bullet = m_bullets[it.index];

		//	Pointi windowPos = Scene::ToWindowPos(bullet.getTopLeft());
		//	cv::Rect rect(windowPos.x, windowPos.y, int_t(bullet.width), int_t(bullet.height));
		//	cv::rectangle(m_image.m_data, rect, red, -1);
		//}
		for (const BulletView& view : m_focusBullets)
		{
			const Bullet& bullet = m_bullets[view.index];

			Pointi windowPos = Scene::ToWindowPos(bullet.getTopLeft());
			cv::Rect rect(windowPos.x, windowPos.y, int_t(bullet.width), int_t(bullet.height));
			cv::rectangle(m_image.m_data, rect, red, -1);

			// 显示垂足
			Pointi p1 = Scene::ToWindowPos(bullet.getPosition());
			Pointi p2 = Scene::ToWindowPos(Pointf(view.footPoint.x, view.footPoint.y));
			Pointi p3 = Scene::ToWindowPos(m_player.getPosition());
			cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p2.x, p2.y), orange);
			cv::line(m_image.m_data, cv::Point(p2.x, p2.y), cv::Point(p3.x, p3.y), orange);

			// 显示方向
			if (view.dir == DIR_UP)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x, p1.y - 20), yellow);
			else if (view.dir == DIR_DOWN)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x, p1.y + 20), yellow);
			else if (view.dir == DIR_LEFT)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x - 20, p1.y), yellow);
			else if (view.dir == DIR_RIGHT)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x + 20, p1.y), yellow);
			else if (view.dir == DIR_UPLEFT)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x - 20, p1.y - 20), yellow);
			else if (view.dir == DIR_UPRIGHT)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x + 20, p1.y - 20), yellow);
			else if (view.dir == DIR_DOWNLEFT)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x - 20, p1.y + 20), yellow);
			else if (view.dir == DIR_DOWNRIGHT)
				cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p1.x + 20, p1.y + 20), yellow);
		}

		//for (const Laser& laser : m_lasers)
		////for (const LaserLv1& lv1 : m_focusLasers)
		//{
		//	//const Laser& laser = m_lasers[lv1.index];

		//	LaserBox laserBox(laser);
		//	Pointi p1 = Scene::ToWindowPos(laserBox.topLeft);
		//	Pointi p2 = Scene::ToWindowPos(laserBox.topRight);
		//	Pointi p3 = Scene::ToWindowPos(laserBox.bottomLeft);
		//	Pointi p4 = Scene::ToWindowPos(laserBox.bottomRight);
		//	cv::line(m_image.m_data, cv::Point(p1.x, p1.y), cv::Point(p2.x, p2.y), red);
		//	cv::line(m_image.m_data, cv::Point(p2.x, p2.y), cv::Point(p3.x, p3.y), red);
		//	cv::line(m_image.m_data, cv::Point(p3.x, p3.y), cv::Point(p4.x, p4.y), red);
		//	cv::line(m_image.m_data, cv::Point(p4.x, p4.y), cv::Point(p1.x, p1.y), red);
		//}

		cv::imshow("TH10", m_image.m_data);
		cv::waitKey(1);
#endif
	}

	// Y轴系数
	float_t TH10Bot::GetYFactor(const Pointf& source, const Pointf& next)
	{
		float_t factor;

		if (next.y > source.y)
			factor = 1.0;
		else if (next.y < source.y)
			factor = -1.0;
		else
			factor = 0.0;

		return factor;
	}

	// 距离系数，远离加分，靠近减分
	float_t TH10Bot::GetDistFactor(float_t source, float_t next, float_t target)
	{
		float_t factor;

		float_t dSrc = std::abs(source - target);
		float_t dNext = std::abs(next - target);
		if (dNext > dSrc)
			factor = 1.0;
		else if (dNext < dSrc)
			factor = -1.0;
		else
			factor = 0.0;

		return factor;
	}

	float_t TH10Bot::GetDistXScore(float_t xNext, float_t xTarget)
	{
		float_t dx = std::abs(xNext - xTarget);
		if (dx > SCENE_SIZE.width)
			dx = SCENE_SIZE.width;
		return dx / SCENE_SIZE.width;
	}

	float_t TH10Bot::GetDistYScore(float_t yNext, float_t yTarget)
	{
		float_t dy = std::abs(yNext - yTarget);
		if (dy > SCENE_SIZE.height)
			dy = SCENE_SIZE.height;
		return dy / SCENE_SIZE.height;
	}

	// 处理炸弹
	bool TH10Bot::handleBomb()
	{
		if (m_keyX.isPressed())
		{
			m_keyX.release();
			//std::cout << "炸弹 RELEASE" << std::endl;
		}

		// 放了炸弹3秒后再检测碰撞
		//if (m_clock.getTimestamp() - m_bombCooldown >= 3000)
		//{
		//	if (collideBomb())
		if (m_player.status == 4)
		{
			//m_bombCooldown = m_clock.getTimestamp();
			m_keyX.press();
			//std::cout << "炸弹 PRESS" << std::endl;
			return true;
		}
		//}

		return false;
	}

	//bool TH10Bot::collideBomb()
	//{
	//	for (const Enemy& enemy : m_enemies)
	//	{
	//		if (m_player.collide(enemy))
	//			return true;
	//	}

	//	for (const Bullet& bullet : m_bullets)
	//	{
	//		if (m_player.collide(bullet))
	//			return true;
	//	}

	//	for (const Laser& laser : m_lasers)
	//	{
	//		if (m_player.collideSAT(laser))
	//			return true;
	//	}

	//	return false;
	//}

	// 处理对话
	bool TH10Bot::handleTalk()
	{
		if (m_enemies.size() <= 1 && m_bullets.empty() && m_lasers.empty())
		{
			// 延时2秒后对话
			if (m_clock.getTimestamp() - m_talkCooldown >= 2000)
			{
				if (m_keyZ.isPressed())
				{
					m_keyZ.release();
					//std::cout << "对话 RELEASE" << std::endl;
				}
				else
				{
					m_keyZ.press();
					//std::cout << "对话 PRESS" << std::endl;
				}
				return true;
			}
		}
		else
		{
			m_talkCooldown = m_clock.getTimestamp();
		}
		return false;
	}

	// 处理攻击
	bool TH10Bot::handleShoot()
	{
		if (!m_enemies.empty())
		{
			m_shootCooldown = m_clock.getTimestamp();
			m_keyZ.press();
			//std::cout << "攻击 PRESS" << std::endl;
		}
		else
		{
			// 没有敌人1秒后停止攻击
			if (m_clock.getTimestamp() - m_shootCooldown >= 1000)
			{
				m_keyZ.release();
				//std::cout << "攻击 RELEASE" << std::endl;
				return false;
			}
		}

		return true;
	}

	// 处理移动
	bool TH10Bot::handleMove()
	{
		m_itemId = findItem();
		m_enemyId = findEnemy();

		Node start = {};
		Node goal = {};
		aStar(start, goal);

		NodeResult result = dfs(m_player, 0.0f, 3);
		if (result.nextDir != DIR_NONE)
		{
			move(result.nextDir);
		}
		else
		{
			move(DIR_HOLD);
			std::cout << "无路可走。" << std::endl;
		}

		return true;
	}

	NodeResult TH10Bot::dfs(const Player& player, float_t frame, int_t depth)
	{
		NodeResult curResult = { 0.0f, DIR_NONE, 0.0f };

		// 计算当前节点分数
		float_t minCollideFrame = std::numeric_limits<float_t>::max();
		Direction minCollideDir = DIR_NONE;
		int_t willCollideCount = 0;
		for (const BulletView& view : m_focusBullets)
		{
			Bullet& bullet = m_bullets[view.index];

			float_t collideFrame = bullet.willCollideWith(m_player, frame);
			if (collideFrame > -1.0f)	// 已？/会碰撞
			{
				if (collideFrame < minCollideFrame)
				{
					minCollideFrame = collideFrame;
					minCollideDir = view.dir;
				}
				willCollideCount += 1;
			}
		}
		curResult.minCollideFrame = minCollideFrame;

		//thisResult.score += thisScore.score;
		//if (collideMove(player, frame))
		//{
		//	thisResult.score += -10000.0f;
		//	return res;
		//}
		//else
		//{
		//	thisResult.score += 1000.0f;
		//}

		//if (m_itemId != -1)
		//	thisResult.score += getCollectItemScore(player, m_itemId);
		//else if (m_enemyId != -1)
		//	thisResult.score += getShootEnemyScore(player, m_enemyId);
		//else
		//	thisResult.score += getGobackScore(player);

		if (depth <= 0)
			return curResult;

		//if (depth == 3)
		//{
		//	std::cout << "---------------------------------" << std::endl;
		//	std::cout << "bullet " << dr.score << " " << dr.minFrame << " " << dr.minDir << std::endl;
		//}

		// 计算子节点分数
		float_t nextMaxCollideFrame = std::numeric_limits<float_t>::lowest();
		Direction nextDir = DIR_NONE;
		for (int_t i = DIR_HOLD; i < 9; ++i)
		{
			Direction dir;
			if (minCollideDir == DIR_NONE)
				dir = DIRECTIONS[i];
			else
				dir = PD[minCollideDir][i];

			Pointf nextPos = m_player.getPosition() + MOVE_SPEED[dir];

			if (!Scene::IsInScene(nextPos))
				continue;

			Player nextPlayer = player;
			nextPlayer.setPosition(nextPos);

			NodeResult nextResult = dfs(nextPlayer, frame + 1.0f, depth - 1);

			//if (minCollideDir != DIR_NONE)
			//	nextResult.score += PS[i];

			if (nextResult.minCollideFrame > nextMaxCollideFrame)
			{
				nextMaxCollideFrame = nextResult.minCollideFrame;
				nextDir = dir;
			}

			//if (depth == 3)
			//{
			//	std::cout << dir << " " << nextRes.score << std::endl;
			//}
		}
		curResult.nextDir = nextDir;

		//if (depth == 3)
		//{
		//	std::cout << "best " << bestDir << " " << bestScore << std::endl;
		//}

		return curResult;
	}

	void TH10Bot::aStar(Node& start, Node& goal)
	{
		//std::set<Node, PositionLess> openSet;
		//std::set<Node, PositionLess> closedSet;

		//start.gScore = 0.0f;
		//start.hScore = heuristicCostEstimate(start, goal);
		//start.fScore = start.gScore + start.hScore;
		//openSet.insert(start);

		//while (!openSet.empty())
		//{
		//	// fScore取最大值，优先靠近终点的节点，和wiki的A*相反
		//	auto maxIt = --openSet.end();
		//	Node current = *maxIt;
		//	if (current == goal)
		//	{
		//		reconstructPath(current);
		//		return;
		//	}

		//	openSet.erase(maxIt);
		//	closedSet.insert(current);	// O(log(N))1

		//	for (Node neighbor : current)
		//	{
		//		if (closedSet.count(neighbor) == 1)	// O(log(N))2
		//			continue;

		//		// gScore在递增
		//		neighbor.gScore = current.gScore + distBetween(current, neighbor);
		//		neighbor.hScore = heuristicCostEstimate(neighbor, goal);
		//		neighbor.fScore = neighbor.gScore + neighbor.hScore;

		//		auto oldIt = openSet.find(neighbor);	// O(log(N))3
		//		if (oldIt == openSet.end())
		//		{
		//			openSet.insert(neighbor);	// O(log(N))4
		//		}
		//		else if (neighbor.gScore > (*oldIt).gScore)
		//		{
		//			openSet.erase(oldIt);
		//			openSet.insert(neighbor);	// O(log(N))4
		//		}
		//	}
		//}
	}

	float_t TH10Bot::distBetween(const Node& current, const Node& neighbor)
	{
		return 0.0f;
	}

	float_t TH10Bot::heuristicCostEstimate(const Node& neighbor, const Node& goal)
	{
		return 0.0f;
	}

	void TH10Bot::reconstructPath(const Node& goal)
	{
		m_path.clear();
	}

	NodeScore TH10Bot::getNodeScore(const Player& player, float_t frame)
	{
		NodeScore score = { 0.0f, std::numeric_limits<float_t>::max(), DIR_NONE, 0 };

		for (const BulletView& view : m_focusBullets)
		{
			Bullet& bullet = m_bullets[view.index];

			float_t collideFrame = bullet.willCollideWith(m_player, frame);
			if (collideFrame > -1.0f)	// 已？/会碰撞
			{
				if (collideFrame < score.minCollideFrame)
				{
					score.minCollideFrame = collideFrame;
					score.minCollideDir = view.dir;
				}
				score.willCollideCount += 1;
			}
		}

		return score;
	}

	//bool TH10Bot::collideMove(const Player& player, int_t frame)
	//{
	//	for (Enemy& enemy : m_enemies)
	//	{
	//		Pointf oldPos = enemy.getPos();
	//		Pointf newPos = enemy.advanceTo(frame);
	//		enemy.setPos(newPos);
	//		if (player.collide(enemy))
	//		{
	//			enemy.setPos(oldPos);
	//			return true;
	//		}
	//		enemy.setPos(oldPos);
	//	}

	//	for (const BulletView& view : m_focusBullets)
	//	{
	//		Bullet& bullet = m_bullets[view.index];

	//		float_t collided = bullet.willCollideWith(m_player);
	//		if (collided >= 0.0f)
	//			return true;
	//	}

	//	for (const LaserLv1& lv1 : m_focusLasers)
	//	{
	//		Laser& laser = m_lasers[lv1.index];

	//		Pointf oldPos = laser.getPos();
	//		Pointf newPos = laser.advanceTo(frame);
	//		laser.setPos(newPos);
	//		if (player.collideSAT(laser))
	//		{
	//			laser.setPos(oldPos);
	//			return true;
	//		}
	//		laser.setPos(oldPos);
	//	}

	//	return false;
	//}

	float_t TH10Bot::getTargetScore(const Player& pNext, const Pointf& target)
	{
		float_t score = 0.0f;

		if (pNext.getDistance(target) < 10.0f)
		{
			score += 300.0f;
		}
		else
		{
			score += 150.0f * (1.0f - GetDistXScore(pNext.x, target.x));
			score += 150.0f * (1.0f - GetDistYScore(pNext.y, target.y));
		}

		return score;
	}

	// 查找道具
	int_t TH10Bot::findItem()
	{
		int_t id = -1;

		if (m_player.y < SCENE_SIZE.height / 4.0f)
			return id;

		float_t minDist = std::numeric_limits<float_t>::max();
		for (uint_t i = 0; i < m_items.size(); ++i)
		{
			const Item& item = m_items[i];

			// 高于1/5屏
			if (item.y < SCENE_SIZE.height / 5.0f)
				continue;

			// 不在自机半屏内
			float_t dy = std::abs(m_player.y - item.y);
			if (dy > SCENE_SIZE.height / 3.0f)
				continue;

			// 与自机距离最近的
			float_t distance = m_player.getDistance(item);
			if (distance < minDist)
			{
				minDist = distance;
				id = i;
			}
		}

		return id;
	}

	// 查找敌人
	int_t TH10Bot::findEnemy()
	{
		int_t id = -1;

		if (m_player.y < SCENE_SIZE.height / 4.0f)
			return id;

		float_t minDist = std::numeric_limits<float_t>::max();
		for (uint_t i = 0; i < m_enemies.size(); ++i)
		{
			const Enemy& enemy = m_enemies[i];

			//  与自机X轴距离最近
			float_t dx = std::abs(m_player.x - enemy.x);
			if (dx < minDist)
			{
				minDist = dx;
				id = i;
			}
		}

		return id;
	}

	float_t TH10Bot::getDodgeEnemyScore(const Player& pNext, float_t epsilon)
	{
		float_t score = 0.0f;

		for (const Enemy& enemy : m_enemies)
		{
			if (pNext.collide(enemy))
			{
				score = -10000.0f;
				break;
			}
		}

		return score;
	}

	float_t TH10Bot::getDodgeLaserScore(const Player& pNext, float_t epsilon)
	{
		float_t score = 0.0f;

		for (const Laser& laser : m_lasers)
		{
			if (pNext.collideSAT(laser))
			{
				score = -10000.0f;
				break;
			}
		}

		return score;
	}

	// 拾取道具评分
	float_t TH10Bot::getCollectItemScore(const Player& pNext, int_t itemId)
	{
		float_t score = 0.0f;

		if (itemId == -1)
			return score;

		const Item& item = m_items[itemId];
		if (pNext.getDistance(item) < 5.0f)
		{
			score += 300.0f;
		}
		else
		{
			score += 150.0f * (1.0f - GetDistXScore(pNext.x, item.x));
			score += 150.0f * (1.0f - GetDistYScore(pNext.y, item.y));
		}

		return score;
	}

	// 攻击敌人评分
	float_t TH10Bot::getShootEnemyScore(const Player& pNext, int_t enemyId)
	{
		float_t score = 0.0f;

		if (enemyId == -1)
			return score;

		const Enemy& enemy = m_enemies[enemyId];
		float_t dx = std::abs(pNext.x - enemy.x);
		if (dx < 15.0f)
		{
			score += 300.0f;
		}
		else
		{
			// X轴距离越远得分越少
			if (dx > SCENE_SIZE.width)
				dx = SCENE_SIZE.width;
			score += 300.0f * (1.0f - dx / SCENE_SIZE.width);
		}

		return score;
	}

	float_t TH10Bot::getGobackScore(const Player& pNext)
	{
		float_t score = 0.0f;

		if (pNext.getDistance(INIT_POS) < 10.0f)
		{
			score += 30.0f;
		}
		else
		{
			score += 15.0f * (1.0f - GetDistXScore(pNext.x, INIT_POS.x));
			score += 15.0f * (1.0f - GetDistYScore(pNext.y, INIT_POS.y));
		}

		return score;
	}

	bool TH10Bot::IsSlow(Direction dir)
	{
		return (dir == DIR_HOLD_SLOW) || (dir >= DIR_UP_SLOW && dir <= DIR_DOWNRIGHT_SLOW);
	}

	void TH10Bot::move(Direction dir)
	{
		if (IsSlow(dir))
		{
			m_keyShift.press();
			//std::cout << "慢速 PRESS" << std::endl;
		}
		else
		{
			m_keyShift.release();
			//std::cout << "慢速 RELEASE" << std::endl;
		}

		switch (dir)
		{
		case DIR_HOLD_SLOW:
		case DIR_HOLD:
			m_keyUp.release(), m_keyDown.release(), m_keyLeft.release(), m_keyRight.release();
			//std::cout << "不动" << std::endl;
			break;

		case DIR_UP:
		case DIR_UP_SLOW:
			m_keyUp.press(), m_keyDown.release(), m_keyLeft.release(), m_keyRight.release();
			//std::cout << "上" << std::endl;
			break;

		case DIR_DOWN:
		case DIR_DOWN_SLOW:
			m_keyUp.release(), m_keyDown.press(), m_keyLeft.release(), m_keyRight.release();
			//std::cout << "下" << std::endl;
			break;

		case DIR_LEFT:
		case DIR_LEFT_SLOW:
			m_keyUp.release(), m_keyDown.release(), m_keyLeft.press(), m_keyRight.release();
			//std::cout << "左" << std::endl;
			break;

		case DIR_RIGHT:
		case DIR_RIGHT_SLOW:
			m_keyUp.release(), m_keyDown.release(), m_keyLeft.release(), m_keyRight.press();
			//std::cout << "右" << std::endl;
			break;

		case DIR_UPLEFT:
		case DIR_UPLEFT_SLOW:
			m_keyUp.press(), m_keyDown.release(), m_keyLeft.press(), m_keyRight.release();
			//std::cout << "左上" << std::endl;
			break;

		case DIR_UPRIGHT:
		case DIR_UPRIGHT_SLOW:
			m_keyUp.press(), m_keyDown.release(), m_keyLeft.release(), m_keyRight.press();
			//std::cout << "右上" << std::endl;
			break;

		case DIR_DOWNLEFT:
		case DIR_DOWNLEFT_SLOW:
			m_keyUp.release(), m_keyDown.press(), m_keyLeft.press(), m_keyRight.release();
			//std::cout << "左下" << std::endl;
			break;

		case DIR_DOWNRIGHT:
		case DIR_DOWNRIGHT_SLOW:
			m_keyUp.release(), m_keyDown.press(), m_keyLeft.release(), m_keyRight.press();
			//std::cout << "右下" << std::endl;
			break;
		}
	}

	// 处理道具
	//bool TH10Bot::handleItem()
	//{
	//	if (checkCollectStatus())
	//	{
	//		int_t itemId = findItem();
	//		if (itemId != -1)
	//			return collectItem(itemId);
	//	}
	//	return false;
	//}

	// 检测拾取状况
	//bool TH10Bot::checkCollectStatus()
	//{
	//	// 拾取冷却中
	//	if (m_clock.getTimestamp() - m_collectCooldown < 2000)
	//		return false;
	//
	//	// 无道具
	//	if (m_items.empty())
	//	{
	//		// 进入冷却
	//		m_collectCooldown = m_clock.getTimestamp();
	//		return false;
	//	}
	//
	//	// 自机在上半屏，道具少于10个，敌人多于2个
	//	if (m_player.y < SCENE_HEIGHT / 2.0 && m_items.size() < 10 && m_enemies.size() > 2)
	//	{
	//		// 进入冷却
	//		m_collectCooldown = m_clock.getTimestamp();
	//		return false;
	//	}
	//
	//	// 自机高于1/4屏
	//	if (m_player.y < SCENE_HEIGHT * 0.25)
	//	{
	//		// 进入冷却
	//		m_collectCooldown = m_clock.getTimestamp();
	//		return false;
	//	}
	//
	//	return true;
	//}

	// 拾取道具
	//bool TH10Bot::collectItem(int_t itemId)
	//{
	//	const Item& item = m_items[itemId];
	//
	//	// 靠近道具了
	//	if (m_player.collide(item, 5.0))
	//		return true;
	//
	//	int_t lastDir = DIR_NONE;
	//	bool lastShift = false;
	//	float_t maxScore = std::numeric_limits<float_t>::lowest();
	//	for (int_t i = 0; i < DIR_LENGHT; ++i)
	//	{
	//		int_t dir = DIRECTIONS[i];
	//		bool shift = false;
	//		float_t score = 0.0;
	//
	//		float_t xNext = m_player.x + MOVE_FACTORS[i].x * MOVE_SPEED;
	//		float_t yNext = m_player.y + MOVE_FACTORS[i].y * MOVE_SPEED;
	//		FixPos(xNext, yNext);
	//		Player next = { static_cast<float_t>(xNext), static_cast<float_t>(yNext), m_player.w, m_player.h };
	//		if (collide(next, 0.0))
	//		{
	//			shift = true;
	//			xNext = m_player.x + MOVE_FACTORS[i].x * MOVE_SPEED_SLOW;
	//			yNext = m_player.y + MOVE_FACTORS[i].y * MOVE_SPEED_SLOW;
	//			FixPos(xNext, yNext);
	//			next = { static_cast<float_t>(xNext), static_cast<float_t>(yNext), m_player.w, m_player.h };
	//			if (collide(next, 0.0))
	//				continue;
	//		}
	//
	//		score += collectItemScore(next, item);
	//
	//		if (score > maxScore)
	//		{
	//			maxScore = score;
	//			lastDir = dir;
	//			lastShift = shift;
	//		}
	//	}
	//	if (lastDir != DIR_NONE)
	//		move(lastDir, lastShift);
	//	else
	//		std::cout << "collectItem()无路可走" << std::endl;
	//
	//	return true;
	//}

	// 处理敌人
	//bool TH10Bot::handleEnemy()
	//{
	//	if (checkShootStatus())
	//	{
	//		int_t enemyId = findEnemy();
	//		if (enemyId != -1)
	//			return shootEnemy(enemyId);
	//	}
	//	return false;
	//}

	// 检测攻击状况
	//bool TH10Bot::checkShootStatus()
	//{
	//	// 无敌人
	//	if (m_enemies.empty())
	//	{
	//		return false;
	//	}
	//
	//	return true;
	//}

	// 攻击敌人
	//bool TH10Bot::shootEnemy(int_t enemyId)
	//{
	//	const Enemy& enemy = m_enemies[enemyId];
	//
	//	int_t lastDir = DIR_NONE;
	//	bool lastShift = false;
	//	float_t maxScore = 1e-15;
	//	for (int_t i = 0; i < DIR_LENGHT; ++i)
	//	{
	//		int_t dir = DIRECTIONS[i];
	//		bool shift = false;
	//		float_t score = 0.0;
	//
	//		float_t xNext = m_player.x + MOVE_FACTORS[i].x * MOVE_SPEED;
	//		float_t yNext = m_player.y + MOVE_FACTORS[i].y * MOVE_SPEED;
	//		FixPos(xNext, yNext);
	//		Player next = { static_cast<float_t>(xNext), static_cast<float_t>(yNext), m_player.w, m_player.h };
	//		if (collide(next, 0.0))
	//		{
	//			shift = true;
	//			xNext = m_player.x + MOVE_FACTORS[i].x * MOVE_SPEED_SLOW;
	//			yNext = m_player.y + MOVE_FACTORS[i].y * MOVE_SPEED_SLOW;
	//			FixPos(xNext, yNext);
	//			next = { static_cast<float_t>(xNext), static_cast<float_t>(yNext), m_player.w, m_player.h };
	//			if (collide(next, 0.0))
	//				continue;
	//		}
	//
	//		score += dodgeEnemyScore(next);
	//		score += shootEnemyScore(next, enemy);
	//
	//		if (score > maxScore)
	//		{
	//			maxScore = score;
	//			lastDir = dir;
	//			lastShift = shift;
	//		}
	//	}
	//	if (lastDir != DIR_NONE)
	//		move(lastDir, lastShift);
	//	else
	//		std::cout << "shootEnemy()无路可走" << std::endl;
	//
	//	return true;
	//}

	// 闪避敌人评分
	//float_t TH10Bot::dodgeEnemyScore(const Player& next)
	//{
	//	float_t allScore = 0.0;
	//	for (const Enemy& enemy : m_enemies)
	//	{
	//		float_t score = 0.0;
	//
	//		// 在敌机范围50外 
	//		if (!next.collide(enemy, 50.0))
	//		{
	//			score += 100.0;
	//		}
	//		else
	//		{
	//			Point2d dirFactor = GetDirFactor(m_player, next, enemy);
	//			score += (dirFactor.x * 50.0 + dirFactor.y * 50.0);
	//		}
	//
	//		score += GetYFactor(m_player, next) * 100.0;
	//
	//		allScore += score;
	//	}
	//	return allScore;
	//}

	// 归位
	//void TH10Bot::goback()
	//{
	//	// 靠近初始位置了
	//	if (m_player.collide(INIT_RECT, 5.0))
	//	{
	//		move(DIR_CENTER, false);
	//		return;
	//	}
	//
	//	int_t lastDir = DIR_NONE;
	//	bool lastShift = false;
	//	float_t maxScore = 1e-15;
	//	for (int_t i = 0; i < DIR_LENGHT; ++i)
	//	{
	//		int_t dir = DIRECTIONS[i];
	//		bool shift = false;
	//		float_t score = 0.0;
	//
	//		float_t nextX = m_player.x + MOVE_FACTORS[i].x * MOVE_SPEED;
	//		float_t nextY = m_player.y + MOVE_FACTORS[i].y * MOVE_SPEED;
	//		FixPos(nextX, nextY);
	//		Player player = { static_cast<float_t>(nextX), static_cast<float_t>(nextY), m_player.w, m_player.h };
	//		if (collide(player, 0.0))
	//		{
	//			shift = true;
	//			nextX = m_player.x + MOVE_FACTORS[i].x * MOVE_SPEED_SLOW;
	//			nextY = m_player.y + MOVE_FACTORS[i].y * MOVE_SPEED_SLOW;
	//			FixPos(nextX, nextY);
	//			player = { static_cast<float_t>(nextX), static_cast<float_t>(nextY), m_player.w, m_player.h };
	//			if (collide(player, 0.0))
	//				continue;
	//		}
	//
	//		score += 1.0 - GetDistScore(player, INIT_RECT);
	//
	//		//std::cout << (int_t)dir << " " << score << std::endl;
	//		if (score > maxScore)
	//		{
	//			maxScore = score;
	//			lastDir = dir;
	//			lastShift = shift;
	//		}
	//	}
	//	//std::cout << "last " << (int_t)lastDir << " " << lastShift << std::endl;
	//	if (lastDir != DIR_NONE)
	//		move(lastDir, lastShift);
	//	else
	//		std::cout << "goback()无路可走" << std::endl;
	//}
}
