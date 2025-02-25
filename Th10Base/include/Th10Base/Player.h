#pragma once

#include "Th10Base/Common.h"

#include "Th10Base/Entity.h"
#include "Th10Base/Th10Raws.h"
#include "Th10Base/Direction.h"

namespace th
{
	class Player :
		public Entity
	{
	public:
		static constexpr vec2 INIT_POS = vec2(_F(0.0), _F(400.0));

		// 3.535533905029297 = 5 * sqrt(0.5)
		static constexpr vec2 MOVE_SPEED_HIGH_MARISA[to_underlying(DIR::MAX_COUNT)] =
		{
			vec2(_F(0.0), _F(0.0)),
			vec2(_F(0.0), _F(-5.0)),
			vec2(_F(0.0), _F(5.0)),
			vec2(_F(-5.0), _F(0.0)),
			vec2(_F(5.0), _F(0.0)),
			vec2(_F(-3.535533905029297), _F(-3.535533905029297)),
			vec2(_F(3.535533905029297), _F(-3.535533905029297)),
			vec2(_F(-3.535533905029297), _F(3.535533905029297)),
			vec2(_F(3.535533905029297), _F(3.535533905029297))
		};

		// 3.181980609893799 = 4.5 * sqrt(0.5)
		static constexpr vec2 MOVE_SPEED_HIGH[to_underlying(DIR::MAX_COUNT)] =
		{
			vec2(_F(0.0), _F(0.0)),
			vec2(_F(0.0), _F(-4.5)),
			vec2(_F(0.0), _F(4.5)),
			vec2(_F(-4.5), _F(0.0)),
			vec2(_F(4.5), _F(0.0)),
			vec2(_F(-3.181980609893799), _F(-3.181980609893799)),
			vec2(_F(3.181980609893799), _F(-3.181980609893799)),
			vec2(_F(-3.181980609893799), _F(3.181980609893799)),
			vec2(_F(3.181980609893799), _F(3.181980609893799))
		};

		// 1.4142135381698608 = 2 * sqrt(0.5)
		static constexpr vec2 MOVE_SPEED_LOW[to_underlying(DIR::MAX_COUNT)] =
		{
			vec2(_F(0.0), _F(0.0)),
			vec2(_F(0.0), _F(-2.0)),
			vec2(_F(0.0), _F(2.0)),
			vec2(_F(-2.0), _F(0.0)),
			vec2(_F(2.0), _F(0.0)),
			vec2(_F(-1.4142135381698608), _F(-1.4142135381698608)),
			vec2(_F(1.4142135381698608), _F(-1.4142135381698608)),
			vec2(_F(-1.4142135381698608), _F(1.4142135381698608)),
			vec2(_F(1.4142135381698608), _F(1.4142135381698608))
		};

		Player();
		void set(const PlayerRaw* raw);

		void move(DIR dir, bool slow);

		bool isRebirthStatus() const;
		bool isNormalStatus() const;
		bool isColliding() const;
		bool isInvincible() const;
		bool isSlow() const;
		bool isReimu() const;
		bool isMarisa() const;

	public:
		// status == 0 重生状态，此时无敌
		// status == 1 正常状态
		// status == 2 死亡
		// status == 3 Unknown
		// status == 4 被弹、决死，此时放B可以决死
		int_t status;
		// 剩余无敌时间
		int_t invincibleTime;
		// slow == 0 High Speed
		// slow == 1 Low Speed
		int_t slow;

		float_t powers;
		// character == 0 Reimu
		// character == 1 Marisa
		int_t character;
		int_t lives;
		float_t itemGetRange;

		int_t stageFrame;
	};
}
