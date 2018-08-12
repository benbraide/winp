#pragma once

#include <list>
#include <mutex>
#include <memory>
#include <unordered_map>

#include "../utility/random_number.h"

#include "thread_value.h"
#include "thread_item.h"

namespace winp::thread{
	class object;

	class value_manager{
	public:
		struct managed_info_type{
			item *receiver;
			unsigned int msg;
		};

		using managed_info_list_type = std::list<managed_info_type>;
		using value_list_type = std::unordered_map<unsigned __int64, std::shared_ptr<value>>;

	private:
		friend class object;

		void add_(item *receiver, unsigned int msg){
			managed_info_list_.push_back(managed_info_type{ receiver, msg });
		}

		template <typename value_type>
		unsigned __int64 add_(const value_type &value){
			unsigned __int64 key = rand_;
			value_list_[key] = new typed_value<value_type>(value);
			return key;
		}

		managed_info_type pop_(){
			if (managed_info_list_.empty())
				return managed_info_type{};

			auto info = *managed_info_list_.begin();
			managed_info_list_.erase(managed_info_list_.begin());

			return info;
		}

		std::shared_ptr<value> pop_(unsigned __int64 key){
			if (value_list_.empty())
				return nullptr;

			auto info = value_list_.find(key);
			if (info == value_list_.end())
				return nullptr;

			auto value = info->second;
			value_list_.erase(info);

			return value;
		}

		managed_info_list_type managed_info_list_;
		value_list_type value_list_;
		utility::random_integral_number rand_;
	};
}
