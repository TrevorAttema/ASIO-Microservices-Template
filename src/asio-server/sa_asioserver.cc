#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <agrpc/asioGrpc.hpp>

#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>

#include <optional>
#include <thread>
#include <fstream>
#include <filesystem>

#include "services.accounts.grpc.pb.h"
#include "authServer.h"

namespace po = boost::program_options;

struct ServerShutdown
{
    grpc::Server& server;
    boost::asio::basic_signal_set<agrpc::GrpcContext::executor_type> signals;
    std::optional<std::thread> shutdown_thread;

    ServerShutdown(grpc::Server& server, agrpc::GrpcContext& grpc_context)
        : server(server), signals(grpc_context, SIGINT, SIGTERM)
    {
        signals.async_wait(
            [&](boost::system::error_code /*ec*/, int /*signo*/)
            {
                shutdown();
            });
    }

    void shutdown()
    {
        if (!shutdown_thread)
        {
            // This will cause all coroutines to run to completion normally
            // while returning `false` from RPC related steps, cancelling the signal
            // so that the GrpcContext will eventually run out of work and return
            // from `run()`.
            shutdown_thread.emplace(
                [&]
                {
                    signals.cancel();
                    server.Shutdown();
                });
            // Alternatively call `grpc_context.stop()` here instead which causes all coroutines
            // to end at their next suspension point.
            // Then call `server->Shutdown()` after the call to `grpc_context.run()` returns
            // or `.reset()` the grpc_context and go into another `grpc_context.run()`
        }
    }

    ~ServerShutdown()
    {
        if (shutdown_thread)
        {
            shutdown_thread->join();
        }
    }
};

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
	//--user odb_test --database odb_test
	Options options;
	if (!GetOptions(options, argc, argv))
		return 0;

	std::cout << "Press Ctrl-C to terminate" << std::endl;

	authServer as(options);
	ServerShutdown server_shutdown{ as.getServer(), as.getContext() };
	as.run();

    std::cout << "exiting\n";
}
