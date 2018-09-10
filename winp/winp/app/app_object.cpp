#include "app_object.h"

void winp::app::object::init(){
	auto setter = [](const prop::base &prop, const void *value, std::size_t index){
		if (is_shut_down_)
			return;

		auto nc_value = const_cast<void *>(value);
		if (&prop == &threads){
			switch (index){
			case prop::list_action::action_add:
			{
				auto info = static_cast<std::pair<std::size_t, m_thread_type *> *>(nc_value);
				if (info->second != nullptr)
					info->first = add_thread_(*info->second);
				break;
			}
			case prop::list_action::action_remove:
			{
				auto info = static_cast<std::pair<bool, m_thread_type *> *>(nc_value);
				if (info->second != nullptr)
					info->first = remove_thread_(*info->second);
				break;
			}
			case prop::list_action::action_remove_index:
			{
				auto info = static_cast<std::pair<bool, std::size_t> *>(nc_value);
				info->first = remove_thread_at_(info->second);
				break;
			}
			case prop::list_action::action_at:
			{
				auto info = static_cast<std::pair<std::size_t, m_thread_type *> *>(nc_value);
				info->second = get_thread_at_(info->first);
				break;
			}
			case prop::list_action::action_find:
			{
				auto info = static_cast<std::pair<std::size_t, m_thread_type *> *>(nc_value);
				if (info->second != nullptr)
					info->first = find_thread_(*info->second);
				break;
			}
			case prop::list_action::action_first:
				*static_cast<m_thread_type **>(nc_value) = get_thread_at_(0);
				break;
			case prop::list_action::action_last:
				*static_cast<m_thread_type **>(nc_value) = get_thread_at_(threads_.size() - 1u);
				break;
			case prop::list_action::action_begin:
				*static_cast<std::list<m_thread_type *>::iterator *>(nc_value) = threads_.begin();
				break;
			case prop::list_action::action_end:
				*static_cast<std::list<m_thread_type *>::iterator *>(nc_value) = threads_.end();
				break;
			case prop::list_action::action_size:
				*static_cast<std::size_t *>(nc_value) = threads_.size();
				break;
			default:
				break;
			}
		}
	};

	auto getter = [](const prop::base &prop, void *buf, std::size_t index){
		if (&prop == &is_shut_down)
			*static_cast<bool *>(buf) = is_shut_down_;

		if (is_shut_down_)
			return;

		if (&prop == &current_thread)
			*static_cast<m_thread_type **>(buf) = get_current_thread_();
		else if (&prop == &main_thread)
			*static_cast<m_thread_type **>(buf) = main_thread_.get();
	};

	threads.init_(nullptr, setter, nullptr);
	current_thread.init_(nullptr, nullptr, getter);
	main_thread.init_(nullptr, nullptr, getter);
	is_shut_down.init_(nullptr, nullptr, getter);

	main_thread_.reset(new thread::object(true));
}

void winp::app::object::shut_down(){
	is_shut_down_ = true;
}

int winp::app::object::run(bool shut_down_after){
	if (is_shut_down_)
		return -1;

	auto result = main_thread_->run_();
	if (shut_down_after)
		shut_down();

	return result;
}

std::size_t winp::app::object::add_thread_(m_thread_type &thread){
	if (is_shut_down_)
		return static_cast<std::size_t>(-1);

	std::lock_guard<std::mutex> guard(lock);
	threads_.push_back(&thread);

	return (threads_.size() - 1u);
}

bool winp::app::object::remove_thread_(m_thread_type &thread){
	return remove_thread_at_(find_thread_(thread));
}

bool winp::app::object::remove_thread_at_(std::size_t index){
	if (is_shut_down_)
		return false;

	std::lock_guard<std::mutex> guard(lock);
	if (index >= threads_.size())
		return false;

	threads_.erase(std::next(threads_.begin(), index));
	return true;
}

std::size_t winp::app::object::find_thread_(const m_thread_type &thread){
	if (is_shut_down_)
		return static_cast<std::size_t>(-1);

	std::lock_guard<std::mutex> guard(lock);
	return std::distance(threads_.begin(), std::find(threads_.begin(), threads_.end(), &thread));
}

winp::app::object::m_thread_type *winp::app::object::get_thread_at_(std::size_t index){
	if (is_shut_down_)
		return nullptr;

	std::lock_guard<std::mutex> guard(lock);
	return ((index < threads_.size()) ? *std::next(threads_.begin(), index) : nullptr);
}

winp::app::object::m_thread_type *winp::app::object::get_current_thread_(){
	if (is_shut_down_)
		return nullptr;

	std::lock_guard<std::mutex> guard(lock);
	auto id = std::this_thread::get_id();

	for (auto thread : threads_){
		if (thread->id_ == id)
			return thread;
	}

	return nullptr;
}

winp::prop::list<std::list<winp::app::object::m_thread_type *>, winp::app::object, winp::prop::proxy_value> winp::app::object::threads;

winp::prop::scalar<winp::app::object::m_thread_type *, winp::app::object, winp::prop::proxy_value> winp::app::object::current_thread;

winp::prop::scalar<winp::app::object::m_thread_type *, winp::app::object, winp::prop::proxy_value> winp::app::object::main_thread;

winp::prop::scalar<bool, winp::app::object, winp::prop::proxy_value> winp::app::object::is_shut_down;

winp::prop::error<winp::app::object> winp::app::object::error;

std::shared_ptr<winp::thread::object> winp::app::object::main_thread_;

std::list<winp::app::object::m_thread_type *> winp::app::object::threads_;

std::mutex winp::app::object::lock;

bool winp::app::object::is_shut_down_ = false;

