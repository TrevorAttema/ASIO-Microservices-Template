#pragma once

#include <string>
#include <vector>
#include <thread>

struct Options
{
	int numThreads{ 0 };
	std::string port;
	std::string host;
};

class TestRunner
{
public:
	TestRunner(Options options);
	void Run();
	void Shutdown();
	void Wait();
private:
	Options m_options;
	std::vector<std::thread> m_threads;
	bool m_running{ false };
	void RunTests();
};

