#pragma once

#include "TH10Bot/Entity/Entity.h"

namespace th
{
	struct Player :
		Entity
	{
		Player advance(float_t frame) const;

		float_t powers;
		int_t life;
		// type == 0 Reimu
		// type == 1 Marisa
		int_t type;
		// slow == 0 High Speed
		// slow == 1 Low Speed
		int_t slow;
		float_t itemObtainRange;
		// status == 0 重生状态，此时无敌
		// status == 1 正常状态
		// status == 2 死亡
		// status == 3 Unknown
		// status == 4 被弹、决死，此时放B可以决死
		int_t status;
		// 剩余无敌时间
		int_t invinibleTime;
	};
}
