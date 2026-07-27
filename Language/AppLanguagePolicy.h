#pragma once

#include <string>

namespace creation_live::language
{
std::string getLanguageRuntimeSummary();
std::string getAppDomainName();
bool canRunNodeDomain(const std::string& domainName);
}
