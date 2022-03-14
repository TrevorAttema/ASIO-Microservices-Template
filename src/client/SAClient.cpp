#include "SAClient.h"

SAClient::SAClient(std::shared_ptr<grpc::Channel> channel)
	: stub_(Services::Account::NewStub(channel))
{
}

grpc::Status SAClient::Login(const SAClient::loginRequest& req, SAClient::loginResponse& rsp)
{
	Services::LoginRequest request;
	Services::LoginResponse response;
	request.set_username(req.username);
	request.set_password(req.password);
	request.set_tfa_code(req.tfa_code);
	grpc::ClientContext context;
	grpc::Status status = stub_->Login(&context, request, &response);
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

grpc::Status SAClient::ResetPassword(const SAClient::passwordRequest& req, SAClient::passwordResponse& rsp)
{
	Services::PasswordRequest request;
	Services::PasswordResponse response;
	request.set_old_password(req.old_password);
	request.set_new_password(req.new_password);
	grpc::ClientContext context;
	grpc::Status status = stub_->ResetPassword(&context, request, &response);
	if (!status.ok())
	{
		return status;
	}
	rsp.status = response.status();
	rsp.message = response.message();
	return status;
}

grpc::Status SAClient::CreateAccount(const SAClient::createAccountRequest& req, SAClient::createAccountResponse& rsp)
{
	Services::CreateAccountRequest request;
	Services::CreateAccountResponse response;
	request.set_password(req.password);
	auto f = request.mutable_fields();
	SetProtoFieldsFromFields(f, req.fields);

	grpc::ClientContext context;
	grpc::Status status = stub_->CreateAccount(&context, request, &response);
	if (!status.ok())
	{
		return status;
	}
	rsp.status = response.status();
	rsp.message = response.message();
	return status;
}

void SAClient::SetProtoFieldsFromFields(::Services::AccountFields* f, const SAClient::AccountFields& fields)
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

grpc::Status SAClient::UpdateAccount(const SAClient::updateAccountRequest& req, SAClient::updateAccountResponse& rsp)
{
	Services::UpdateAccountRequest request;
	Services::UpdateAccountResponse response;
	request.set_accountid(req.accountId);
	request.set_authkey(req.authkey);
	auto f = request.mutable_fields();
	SetProtoFieldsFromFields(f, req.fields);

	grpc::ClientContext context;
	grpc::Status status = stub_->UpdateAccount(&context, request, &response);
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

void SAClient::SetFieldsFromProtoFields(SAClient::AccountFields& fields, const Services::AccountFields& f)
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

grpc::Status SAClient::ReadAccount(const SAClient::readAccountRequest& req, SAClient::readAccountResponse& rsp)
{
	Services::ReadAccountRequest request;
	Services::ReadAccountResponse response;
	request.set_accountid(req.accountId);
	request.set_authkey(req.authkey);
	grpc::ClientContext context;
	grpc::Status status = stub_->ReadAccount(&context, request, &response);
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

void SAClient::SetAccountFieldSelectFromMask(::Services::AccountFieldSelect* f, std::int32_t mask)
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

grpc::Status SAClient::ListAccounts(const SAClient::listAccountsRequest& req, SAClient::listAccountsResponse& rsp)
{
	Services::ListAccountRequests request;
	Services::ListAccountResponse response;
	request.set_authkey(req.authkey);
	request.set_start(req.start);
	request.set_end(req.end);
	SetAccountFieldSelectFromMask(request.mutable_fields(), req.fields.mask);
	grpc::ClientContext context;
	grpc::Status status = stub_->ListAccounts(&context, request, &response);
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

