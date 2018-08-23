#include "../app/app_object.h"

winp::prop::untyped_base::~untyped_base() = default;

void winp::prop::untyped_base::change_(const void *value, std::size_t size){}

void winp::prop::untyped_base::throw_(error_value_type value) const{
	app::object::error = value;
}
