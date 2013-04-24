/* SconeServer (http://www.sconemad.com)

HTTP Sessions

Copyright (c) 2000-2011 Andrew Wedgbury <wedge@sconemad.com>

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

#ifndef httpSession_h
#define httpSession_h

#include <http/http.h>
#include <sconex/ScriptBase.h>
#include <sconex/ScriptTypes.h>
#include <sconex/Job.h>
namespace http {

class HTTPModule;
class SessionManager;
  
const scx::Time DEFAULT_SESSION_TIMEOUT = scx::Time(60 * 60);

//=============================================================================
// Session - Holds information about a user's interaction with a web site 
// from a particular client. Used to maintain state across (otherwise 
// stateless) HTTP protocol interactions.
//
class HTTP_API Session : public scx::ScriptMap {
public:

  Session(SessionManager& manager,
          const std::string& id = "");

  virtual ~Session();

  const std::string get_id() const;

  void set_timeout(const scx::Time& time);
  const scx::Time& get_timeout() const;

  void set_last_used(const scx::Date& used = scx::Date::now());
  const scx::Date& get_last_used() const;
  
  scx::Date get_expiry() const;
  bool valid() const;

  void set_locked(bool locked);
  bool is_locked() const;
  
  bool allow_upload() const;

  SessionManager& get_manager() const;
  
  // ScriptObject methods
  virtual std::string get_string() const;
  virtual int get_int() const;

  virtual scx::ScriptRef* script_op(const scx::ScriptAuth& auth,
				    const scx::ScriptRef& ref,
				    const scx::ScriptOp& op,
				    const scx::ScriptRef* right=0);

  virtual scx::ScriptRef* script_method(const scx::ScriptAuth& auth,
					const scx::ScriptRef& ref,
					const std::string& name,
					const scx::ScriptRef* args);

  typedef scx::ScriptRefTo<Session> Ref;
  
protected:

private:

  SessionManager& m_manager;

  std::string m_id;
  static unsigned long long int m_next_id;

  scx::Time m_timeout;
  scx::Date m_last_used;
  bool m_locked;
  
};

//=============================================================================
// SessionManager - Maintains a list of active HTTP sessions
//
class HTTP_API SessionManager : public scx::ScriptObject {
public:

  SessionManager(HTTPModule& module);

  virtual ~SessionManager();

  // Lookup an existing session by ID
  Session::Ref* lookup_session(const std::string& id);

  // Create a new session
  Session::Ref* new_session();

  // Release a locked session
  void release_session(Session::Ref* session);
  
  // Check through sessions, removing any that have timed-out
  int check_sessions();

  // ScriptObject methods
  virtual scx::ScriptRef* script_op(const scx::ScriptAuth& auth,
				    const scx::ScriptRef& ref,
				    const scx::ScriptOp& op,
				    const scx::ScriptRef* right=0);

  virtual scx::ScriptRef* script_method(const scx::ScriptAuth& auth,
					const scx::ScriptRef& ref,
					const std::string& name,
					const scx::ScriptRef* args);

  typedef scx::ScriptRefTo<SessionManager> Ref;

 private:
  
  HTTPModule& m_module;
  scx::Mutex m_mutex;
  scx::ConditionEvent m_release;
  
  typedef HASH_TYPE<std::string,Session::Ref*> SessionMap;
  SessionMap m_sessions;
  scx::JobID m_job;
  
};
 
};
#endif
