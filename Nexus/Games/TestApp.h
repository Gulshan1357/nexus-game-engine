#pragma once

class TestApp
{
public:
	TestApp();
	~TestApp();

	void Initialize();
	void Update(float deltaTime);
	void Render();
	void Shutdown();
};