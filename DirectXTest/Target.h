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

	//	ˆê”Ô‹ß‚­‚É‚ ‚é•¨‚ğ’T‚·ŠÖ”
	XMFLOAT3 GetNearestPos(XMFLOAT3 _pos, vector<XMFLOAT3> _vector);

	//	ˆê”Ô‹ß‚­‚É‚ ‚é•¨‚Ì“Yš‚ğ•Ô‚·
	int GetNearestPosNum() { return m_nearestPosNum; }
private:
	int m_nearestPosNum;
};