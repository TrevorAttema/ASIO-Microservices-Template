#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>

#include "authServer.h"

authServer::authServer(const Options& options)
	:grpc_context(builder.AddCompletionQueue())
{
	grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp;
	pkcp.private_key = options.serverkey;
	pkcp.cert_chain = options.servercert;

	grpc::SslServerCredentialsOptions ssl_opts;
	ssl_opts.pem_root_certs = "";
	ssl_opts.pem_key_cert_pairs.push_back(pkcp);

	std::shared_ptr<grpc::ServerCredentials> creds;
	creds = grpc::SslServerCredentials(ssl_opts);

	const auto host = std::string("0.0.0.0:") + options.port;
	std::cout << "Server listening on " << host << std::endl;

	builder.AddListeningPort(host, creds);
	builder.RegisterService(&service);
	server = builder.BuildAndStart();
}

template<class reqType, class rspType, class receiverType, class handlerType>
boost::asio::awaitable<void> HandleRequest(Services::Account::AsyncService& service, receiverType receiver, handlerType handler, authServer *ptr)
{
	grpc::ServerContext server_context;
	reqType request;
	grpc::ServerAsyncResponseWriter<rspType> writer{ &server_context };
	if (!co_await agrpc::request(receiver, service, server_context,
		request, writer))
	{
		co_return;
	}

	rspType response;
	auto status = handler(request, response, ptr);
	co_await agrpc::finish(writer, response, status);
}

void authServer::run()
{
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleRequest<::Services::LoginRequest,
				::Services::LoginResponse,
				decltype(&Services::Account::AsyncService::RequestLogin),
				decltype(_Login)>
				(service, &Services::Account::AsyncService::RequestLogin, &_Login, this);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleRequest<::Services::PasswordRequest,
				::Services::PasswordResponse,
				decltype(&Services::Account::AsyncService::RequestResetPassword),
				decltype(_ResetPassword)>
				(service, &Services::Account::AsyncService::RequestResetPassword, &_ResetPassword, this);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleRequest<::Services::CreateAccountRequest,
				::Services::CreateAccountResponse,
				decltype(&Services::Account::AsyncService::RequestCreateAccount),
				decltype(_CreateAccount)>
				(service, &Services::Account::AsyncService::RequestCreateAccount, &_CreateAccount, this);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleRequest<::Services::UpdateAccountRequest,
				::Services::UpdateAccountResponse,
				decltype(&Services::Account::AsyncService::RequestUpdateAccount),
				decltype(_UpdateAccount)>
				(service, &Services::Account::AsyncService::RequestUpdateAccount, &_UpdateAccount, this);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleRequest<::Services::ReadAccountRequest,
				::Services::ReadAccountResponse,
				decltype(&Services::Account::AsyncService::RequestReadAccount),
				decltype(_ReadAccount)>
				(service, &Services::Account::AsyncService::RequestReadAccount, &_ReadAccount, this);
		},
		boost::asio::detached);
	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await HandleRequest<::Services::ListAccountRequests,
				::Services::ListAccountResponse,
				decltype(&Services::Account::AsyncService::RequestListAccounts),
				decltype(_ListAccounts)>
				(service, &Services::Account::AsyncService::RequestListAccounts, &_ListAccounts, this);
		},
		boost::asio::detached);

	grpc_context.run();
}

::grpc::Status authServer::_Login(const ::Services::LoginRequest& request, ::Services::LoginResponse& response, authServer* ptr)
{
	return ptr->Login(request, response);
}

::grpc::Status authServer::Login(const ::Services::LoginRequest& request, ::Services::LoginResponse& response)
{
	std::cout << "Login: username = " << request.username() << ", password = " << request.password() << ", tfa_code = " << request.tfa_code() << std::endl;
	response.set_accountid("accountid1");
	response.set_authkey("accountid1");
	response.set_message("message1");
	response.set_status(Services::LoginResponse_Status::LoginResponse_Status_ERROR_ACCOUNT_LOCKED);
	return ::grpc::Status::OK;
}

::grpc::Status authServer::_ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response, authServer* ptr)
{
	return ptr->ResetPassword(request, response);
}

::grpc::Status authServer::ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response)
{
	std::cout << "ResetPassword: old_password = " << request.old_password() << ", new_password = " << request.new_password() << std::endl;
	response.set_message("message1");
	response.set_status(Services::PasswordResponse_Status::PasswordResponse_Status_ERROR_PASSWORD_POLICY);
	return ::grpc::Status::OK;
}

::grpc::Status authServer::_CreateAccount(const ::Services::CreateAccountRequest& request, ::Services::CreateAccountResponse& response, authServer* ptr)
{
	return ptr->CreateAccount(request, response);
}

::grpc::Status authServer::CreateAccount(const ::Services::CreateAccountRequest& request, ::Services::CreateAccountResponse& response)
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

::grpc::Status authServer::_UpdateAccount(const ::Services::UpdateAccountRequest& request, ::Services::UpdateAccountResponse& response, authServer* ptr)
{
	return ptr->UpdateAccount(request, response);
}

::grpc::Status authServer::UpdateAccount(const ::Services::UpdateAccountRequest& request, ::Services::UpdateAccountResponse& response)
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

::grpc::Status authServer::_ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response, authServer* ptr)
{
	return ptr->ReadAccount(request, response);
}

::grpc::Status authServer::ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response)
{
	std::cout << "ReadAccount: accountid = " << request.accountid() << ", authkey = " << request.authkey() << std::endl;
	response.set_message("message1");
	response.set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
	return ::grpc::Status::OK;
}

::grpc::Status authServer::_ListAccounts(const ::Services::ListAccountRequests& request, ::Services::ListAccountResponse& response, authServer* ptr)
{
	return ptr->ListAccounts(request, response);
}

::grpc::Status authServer::ListAccounts(const ::Services::ListAccountRequests& request, ::Services::ListAccountResponse& response)
{
	std::cout << "ListAccounts: authkey  = " << request.authkey() << ", start = " << request.start() << ", end = " << request.end() << std::endl;
	response.set_message("message1");
	response.set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
	auto f = response.add_fields();
	f->set_first_name("firstname1");
	f->set_email("email1");
	return ::grpc::Status::OK;
}
