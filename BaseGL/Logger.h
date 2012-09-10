#pragma once

#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>

using namespace boost::log;

enum severity_level
{
  normal,
  info,
  warning,
  error,
  critical
};

std::ostream &operator<<( std::ostream &os, const severity_level level );

BOOST_LOG_DECLARE_GLOBAL_LOGGER_INIT(global_log, boost::log::sources::severity_logger<severity_level>)
{
  boost::function< void (std::ostream&, record const&) > formatter =
        formatters::stream
            << formatters::attr< unsigned int >("LineID", "[% 9u] ")
            << formatters::date_time< boost::posix_time::ptime >("TimeStamp") << " *" //<< formatters::message();
            << formatters::attr< severity_level >("Severity") << "* " << formatters::message();
    //sink->locked_backend()->set_formatter(formatter);
    init_log_to_console()->locked_backend()->set_formatter(formatter);
        // Register common attributes
    add_common_attributes();
    
    // Here we go, we can write logs
    sources::severity_logger<severity_level> lg;
    return lg;
}