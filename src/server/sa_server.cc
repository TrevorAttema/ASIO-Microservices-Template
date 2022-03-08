#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "services.accounts.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using Services::Account;

class AccountsImpl final : public Services::Account::Service
{
public:
	explicit AccountsImpl()
	{
	}

	::grpc::Status Login(::grpc::ServerContext* context, const ::Services::LoginRequest* request, ::Services::LoginResponse* response) override
	{
		std::cout << "AccountsImpl::Login: username = " << request->username() << ", password = " << request->password() << ", tfa_code = " << request->tfa_code() << std::endl;
		response->set_accountid("accountid1");
		response->set_authkey("accountid1");
		response->set_message("message1");
		response->set_status(Services::LoginResponse_Status::LoginResponse_Status_ERROR_ACCOUNT_LOCKED);
		return ::grpc::Status::OK;
	}

	::grpc::Status ResetPassword(::grpc::ServerContext* context, const ::Services::PasswordRequest* request, ::Services::PasswordResponse* response)
	{
		std::cout << "AccountsImpl::ResetPassword: old_password = " << request->old_password() << ", new_password = " << request->new_password() << std::endl;
		response->set_message("message1");
		response->set_status(Services::PasswordResponse_Status::PasswordResponse_Status_ERROR_PASSWORD_POLICY);
		return ::grpc::Status::OK;
	}

	::grpc::Status CreateAccount(::grpc::ServerContext* context, const ::Services::CreateAccountRequest* request, ::Services::CreateAccountResponse* response)
	{
		std::cout << "AccountsImpl::CreateAccount: password = " << request->password() << std::endl;
		auto f = request->fields();
		if (f.has_first_name())
		{
			std::cout << "AccountsImpl::CreateAccount: first_name = " << f.first_name() << std::endl;
		}
		if (f.has_surname())
		{
			std::cout << "AccountsImpl::CreateAccount: surname = " << f.surname() << std::endl;
		}
		if (f.has_email())
		{
			std::cout << "AccountsImpl::CreateAccount: email = " << f.email() << std::endl;
		}
		if (f.has_sms())
		{
			std::cout << "AccountsImpl::CreateAccount: sms = " << f.sms() << std::endl;
		}
		if (f.has_dob())
		{
			std::cout << "AccountsImpl::CreateAccount: dob = " << f.dob() << std::endl;
		}
		if (f.has_notes())
		{
			std::cout << "AccountsImpl::CreateAccount: notes = " << f.notes() << std::endl;
		}
		if (f.has_image())
		{
			std::cout << "AccountsImpl::CreateAccount: image size = " << f.image().size() << std::endl;
		}
		if (f.has_active_state())
		{
			std::cout << "AccountsImpl::CreateAccount: active_state = " << f.active_state() << std::endl;
		}
		if (f.has_created())
		{
			std::cout << "AccountsImpl::CreateAccount: created = " << f.created() << std::endl;
		}
		if (f.has_updated())
		{
			std::cout << "AccountsImpl::CreateAccount: updated = " << f.updated() << std::endl;
		}
		if (f.has_account_type())
		{
			std::cout << "AccountsImpl::CreateAccount: account_type = " << f.account_type() << std::endl;
		}
		response->set_accountid("accountid1");
		response->set_message("message1");
		response->set_status(Services::CreateAccountResponse_Status::CreateAccountResponse_Status_ERROR_ACCOUNT_EXISTS);
		return ::grpc::Status::OK;
	}

	::grpc::Status UpdateAccount(::grpc::ServerContext* context, const ::Services::UpdateAccountRequest* request, ::Services::UpdateAccountResponse* response)
	{
		std::cout << "AccountsImpl::UpdateAccount: accountid = " << request->accountid() << ", authkey = " << request->authkey() << std::endl;
		auto f = request->fields();
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
		response->set_message("message1");
		response->set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
		return ::grpc::Status::OK;
	}

	::grpc::Status ReadAccount(::grpc::ServerContext* context, const ::Services::ReadAccountRequest* request, ::Services::ReadAccountResponse* response)
	{
		std::cout << "AccountsImpl::ReadAccount: accountid = " << request->accountid() << ", authkey = " << request->authkey() << std::endl;
		response->set_message("message1");
		response->set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
		return ::grpc::Status::OK;
	}

	::grpc::Status ListAccounts(::grpc::ServerContext* context, const ::Services::ListAccountRequests* request, ::Services::ListAccountResponse* response)
	{
		std::cout << "AccountsImpl::ListAccounts" << std::endl;
		std::cout << "AccountsImpl::ListAccounts: authkey  = " << request->authkey() << ", start = " << request->start() << ", end = " << request->end() << std::endl;
		response->set_message("message1");
		response->set_status(Services::AccountResponseStatus::ERROR_FIELD_INVALID);
		auto f = response->add_fields();
		f->set_first_name("firstname1");
		f->set_email("email1");
		return ::grpc::Status::OK;
	}
};

std::unique_ptr<Server> server;
HANDLE hEvent;
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	std::cout << "CtrlHandler: calling server->Shutdown()" << std::endl;
	server->Shutdown();
	std::cout << "CtrlHandler: calling server->Wait()" << std::endl;
	server->Wait();
	std::cout << "CtrlHandler: shutdown completed" << std::endl;
	SetEvent(hEvent);
	return TRUE;
}

void RunServer()
{
	std::string server_address("0.0.0.0:50051");
	AccountsImpl service;

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	server = builder.BuildAndStart();
	std::cout << "Server listening on " << server_address << std::endl;
	std::cout << "Press Ctrl-C to terminate" << std::endl;

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	SetConsoleCtrlHandler(CtrlHandler, TRUE);
	std::cout << "Waiting for event" << std::endl;
	WaitForSingleObject(hEvent, INFINITE);
}

int main(int argc, char** argv)
{
	RunServer();
	std::cout << "exiting" << std::endl;
	return 0;
}
