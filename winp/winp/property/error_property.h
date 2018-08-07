#pragma once

#include <string>
#include <variant>

#include <comdef.h>

#include "../utility/windows.h"

#include "property_base.h"

namespace winp::prop{
	enum class error_throw_policy_type{
		disabled,
		disabled_once,
		enabled_once,
		enabled,
		restore,
	};

	enum class error_throw_policy_frequency_type{
		always,
		once,
	};

	enum class error_held_type{
		nil,
		value,
		local,
		status,
	};

	template <class in_owner_type, class error_mapper = default_error_mapper, class value_type = typename error_mapper::value_type>
	class error : public proxy_value<value_type, in_owner_type>{
	public:
		using m_error_mapper = error_mapper;
		using m_value_holder_type = proxy_value<value_type, in_owner_type>;

		using m_base_type = proxy_value<value_type, in_owner_type>;
		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		using m_variant_type = std::variant<base_value_type, DWORD, HRESULT>;

		error() = default;

		operator error_throw_policy_type() const{
			return policy_;
		}

		operator error_throw_policy_frequency_type() const{
			return ((policy_ == error_throw_policy_type::disabled_once || policy_ == error_throw_policy_type::enabled_once) ?
				error_throw_policy_frequency_type::once : error_throw_policy_frequency_type::always);
		}

		operator error_held_type() const{
			if (std::holds_alternative<base_value_type>(value_))
				return ((std::get<base_value_type>(value_) == static_cast<base_value_type>(0)) ? error_held_type::nil : error_held_type::value);

			if (std::holds_alternative<DWORD>(value_))
				return ((std::get<DWORD>(value_) == 0u) ? error_held_type::nil : error_held_type::local);

			if (std::holds_alternative<HRESULT>(value_))
				return ((std::get<HRESULT>(value_) == S_OK) ? error_held_type::nil : error_held_type::status);

			return error_held_type::nil;
		}

		operator base_value_type() const{
			return get_value_();
		}

		operator DWORD() const{
			return get_local_value_();
		}

		operator short() const{
			return static_cast<short>(operator DWORD());
		}

		operator unsigned short() const{
			return static_cast<unsigned short>(operator DWORD());
		}

		operator int() const{
			return static_cast<int>(operator DWORD());
		}

		operator unsigned int() const{
			return static_cast<unsigned int>(operator DWORD());
		}

		operator long long() const{
			return static_cast<long long>(operator DWORD());
		}

		operator unsigned long long() const{
			return static_cast<unsigned long long>(operator DWORD());
		}

		operator HRESULT() const{
			return get_status_value_();
		}

		operator const std::wstring &() const{
			return convert_value_();
		}

		operator bool() const{
			return (get_value_() != static_cast<base_value_type>(0));
		}

		error &operator =(const error &target){
			if (std::holds_alternative<base_value_type>(target.value_))
				assign_(std::get<base_value_type>(target.value_));
			else if (std::holds_alternative<DWORD>(target.value_))
				assign_(std::get<DWORD>(target.value_));
			else if (std::holds_alternative<HRESULT>(target.value_))
				assign_(std::get<HRESULT>(target.value_));
			else
				assign_(static_cast<base_value_type>(0));

			return *this;
		}

		error &operator =(error_throw_policy_type policy){
			switch (policy){
			case error_throw_policy_type::disabled_once:
			case error_throw_policy_type::enabled_once:
				if (policy_ != error_throw_policy_type::disabled_once && policy_ != error_throw_policy_type::enabled_once)
					previous_policy_ = policy_;
				policy_ = policy;
				break;
			case error_throw_policy_type::restore:
				if (policy_ == error_throw_policy_type::disabled_once || policy_ == error_throw_policy_type::enabled_once)
					policy_ = previous_policy_;
				break;
			default:
				policy_ = policy;
				break;
			}

			return *this;
		}

		error &operator =(error_throw_policy_frequency_type freq){
			switch (freq){
			case error_throw_policy_frequency_type::once:
				if (policy_ == error_throw_policy_type::disabled){
					previous_policy_ = policy_;
					policy_ = error_throw_policy_type::disabled_once;
				}
				else if (policy_ == error_throw_policy_type::enabled){
					previous_policy_ = policy_;
					policy_ = error_throw_policy_type::enabled_once;
				}
				break;
			default:
				if (policy_ == error_throw_policy_type::disabled_once)
					policy_ = error_throw_policy_type::disabled;
				else if (policy_ == error_throw_policy_type::enabled_once)
					policy_ = error_throw_policy_type::enabled;
				break;
			}

			return *this;
		}

