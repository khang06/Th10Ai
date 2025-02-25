#include "Th10Base/Bullet.h"

namespace th
{
	Bullet::Bullet() :
		status(),
		type(),
		id()
	{
	}

	Bullet::Bullet(const BulletRaw* raw, uint_t id0)
	{
		pos.x = raw->x;
		pos.y = raw->y;
		delta.x = raw->dx;
		delta.y = raw->dy;
		size.x = raw->width + 1;
		size.y = raw->height + 1;
		status = raw->status;
		//type = raw->type;
		type = static_cast<int_t>(raw->width);
		id = id0;
	}
}
