/* SconeServer (http://www.sconemad.com)

Markup renderer

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

#ifndef sconesiteRenderMarkup_h
#define sconesiteRenderMarkup_h

#include "Context.h"
#include "XMLDoc.h"
#include "http/Request.h"
#include "http/Response.h"
#include "sconex/ArgObject.h"
#include "sconex/Stream.h"
#include "sconex/Descriptor.h"
#include "sconex/FilePath.h"
#include "sconex/FileDir.h"
#include "sconex/Job.h"

class Profile;
class Article;

//=========================================================================
class RenderMarkupContext : public Context {

public:

  RenderMarkupContext(
    Profile& profile,
    scx::Descriptor* output,
    const http::Request& request,
    http::Response& response
  );

  ~RenderMarkupContext();

  Profile& get_profile();
  const http::Request& get_request() const;

  void set_article(Article* article);
  const Article* get_article() const;

  // XMLDoc interface
  virtual bool handle_start(const std::string& name, XMLAttrs& attrs, bool empty);
  virtual bool handle_end(const std::string& name, XMLAttrs& attrs);
  virtual void handle_process(const std::string& name, const char* data);
  virtual void handle_text(const char* text);
  virtual void handle_comment(const char* text);
  virtual void handle_error(const std::string& msg);

  // ArgObject interface
  virtual scx::Arg* arg_resolve(const std::string& name);
  virtual scx::Arg* arg_lookup(const std::string& name);
  virtual scx::Arg* arg_function(const scx::Auth& auth,const std::string& name,scx::Arg* args);
  
protected:
  
  Profile& m_profile;
  scx::Descriptor* m_output;
  http::Request m_request;
  scx::ArgObject* m_response_obj;

  Article* m_article;
  bool m_processing;
};

//=========================================================================
class RenderMarkupJob : public scx::Job {

public:

  RenderMarkupJob(RenderMarkupContext* ctx);
  virtual ~RenderMarkupJob();

  virtual bool should_run();
  virtual bool run();
  virtual std::string describe() const;

protected:

  RenderMarkupContext* m_context;

};

#endif
