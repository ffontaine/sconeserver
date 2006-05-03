/* SconeServer (http://www.sconemad.com)

HTTP filesystem file node

Copyright (c) 2000-2004 Andrew Wedgbury <wedge@sconemad.com>

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

#include "http/FSFile.h"
namespace http {

//=============================================================================
FSFile::FSFile(
  const std::string& name
) : FSNode(FSNode::File,name)
{
  DEBUG_COUNT_CONSTRUCTOR(FSFile);
}
	
//=============================================================================
FSFile::~FSFile()
{
  DEBUG_COUNT_DESTRUCTOR(FSFile);
}

//=============================================================================
bool FSFile::build()
{
  return true;
}

};
