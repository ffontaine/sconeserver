/* SconeServer (http://www.sconemad.com)

http Request

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

#ifndef httpRequest_h
#define httpRequest_h

#include "http/DocRoot.h"
#include "sconex/Arg.h"
#include "sconex/VersionTag.h"
#include "sconex/Uri.h"
#include "sconex/MimeHeader.h"
namespace http {

//=============================================================================
class HTTP_API Request : public scx::Arg {

public:

  Request(const std::string& profile);
  Request(const Request& c);
  
  virtual ~Request();

  scx::Arg* new_copy() const;

  const std::string& get_method() const;
  const scx::Uri& get_uri() const;
  const scx::VersionTag& get_version() const;
  bool parse_request(const std::string& str, bool secure);

  std::string get_header(const std::string& name) const;
  scx::MimeHeader get_header_parsed(const std::string& name) const;
  bool parse_header(const std::string& str);

  void set_host(Host* host);
  const Host* get_host() const;

  const std::string& get_profile() const;

  void set_docroot(DocRoot* docroot);
  const DocRoot* get_docroot() const;

  void set_path(const scx::FilePath& path);
  const scx::FilePath& get_path() const;

  void set_auth_user(const std::string& user);
  const std::string& get_auth_user() const;

  void set_path_info(const std::string& pathinfo);
  const std::string& get_path_info() const;  
  
  // Arg methods
  virtual std::string get_string() const;
  virtual int get_int() const;
  virtual scx::Arg* op(
    scx::Arg::OpType optype,
    const std::string& opname,
    scx::Arg* right
  );
  
protected:

  std::string m_method;
  scx::Uri m_uri;
  scx::VersionTag m_version;

  scx::MimeHeaderTable m_headers;

  Host* m_host;
  std::string m_profile;
  DocRoot* m_docroot;
  scx::FilePath m_path;

  std::string m_auth_user;
  std::string m_pathinfo;
  
private:

};

};
#endif
