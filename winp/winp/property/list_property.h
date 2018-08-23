#pragma once

#include "scalar_property.h"
#include "map_property.h"

namespace winp::prop{
	struct list_action{
		static const unsigned int action_add				= (0 << 0x0000);
		static const unsigned int action_remove				= (1 << 0x0000);
		static const unsigned int action_remove_index		= (1 << 0x0001);
		static const unsigned int action_at					= (1 << 0x0002);
		static const unsigned int action_find				= (1 << 0x0003);
		static const unsigned int action_first				= (1 << 0x0004);
		static const unsigned int action_last				= (1 << 0x0005);
		static const unsigned int action_begin				= (1 << 0x0006);
		static const unsigned int action_end				= (1 << 0x0007);
		static const unsigned int action_size				= (1 << 0x0008);
	};

	template <class list_type, class in_owner_type, template <class, class> class value_holder_type>
	class list : public value_holder_type<list_type, in_owner_type>{
	public:
		using m_value_holder_type = value_holder_type<list_type, in_owner_type>;
		using m_base_type = value_holder_type<list_type, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using change_callback_type = typename m_value_holder_type::change_callback_type;
		using setter_type = typename m_value_holder_type::setter_type;
		using getter_type = typename m_value_holder_type::getter_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		using m_item_value_type = typename list_type::value_type;
		using m_iterator_type = typename list_type::iterator;
		using m_const_iterator_type = typename list_type::const_iterator;

