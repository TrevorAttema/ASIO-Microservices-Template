#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "services.accounts.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using Services::Account;

class SAClient
{
public:
	struct loginRequest
	{
		std::string username;
		std::string password;
		std::string tfa_code;
	};

	struct loginResponse
	{
		Services::LoginResponse_Status status;
		std::string message;
		std::string authkey;
		std::string accountId;
	};

	struct passwordRequest
	{
		std::string old_password;
		std::string new_password;
	};

	struct passwordResponse
	{
		Services::PasswordResponse_Status status;
		std::string message;
	};

	enum class AccountFieldsFlags
	{
		first_name =	1 << 0,
		surname =		1 << 1,
		email =			1 << 2,
		sms =			1 << 3,
		dob =			1 << 4,
		notes =			1 << 5,
		image =			1 << 6,
		active_state =	1 << 7,
		created =		1 << 8,
		updated =		1 << 9,
		account_type =	1 << 10,
	};

	struct AccountFields
	{
		std::uint32_t mask;
		std::string first_name;
		std::string surname;
		std::string email;
		std::string sms;
		std::uint64_t dob;
		std::string notes;
		std::string image;
		Services::AccountActiveState active_state;
		std::uint64_t created;
		std::uint64_t updated;
		Services::AccountType account_type;
	};

	struct createAccountRequest
	{
		AccountFields fields;
		std::string password;
	};

	struct createAccountResponse
	{
		Services::CreateAccountResponse_Status status;
		std::string message;
	};

	struct updateAccountRequest
	{
		std::string authkey;
		std::string accountId;
		AccountFields fields;
	};

	struct updateAccountResponse
	{
		Services::AccountResponseStatus status;
		std::string message;
		AccountFields fields;
	};

	struct readAccountRequest
	{
		std::string authkey;
		std::string accountId;
	};

	struct readAccountResponse
	{
		Services::AccountResponseStatus status;
		std::string message;
		AccountFields fields;
	};

	struct AccountFieldSelect
	{
		std::uint32_t mask;
	};

	struct listAccountsRequest
	{
		std::string authkey;
		std::uint64_t start;
		std::uint64_t end;
		AccountFieldSelect fields;
	};

	struct listAccountsResponse
	{
		Services::AccountResponseStatus status;
		std::string message;
		std::vector<AccountFields> fields;
	};

	SAClient(std::shared_ptr<Channel> channel)
		: stub_(Account::NewStub(channel))
	{
	}

	::grpc::Status Login(const loginRequest &req, loginResponse &rsp)
	{
		::Services::LoginRequest request;
		::Services::LoginResponse response;
		request.set_username(req.username);
		request.set_password(req.password);
		request.set_tfa_code(req.tfa_code);
		ClientContext context;
		Status status = stub_->Login(&context, request, &response);
		if (!status.ok())
		{
			return status;
		}
		rsp.status = response.status();
		rsp.message = response.message();
		rsp.authkey = response.authkey();
		rsp.accountId = response.accountid();
		return status;
	}

	::grpc::Status ResetPassword(const passwordRequest &req, passwordResponse &rsp)
	{
		::Services::PasswordRequest request;
		::Services::PasswordResponse response;
		request.set_old_password(req.old_password);
		request.set_new_password(req.new_password);
		ClientContext context;
		Status status = stub_->ResetPassword(&context, request, &response);
		if (!status.ok())
		{
			return status;
		}
		rsp.status = response.status();
		rsp.message = response.message();
		return status;
	}

	void SetProtoFieldsFromFields(::Services::AccountFields* f, const AccountFields &fields)
	{
		std::uint32_t mask = fields.mask;
		if (mask & (std::uint32_t)AccountFieldsFlags::first_name)
			f->set_first_name(fields.first_name);
		if (mask & (std::uint32_t)AccountFieldsFlags::surname)
			f->set_surname(fields.surname);
		if (mask & (std::uint32_t)AccountFieldsFlags::email)
			f->set_email(fields.email);
		if (mask & (std::uint32_t)AccountFieldsFlags::sms)
			f->set_sms(fields.sms);
		if (mask & (std::uint32_t)AccountFieldsFlags::dob)
			f->set_dob(fields.dob);
		if (mask & (std::uint32_t)AccountFieldsFlags::notes)
			f->set_notes(fields.notes);
		if (mask & (std::uint32_t)AccountFieldsFlags::image)
			f->set_image(fields.image);
		if (mask & (std::uint32_t)AccountFieldsFlags::active_state)
			f->set_active_state(fields.active_state);
		if (mask & (std::uint32_t)AccountFieldsFlags::created)
			f->set_created(fields.created);
		if (mask & (std::uint32_t)AccountFieldsFlags::updated)
			f->set_updated(fields.updated);
		if (mask & (std::uint32_t)AccountFieldsFlags::account_type)
			f->set_account_type(fields.account_type);
	}

