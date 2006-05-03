/* SconeServer (http://www.sconemad.com)

External program execution Module

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

#ifndef execModule_h
#define execModule_h

#include "sconex/Module.h"

//#########################################################################
class ExecModule : public scx::Module {

public:

  ExecModule();
  virtual ~ExecModule();

  virtual std::string info() const;
  
  virtual bool connect(
    scx::Descriptor* endpoint,
    scx::ArgList* args
  );

protected:

private:
 
};

#endif

