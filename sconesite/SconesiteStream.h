/* SconeServer (http://www.sconemad.com)

Sconesite Stream

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

#ifndef sconesiteStream_h
#define sconesiteStream_h

#include "sconex/Stream.h"
#include "http/ResponseStream.h"

class TestBuilderModule;
class Article;
class Template;

//=========================================================================
class SconesiteStream : public http::ResponseStream {

public:

  SconesiteStream(
    SconesiteModule& module,
    const std::string& profile
  );
  
  ~SconesiteStream();
  
protected:

  virtual scx::Condition start_section(const scx::MimeHeaderTable& headers);
  virtual scx::Condition send_response();

private:
  
  SconesiteModule& m_module;

  std::string m_profile;

  enum Sequence {
    Start,
    RunTemplate
  };

  Sequence m_seq;
  Article* m_article;
  scx::Condition m_prev_cond;

  int m_section;
  
  Template* m_template;
};

#endif
