#include "stdafx.h"
#include "TestApp.h"

#include "../App/app.h"

#include "src/Utils/Logger.h"

TestApp::TestApp()
{
	Logger::Log("Game constructor called!");
}
TestApp::~TestApp()
{
	Logger::Warn("Game destructor called!");
}

void TestApp::Initialize()
{
	Logger::Log("TestApp::Initialize()");
}

void TestApp::Update(float deltaTime)
{
	Logger::Log("TestApp::Update()");
}

void TestApp::Render()
{
	App::Print(100, 100, "Sample Text");
}

void TestApp::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
