#pragma once

#include <functional>

namespace winp::utility{
	template <class in_value_type, class owner>
	class dynamic_list;

	template <class in_value_type>
	class dynamic_iterator{
	public:
		using m_value_type = in_value_type;
		using callback_type = std::function<m_value_type *(m_value_type *)>;

		using iterator_category = std::forward_iterator_tag;
		using value_type = in_value_type;
		using difference_type = std::ptrdiff_t;
		using pointer = in_value_type *;
		using reference = in_value_type &;

		dynamic_iterator()
			: value_(){}

		dynamic_iterator(value_type *value, const callback_type &callback)
			: value_(value), callback_(callback){}

		auto &operator *() const{
			return *value_;
		}

		auto &operator *(){
			return *value_;
		}

		value_type operator ->() const{
			return value_;
		}

		dynamic_iterator &operator ++(){
			value_ = callback_(value_);
			return *this;
		}

		dynamic_iterator operator ++(int){
			dynamic_iterator copy(*this);
			operator ++();
			return copy;
		}

		bool operator ==(const dynamic_iterator &target) const{
			return (value_ == target.value_);
		}

		bool operator !=(const dynamic_iterator &target) const{
			return !(*this == target);
		}

	private:
		template <class, class> friend class dynamic_list;

		value_type *value_;
		callback_type callback_;
	};

	template <class in_value_type, class owner>
	class dynamic_list{
	public:
		using m_owner = owner;
		using value_type = in_value_type;
		using callback_type = std::function<value_type *()>;

		using iterator = dynamic_iterator<value_type>;
		using const_iterator = dynamic_iterator<const value_type>;

		using next_callback_type = typename iterator::callback_type;

		dynamic_list(){}

		dynamic_list(const callback_type &begin, const next_callback_type &next, const callback_type &end = nullptr)
			: begin_(begin), end_(end), next_(next){}

		iterator begin() const{
			return iterator(begin_(), next_);
		}

		iterator end() const{
			return iterator(((end_ == nullptr) ? nullptr : end_()), nullptr);
		}

		std::size_t size() const{
			std::size_t value = 0;
			for (auto it = begin(), eit = end(); it != eit; ++it)
				++value;

			return value;
		}

		bool empty() const{
			return (begin() == end());
		}

	private:
		friend owner;

		void init_(const callback_type &begin, const next_callback_type &next, const callback_type &end = nullptr){
			begin_ = begin;
			end_ = end;
			next_ = next;
		}

		callback_type begin_;
		callback_type end_;
		next_callback_type next_;
	};
}
