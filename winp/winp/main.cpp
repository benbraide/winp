#include <windows.h>

#include "property/variant_property.h"

//ReSharper disable once CppInconsistentNaming
int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::prop::scalar<int> si(9), si2;
	int v = si;
	si2 = si;
	si = 18;

	winp::prop::scalar<std::string> ss;
	auto sz = ss->size();

	winp::prop::scalar<int *> sp;
	winp::prop::scalar<const int *> scp;
	auto bp = sp == scp;
	auto bpn = sp == nullptr;
	auto bcpn = scp == nullptr;

	struct vhdl{
		vhdl(){
			vp.init_(*this, &vhdl::on_change, [this](const winp::prop::base<vhdl> &sender, const void *value, std::size_t index){
				if (index == 0u)
					i = *static_cast<const int *>(value);
				else
					b = *static_cast<const bool *>(value);
			}, [this](const winp::prop::base<vhdl> &sender, void *buf, std::size_t index){
				if (index == 0u)
					*static_cast<int *>(buf) = i;
				else
					*static_cast<bool *>(buf) = b;
			});
		}

		void on_change(const winp::prop::base<vhdl> &sender, const void *, std::size_t){
			
		}

		int i;
		bool b;
		winp::prop::variant<vhdl, int, bool> vp;
	};

	vhdl vhdli;
	vhdli.vp = 45;
	vhdli.vp = false;
	int vpi = vhdli.vp;
	bool vpb = vhdli.vp;

	return 0;
}
