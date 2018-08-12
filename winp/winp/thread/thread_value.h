#pragma once

#include <string>

namespace winp::thread{
	class value{
	public:
		template <class target_type>
		struct is_basic_value{
			static constexpr bool value = (
				   std::is_same_v<target_type, bool>
				|| std::is_pointer_v<target_type>
				|| std::is_integral_v<target_type>
			);
		};

		virtual ~value() = default;

		virtual unsigned __int64 get_local() const = 0;

		template <typename target_type>
		target_type get() const{
			return get_<target_type>(std::bool_constant<is_basic_value<target_type>::value>());
		}

		template <typename target_type>
		std::remove_reference_t<target_type> &get_ref() const{
			return *reinterpret_cast<std::remove_reference_t<target_type> *>(get_local());
		}

	protected:
		template <typename target_type>
		target_type get_(std::true_type) const{
			return getb_<target_type>(std::bool_constant<std::is_same_v<target_type, bool>>());
		}

		template <typename target_type>
		target_type get_(std::false_type) const{
			return *reinterpret_cast<std::remove_reference_t<target_type> *>(get_local());
		}

		template <typename target_type>
		target_type getb_(std::true_type) const{
			return (get_local() != 0u);
		}

		template <typename target_type>
		target_type getb_(std::false_type) const{
			return getp_<target_type>(std::bool_constant<std::is_pointer_v<target_type>>());
		}

		template <typename target_type>
		target_type getp_(std::true_type) const{
			return reinterpret_cast<target_type>(get_local());
		}

		template <typename target_type>
		target_type getp_(std::false_type) const{
			return (target_type)get_local();
		}
	};

	template <class value_type>
	class typed_value : public value{
	public:
		using m_value_type = value_type;

		explicit typed_value(const m_value_type &target)
			: value_(target){}

		virtual ~typed_value() = default;

		virtual unsigned __int64 get_local() const override{
			return get_local_<m_value_type>(std::bool_constant<value::is_basic_value<m_value_type>::value>());
		}

	protected:
		template <typename dummy_type>
		unsigned __int64 get_local_(std::true_type) const{
			return (unsigned __int64)value_;
		}

		template <typename dummy_type>
		unsigned __int64 get_local_(std::false_type) const{
			return reinterpret_cast<unsigned __int64>(&value_);
		}

		m_value_type value_;
	};
}
