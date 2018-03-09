#pragma once

#include "GameNode.h"

class TestScene2 : public GameNode
{
private:
	Image * bg;
public:
	TestScene2();
	~TestScene2();

	virtual HRESULT Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void Render() override;
};

