#include "Object.h"

Object::Object() :
	_handle(-1),
	_pos(VGet(0.0f, 0.0f, 0.0f))
{

}

Object::~Object()
{
	for (int i = 0; i < 4; i++)
	{
		MV1DeleteModel(_handle[i]);
	}
}

void Object::Init()
{
	for (int i = 0; i < 4; i++)
	{
		_handle[i] = MV1LoadModel("data/Cube_Crate.mv1");
	}

	_pos[0] = VGet(100.0f, 0.0f, 100.0f);
	_pos[1] = VGet(-1000.0f, 0.0f, -100.0f);
	_pos[2] = VGet(1000.0f, 0.0f, -100.0f);
	_pos[3] = VGet(-1000.0f, 0.0f, 100.0f);

	for (int i = 0; i < 4; i++)
	{
		MV1SetPosition(_handle[i], _pos[i]);
	}
}

void Object::Draw()
{
	for (int i = 0; i < 4; i++)
	{
		MV1DrawModel(_handle[i]);
	}
}