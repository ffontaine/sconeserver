/* SconeServer (http://www.sconemad.com)

Sconesite Module

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


#include "SconesiteModule.h"
#include "SconesiteStream.h"
#include "SconesiteArticle.h"

#include "sconex/ModuleInterface.h"
#include "sconex/Module.h"
#include "sconex/Arg.h"
#include "sconex/Stream.h"

SCONESERVER_MODULE(SconesiteModule);

//=========================================================================
SconesiteModule::SconesiteModule()
  : scx::Module("sconesite",scx::version())
{

}

//=========================================================================
SconesiteModule::~SconesiteModule()
{
  for (std::map<std::string,SconesiteArticleManager*>::const_iterator it =
         m_profiles.begin();
       it != m_profiles.end();
       ++it) {
    delete (*it).second;
  }
}

//=========================================================================
std::string SconesiteModule::info() const
{
  return "Copyright (c) 2000-2009 Andrew Wedgbury\n"
         "Sconesite\n";
}

//=========================================================================
bool SconesiteModule::connect(
  scx::Descriptor* endpoint,
  scx::ArgList* args
)
{
  const scx::ArgString* profile =
    dynamic_cast<const scx::ArgString*>(args->get(0));
  if (!profile) {
    log("No profile specified, aborting connection");
    return false;
  }

  SconesiteStream* s = new SconesiteStream(*this,profile->get_string());
  s->add_module_ref(ref());
  
  endpoint->add_stream(s);
  return true;
}

//=========================================================================
SconesiteArticleManager* SconesiteModule::lookup_profile(const std::string& profile)
{
  std::map<std::string,SconesiteArticleManager*>::const_iterator it =
    m_profiles.find(profile);
  if (it != m_profiles.end()) {
    return (*it).second;
  }

  return 0;
}

//=============================================================================
scx::Arg* SconesiteModule::arg_lookup(const std::string& name)
{
  // Methods

  if ("add" == name) {
    return new scx::ArgObjectFunction(
      new scx::ArgModule(ref()),name);
  }      

  // Properties
  
  if ("test_property" == name) {
    return new scx::ArgInt(1);
  }

  return SCXBASE Module::arg_lookup(name);
}

//=============================================================================
scx::Arg* SconesiteModule::arg_function(
  const std::string& name,
  scx::Arg* args
)
{
  scx::ArgList* l = dynamic_cast<scx::ArgList*>(args);
  
  if ("add" == name) {
    const scx::ArgString* a_profile =
      dynamic_cast<const scx::ArgString*>(l->get(0));
    if (!a_profile) {
      return new scx::ArgError("add() No profile name specified");
    }
    std::string s_profile = a_profile->get_string();
    
    const scx::ArgString* a_path =
      dynamic_cast<const scx::ArgString*>(l->get(1));
    if (!a_path) {
      return new scx::ArgError("add() No path specified");
    }

    std::map<std::string,SconesiteArticleManager*>::const_iterator it =
      m_profiles.find(s_profile);
    if (it != m_profiles.end()) {
      return new scx::ArgError("add() Profile already exists");
    }
        
    scx::FilePath path = a_path->get_string();
    log("Adding profile '" + s_profile + "' dir '" +
        path.path() + "'");
    m_profiles[s_profile] = new SconesiteArticleManager(*this,path);

    return 0;
  }
  
  return SCXBASE Module::arg_function(name,args);
}