#pragma once

#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class Target
{
public:
	Target();
	~Target();

	//	��ԋ߂��ɂ��镨��T���֐�
	XMFLOAT3 GetNearestPos(XMFLOAT3 _pos, vector<XMFLOAT3> _vector);

	//	��ԋ߂��ɂ��镨�̓Y����Ԃ�
	int GetNearestPosNum() { return m_nearestPosNum; }
private:
	int m_nearestPosNum;
};