#include "AppLanguagePolicy.h"

#if CL_ENABLE_SCRIPTING
 #include <llvm/Config/llvm-config.h>
#endif

namespace creation_live::language
{
std::string getLanguageRuntimeSummary()
{
#if CL_ENABLE_SCRIPTING
    return "LLVM-enabled host layer ready for video-domain scripting (" LLVM_VERSION_STRING ")";
#else
    return "LLVM scripting disabled for this build.";
#endif
}

std::string getAppDomainName()
{
    return "live";
}

bool canRunNodeDomain(const std::string& domainName)
{
    return domainName == "shared" || domainName == "live" || domainName == "scene" || domainName == "broadcast";
}
}
