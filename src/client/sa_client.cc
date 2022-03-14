#include <memory>
#include <iostream>
#include <Windows.h>

#include <boost/program_options.hpp>

#include "TestRunner.h"

namespace po = boost::program_options;
bool GetOptions(Options& options, int ac, const char** av)
{
	po::options_description desc("Allowed options");
	try
	{
		desc.add_options()
			("help", "produce help message")
			("threads", po::value<int>(), "set number of threads. If not given, the test will run once")
			("port", po::value<int>(), "specify port number")
			("host", po::value<std::string>(), "specify host name")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help") || ac == 1)
		{
			std::cout << desc << "\n";
			return 0;
		}
		if (vm.count("threads"))
		{
			options.numThreads = (vm["threads"].as<int>());
		}
		else
		{
			options.numThreads = 0;
		}
		if (vm.count("port"))
		{
			options.port = std::to_string(vm["port"].as<int>());
		}
		else
		{
			throw std::exception("port must be given");
		}
		if (vm.count("host"))
		{
			options.host = vm["host"].as<std::string>();
		}
		else
		{
			throw std::exception("host must be given");
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "error: " << e.what() << "\n";
		std::cout << desc << "\n";
		return false;
	}
	catch (...)
	{
		std::cout << desc << "\n";
		return false;
	}
	return true;
}

std::unique_ptr<TestRunner> tr;
HANDLE hEvent;
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	std::cout << "CtrlHandler: calling tr->Shutdown()" << std::endl;
	tr->Shutdown();
	std::cout << "CtrlHandler: calling tr->Wait()" << std::endl;
	tr->Wait();
	std::cout << "CtrlHandler: shutdown completed" << std::endl;
	SetEvent(hEvent);
	return TRUE;
}

int main(int argc, const char** argv)
{
	Options options;
	if (!GetOptions(options, argc, argv))
		return 0;
	std::cout << "options.numThreads = " << options.numThreads << "\n";
	std::cout << "options.host = " << options.host << "\n";
	std::cout << "options.port = " << options.port << "\n";

	tr = std::make_unique<TestRunner>(options);
	tr->Run();
	if (options.numThreads != 0)
	{
		std::cout << "Press Ctrl-C to terminate" << std::endl;

		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		SetConsoleCtrlHandler(CtrlHandler, TRUE);
		std::cout << "Waiting for event" << std::endl;
		WaitForSingleObject(hEvent, INFINITE);
	}
	return 0;
}
