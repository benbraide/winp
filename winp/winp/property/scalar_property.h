#pragma once

#include <string>

#include "property_base.h"

namespace winp::prop{
	template <class value_type, class in_owner_type = void, template <class, class> class value_holder_type = immediate_value>
	class scalar : public value_holder_type<value_type, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<value_type, in_owner_type>;
		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		scalar() = default;

		template <typename target_type>
		explicit scalar(const target_type &target)
			: m_value_holder_type(static_cast<const_ref_value_type>(target)){}

		operator const_ref_value_type() const{
			return m_value_holder_type::value_();
		}

		template <typename target_type>
		scalar &operator =(const target_type &target){
			m_value_holder_type::change_(static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator +=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() + static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator -=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() - static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator *=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() * static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator /=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() / static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator %=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() % static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator &=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() & static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator |=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() | static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator ^=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() ^ static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		base_value_type operator +(const target_type &target) const{
			return (m_value_holder_type::value_() + static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator -(const target_type &target) const{
			return (m_value_holder_type::value_() - static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator *(const target_type &target) const{
			return (m_value_holder_type::value_() * static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator /(const target_type &target) const{
			return (m_value_holder_type::value_() / static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator %(const target_type &target) const{
			return (m_value_holder_type::value_() % static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator &(const target_type &target) const{
			return (m_value_holder_type::value_() & static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator |(const target_type &target) const{
			return (m_value_holder_type::value_() | static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		base_value_type operator ^(const target_type &target) const{
			return (m_value_holder_type::value_() ^ static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator <(const target_type &target) const{
			return (m_value_holder_type::value_() < static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator <=(const target_type &target) const{
			return (m_value_holder_type::value_() <= static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator ==(const target_type &target) const{
			return (m_value_holder_type::value_() == static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator !=(const target_type &target) const{
			return (m_value_holder_type::value_() != static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator >=(const target_type &target) const{
			return (m_value_holder_type::value_() >= static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator >(const target_type &target) const{
			return (m_value_holder_type::value_() > static_cast<const_ref_value_type>(target));
		}

		scalar &operator ++(){
			m_value_holder_type::change_(m_value_holder_type::value_() + static_cast<base_value_type>(1));
			return *this;
		}

		base_value_type operator ++(int){
			auto value = m_value_holder_type::value_();
			m_value_holder_type::change_(value + static_cast<base_value_type>(1));
			return value;
		}

		scalar &operator --(){
			m_value_holder_type::change_(m_value_holder_type::value_() - static_cast<base_value_type>(1));
			return *this;
		}

		base_value_type operator --(int){
			auto value = m_value_holder_type::value_();
			m_value_holder_type::change_(value - static_cast<base_value_type>(1));
			return value;
		}

		base_value_type operator +() const{
			return +m_value_holder_type::value_();
		}

		base_value_type operator -() const{
			return -m_value_holder_type::value_();
		}

		base_value_type operator ~() const{
			return ~m_value_holder_type::value_();
		}

		auto operator !() const{
			return !m_value_holder_type::value_();
		}
	};

	template <class in_owner_type, template <class, class> class value_holder_type>
	class scalar<std::string, in_owner_type, value_holder_type> : public value_holder_type<std::string, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<std::string, in_owner_type>;
		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		scalar() = default;

		template <typename target_type>
		explicit scalar(const target_type &target)
			: m_value_holder_type(static_cast<const_ref_value_type>(target)){}

		operator const_ref_value_type() const{
			return m_value_holder_type::value_();
		}

		template <typename target_type>
		scalar &operator =(const target_type &target){
			m_value_holder_type::change_(static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator +=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() + static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		base_value_type operator +(const target_type &target) const{
			return (m_value_holder_type::value_() + static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator <(const target_type &target) const{
			return (m_value_holder_type::value_() < static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator <=(const target_type &target) const{
			return (m_value_holder_type::value_() <= static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator ==(const target_type &target) const{
			return (m_value_holder_type::value_() == static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator !=(const target_type &target) const{
			return (m_value_holder_type::value_() != static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator >=(const target_type &target) const{
			return (m_value_holder_type::value_() >= static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator >(const target_type &target) const{
			return (m_value_holder_type::value_() > static_cast<const_ref_value_type>(target));
		}

		const base_value_type *operator ->() const{
			return &m_value_holder_type::value_();
		}
	};

	template <class in_owner_type, template <class, class> class value_holder_type>
	class scalar<std::wstring, in_owner_type, value_holder_type> : public value_holder_type<std::wstring, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<std::wstring, in_owner_type>;
		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		scalar() = default;

		template <typename target_type>
		explicit scalar(const target_type &target)
			: m_value_holder_type(static_cast<const_ref_value_type>(target)){}

		operator const_ref_value_type() const{
			return m_value_holder_type::value_();
		}

		template <typename target_type>
		scalar &operator =(const target_type &target){
			m_value_holder_type::change_(static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		scalar &operator +=(const target_type &target){
			m_value_holder_type::change_(m_value_holder_type::value_() + static_cast<const_ref_value_type>(target));
			return *this;
		}

		template <typename target_type>
		base_value_type operator +(const target_type &target) const{
			return (m_value_holder_type::value_() + static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator <(const target_type &target) const{
			return (m_value_holder_type::value_() < static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator <=(const target_type &target) const{
			return (m_value_holder_type::value_() <= static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator ==(const target_type &target) const{
			return (m_value_holder_type::value_() == static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator !=(const target_type &target) const{
			return (m_value_holder_type::value_() != static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator >=(const target_type &target) const{
			return (m_value_holder_type::value_() >= static_cast<const_ref_value_type>(target));
		}

		template <typename target_type>
		bool operator >(const target_type &target) const{
			return (m_value_holder_type::value_() > static_cast<const_ref_value_type>(target));
		}

		const base_value_type *operator ->() const{
			return &m_value_holder_type::value_();
		}
	};

	template <class value_type, class in_owner_type, template <class, class> class value_holder_type>
	class scalar<value_type *, in_owner_type, value_holder_type> : public value_holder_type<value_type *, in_owner_type>{
		public:
			using m_value_holder_type = value_holder_type<value_type *, in_owner_type>;
			using base_type = typename m_value_holder_type::base_type;
			using owner_type = typename m_value_holder_type::owner_type;

			using m_value_type = typename m_value_holder_type::m_value_type;
			using base_value_type = typename m_value_holder_type::base_value_type;
			using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
			using ref_value_type = typename m_value_holder_type::ref_value_type;
			using ptr_value_type = typename m_value_holder_type::ptr_value_type;
			using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

			scalar()
				: m_value_holder_type(nullptr){}

			template <typename target_type>
			explicit scalar(const target_type &target)
				: m_value_holder_type(static_cast<m_value_type>(target)){}

			operator ptr_value_type() const{
				return m_value_holder_type::value_();
			}

			std::remove_pointer_t<base_value_type> &operator *() const{
				return *m_value_holder_type::value_();
			}

			template <typename target_type>
			scalar &operator =(const target_type &target){
				m_value_holder_type::change_(static_cast<base_value_type>(target));
				return *this;
			}

			template <typename target_type>
			bool operator <(const target_type &target) const{
				return (m_value_holder_type::value_() < static_cast<const_ptr_value_type>(target));
			}

			template <typename target_type>
			bool operator <=(const target_type &target) const{
				return (m_value_holder_type::value_() <= static_cast<const_ptr_value_type>(target));
			}

			template <typename target_type>
			bool operator ==(const target_type &target) const{
				return (m_value_holder_type::value_() == static_cast<const_ptr_value_type>(target));
			}

			template <typename target_type>
			bool operator !=(const target_type &target) const{
				return (m_value_holder_type::value_() != static_cast<const_ptr_value_type>(target));
			}

			template <typename target_type>
			bool operator >=(const target_type &target) const{
				return (m_value_holder_type::value_() >= static_cast<const_ptr_value_type>(target));
			}

			template <typename target_type>
			bool operator >(const target_type &target) const{
				return (m_value_holder_type::value_() > static_cast<const_ptr_value_type>(target));
			}

			auto operator !() const{
				return !m_value_holder_type::value_();
			}

			const_ptr_value_type operator ->() const{
				return m_value_holder_type::value_();
			}
		};
}
