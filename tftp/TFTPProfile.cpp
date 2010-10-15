/* SconeServer (http://www.sconemad.com)

Trivial File Transfer Protocol (TFTP) Profile

Copyright (c) 2000-2007 Andrew Wedgbury <wedge@sconemad.com>

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

#include "sconex/ConfigFile.h"

#include "TFTPProfile.h"

//=========================================================================
TFTPProfile::TFTPProfile(
  TFTPModule& mod,
  const std::string& name,
  const scx::FilePath& path
)
  : m_mod(mod),
    m_name(name),
    m_path(path)
{

}

//=========================================================================
TFTPProfile::~TFTPProfile()
{

}

//=============================================================================
const scx::FilePath& TFTPProfile::get_path() const
{
  return m_path;
}

//=============================================================================
std::string TFTPProfile::name() const
{
  return m_name;
}

//=============================================================================
scx::Arg* TFTPProfile::arg_lookup(
  const std::string& name
)
{
  // Methods
  if ("test" == name) {
    return new_method(name);
  }

  return SCXBASE ArgObjectInterface::arg_lookup(name);
}

//=============================================================================
scx::Arg* TFTPProfile::arg_method(
  const scx::Auth& auth,
  const std::string& name,
  scx::Arg* args
)
{
  scx::ArgList* l = dynamic_cast<scx::ArgList*>(args);

  return SCXBASE ArgObjectInterface::arg_method(auth,name,args);
}
