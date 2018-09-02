#pragma once

#include <string>

#include "../utility/convert_param.h"

#include "property_base.h"

namespace winp::prop{
	template <class value_type, class in_owner_type = void, template <class, class> class value_holder_type = immediate_value>
	class scalar : public value_holder_type<value_type, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<value_type, in_owner_type>;
		using m_base_type = value_holder_type<value_type, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;
		using m_value_type = typename m_value_holder_type::m_value_type;

		scalar() = default;

		template <typename target_type>
		operator target_type() const{
			return utility::convert_param<m_value_type, target_type>::convert(m_value_holder_type::typed_get_value_());
		}

		m_value_type operator()() const{
			return operator m_value_type();
		}

		template <typename target_type>
		scalar &operator =(target_type target){
			m_value_holder_type::typed_change_(utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator +=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() + utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator -=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() - utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator *=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() * utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator /=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() / utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator %=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() % utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator &=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() & utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator |=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() | utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator ^=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() ^ utility::convert_param<target_type, m_value_type>::convert(target));
			return *this;
		}

		template <typename target_type>
		m_value_type operator +(target_type target) const{
			return (m_value_holder_type::typed_get_value_() + utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator -(target_type target) const{
			return (m_value_holder_type::typed_get_value_() - utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator *(target_type target) const{
			return (m_value_holder_type::typed_get_value_() * utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator /(target_type target) const{
			return (m_value_holder_type::typed_get_value_() / utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator %(target_type target) const{
			return (m_value_holder_type::typed_get_value_() % utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator &(target_type target) const{
			return (m_value_holder_type::typed_get_value_() & utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator |(target_type target) const{
			return (m_value_holder_type::typed_get_value_() | utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		m_value_type operator ^(target_type target) const{
			return (m_value_holder_type::typed_get_value_() ^ utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		bool operator <(target_type target) const{
			return (m_value_holder_type::typed_get_value_() < utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		bool operator <=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() <= utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		bool operator ==(target_type target) const{
			return (m_value_holder_type::typed_get_value_() == utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		bool operator !=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() != utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		bool operator >=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() >= utility::convert_param<target_type, m_value_type>::convert(target));
		}

		template <typename target_type>
		bool operator >(target_type target) const{
			return (m_value_holder_type::typed_get_value_() > utility::convert_param<target_type, m_value_type>::convert(target));
		}

		scalar &operator ++(){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() + static_cast<m_value_type>(1));
			return *this;
		}

		m_value_type operator ++(int){
			auto value = m_value_holder_type::typed_get_value_();
			m_value_holder_type::typed_change_(value + static_cast<m_value_type>(1));
			return value;
		}

		scalar &operator --(){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() - static_cast<m_value_type>(1));
			return *this;
		}

		m_value_type operator --(int){
			auto value = m_value_holder_type::typed_get_value_();
			m_value_holder_type::typed_change_(value - static_cast<m_value_type>(1));
			return value;
		}

		m_value_type operator +() const{
			return +m_value_holder_type::typed_get_value_();
		}

		m_value_type operator -() const{
			return -m_value_holder_type::typed_get_value_();
		}

		m_value_type operator ~() const{
			return ~m_value_holder_type::typed_get_value_();
		}

		auto operator !() const{
			return !m_value_holder_type::typed_get_value_();
		}
	};

	template <class in_owner_type, template <class, class> class value_holder_type>
	class scalar<std::string, in_owner_type, value_holder_type> : public value_holder_type<std::string, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<std::string, in_owner_type>;
		using m_base_type = value_holder_type<std::string, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;
		using m_value_type = typename m_value_holder_type::m_value_type;

		scalar() = default;

		operator m_value_type() const{
			return m_value_holder_type::typed_get_value_();
		}

		m_value_type operator()() const{
			return operator m_value_type();
		}

		template <typename target_type>
		scalar &operator =(target_type target){
			m_value_holder_type::typed_change_(static_cast<const m_value_type &>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator +=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() + static_cast<const m_value_type &>(target));
			return *this;
		}

		template <typename target_type>
		m_value_type operator +(target_type target) const{
			return (m_value_holder_type::typed_get_value_() + static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator <(target_type target) const{
			return (m_value_holder_type::typed_get_value_() < static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator <=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() <= static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator ==(target_type target) const{
			return (m_value_holder_type::typed_get_value_() == static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator !=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() != static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator >=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() >= static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator >(target_type target) const{
			return (m_value_holder_type::typed_get_value_() > static_cast<const m_value_type &>(target));
		}
	};

	template <class in_owner_type, template <class, class> class value_holder_type>
	class scalar<std::wstring, in_owner_type, value_holder_type> : public value_holder_type<std::wstring, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<std::wstring, in_owner_type>;
		using m_base_type = value_holder_type<std::wstring, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;
		using m_value_type = typename m_value_holder_type::m_value_type;

		scalar() = default;

		operator m_value_type() const{
			return m_value_holder_type::typed_get_value_();
		}

		m_value_type operator()() const{
			return operator m_value_type();
		}

		template <typename target_type>
		scalar &operator =(target_type target){
			m_value_holder_type::typed_change_(static_cast<const m_value_type &>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator +=(target_type target){
			m_value_holder_type::typed_change_(m_value_holder_type::typed_get_value_() + static_cast<const m_value_type &>(target));
			return *this;
		}

		template <typename target_type>
		m_value_type operator +(target_type target) const{
			return (m_value_holder_type::typed_get_value_() + static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator <(target_type target) const{
			return (m_value_holder_type::typed_get_value_() < static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator <=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() <= static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator ==(target_type target) const{
			return (m_value_holder_type::typed_get_value_() == static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator !=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() != static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator >=(target_type target) const{
			return (m_value_holder_type::typed_get_value_() >= static_cast<const m_value_type &>(target));
		}

		template <typename target_type>
		bool operator >(target_type target) const{
			return (m_value_holder_type::typed_get_value_() > static_cast<const m_value_type &>(target));
		}
	};

	template <class value_type, class in_owner_type, template <class, class> class value_holder_type>
	class scalar<value_type *, in_owner_type, value_holder_type> : public value_holder_type<value_type *, in_owner_type>{
		public:
			using m_value_holder_type = value_holder_type<value_type *, in_owner_type>;
			using m_base_type = value_holder_type<value_type *, in_owner_type>;

			using base_type = typename m_value_holder_type::base_type;
			using owner_type = typename m_value_holder_type::owner_type;
			using m_value_type = typename m_value_holder_type::m_value_type;

			scalar() = default;

			template <typename target_type>
			operator target_type() const{
				return utility::convert_param<m_value_type, target_type>::convert(m_value_holder_type::typed_get_value_());
			}

			m_value_type operator()() const{
				return operator m_value_type();
			}

			operator std::remove_pointer_t<m_value_type> &() const{
				return *operator m_value_type();
			}

			std::remove_pointer_t<m_value_type> &operator *() const{
				return *operator m_value_type();
			}

			template <typename target_type>
			scalar &operator =(target_type target){
				m_value_holder_type::typed_change_(utility::convert_param<target_type, m_value_type>::convert(target));
				return *this;
			}

			scalar &operator =(std::remove_pointer_t<m_value_type> &target){
				m_value_holder_type::typed_change_(&target);
				return *this;
			}

			template <typename target_type>
			bool operator <(target_type target) const{
				return (m_value_holder_type::typed_get_value_() < utility::convert_param<target_type, m_value_type>::convert(target));
			}

			template <typename target_type>
			bool operator <=(target_type target) const{
				return (m_value_holder_type::typed_get_value_() <= utility::convert_param<target_type, m_value_type>::convert(target));
			}

			template <typename target_type>
			bool operator ==(target_type target) const{
				return (m_value_holder_type::typed_get_value_() == utility::convert_param<target_type, m_value_type>::convert(target));
			}

			template <typename target_type>
			bool operator !=(target_type target) const{
				return (m_value_holder_type::typed_get_value_() != utility::convert_param<target_type, m_value_type>::convert(target));
			}

			template <typename target_type>
			bool operator >=(target_type target) const{
				return (m_value_holder_type::typed_get_value_() >= utility::convert_param<target_type, m_value_type>::convert(target));
			}

			template <typename target_type>
			bool operator >(target_type target) const{
				return (m_value_holder_type::typed_get_value_() > utility::convert_param<target_type, m_value_type>::convert(target));
			}

			auto operator !() const{
				return !m_value_holder_type::typed_get_value_();
			}

			m_value_type operator ->() const{
				return m_value_holder_type::typed_get_value_();
			}
		};
}
