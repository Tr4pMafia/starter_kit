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

#include <bfvmm/hve/arch/intel_x64/exit_handler/exit_handler.h>
#include <bfvmm/hve/arch/intel_x64/vcpu/vcpu.h>
#include <bfvmm/vcpu/vcpu.h>
#include <bfdebug.h>
#include <bfvmm/vcpu/vcpu_factory.h>
#include <bfgsl.h>

namespace mafia
{
namespace intel_x64
{

static bool
handle_vmxoff(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    vmcs->promote();
    return true;
}

static bool
handle_init_signal(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    // [NOTE] do nothing here, but is it correct??
    return advance(vmcs);
}

class exit_handler_mafia : public bfvmm::intel_x64::exit_handler
{
public:
    exit_handler_mafia(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
    : exit_handler(vmcs)
    {
        using namespace ::intel_x64::vmcs;
        bfdebug_info(0, "mafia hype you");

        add_handler(
            exit_reason::basic_exit_reason::vmxoff,
            handler_delegate_t::create<mafia::intel_x64::handle_vmxoff>()
        );

        add_handler(
            exit_reason::basic_exit_reason::init_signal,
            handler_delegate_t::create<mafia::intel_x64::handle_init_signal>()
        );
    }
};

class mafia_vcpu : public bfvmm::intel_x64::vcpu
{
public:
    mafia_vcpu(vcpuid::type id)
    : vcpu(id)
    {
        m_exit_handler_mafia = std::make_unique<mafia::intel_x64::exit_handler_mafia>(vmcs());
    }
    mafia::intel_x64::exit_handler_mafia *exit_handler()
    { return m_exit_handler_mafia.get(); }
private:
    std::unique_ptr<mafia::intel_x64::exit_handler_mafia> m_exit_handler_mafia;
};
}
}

namespace bfvmm
{
std::unique_ptr<bfvmm::vcpu>
vcpu_factory::make_vcpu(vcpuid::type vcpuid, bfobject *obj)
{
    bfignored(obj);
    return std::make_unique<mafia::intel_x64::mafia_vcpu>(vcpuid);
}
}

