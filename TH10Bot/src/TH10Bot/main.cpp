#include "TH10Bot/Common.h"

#include <memory>
#include <boost/log/utility/setup/file.hpp>
#include <Base/Util.h>

#include "TH10Bot/TH10Bot.h"

namespace bl = boost::log;

int main()
{
	HWND window = GetConsoleWindow();
	HMENU menu = GetSystemMenu(window, FALSE);
	EnableMenuItem(menu, SC_CLOSE, MF_GRAYED | MF_BYCOMMAND);

	std::string logName = base::Util::GetModuleDir() + "/TH10Bot.log";
	bl::add_file_log(logName);

	try
	{
		std::shared_ptr<th::TH10Bot> bot = std::make_shared<th::TH10Bot>();
		bot->run();
	}
	catch (...)
	{
		std::string what = boost::current_exception_diagnostic_information();
		BOOST_LOG_TRIVIAL(error) << what;
		std::cout << what << std::endl;
	}

	//system("pause");
	return 0;
}
