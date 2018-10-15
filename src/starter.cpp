//
// Bareflank Hypervisor Starter Kit
// Copyright (C) 2018 morimolymoly (Tr4pMafia)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <bfvmm/hve/arch/intel_x64/vcpu.h>
#include <bfvmm/vcpu/vcpu.h>
#include <bfdebug.h>
#include <bfvmm/vcpu/vcpu_factory.h>
#include <bfgsl.h>

namespace mafia
{
namespace intel_x64
{

static bool
handle_cpuid_mafia(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    if (vmcs->save_state()->rax == 0xBF01) {
        bfdebug_info(0, "[MAFIA] host os is" bfcolor_green " now " bfcolor_end "in a vm");
        return false;
    }

    if (vmcs->save_state()->rax == 0xBF00) {
        bfdebug_info(0, "[MAFIA] host os is" bfcolor_red " not " bfcolor_end "in a vm");
        return false;
    }
    return false;
}

class mafia_vcpu : public bfvmm::intel_x64::vcpu
{
public:
    mafia_vcpu(vcpuid::type id)
    : vcpu(id)
    {
        exit_handler()->add_handler(
            ::intel_x64::vmcs::exit_reason::basic_exit_reason::cpuid,
            handler_delegate_t::create<mafia::intel_x64::handle_cpuid_mafia>()
        );
    }
};
}
}

namespace bfvmm
{
std::unique_ptr<bfvmm::vcpu>
vcpu_factory::make(vcpuid::type vcpuid, bfobject *obj)
{
    bfignored(obj);
    return std::make_unique<mafia::intel_x64::mafia_vcpu>(vcpuid);
}
}

