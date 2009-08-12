/* SconeServer (http://www.sconemad.com)

http Response

Copyright (c) 2000-2009 Andrew Wedgbury <wedge@sconemad.com>

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

#ifndef httpResponse_h
#define httpResponse_h

#include "http/http.h"
#include "http/Status.h"
#include "sconex/Arg.h"
#include "sconex/VersionTag.h"
#include "sconex/MimeHeader.h"
namespace http {

//=============================================================================
class HTTP_API Response : public scx::Arg {

public:

  Response();
  Response(const Response& c);
  
  virtual ~Response();

  scx::Arg* new_copy() const;

  void set_version(const scx::VersionTag& ver);
  const scx::VersionTag& get_version() const;
  
  void set_status(const Status& status);
  const Status& get_status() const;

  void set_header(const std::string& name, const std::string& value);
  bool remove_header(const std::string& name);
  std::string get_header(const std::string& name) const;

  std::string build_header_string();
  
  // Arg methods
  virtual std::string get_string() const;
  virtual int get_int() const;
  virtual scx::Arg* op(
    scx::Arg::OpType optype,
    const std::string& opname,
    scx::Arg* right
  );
  
protected:

  scx::VersionTag m_version;
  Status m_status;

  scx::MimeHeaderTable m_headers;
  
private:

};

};
#endif
