#include "authServer.h"

authServer::authServer(const Options& options)
{
	grpc::ServerBuilder builder;
	grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp;
	pkcp.private_key = options.serverkey;
	pkcp.cert_chain = options.servercert;

	grpc::SslServerCredentialsOptions ssl_opts;
	ssl_opts.pem_root_certs = "";
	ssl_opts.pem_key_cert_pairs.push_back(pkcp);

	auto creds = grpc::SslServerCredentials(ssl_opts);

	const auto host = std::string("localhost:") + options.port;
	std::cout << "Server listening on " << host << std::endl;

	builder.AddListeningPort(host, creds);
	builder.RegisterService(&service);
	queue = builder.AddCompletionQueue();
	server = builder.BuildAndStart();
}

#if 0
void CreateAccountCallData::Proceed()
{
	std::cout << "CreateAccountCallData::Proceed: _status = " << (int)_status << std::endl;
}

void UpdateAccountCallData::Proceed()
{
	std::cout << "UpdateAccountCallData::Proceed: _status = " << (int)_status << std::endl;
}

void ReadAccountCallData::Proceed()
{
	std::cout << "ReadAccountCallData::Proceed: _status = " << (int)_status << std::endl;
}

void ListAccountsCallData::Proceed()
{
	std::cout << "ListAccountsCallData::Proceed: _status = " << (int)_status << std::endl;
}
#endif

void authServer::HandleRPCs()
{
	using LoginCallData = CallData<Services::LoginRequest, Services::LoginResponse, authServer, Services::Account::AsyncService,
		decltype(&Services::Account::AsyncService::RequestLogin), decltype(&authServer::Login)>;
	using ResetPasswordCallData = CallData<Services::PasswordRequest, Services::PasswordResponse, authServer, Services::Account::AsyncService,
		decltype(&Services::Account::AsyncService::RequestResetPassword), decltype(&authServer::ResetPassword)>;
	using CreateAccountCallData = CallData<Services::CreateAccountRequest, Services::CreateAccountResponse, authServer, Services::Account::AsyncService,
		decltype(&Services::Account::AsyncService::RequestCreateAccount), decltype(&authServer::CreateAccount)>;
	using UpdateAccountCallData = CallData<Services::UpdateAccountRequest, Services::UpdateAccountResponse, authServer, Services::Account::AsyncService,
		decltype(&Services::Account::AsyncService::RequestUpdateAccount), decltype(&authServer::UpdateAccount)>;
	using ReadAccountCallData = CallData<Services::ReadAccountRequest, Services::ReadAccountResponse, authServer, Services::Account::AsyncService,
		decltype(&Services::Account::AsyncService::RequestReadAccount), decltype(&authServer::ReadAccount)>;
	using ListAccountsCallData = CallData<Services::ListAccountRequests, Services::ListAccountResponse, authServer, Services::Account::AsyncService,
		decltype(&Services::Account::AsyncService::RequestListAccounts), decltype(&authServer::ListAccounts)>;

	std::cout << "authServer::HandleRPCs: entry" << std::endl;
	auto cd1 = new LoginCallData(&service, queue.get(), this,
			&Services::Account::AsyncService::RequestLogin, &authServer::Login);
	auto cd2 = new ResetPasswordCallData(&service, queue.get(), this,
			&Services::Account::AsyncService::RequestResetPassword, &authServer::ResetPassword);
	auto cd3 = new CreateAccountCallData(&service, queue.get(), this,
			&Services::Account::AsyncService::RequestCreateAccount, &authServer::CreateAccount);
	auto cd4 = new UpdateAccountCallData(&service, queue.get(), this,
			&Services::Account::AsyncService::RequestUpdateAccount, &authServer::UpdateAccount);
	auto cd5 = new ReadAccountCallData(&service, queue.get(), this,
			&Services::Account::AsyncService::RequestReadAccount, &authServer::ReadAccount);
	auto cd6 = new ListAccountsCallData(&service, queue.get(), this,
			&Services::Account::AsyncService::RequestListAccounts, &authServer::ListAccounts);
	cd1->Proceed();
	cd2->Proceed();
	cd3->Proceed();
	cd4->Proceed();
	cd5->Proceed();
	cd6->Proceed();
	void* tag;
	bool ok;
	while (queue->Next(&tag, &ok))
	{
		auto* tag_ptr = static_cast<CallDataBase<authServer, Services::Account::AsyncService>*>(tag);
		if (tag_ptr)
		{
			if (ok)
			{
				tag_ptr->Proceed();
			}
			else
			{
				delete tag_ptr;
			}
		}
	}
	std::cout << "authServer::HandleRPCs: exit" << std::endl;
}

void authServer::run()
{
	HandleRPCs();
}

void authServer::shutdown()
{
	std::cout << "authServer::shutdown: calling server->Shutdown()" << std::endl;
	server->Shutdown();
	std::cout << "authServer::shutdown: calling queue->Shutdown()" << std::endl;
	queue->Shutdown();
	std::cout << "calling server->Wait()" << std::endl;
	server->Wait();
	std::cout << "shutdown completed" << std::endl;
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

::grpc::Status authServer::ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response)
{
	std::cout << "ResetPassword: old_password = " << request.old_password() << ", new_password = " << request.new_password() << std::endl;
	response.set_message("message1");
	response.set_status(Services::PasswordResponse_Status::PasswordResponse_Status_ERROR_PASSWORD_POLICY);
	return ::grpc::Status::OK;
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

::grpc::Status authServer::ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response)
{
	std::cout << "ReadAccount: accountid = " << request.accountid() << ", authkey = " << request.authkey() << std::endl;
	response.set_message("message1");
	response.set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
	return ::grpc::Status::OK;
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
