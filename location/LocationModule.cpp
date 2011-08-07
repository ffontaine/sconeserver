/* SconeServer (http://www.sconemad.com)

Location module

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


#include <sconex/ModuleInterface.h>
#include <sconex/Module.h>
#include <sconex/ScriptTypes.h>
#include <sconex/ScriptExpr.h>

#include <libgpsmm.h>

//=============================================================================
class LocationModule : public scx::Module {
public:

  LocationModule();
  virtual ~LocationModule();

  //---------------------------------------------------------------------------
  // Module overrides.
  virtual std::string info() const;

  //---------------------------------------------------------------------------
  // ScriptObject overrides.
  virtual scx::ScriptRef* script_op(const scx::ScriptAuth& auth,
                                    const scx::ScriptRef& ref,
                                    const scx::ScriptOp& op,
                                    const scx::ScriptRef* right);

  virtual scx::ScriptRef* script_method(const scx::ScriptAuth& auth,
                                        const scx::ScriptRef& ref,
                                        const std::string& name,
                                        const scx::ScriptRef* args);

private:

  LocationModule(const LocationModule& original);
  LocationModule& operator=(const LocationModule& rhs);
  // Prohibit copy.

  bool initialise_interface();

  bool query(double& latitude, double& longitude, double& speed);

  std::string m_host;
  std::string m_port;

  gpsmm m_interface;

  bool m_interface_usable;
};

SCONEX_MODULE(LocationModule);

//=============================================================================
// N.B. "Sensible" defaults for host and port.
LocationModule::LocationModule()
  : scx::Module("location",scx::version()),
    m_host("localhost"),
    m_port("gpsd"),
    m_interface(),
    m_interface_usable(false)
{
}

//=============================================================================
LocationModule::~LocationModule()
{
}

//=============================================================================
std::string LocationModule::info() const
{
  return "Location module";
}

//=============================================================================
scx::ScriptRef* LocationModule::script_op(const scx::ScriptAuth& auth,
                                          const scx::ScriptRef& ref,
                                          const scx::ScriptOp& op,
                                          const scx::ScriptRef* right)
{
  if (op.type() == scx::ScriptOp::Lookup) {
    const std::string name = right->object()->get_string();

    // Methods
    if ("connect" == name ||"query" == name) {
      return new scx::ScriptMethodRef(ref,name);
    }

  }

  return scx::Module::script_op(auth,ref,op,right);
}

//=============================================================================
scx::ScriptRef* LocationModule::script_method(const scx::ScriptAuth& auth,
                                              const scx::ScriptRef& ref,
                                              const std::string& name,
                                              const scx::ScriptRef* args)
{
  if ("connect" == name) {

    // Get host specification.
    const scx::ScriptString* a_host =
      scx::get_method_arg<scx::ScriptString>(args,0,"host");
    if (NULL != a_host) {
      m_host = a_host->get_string();
    }

    // Get port specification: could be string (service name) or numeric
    // (port number).
    const scx::ScriptInt* a_service_num =
      scx::get_method_arg<scx::ScriptInt>(args,1,"port");
    const scx::ScriptString* a_service_str =
      scx::get_method_arg<scx::ScriptString>(args,1,"service");

    if (NULL != a_service_num) {
      const int p = a_service_num->get_int();
      if (0 <= p && p <= 65535) {
        std::stringstream ss;
        ss << p;
        m_port = ss.str();
      } else {
        return scx::ScriptError::new_ref("Invalid port designation");
      }

    } else if (NULL != a_service_str) {
      m_port = a_service_str->get_string();
    }

    // FIXME: Not sure what to return here...?
    return scx::ScriptError::new_ref("NOT YET WRITTEN!");
  } else if ("query" == name) {
    double latitude = 0.0;
    double longitude = 0.0;
    double speed = 0.0;
    if (query(latitude, longitude, speed)) {
      scx::ScriptMap::Ref position(new scx::ScriptMap());
      position.object()->give("latitude",
                              scx::ScriptReal::new_ref(latitude));
      position.object()->give("longitude",
                              scx::ScriptReal::new_ref(longitude));
      position.object()->give("speed",
                              scx::ScriptReal::new_ref(speed));

      return new scx::ScriptRef(new scx::ScriptMap(*position.object()));
    } else {
      return scx::ScriptError::new_ref("Failed to query location");
    }
  }

  return scx::Module::script_method(auth,ref,name,args);
}

//=============================================================================
bool LocationModule::initialise_interface()
{
  if (!m_interface_usable) {
    if (NULL != m_interface.open(m_host.c_str(), m_port.c_str())) {

      // Attempt to enable "watcher mode".
      if (NULL != m_interface.stream(WATCH_ENABLE)) {
        m_interface_usable = true;
      } else {
        DEBUG_LOG("Failed to enable watcher mode on interface");
      }

    } else {
      DEBUG_LOG("Failed to open interface");
    }
  }

  return m_interface_usable;
}

//=============================================================================
bool LocationModule::query(double& latitude, double& longitude, double& speed)
{
  if (initialise_interface()) {
    // Attempt to get a fix.
    const struct gps_data_t* data = NULL;

    // Discard the backlog of data.
    m_interface.clear_fix();
    while (m_interface.waiting()) {
      data = m_interface.poll();
    }

    if (NULL != data) {
      const gps_fix_t fix = data->fix;
      switch (fix.mode) {
      case MODE_NOT_SEEN:
        DEBUG_LOG("MODE_NOT_SEEN");
        break;
      case MODE_NO_FIX:
        DEBUG_LOG("MODE_NO_FIX");
        break;
      case MODE_2D:
      case MODE_3D:
        latitude = fix.latitude;
        longitude = fix.longitude;
        speed = fix.speed;
        return true;
      }
    } else {
      DEBUG_LOG("No data");
    }
  } else {
    DEBUG_LOG("No interface");
  }

  return false;
}