		std::size_t operator +=(const m_item_value_type &target){
			if (!m_base_type::changed_(&target, list_action::action_add))
				return static_cast<std::size_t>(-1);

			m_base_type::m_value_.push_back(target);
			return (m_base_type::m_value_.size() - 1u);
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<std::is_pointer_v<typename dummy_type::value_type>, std::size_t> operator +=(std::remove_pointer_t<m_item_value_type> &target){
			return operator +=(&target);
		}

		bool operator -=(const m_item_value_type &target){
			if (!m_base_type::changed_(&target, list_action::action_remove))
				return false;

			auto it = std::find(m_base_type::m_value_.begin(), m_base_type::m_value_.end(), target);
			if (it == m_base_type::m_value_.end())
				return false;

			m_base_type::m_value_.erase(it);
			return true;
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<std::is_pointer_v<typename dummy_type::value_type>, bool> operator -=(std::remove_pointer_t<m_item_value_type> &target){
			return operator -=(&target);
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<!std::is_same_v<typename dummy_type::value_type, std::size_t>, bool> operator -=(std::size_t index){
			if (index >= m_base_type::m_value_.size() || !m_base_type::changed_(&index, list_action::action_remove_index))
				return false;

			m_base_type::m_value_.erase(std::next(m_base_type::m_value_.begin(), index));
			return true;
		}

		m_item_value_type *operator [](std::size_t index) const{
			return ((index < m_base_type::m_value_.size()) ? &*std::next(m_base_type::m_value_.begin(), index) : nullptr);
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<!std::is_same_v<typename dummy_type::value_type, std::size_t>, std::size_t> operator [](const m_item_value_type &target) const{
			auto it = std::find(m_base_type::m_value_.begin(), m_base_type::m_value_.end(), target);
			return ((it == m_base_type::m_value_.end()) ? static_cast<std::size_t>(-1) : std::distance(m_base_type::m_value_.begin(), it));
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<std::is_pointer_v<typename dummy_type::value_type> &&!std::is_same_v<m_item_value_type, std::size_t>, std::size_t> operator [](const std::remove_pointer_t<m_item_value_type> &target) const{
			return operator [](&target);
		}

		scalar<m_item_value_type *, list, proxy_value> first;
		scalar<m_item_value_type *, list, proxy_value> last;

		scalar<m_iterator_type, list, proxy_value> begin;
		scalar<m_iterator_type, list, proxy_value> end;

		scalar<std::size_t, list, proxy_value> size;

	protected:
		friend in_owner_type;

		virtual void init_(owner_type &owner, change_callback_type callback) override{
			m_base_type::init_(owner, callback);

			auto getter = [this](const prop::base<list> &prop, void *buf, std::size_t index){
				if (&prop == &last){
					auto prev_it = m_base_type::m_value_.begin(), it = prev_it, end_it = m_base_type::m_value_.end();
					for (; it != end_it; ){
						prev_it = it;
						++it;
					}

					*static_cast<m_item_value_type **>(buf) = ((prev_it == end_it) ? nullptr : &*prev_it);
				}
				else if (&prop == &first)
					*static_cast<m_item_value_type **>(buf) = (m_base_type::m_value_.empty() ? nullptr : &*m_base_type::m_value_.begin());
				else if (&prop == &begin)
					*static_cast<m_iterator_type *>(buf) = m_base_type::m_value_.begin();
				else if (&prop == &end)
					*static_cast<m_iterator_type *>(buf) = m_base_type::m_value_.end();
				else if (&prop == &size)
					*static_cast<std::size_t *>(buf) = m_base_type::m_value_.size();
			};

			first.init_(*this, nullptr, nullptr, getter);
			last.init_(*this, nullptr, nullptr, getter);
			begin.init_(*this, nullptr, nullptr, getter);
			end.init_(*this, nullptr, nullptr, getter);
			size.init_(*this, nullptr, nullptr, getter);
		}

		virtual void init_(owner_type &owner, change_callback_type callback, setter_type setter, getter_type getter) override{
			init_(owner, callback);
		}
	};

	template <class list_type, class in_owner_type>
	class list<list_type, in_owner_type, proxy_value> : public proxy_value<list_type, in_owner_type>{
	public:
		using m_value_holder_type = proxy_value<list_type, in_owner_type>;
		using m_base_type = proxy_value<list_type, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using change_callback_type = typename m_value_holder_type::change_callback_type;
		using setter_type = typename m_value_holder_type::setter_type;
		using getter_type = typename m_value_holder_type::getter_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		using m_item_value_type = typename list_type::value_type;
		using m_iterator_type = typename list_type::iterator;
		using m_const_iterator_type = typename list_type::const_iterator;

		std::size_t operator +=(m_item_value_type target){
			auto info = std::make_pair(static_cast<std::size_t>(-1), target);
			m_base_type::change_(&info, list_action::action_add);
			return info.first;
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<std::is_pointer_v<m_item_value_type>, std::size_t> operator +=(std::remove_pointer_t<m_item_value_type> &target){
			return operator +=(&target);
		}

		bool operator -=(m_item_value_type target){
			auto info = std::make_pair(false, target);
			m_base_type::change_(&info, list_action::action_remove);
			return info.first;
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<std::is_pointer_v<typename dummy_type::value_type>, list> operator -=(std::remove_pointer_t<m_item_value_type> &target){
			return operator -=(&target);
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<!std::is_same_v<typename dummy_type::value_type, std::size_t>, bool> operator -=(std::size_t index){
			auto info = std::make_pair(false, index);
			m_base_type::change_(&index, list_action::action_remove_index);
			return info.first;
		}

		std::conditional_t<std::is_pointer_v<m_item_value_type>, m_item_value_type, m_item_value_type *> operator [](std::size_t index) const{
			using return_type = std::conditional_t<std::is_pointer_v<m_item_value_type>, m_item_value_type, m_item_value_type *>;

			auto info = std::make_pair<std::size_t, return_type>(index, nullptr);
			m_base_type::change_(&info, list_action::action_at);

			return info.second;
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<!std::is_same_v<typename dummy_type::value_type, std::size_t>, std::size_t> operator [](const m_item_value_type &target) const{
			auto info = std::make_pair<std::size_t, m_item_value_type>(static_cast<std::size_t>(-1), target);
			m_base_type::change_(&info, list_action::action_find);
			return info.first;
		}

		template <typename dummy_type = list_type>
		std::enable_if_t<std::is_pointer_v<typename dummy_type::value_type> && !std::is_same_v<m_item_value_type, std::size_t>, std::size_t> operator [](std::remove_pointer_t<m_item_value_type> &target) const{
			return operator [](&target);
		}

		scalar<m_item_value_type *, list, proxy_value> first;
		scalar<m_item_value_type *, list, proxy_value> last;

		scalar<m_iterator_type, list, proxy_value> begin;
		scalar<m_iterator_type, list, proxy_value> end;

		scalar<std::size_t, list, proxy_value> size;

	protected:
		friend in_owner_type;

		virtual void init_(owner_type &owner, change_callback_type callback, setter_type setter, getter_type getter) override{
			m_base_type::init_(owner, callback, setter, getter);

			auto this_getter = [this](const prop::base<list> &prop, void *buf, std::size_t index){
				if (&prop == &first)
					m_base_type::change_(buf, list_action::action_first);
				else if (&prop == &last)
					m_base_type::change_(buf, list_action::action_last);
				else if (&prop == &begin)
					m_base_type::change_(buf, list_action::action_begin);
				else if (&prop == &end)
					m_base_type::change_(buf, list_action::action_end);
				else if (&prop == &size)
					m_base_type::change_(buf, list_action::action_size);
			};

			first.init_(*this, nullptr, nullptr, this_getter);
			last.init_(*this, nullptr, nullptr, this_getter);
			begin.init_(*this, nullptr, nullptr, this_getter);
			end.init_(*this, nullptr, nullptr, this_getter);
			size.init_(*this, nullptr, nullptr, this_getter);
		}
	};
}
