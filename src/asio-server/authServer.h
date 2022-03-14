#pragma once

namespace std {
	template <typename T>
	class auto_ptr;
}

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <agrpc/asioGrpc.hpp>

#include <memory>
#include <string>

#include <odb/database.hxx>
#include <odb/mysql/database.hxx>

#include "services.accounts.grpc.pb.h"

struct Options
{
	std::string port;
	std::string certPath;
	std::string keyPath;
	std::string servercert;
	std::string serverkey;
	std::unique_ptr<odb::database> db;
};

class authServer
{
private:
	std::unique_ptr<grpc::Server> server;
	grpc::ServerBuilder builder;
	agrpc::GrpcContext grpc_context;
	Services::Account::AsyncService service;
public:
	authServer(const Options& options);
	void run();
	grpc::Server& getServer()
	{
		return *server;
	}
	agrpc::GrpcContext& getContext()
	{
		return grpc_context;
	}
	// gRPC methods
	static ::grpc::Status _Login(const ::Services::LoginRequest& request, ::Services::LoginResponse& response, authServer* ptr);
	static ::grpc::Status _ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response, authServer* ptr);
	static ::grpc::Status _CreateAccount(const ::Services::CreateAccountRequest& request, ::Services::CreateAccountResponse& response, authServer* ptr);
	static ::grpc::Status _UpdateAccount(const ::Services::UpdateAccountRequest& request, ::Services::UpdateAccountResponse& response, authServer* ptr);
	static ::grpc::Status _ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response, authServer* ptr);
	static ::grpc::Status _ListAccounts(const ::Services::ListAccountRequests& request, ::Services::ListAccountResponse& response, authServer* ptr);
	::grpc::Status Login(const ::Services::LoginRequest& request, ::Services::LoginResponse& response);
	::grpc::Status ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response);
	::grpc::Status CreateAccount(const ::Services::CreateAccountRequest& request, ::Services::CreateAccountResponse& response);
	::grpc::Status UpdateAccount(const ::Services::UpdateAccountRequest& request, ::Services::UpdateAccountResponse& response);
	::grpc::Status ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response);
	::grpc::Status ListAccounts(const ::Services::ListAccountRequests& request, ::Services::ListAccountResponse& response);
};
