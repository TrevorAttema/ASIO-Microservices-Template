#pragma once

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <memory>
#include <string>

#if 0
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#endif

#include "services.accounts.grpc.pb.h"

struct Options
{
	std::string port;
	std::string certPath;
	std::string keyPath;
	std::string servercert;
	std::string serverkey;
	int threadCount;
	int queueCount;
	int callDataCount;
	//std::unique_ptr<odb::database> db;
};

class authServer
{
private:
	std::unique_ptr<grpc::Server> server;
	std::vector<std::unique_ptr<grpc::ServerCompletionQueue>> m_cq;
	Services::Account::AsyncService service;
	Options m_options;

public:
	authServer(const Options& options);
	void run();
	void shutdown();

private:
	void HandleRPCs(int n);

	::grpc::Status Login(const ::Services::LoginRequest& request, ::Services::LoginResponse& response);
	::grpc::Status ResetPassword(const ::Services::PasswordRequest& request, ::Services::PasswordResponse& response);
	::grpc::Status CreateAccount(const ::Services::CreateAccountRequest& request, ::Services::CreateAccountResponse& response);
	::grpc::Status UpdateAccount(const ::Services::UpdateAccountRequest& request, ::Services::UpdateAccountResponse& response);
	::grpc::Status ReadAccount(const ::Services::ReadAccountRequest& request, ::Services::ReadAccountResponse& response);
	::grpc::Status ListAccounts(const ::Services::ListAccountRequests& request, ::Services::ListAccountResponse& response);
};

enum class CallStatus
{
	CREATE, PROCESS, FINISH
};

template<class serverType, class serviceType>
class CallDataBase
{
public:
	CallDataBase(serviceType* service, grpc::ServerCompletionQueue* queue, serverType *server)
		: _service( service)
		, _queue(queue)
		, _server(server)
		, _status(CallStatus::CREATE)
	{
	}

	virtual void Proceed() = 0;

protected:
	serviceType* _service;
	grpc::ServerCompletionQueue* _queue;
	grpc::ServerContext _context;
	serverType* _server;
	CallStatus _status;
};

template<class requestType, class responseType, class serverType, class serviceType, typename funcType1, typename funcType2>
class CallData : public CallDataBase<serverType, serviceType>
{
public:
	CallData(serviceType* service, grpc::ServerCompletionQueue* queue, serverType* server, funcType1 func1, funcType2 func2)
		: CallDataBase<serverType, serviceType>(service, queue, server)
		, _responder(&this->_context)
		, _func1(func1)
		, _func2(func2)
	{
	}
	void Proceed()
	{
		switch (this->_status)
		{
		case CallStatus::CREATE:
		{
			this->_status = CallStatus::PROCESS;
			(this->_service->*_func1)(&this->_context, &_request, &_responder, this->_queue, this->_queue, (void*)this);
			break;
		}
		case CallStatus::PROCESS:
		{
			auto cd = new CallData<requestType, responseType, serverType, serviceType, funcType1, funcType2>(this->_service, this->_queue, this->_server, _func1, _func2);
			cd->Proceed();
			auto status = (this->_server->*_func2)(_request, _response);
			this->_status = CallStatus::FINISH;
			_responder.Finish(_response, status, (void*)this);
			break;
		}
		default:
		{
			delete this;
		}
		}
	}
private:
	funcType1 _func1;
	funcType2 _func2;
	requestType _request;
	responseType _response;
	grpc::ServerAsyncResponseWriter<responseType> _responder;
};
