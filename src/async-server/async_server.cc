#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <signal.h>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <fstream>
#include <filesystem>

#include "services.accounts.grpc.pb.h"
#include "authServer.h"

namespace po = boost::program_options;
namespace fs = std::filesystem;

bool GetDataFromFile(const std::string& path, std::string& data)
{
	std::vector<std::uint8_t> v;
	try
	{
		std::basic_fstream<std::uint8_t> dataStream(path.c_str(), std::ios::in | std::ios::binary);
		if (dataStream.fail())
		{
			return false;
		}

		auto dataSize = fs::file_size(path);

		v.resize(dataSize);
		dataStream.read(v.data(), dataSize);

		if (dataStream.fail())
		{
			return false;
		}
		data.assign(v.begin(), v.end());
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool GetOptions(Options& options, int ac, const char** av)
{
	try
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("port", po::value<int>(), "set port number")
			("cert", po::value<std::string>(), "specify certificate")
			("key", po::value<std::string>(), "specify private key for certificate")
			("thread", po::value<int>(), "specify number of threads per cq")
			("cq", po::value<int>(), "specify number of cq")
			("cd", po::value<int>(), "specify number of calldata per cq")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help") || ac == 1)
		{
			std::cout << desc << "\n";
			return 0;
		}
		if (vm.count("port"))
		{
			options.port = std::to_string(vm["port"].as<int>());
		}
		else
		{
			options.port = "50051";
		}
		if (vm.count("cert"))
		{
			options.certPath = vm["cert"].as<std::string>();
		}
		else
		{
			throw std::exception("cert must be given");
		}
		if (vm.count("key"))
		{
			options.keyPath = vm["key"].as<std::string>();
		}
		else
		{
			throw std::exception("key must be given");
		}
		if (vm.count("thread"))
		{
			options.threadCount = vm["thread"].as<int>();
		}
		else
		{
			options.threadCount = 1;
		}
		if (vm.count("cq"))
		{
			options.queueCount = vm["cq"].as<int>();
		}
		else
		{
			options.queueCount = 1;
		}
		if (vm.count("cd"))
		{
			options.callDataCount = vm["cd"].as<int>();
		}
		else
		{
			options.callDataCount = 1;
		}
		if (!GetDataFromFile(options.certPath, options.servercert))
		{
			std::cout << "error reading cert file" << std::endl;
			return 0;
		}
		if (!GetDataFromFile(options.keyPath, options.serverkey))
		{
			std::cout << "error reading key file" << std::endl;
			return 0;
		}
		//options.db.reset(new odb::mysql::database(ac, (char**)av));
	}
	catch (std::exception& e)
	{
		std::cerr << "error: " << e.what() << "\n";
		return false;
	}
	catch (...)
	{
		std::cerr << "Exception of unknown type!\n";
	}
	return true;
}

int main(int argc, const char** argv)
{
	int n = std::thread::hardware_concurrency();
	std::cout << "std::thread::hardware_concurrency() is: " << n << std::endl;
	//--user odb_test --database odb_test
	Options options;
	if (!GetOptions(options, argc, argv))
		return 0;

	authServer as(options);
	std::thread t([&]
		{
			as.run();
		});
	std::cout << "Press Ctrl-C to terminate" << std::endl;

	boost::asio::io_service service;
	boost::asio::signal_set signals(service, SIGINT, SIGTERM);
	signals.async_wait(
		[=](boost::system::error_code /*ec*/, int /*signo*/)
		{
		});
	service.run();

	as.shutdown();
	t.join();

	return 0;
}
