#include "ParameterMgr.h"

ParameterMgr::ParameterMgr()
{
}

void ParameterMgr::LoadFile(const std::string &FILE_PASS)
{
	std::ifstream ifs(FILE_PASS);
	rapidjson::IStreamWrapper isw(ifs);
	doc.ParseStream(isw);
}

void ParameterMgr::ExportFile(const std::string &FILE_PASS)
{
	std::ofstream ofs(FILE_PASS);
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	doc.Accept(writer);
}