	void SetAccountFieldSelectFromMask(::Services::AccountFieldSelect* f, std::int32_t mask)
	{
		if (mask & (std::uint32_t)AccountFieldsFlags::first_name)
			f->set_first_name(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::surname)
			f->set_surname(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::email)
			f->set_email(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::sms)
			f->set_sms(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::dob)
			f->set_dob(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::notes)
			f->set_notes(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::image)
			f->set_image(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::active_state)
			f->set_active_state(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::created)
			f->set_created(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::updated)
			f->set_updated(true);
		if (mask & (std::uint32_t)AccountFieldsFlags::account_type)
			f->set_account_type(true);
	}

	void SetFieldsFromProtoFields(AccountFields &fields, const ::Services::AccountFields &f)
	{
		fields.mask = 0;
		if (f.has_first_name())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::first_name;
			fields.first_name = f.first_name();
		}
		if (f.has_surname())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::surname;
			fields.surname = f.surname();
		}
		if (f.has_email())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::email;
			fields.email = f.email();
		}
		if (f.has_sms())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::sms;
			fields.sms = f.sms();
		}
		if (f.has_dob())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::dob;
			fields.dob = f.dob();
		}
		if (f.has_notes())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::notes;
			fields.notes = f.notes();
		}
		if (f.has_image())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::image;
			fields.image = f.image();
		}
		if (f.has_active_state())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::active_state;
			fields.active_state = f.active_state();
		}
		if (f.has_created())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::created;
			fields.created = f.created();
		}
		if (f.has_updated())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::updated;
			fields.updated = f.updated();
		}
		if (f.has_account_type())
		{
			fields.mask |= (std::uint32_t)AccountFieldsFlags::account_type;
			fields.account_type = f.account_type();
		}
	}

	::grpc::Status CreateAccount(const createAccountRequest &req, createAccountResponse &rsp)
	{
		::Services::CreateAccountRequest request;
		::Services::CreateAccountResponse response;
		request.set_password(req.password);
		auto f = request.mutable_fields();
		SetProtoFieldsFromFields(f, req.fields);

		ClientContext context;
		Status status = stub_->CreateAccount(&context, request, &response);
		if (!status.ok())
		{
			return status;
		}
		rsp.status = response.status();
		rsp.message = response.message();
		return status;
	}

	::grpc::Status UpdateAccount(const updateAccountRequest &req, updateAccountResponse &rsp)
	{
		::Services::UpdateAccountRequest request;
		::Services::UpdateAccountResponse response;
		request.set_accountid(req.accountId);
		request.set_authkey(req.authkey);
		auto f = request.mutable_fields();
		SetProtoFieldsFromFields(f, req.fields);

		ClientContext context;
		Status status = stub_->UpdateAccount(&context, request, &response);
		if (!status.ok())
		{
			return status;
		}
		auto f1 = response.fields();
		SetFieldsFromProtoFields(rsp.fields, f1);
		rsp.status = response.status();
		rsp.message = response.message();
		return status;
	}

	::grpc::Status ReadAccount(const readAccountRequest &req, readAccountResponse &rsp)
	{
		::Services::ReadAccountRequest request;
		::Services::ReadAccountResponse response;
		request.set_accountid(req.accountId);
		request.set_authkey(req.authkey);
		ClientContext context;
		Status status = stub_->ReadAccount(&context, request, &response);
		if (!status.ok())
		{
			return status;
		}
		auto f1 = response.fields();
		SetFieldsFromProtoFields(rsp.fields, f1);
		rsp.status = response.status();
		rsp.message = response.message();
		return status;
	}

	::grpc::Status ListAccounts(const listAccountsRequest &req, listAccountsResponse &rsp)
	{
		::Services::ListAccountRequests request;
		::Services::ListAccountResponse response;
		request.set_authkey(req.authkey);
		request.set_start(req.start);
		request.set_end(req.end);
		SetAccountFieldSelectFromMask(request.mutable_fields(), req.fields.mask);
		ClientContext context;
		Status status = stub_->ListAccounts(&context, request, &response);
		if (!status.ok())
		{
			return status;
		}
		rsp.status = response.status();
		rsp.message = response.message();
		int n = response.fields_size();
		rsp.fields.clear();
		for (int j = 0; j < n; j++)
		{
			auto f = response.fields(j);
			AccountFields fields;
			SetFieldsFromProtoFields(fields, f);
			rsp.fields.push_back(fields);
		}
		return status;
	}

