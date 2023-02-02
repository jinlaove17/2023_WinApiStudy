#pragma once

class CObject
{
private:
	Vec2 m_position;
	Vec2 m_scale;

public:
	CObject();
	virtual ~CObject();

	void SetPosition(const Vec2& position);
	const Vec2& GetPosition();

	void SetScale(const Vec2& scale);
	const Vec2& GetScale();

	virtual void Update() = 0;
	virtual void Render(HDC hDC);
};
