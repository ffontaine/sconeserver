/* SconeServer (http://www.sconemad.com)

Launch an external process

Copyright (c) 2000-2006 Andrew Wedgbury <wedge@sconemad.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (see the file COPYING); if not, write to the
Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA */

#ifndef scxProcess_h
#define scxProcess_h

#include "sconex/sconex.h"
#include "sconex/StreamSocket.h"
namespace scx {

//============================================================================
class SCONEX_API Process : public StreamSocket {
public:

  static void init();
  
  Process(const std::string& exe);	

  ~Process();
  
  void add_arg(const std::string& arg);
  // Add an argument to the list
  
  void set_env(const std::string& name, const std::string& value);
  // Set environment variables
  
  bool launch();
  // Launch the process
  
  bool kill();
  // Kill the process

  bool is_running();
  // Is the process running
  
  bool get_exitcode(int& code);
  // Get the exitcode
  
  void set_detatched(bool onoff);
  // Set the detatch mode

  enum RunState { Unstarted, Running, Detatched, Terminated };
  
private:

  Process(const Process& c);
  // Disallow copying
  
#ifdef WIN32
  DWORD m_pid;
#else
  pid_t m_pid;
  static pid_t s_proxy_pid;
  static SOCKET s_proxy_sock;
#endif	
  
  std::string m_exe;
  std::list<std::string> m_args;
  std::map<std::string,std::string> m_env;
  RunState m_runstate;
  int m_exitcode;
  
};

};
#endif
