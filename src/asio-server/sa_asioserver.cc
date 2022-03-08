#include <string>

#include <grpcpp/grpcpp.h>

#include "services.accounts.grpc.pb.h"

#include <agrpc/asioGrpc.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/signal_set.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <optional>
#include <thread>

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

::grpc::Status Login(const ::Services::LoginRequest& request, ::Services::LoginResponse& response)
{
	std::cout << "Login: username = " << request.username() << ", password = " << request.password() << ", tfa_code = " << request.tfa_code() << std::endl;
	response.set_accountid("accountid1");
	response.set_authkey("accountid1");
	response.set_message("message1");
	response.set_status(Services::LoginResponse_Status::LoginResponse_Status_ERROR_ACCOUNT_LOCKED);
	return ::grpc::Status::OK;
}

::grpc::Status ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response)
{
	std::cout << "ResetPassword: old_password = " << request.old_password() << ", new_password = " << request.new_password() << std::endl;
	response.set_message("message1");
	response.set_status(Services::PasswordResponse_Status::PasswordResponse_Status_ERROR_PASSWORD_POLICY);
	return ::grpc::Status::OK;
}

::grpc::Status CreateAccount(const ::Services::CreateAccountRequest& request, ::Services::CreateAccountResponse& response)
{
	std::cout << "CreateAccount: password = " << request.password() << std::endl;
	auto f = request.fields();
	if (f.has_first_name())
	{
		std::cout << "CreateAccount: first_name = " << f.first_name() << std::endl;
	}
	if (f.has_surname())
	{
		std::cout << "CreateAccount: surname = " << f.surname() << std::endl;
	}
	if (f.has_email())
	{
		std::cout << "CreateAccount: email = " << f.email() << std::endl;
	}
	if (f.has_sms())
	{
		std::cout << "CreateAccount: sms = " << f.sms() << std::endl;
	}
	if (f.has_dob())
	{
		std::cout << "CreateAccount: dob = " << f.dob() << std::endl;
	}
	if (f.has_notes())
	{
		std::cout << "CreateAccount: notes = " << f.notes() << std::endl;
	}
	if (f.has_image())
	{
		std::cout << "CreateAccount: image size = " << f.image().size() << std::endl;
	}
	if (f.has_active_state())
	{
		std::cout << "CreateAccount: active_state = " << f.active_state() << std::endl;
	}
	if (f.has_created())
	{
		std::cout << "CreateAccount: created = " << f.created() << std::endl;
	}
	if (f.has_updated())
	{
		std::cout << "CreateAccount: updated = " << f.updated() << std::endl;
	}
	if (f.has_account_type())
	{
		std::cout << "CreateAccount: account_type = " << f.account_type() << std::endl;
	}
	response.set_accountid("accountid1");
	response.set_message("message1");
	response.set_status(Services::CreateAccountResponse_Status::CreateAccountResponse_Status_ERROR_ACCOUNT_EXISTS);
	return ::grpc::Status::OK;
}

::grpc::Status UpdateAccount(const ::Services::UpdateAccountRequest& request, ::Services::UpdateAccountResponse& response)
{
	std::cout << "AccountsImpl::UpdateAccount: accountid = " << request.accountid() << ", authkey = " << request.authkey() << std::endl;
	auto f = request.fields();
	if (f.has_first_name())
	{
		std::cout << "AccountsImpl::UpdateAccount: first_name = " << f.first_name() << std::endl;
	}
	if (f.has_surname())
	{
		std::cout << "AccountsImpl::UpdateAccount: surname = " << f.surname() << std::endl;
	}
	if (f.has_email())
	{
		std::cout << "AccountsImpl::UpdateAccount: email = " << f.email() << std::endl;
	}
	if (f.has_sms())
	{
		std::cout << "AccountsImpl::UpdateAccount: sms = " << f.sms() << std::endl;
	}
	if (f.has_dob())
	{
		std::cout << "AccountsImpl::UpdateAccount: dob = " << f.dob() << std::endl;
	}
	if (f.has_notes())
	{
		std::cout << "AccountsImpl::UpdateAccount: notes = " << f.notes() << std::endl;
	}
	if (f.has_image())
	{
		std::cout << "AccountsImpl::UpdateAccount: image size = " << f.image().size() << std::endl;
	}
	if (f.has_active_state())
	{
		std::cout << "AccountsImpl::UpdateAccount: active_state = " << f.active_state() << std::endl;
	}
	if (f.has_created())
	{
		std::cout << "AccountsImpl::UpdateAccount: created = " << f.created() << std::endl;
	}
	if (f.has_updated())
	{
		std::cout << "AccountsImpl::UpdateAccount: updated = " << f.updated() << std::endl;
	}
	if (f.has_account_type())
	{
		std::cout << "AccountsImpl::UpdateAccount: account_type = " << f.account_type() << std::endl;
	}
	response.set_message("message1");
	response.set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
	return ::grpc::Status::OK;
}

::grpc::Status ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response)
{
	std::cout << "ReadAccount: accountid = " << request.accountid() << ", authkey = " << request.authkey() << std::endl;
	response.set_message("message1");
	response.set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
	return ::grpc::Status::OK;
}

