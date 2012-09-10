#include "Logger.h"

std::ostream &operator<<( std::ostream &os, const severity_level l )
{
  switch (l)
  {
  case normal:
    os << "NORMAL";
    break;
  case info:
    os << "INFO";
    break;
  case warning:
    os << "WARNING";
    break;
  case error:
    os << "ERROR";
    break;
  case critical:
    os << "CRITICAL";
    break;
  default:
    os << "I HAVE NO IDEA HOW YOU GOT HERE";
    break;
  }
  return os;
}