		error &operator =(base_value_type value){
			assign_(value);
			return *this;
		}

		error &operator =(DWORD value){
			assign_(value);
			return *this;
		}

		error &operator =(short value){
			return operator =(static_cast<DWORD>(value));
		}

		error &operator =(unsigned short value){
			return operator =(static_cast<DWORD>(value));
		}

		error &operator =(int value){
			return operator =(static_cast<DWORD>(value));
		}

		error &operator =(unsigned int value){
			return operator =(static_cast<DWORD>(value));
		}

		error &operator =(long long value){
			return operator =(static_cast<DWORD>(value));
		}

		error &operator =(unsigned long long value){
			return operator =(static_cast<DWORD>(value));
		}

		error &operator =(HRESULT value){
			assign_(value);
			return *this;
		}

	protected:
		friend in_owner_type;

		error(const error &target)
			: policy_(target.policy_), previous_policy_(target.previous_policy_), value_(target.value_), converted_(target.converted_){}

		error(error &&) = default;

		error &operator =(error &&) = default;

		virtual void change_(const void *value, std::size_t index) override{
			switch (index){
			case 1u:
				assign_(*static_cast<const DWORD *>(value));
				break;
			case 2u:
				assign_(*static_cast<const HRESULT *>(value));
				break;
			default:
				assign_(*static_cast<const base_value_type *>(value));
				break;
			}
		}

		base_value_type get_value_() const{
			if (std::holds_alternative<base_value_type>(value_))
				return std::get<base_value_type>(value_);

			if (std::holds_alternative<DWORD>(value_))
				return static_cast<base_value_type>((std::get<DWORD>(value_) == 0u) ? 0 : 1);

			if (std::holds_alternative<HRESULT>(value_))
				return static_cast<base_value_type>((std::get<HRESULT>(value_) == S_OK) ? 0 : 2);

			return static_cast<base_value_type>(0);
		}

		DWORD get_local_value_() const{
			if (std::holds_alternative<DWORD>(value_))
				return std::get<DWORD>(value_);

			if (std::holds_alternative<HRESULT>(value_))
				return HRESULT_CODE(std::get<HRESULT>(value_));

			return 0u;
		}

		HRESULT get_status_value_() const{
			if (std::holds_alternative<HRESULT>(value_))
				return std::get<HRESULT>(value_);

			if (std::holds_alternative<DWORD>(value_))
				return HRESULT_FROM_WIN32(std::get<DWORD>(value_));

			return S_OK;
		}

		const std::wstring &convert_value_() const{
			if (!converted_.empty())
				return converted_;

			switch (operator error_held_type()){
			case error_held_type::value:
				return get_converted_value_();
			case error_held_type::local:
				return get_converted_local_value_();
			case error_held_type::status:
				return get_converted_status_value_();
			default:
				break;
			}

			return (converted_ = L"Success");
		}

		const std::wstring &get_converted_value_() const{
			if (m_base_type::getter_ != nullptr)
				m_base_type::getter_(*this, &converted_, static_cast<int>(std::get<base_value_type>(value_)));
			return (converted_.empty() ? (converted_ = m_error_mapper::map(std::get<base_value_type>(value_))) : converted_);
		}

		const std::wstring &get_converted_local_value_() const{
			void *allocated_buffer = nullptr;
			auto count = FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				std::get<DWORD>(value_),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<wchar_t *>(&allocated_buffer),
				0u,
				nullptr
			);

			if (allocated_buffer != nullptr){
				if (count > 0u)
					converted_.assign(reinterpret_cast<wchar_t *>(allocated_buffer), count);
				LocalFree(allocated_buffer);
			}

			return converted_;
		}

		const std::wstring &get_converted_status_value_() const{
			_com_error err(std::get<HRESULT>(value_));
			return converted_.assign(err.ErrorMessage());
		}

		template <typename target_type>
		void assign_(target_type target, bool do_throw = true){
			value_ = target;
			converted_.clear();

			if (do_throw && get_value_() != static_cast<base_value_type>(0)){
				auto should_throw = (policy_ == error_throw_policy_type::enabled || policy_ == error_throw_policy_type::enabled_once);
				if (policy_ == error_throw_policy_type::disabled_once || policy_ == error_throw_policy_type::enabled_once)
					policy_ = previous_policy_;

				if (should_throw)
					throw *this;
			}
		}

		error_throw_policy_type policy_ = error_throw_policy_type::enabled;
		error_throw_policy_type previous_policy_ = error_throw_policy_type::enabled;
		m_variant_type value_;
		mutable std::wstring converted_;
	};
}
