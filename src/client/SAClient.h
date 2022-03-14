#pragma once

#include <string>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "services.accounts.grpc.pb.h"

#if 0
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using Services::Account;
#endif

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
		first_name = 1 << 0,
		surname = 1 << 1,
		email = 1 << 2,
		sms = 1 << 3,
		dob = 1 << 4,
		notes = 1 << 5,
		image = 1 << 6,
		active_state = 1 << 7,
		created = 1 << 8,
		updated = 1 << 9,
		account_type = 1 << 10,
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

	SAClient(std::shared_ptr<grpc::Channel> channel);
	grpc::Status Login(const loginRequest& req, loginResponse& rsp);
	grpc::Status ResetPassword(const passwordRequest& req, passwordResponse& rsp);
	grpc::Status CreateAccount(const createAccountRequest& req, createAccountResponse& rsp);
	grpc::Status UpdateAccount(const updateAccountRequest& req, updateAccountResponse& rsp);
	grpc::Status ReadAccount(const readAccountRequest& req, readAccountResponse& rsp);
	grpc::Status ListAccounts(const listAccountsRequest& req, listAccountsResponse& rsp);

private:
	void SetProtoFieldsFromFields(Services::AccountFields* f, const AccountFields& fields);
	void SetFieldsFromProtoFields(AccountFields& fields, const Services::AccountFields& f);
	void SetAccountFieldSelectFromMask(Services::AccountFieldSelect* f, std::int32_t mask);

	std::unique_ptr<Services::Account::Stub> stub_;
};

