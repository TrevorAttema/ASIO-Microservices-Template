#pragma once

//odb -d pgsql --std c++11 --generate-session --generate-schema model.accounts.hxx

#include <odb/core.hxx>

#include <vector>
#include <string>
#include <ctime>

#include <odb/tr1/memory.hxx>
#include <odb/tr1/lazy-ptr.hxx>

using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using odb::tr1::lazy_shared_ptr;
using odb::tr1::lazy_weak_ptr;

class account;
class session;

namespace model {
	namespace accounts{

		enum class AccountStatus { ACCOUNT_ACTIVE, ACCOUNT_UNVERIFIED, ACCOUNT_LOCKED_PASSWD };
		enum class EmailStatus { EMAIL_UNVERIFIED, EMAIL_VERIFIED };
		enum class SMSStatus { SMS_UNVERIFIED, SMS_VERIFIED };
		enum class EmailContact { CONTACT_APPROVED, CONTACT_REVOKED };

#pragma db value
		class date_fields {

		protected:
			friend class odb::access;

		public:
			std::time_t			created_;
#pragma db null
			std::time_t			updated_;
#pragma db null
			std::time_t			deleted_;
		};

#pragma db value
		class session {
		protected:
			friend class odb::access;

		public:
			std::string			session_;
			std::time_t			sessionTime_;
		};

#pragma db object
		class account {
		public:
			account(
				std::string first_name,
				std::string surname,
				std::string password
			);


		public:
			const std::string& email() const { return email_; }

		protected:
			friend class odb::access;

			account() {}

#pragma db id auto
			unsigned long id_;

			std::string			email_;
			std::time_t			email_verified_;
			std::string			sms_;
			SMSStatus			sms_status_;

			std::string			first_name_;
			std::string			surname_;
			std::time_t			dob_;
			std::string			notes_;
			std::string			password_;

			AccountStatus		status_;
			EmailStatus			email_status_;
			EmailContact		email_contact_status_;

			session				session_;
			date_fields			dates_;
		};

	}
}