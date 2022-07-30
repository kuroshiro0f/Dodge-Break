#include "Target.h"

Target::Target()
	:m_nearestPosNum(0)
{
}

Target::~Target()
{
}

XMFLOAT3 Target::GetNearestPos(XMFLOAT3 _pos, vector<XMFLOAT3> _vector)
{
	XMFLOAT3 tempPos = { 0,0,0 };
	float SquaredA, SquaredB, SquaredC, SquaredD;
	float num1, num2;

	for (int i = 0; i < _vector.size(); i++)
	{
		if (i == 0)
		{
			tempPos = _vector[i];
		}

		SquaredA = (tempPos.x - _pos.x) * (tempPos.x - _pos.x);
		SquaredB = (tempPos.z - _pos.z) * (tempPos.z - _pos.z);
		SquaredC = (_vector[i].x - _pos.x) * (_vector[i].x - _pos.x);
		SquaredD = (_vector[i].z - _pos.z) * (_vector[i].z - _pos.z);

		num1 = (SquaredA + SquaredB);
		num2 = (SquaredC + SquaredD);

		if (num2 < num1)
		{
			tempPos = _vector[i];
			m_nearestPosNum = i;
		}
	}
	return tempPos;
}
