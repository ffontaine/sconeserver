/* SconeServer (http://www.sconemad.com)

HTTP Response stream

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

#ifndef httpResponseStream_h
#define httpResponseStream_h

#include <sconex/Stream.h>
#include <sconex/LineBuffer.h>
#include <sconex/Buffer.h>
#include <sconex/MimeHeader.h>
#include <http/http.h>
#include <http/MessageStream.h>

namespace http {

class MimeHeaderStream;
  
//=========================================================================
class HTTP_API ResponseStream : public scx::Stream {

public:

  enum ResponseSequence {
    resp_Start,
    resp_ReadSingle,
    resp_ReadMultiStart,
    resp_ReadMultiBoundary,
    resp_ReadMultiHeader,
    resp_ReadMultiBody,
    resp_ReadEnd,
    resp_Write,
    resp_WriteWait,
    resp_End
  };

  enum MimeBoundaryType {
    bound_Initial,
    bound_Intermediate,
    bound_Final
  };
  
  ResponseStream(const std::string& stream_name);
  
  ~ResponseStream();

  virtual std::string stream_status() const;

protected:

  ResponseStream(const ResponseStream& c);
  ResponseStream& operator=(const ResponseStream& v);
  // Prohibit copy

  // from scx::Stream:
  virtual scx::Condition event(scx::Stream::Event e);
  virtual scx::Condition read(void* buffer,int n,int& na);

  
  virtual scx::Condition start_section(const scx::MimeHeaderTable& headers);
  virtual scx::Condition send_response();

  bool decode_param_string(const std::string& str,Request& request);

  bool send_file(const scx::FilePath& path);
  
  bool find_mime_boundary();
  
private:

  friend class MimeHeaderStream;

  void mimeheader_line(const std::string& line);
  void mimeheader_end();

  ResponseSequence m_resp_seq;

  std::string m_param_string;

  scx::Buffer m_buffer;
  std::string m_mime_boundary;
  int m_mime_boundary_pos;
  int m_mime_boundary_len;
  MimeBoundaryType m_mime_boundary_type;
  int m_mime_boundary_num;

  scx::MimeHeaderTable m_section_headers;
  
  scx::Condition m_source_cond;
};

};
#endif
