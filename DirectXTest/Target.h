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

	//	一番近くにある物を探す関数
	XMFLOAT3 GetNearestPos(XMFLOAT3 _pos, vector<XMFLOAT3> _vector);

	//	一番近くにある物の添字を返す
	int GetNearestPosNum() { return m_nearestPosNum; }
private:
	int m_nearestPosNum;
};