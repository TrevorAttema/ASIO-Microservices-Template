#include <chrono>

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "TestRunner.h"
#include "SAClient.h"

TestRunner::TestRunner(Options options)
	:m_options(options)
{
}

std::mutex m;
void TestRunner::Run()
{
	if (m_options.numThreads == 0)
	{
		RunTests();
		return;
	}
	m_running = true;
	for (int j = 0; j < m_options.numThreads; j++)
	{
		std::thread t([=]
			{
				{std::lock_guard<std::mutex> guard(m); std::cout << "TestRunner::Run: thread " << std::this_thread::get_id() << " started" << "\n"; }
				while (m_running)
				{
					{std::lock_guard<std::mutex> guard(m); std::cout << "TestRunner::Run: thread " << std::this_thread::get_id() << "; calling RunTests" << "\n"; }
					RunTests();
					{std::lock_guard<std::mutex> guard(m); std::cout << "TestRunner::Run: thread " << std::this_thread::get_id() << "; after calling RunTests" << "\n"; }
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1s);
				}
				{std::lock_guard<std::mutex> guard(m); std::cout << "TestRunner::Run: thread " << std::this_thread::get_id() << "; exiting" << "\n"; }
			});
		m_threads.push_back(std::move(t));
	}
}

void TestRunner::Shutdown()
{
	std::cout << "TestRunner::Shutdown" << "\n";
	m_running = false;
}

void TestRunner::Wait()
{
	std::cout << "TestRunner::Wait" << "\n";
	for (auto& t : m_threads)
	{
		std::cout << "TestRunner::Wait: joining thread " << t.get_id() << "\n";
		t.join();
	}
}

void TestRunner::RunTests()
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
	grpc::experimental::TlsChannelCredentialsOptions credOptions;
	credOptions.set_certificate_provider(certificate_provider);
	credOptions.watch_root_certs();
	credOptions.set_root_cert_name("dummy");
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
	credOptions.set_server_verification_option(GRPC_TLS_SKIP_ALL_SERVER_VERIFICATION);
	credOptions.set_server_authorization_check_config(noop_auth_check_);
	////////////////////////////////////
	auto channel_creds = grpc::experimental::TlsCredentials(credOptions);
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
#if 0
#endif
}
