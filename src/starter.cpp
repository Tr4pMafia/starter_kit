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
static bool handle_cpuid_mafia()
{
    bfdebug_info(0, "mafia inside");
    return true;
}
class exit_handler_mafia : public bfvmm::intel_x64::exit_handler
{
public:
    exit_handler_mafia(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
    : exit_handler(vmcs)
    {
        bfdebug_info(0, "mafia hype you");
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
WEAK_SYM std::unique_ptr<bfvmm::vcpu>
vcpu_factory::make_vcpu(vcpuid::type vcpuid, bfobject *obj)
{
    bfignored(obj);
    return std::make_unique<mafia::intel_x64::mafia_vcpu>(vcpuid);
}
}

