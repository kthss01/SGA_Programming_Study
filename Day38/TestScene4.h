#pragma once

#include "GameNode.h"

class TestScene3;

class TestScene4 : public GameNode
{
private:

	TestScene3 * test;
	int data;
public:
	TestScene4();
	~TestScene4();

	virtual HRESULT Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void Render() override;

	void SetSceneLink(TestScene3* test) { this->test = test; }
};

