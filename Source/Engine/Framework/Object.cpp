#include "Object.h"

namespace kiko
{
	void Object::Read(const json_t& value)
	{
		READ_DATA(value, name);

	}
}