::grpc::Status ListAccounts(const ::Services::ListAccountRequests& request, ::Services::ListAccountResponse& response)
{
	std::cout << "ListAccounts" << std::endl;
	std::cout << "ListAccounts: authkey  = " << request.authkey() << ", start = " << request.start() << ", end = " << request.end() << std::endl;
	response.set_message("message1");
	response.set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
	auto f = response.add_fields();
	f->set_first_name("firstname1");
	f->set_email("email1");
	return ::grpc::Status::OK;
}
boost::asio::awaitable<void> HandleLoginRequest(Services::Account::AsyncService& service)
{
	grpc::ServerContext server_context;
	::Services::LoginRequest request;
	grpc::ServerAsyncResponseWriter<::Services::LoginResponse> writer{ &server_context };
	if (!co_await agrpc::request(&Services::Account::AsyncService::RequestLogin, service, server_context,
		request, writer))
	{
		co_return;
	}

	::Services::LoginResponse response;
	auto status = Login(request, response);
	co_await agrpc::finish(writer, response, status);
}

boost::asio::awaitable<void> HandleResetPasswordRequest(Services::Account::AsyncService& service)
{
	grpc::ServerContext server_context;
	::Services::PasswordRequest request;
	grpc::ServerAsyncResponseWriter<::Services::PasswordResponse> writer{ &server_context };
	if (!co_await agrpc::request(&Services::Account::AsyncService::RequestResetPassword, service, server_context,
		request, writer))
	{
		co_return;
	}

	::Services::PasswordResponse response;
	auto status = ResetPassword(request, response);
	co_await agrpc::finish(writer, response, status);
}

boost::asio::awaitable<void> HandleCreateAccountRequest(Services::Account::AsyncService& service)
{
	grpc::ServerContext server_context;
	::Services::CreateAccountRequest request;
	grpc::ServerAsyncResponseWriter<::Services::CreateAccountResponse> writer{ &server_context };
	if (!co_await agrpc::request(&Services::Account::AsyncService::RequestCreateAccount, service, server_context,
		request, writer))
	{
		co_return;
	}

	::Services::CreateAccountResponse response;
	auto status = CreateAccount(request, response);
	co_await agrpc::finish(writer, response, status);
}

boost::asio::awaitable<void> HandleUpdateAccountRequest(Services::Account::AsyncService& service)
{
	grpc::ServerContext server_context;
	::Services::UpdateAccountRequest request;
	grpc::ServerAsyncResponseWriter<::Services::UpdateAccountResponse> writer{ &server_context };
	if (!co_await agrpc::request(&Services::Account::AsyncService::RequestUpdateAccount, service, server_context,
		request, writer))
	{
		co_return;
	}

	::Services::UpdateAccountResponse response;
	auto status = UpdateAccount(request, response);
	co_await agrpc::finish(writer, response, status);
}

boost::asio::awaitable<void> HandleReadAccountRequest(Services::Account::AsyncService& service)
{
	grpc::ServerContext server_context;
	::Services::ReadAccountRequest request;
	grpc::ServerAsyncResponseWriter<::Services::ReadAccountResponse> writer{ &server_context };
	if (!co_await agrpc::request(&Services::Account::AsyncService::RequestReadAccount, service, server_context,
		request, writer))
	{
		co_return;
	}

	::Services::ReadAccountResponse response;
	auto status = ReadAccount(request, response);
	co_await agrpc::finish(writer, response, status);
}

boost::asio::awaitable<void> HandleListAccountsRequest(Services::Account::AsyncService& service)
{
	grpc::ServerContext server_context;
	::Services::ListAccountRequests request;
	grpc::ServerAsyncResponseWriter<::Services::ListAccountResponse> writer{ &server_context };
	if (!co_await agrpc::request(&Services::Account::AsyncService::RequestListAccounts, service, server_context,
		request, writer))
	{
		co_return;
	}

	::Services::ListAccountResponse response;
	auto status = ListAccounts(request, response);
	co_await agrpc::finish(writer, response, status);
}

int main(int argc, const char** argv)
{
    const auto port = argc >= 2 ? argv[1] : "50051";
    const auto host = std::string("0.0.0.0:") + port;

	std::cout << "Server listening on " << host << std::endl;
	std::cout << "Press Ctrl-C to terminate" << std::endl;
	std::unique_ptr<grpc::Server> server;

	grpc::ServerBuilder builder;
    agrpc::GrpcContext grpc_context{builder.AddCompletionQueue()};
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());
	Services::Account::AsyncService service;
    builder.RegisterService(&service);
	server = builder.BuildAndStart();

	ServerShutdown server_shutdown{*server, grpc_context};

	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleLoginRequest(service);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleResetPasswordRequest(service);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleCreateAccountRequest(service);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleUpdateAccountRequest(service);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleReadAccountRequest(service);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleListAccountsRequest(service);
		},
		boost::asio::detached);

	grpc_context.run();
    std::cout << "exiting\n";
}