private:

	std::unique_ptr<Account::Stub> stub_;
};

int main(int argc, char** argv)
{
	////////////////////////////////////
	constexpr char kDummyRootCert[] =
		"-----BEGIN CERTIFICATE-----\n"
		"MIICwzCCAaugAwIBAgIJAM12DOkcaqrhMA0GCSqGSIb3DQEBBQUAMBQxEjAQBgNV\n"
		"BAMTCWxvY2FsaG9zdDAeFw0yMDEwMDcwODIyNDFaFw0zMDEwMDUwODIyNDFaMBQx\n"
		"EjAQBgNVBAMTCWxvY2FsaG9zdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
		"ggEBALjJ8KPEpF0P4GjMPrJhjIBHUL0AX9E4oWdgJRCSFkPKKEWzQabTQBikMOhI\n"
		"W4VvBMaHEBuECE5OEyrzDRiAO354I4F4JbBfxMOY8NIW0uWD6THWm2KkCzoZRIPW\n"
		"yZL6dN+mK6cEH+YvbNuy5ZQGNjGG43tyiXdOCAc4AI9POeTtjdMpbbpR2VY4Ad/E\n"
		"oTEiS3gNnN7WIAdgMhCJxjzvPwKszV3f7pwuTHzFMsuHLKr6JeaVUYfbi4DxxC8Z\n"
		"k6PF6dLlLf3ngTSLBJyaXP1BhKMvz0TaMK3F0y2OGwHM9J8np2zWjTlNVEzffQZx\n"
		"SWMOQManlJGs60xYx9KCPJMZZsMCAwEAAaMYMBYwFAYDVR0RBA0wC4IJbG9jYWxo\n"
		"b3N0MA0GCSqGSIb3DQEBBQUAA4IBAQC0LrmbcNKgO+D50d/wOc+vhi9K04EZh8bg\n"
		"WYAK1kLOT4eShbzqWGV/1EggY4muQ6ypSELCLuSsg88kVtFQIeRilA6bHFqQSj6t\n"
		"sqgh2cWsMwyllCtmX6Maf3CLb2ZdoJlqUwdiBdrbIbuyeAZj3QweCtLKGSQzGDyI\n"
		"KH7G8nC5d0IoRPiCMB6RnMMKsrhviuCdWbAFHop7Ff36JaOJ8iRa2sSf2OXE8j/5\n"
		"obCXCUvYHf4Zw27JcM2AnnQI9VJLnYxis83TysC5s2Z7t0OYNS9kFmtXQbUNlmpS\n"
		"doQ/Eu47vWX7S0TXeGziGtbAOKxbHE0BGGPDOAB/jGW/JVbeTiXY\n"
		"-----END CERTIFICATE-----\n";
	auto certificate_provider = std::make_shared<grpc::experimental::StaticDataCertificateProvider>(kDummyRootCert);
	grpc::experimental::TlsChannelCredentialsOptions options;
	options.set_certificate_provider(certificate_provider);
	options.watch_root_certs();
	options.set_root_cert_name("dummy");
	struct NoOpTlsAuthorizationCheck
		: public grpc::experimental::TlsServerAuthorizationCheckInterface {
		int Schedule(grpc::experimental::TlsServerAuthorizationCheckArg* arg) override {
			arg->set_success(1);
			arg->set_status(GRPC_STATUS_OK);
			return 0;
		}
	};
	auto server_authorization_check = std::make_shared<NoOpTlsAuthorizationCheck>();
	auto noop_auth_check_ = std::make_shared<grpc::experimental::TlsServerAuthorizationCheckConfig>(
		server_authorization_check);
	options.set_server_verification_option(GRPC_TLS_SKIP_ALL_SERVER_VERIFICATION);
	options.set_server_authorization_check_config(noop_auth_check_);
	////////////////////////////////////
	auto channel_creds = grpc::experimental::TlsCredentials(options);
	auto channel = grpc::CreateChannel("localhost:50051", channel_creds);
	SAClient client(channel);

	{
		SAClient::loginRequest req{ "login1","pass1","mfa1" };
		SAClient::loginResponse rsp;
		std::cout << "calling Login: username = " << req.username << ", password = " << req.password << ", tfa_code =" << req.tfa_code << std::endl;
		::grpc::Status s = client.Login(req, rsp);
		std::cout << "status = " << s.error_code() << std::endl;
		if (s.ok())
		{
			std::cout << "Login returned: accountId = " << rsp.accountId << ", authkey = " << rsp.authkey << ", message = " << rsp.message << ", status = " << rsp.status << std::endl;
		}
	}
	{
		SAClient::passwordRequest req{ "oldpass1","newpass1" };
		SAClient::passwordResponse rsp;
		std::cout << "calling ResetPassword: old_password = " << req.old_password << ", new_password = " << req.new_password << std::endl;
		::grpc::Status s = client.ResetPassword(req, rsp);
		std::cout << "status = " << s.error_code() << std::endl;
		if (s.ok())
		{
			std::cout << "ResetPassword returned: message = " << rsp.message << ", status = " << rsp.status << std::endl;
		}
	}
	{
		SAClient::createAccountRequest req;
		SAClient::createAccountResponse rsp;
		req.fields.first_name = "firstname1";
		req.fields.dob = 1234;
		req.fields.mask = (std::uint32_t)SAClient::AccountFieldsFlags::first_name | (std::uint32_t)SAClient::AccountFieldsFlags::dob;
		req.password = "password1";
		std::cout << "calling CreateAccount: first_name = " << req.fields.first_name << ", password = " << req.password << ", dob = " << req.fields.dob << std::endl;
		::grpc::Status s = client.CreateAccount(req, rsp);
		std::cout << "status = " << s.error_code() << std::endl;
		if (s.ok())
		{
			std::cout << "CreateAccount returned: message = " << rsp.message << ", status = " << rsp.status << std::endl;
		}
	}
	{
		SAClient::updateAccountRequest req;
		SAClient::updateAccountResponse rsp;
		req.fields.first_name = "firstname1";
		req.fields.dob = 1234;
		req.fields.mask = (std::uint32_t)SAClient::AccountFieldsFlags::first_name | (std::uint32_t)SAClient::AccountFieldsFlags::dob;
		req.authkey = "authkey1";
		req.accountId = "accountId1";
		std::cout << "calling UpdateAccount: authkey = " << req.authkey << ", accountId = " << req.accountId << ", first_name = " << req.fields.first_name << ", dob = " << req.fields.dob << std::endl;
		::grpc::Status s = client.UpdateAccount(req, rsp);
		std::cout << "status = " << s.error_code() << std::endl;
		if (s.ok())
		{
			std::cout << "UpdateAccount returned: message = " << rsp.message << ", status = " << rsp.status << ", mask = " << rsp.fields.mask << std::endl;
		}
	}
	{
		SAClient::readAccountRequest req;
		SAClient::readAccountResponse rsp;
		req.authkey = "authkey1";
		req.accountId = "accountId1";
		std::cout << "calling ReadAccount: authkey = " << req.authkey << ", accountId = " << req.accountId << std::endl;
		std::uint32_t mask = 0;
		::grpc::Status s = client.ReadAccount(req, rsp);
		std::cout << "status = " << s.error_code() << std::endl;
		if (s.ok())
		{
			std::cout << "ReadAccount returned: message = " << rsp.message << ", status = " << rsp.status << ", mask = " << rsp.fields.mask << std::endl;
		}
	}
	{
		SAClient::listAccountsRequest req;
		SAClient::listAccountsResponse rsp;
		req.authkey = "authkey1";
		req.start = 1;
		req.end = 100;
		req.fields.mask = (std::uint32_t)SAClient::AccountFieldsFlags::first_name | (std::uint32_t)SAClient::AccountFieldsFlags::dob;
		std::cout << "calling ListAccounts: authkey = " << req.authkey << ", start = " << req.start << ", end = " << req.end << ", mask = " << req.fields.mask << std::endl;
		::grpc::Status s = client.ListAccounts(req, rsp);
		std::cout << "status = " << s.error_code() << std::endl;
		if (s.ok())
		{
			std::cout << "ListAccounts returned: message = " << rsp.message << ", status = " << rsp.status << ", fields.size() = " << rsp.fields.size() << std::endl;
		}
	}
	return 0;
}
