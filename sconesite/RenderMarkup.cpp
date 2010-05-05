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


#include "RenderMarkup.h"
#include "Profile.h"
#include "Article.h"
#include "Template.h"
#include "SconesiteModule.h"
#include "SconesiteStream.h"

#include "sconex/Stream.h"
#include "sconex/StreamTransfer.h"
#include "sconex/Date.h"
#include "sconex/Kernel.h"
#include "sconex/FileDir.h"
#include "sconex/ArgProc.h"
#include "sconex/MemFile.h"
#include "sconex/ArgScript.h"
#include "sconex/Socket.h"
#include "sconex/StreamSocket.h"
#include "sconex/utils.h"

const char* XHTML_DOCTYPE = "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>";

const char* XHTML_NAMESPACE = "http://www.w3.org/1999/xhtml";

//=========================================================================
RenderMarkupContext::RenderMarkupContext(
  Profile& profile,
  scx::Descriptor& output,
  http::Request& request,
  http::Response& response

) : m_profile(profile),
    m_output(output),
    m_request(request),
    m_response(response),
    m_article(0),
    m_auto_number(false),
    m_inhibit(false)
{

}

//=========================================================================
RenderMarkupContext::~RenderMarkupContext()
{
  for (ArgStatementGroupList::iterator it = m_old_groups.begin();
       it != m_old_groups.end();
       ++it) {
    delete *it;
  }
}

//=========================================================================
Profile& RenderMarkupContext::get_profile()
{
  return m_profile;
}
  
//=========================================================================
const http::Request& RenderMarkupContext::get_request() const
{
  return m_request;
}

//=========================================================================
void RenderMarkupContext::set_article(Article* article)
{
  m_article = article;
}

//=========================================================================
const Article* RenderMarkupContext::get_article() const
{
  return m_article;
}

//=========================================================================
bool RenderMarkupContext::handle_start(
  const std::string& name,
  XMLAttrs& attrs,
  bool empty,
  void* data
)
{
  bool descend = false;
  std::string pre;

  if (name == "article") {
    descend = !empty;
    // Load settings for this article
    m_auto_number = XMLAttr_bool(attrs,"autonumber");

  } else if (name == "template") {
    descend = !empty;

  } else if (name == "if") {
    std::string condition = attrs["condition"];
    scx::ArgObject ctx(this);
    scx::ArgProc proc(scx::Auth::Untrusted,&ctx);
    scx::Arg* arg = proc.evaluate(condition);
    bool result = (arg && arg->get_int() != 0);
    delete arg;
    return result;

  } else if (name == "section") {
    if (!m_section.empty()) {
      m_inhibit = (attrs["name"] != m_section);
    } else {
      m_inhibit = !XMLAttr_bool(attrs,"default");
    }
    descend = !empty;
    
  } else {
    
    std::ostringstream oss;

    // Monkey about with some of the standard HTML tags!
    
    if (name == "img") {
      std::string link = attrs["src"];
      if (link[0] != '/' && link.find(":") == std::string::npos) {
	attrs["src"] = "/" + m_article->get_href_path() + link;
      }

    } else if (name == "a" || name == "area") {
      std::string link = attrs["href"];
      if (link.find(":") == std::string::npos) {
        if (link[0] != '/') {
          // Expand relative hrefs into full paths so they work anywhere
          attrs["href"] = "/" + m_article->get_href_path() + link;
        }
      } else {
        // Add class 'external' to offsite links
        std::string& c = attrs["class"];
        if (c.empty()) {
          c = "external";
        } else {
          c += " external";
        }
      }

    } else if (name == "h1" || name == "h2" || name == "h3" ||
               name == "h4" || name == "h5" || name == "h6") {
      // Automatically insert anchors before headings
      if (data && m_article) { 
        const ArticleHeading* h = (const ArticleHeading*)(data);
        if (h) {
          int index = h->index();
          std::string anchor = m_article->get_headings().lookup_anchor(index);
          if (m_auto_number) {
            std::string href = "/" + m_article->get_href_path() + "#" + anchor;
            pre += "<span class='section'><a href='" +
              href + "' title='Link to this section'>" +
              m_article->get_headings().lookup_section(index) +
              ".</a></span> ";
          }
          
          oss << "<a name='" << anchor << "'></a>";
        }
      }
      
    } else if (name == "html") {
      // Setup for valid XHTML output
      oss << XHTML_DOCTYPE << "\n";
      attrs["xmlns"] = XHTML_NAMESPACE;
    }

    oss << "<" << name;
    for (XMLAttrs::const_iterator it = attrs.begin();
         it != attrs.end();
         ++it) {
      oss << " " << (*it).first << "=\"" << (*it).second << "\"";
    }
    
    if (!empty) {
      oss << ">" << pre;
      descend = true;
    } else {
      oss << "/>";
      descend = false;
    }
    if (!m_inhibit) m_output.write(oss.str());
  }
  return descend;
}

//=========================================================================
bool RenderMarkupContext::handle_end(
  const std::string& name,
  XMLAttrs& attrs,
  void* data
)
{
  bool repeat = false;

  if (name == "if" ||
      name == "article" ||
      name == "template") {
    // Ignore

  } else if (name == "section") {
    if (!m_section.empty()) {
      if (!m_inhibit) {
        m_inhibit = (attrs["name"] == m_section);
      }
    } else {
      if (m_inhibit) {
        m_inhibit = XMLAttr_bool(attrs,"default");
      }
    }
  
  } else {
    std::ostringstream oss;
    oss << "</" << name << ">";
    if (!m_inhibit) m_output.write(oss.str());
  }

  return repeat;
}

//=========================================================================
void RenderMarkupContext::handle_process(const std::string& name, const char* data)
{
  if (m_inhibit) return;
    
  scx::Auth auth(scx::Auth::Untrusted);
  XMLDoc* doc = get_current_doc();
  const std::type_info& ti = typeid(*doc);
  if (ti == typeid(Template)) {
    // Trust templates
    auth = scx::Auth(scx::Auth::Trusted);
  }

  if (name == "scxp") { // Sconescript evaluate and print
    // Create root statement using our environment
    scx::ArgStatementGroup root(&m_scx_env);
    root.set_parent(this);
    scx::ArgObject ctx(&root);
    scx::ArgProc proc(auth,&ctx);
    scx::Arg* arg = 0;
    try {
      arg = proc.evaluate(data);
      if (arg) {
	std::string str = arg->get_string();
	if (!str.empty()) {
	  m_output.write(str);
	}
      }
    } catch (...) {
      delete arg;
      DEBUG_LOG("EXCEPTION in RenderMarkup::handle_process(scxp)");
      throw;
    }
    delete arg;

  } else if (name == "scx") { // Sconescript
    scx::Arg* ret = 0;
    try {
      int len = strlen(data);
      scx::MemFileBuffer fbuf(len);
      fbuf.get_buffer()->push_from(data,len);
      scx::MemFile mfile(&fbuf);

      // Create root statement using our environment
      scx::ArgStatementGroup* root = new scx::ArgStatementGroup(&m_scx_env);
      m_old_groups.push_back(root);
      root->set_parent(this);
      
      // Create a script parser
      scx::ArgScript* script = new scx::ArgScript(root);
      mfile.add_stream(script);

      // Parse statements
      while (script->event(scx::Stream::Readable) == scx::Ok);

      // Run statements
      scx::ArgProc proc(auth);
      ret = root->execute(proc);
      delete ret;

    } catch (...) { 
      delete ret;
      DEBUG_LOG("EXCEPTION in RenderMarkup::handle_process(scx)"); 
      throw; 
    }
  }
}

//=========================================================================
void RenderMarkupContext::handle_text(const char* text)
{
  if (m_inhibit) return;

  const char* p = text;
  while (*p == '\n' || *p == '\r') ++p;
  if (*p != '\0') {
    m_output.write(p);
  }
}

//=========================================================================
void RenderMarkupContext::handle_comment(const char* text)
{
  // m_output.write(text);
}

//=========================================================================
void RenderMarkupContext::handle_error(const std::string& msg)
{
  m_output.write("<html><body>");
  m_output.write("<p class='scxerror'>ERROR: Cannot process article</p>");
  m_output.write("<pre>");
  m_output.write(msg);
  m_output.write("</pre>");
  m_output.write("</body></html>");
}

//=========================================================================
scx::Arg* RenderMarkupContext::arg_resolve(const std::string& name)
{
  scx::Arg* a = arg_lookup(name);
  if (BAD_ARG(a)) {
    delete a;
    a = m_profile.get_module().arg_resolve(name);
  }
  return a;
}

//=========================================================================
scx::Arg* RenderMarkupContext::arg_lookup(const std::string& name)
{
  // Methods
  if ("print" == name ||
      "print_esc" == name ||
      "print_json" == name ||
      "escape" == name ||
      "get_articles" == name ||
      "process_article" == name ||
      "edit_article" == name ||
      "template" == name ||
      "get_files" == name ||
      "abort" == name) {
    return new_method(name);
  }

  // Sub-objects
  if ("request" == name) return new scx::ArgObject(&m_request);
  if ("response" == name) return new scx::ArgObject(&m_response);
  if ("session" == name) {
    if (m_request.get_session()) {
      return new scx::ArgObject(m_request.get_session());
    } else {
      return new scx::ArgError("No session");
    }
  }
  if ("local_addr" == name) {
    scx::Socket* socket = dynamic_cast<scx::Socket*>(&m_output);
    if (socket) {
      return socket->get_local_addr()->new_copy();
    }
    return 0;
  }
  if ("remote_addr" == name) {
    //NOTE: At the moment this is only ever likely to be a StreamSocket,
    // but this may change in the future, who knows?
    scx::StreamSocket* socket = dynamic_cast<scx::StreamSocket*>(&m_output);
    if (socket) {
      return socket->get_remote_addr()->new_copy();
    }
    return 0;
  }
  if ("realms" == name) {
    scx::ModuleRef http = scx::Kernel::get()->get_module("http");
    if (http.valid()) return http.module()->arg_lookup("realms");
  }

  // Article dependent sub-objects
  if ("article" == name) {
    if (m_article) {
      return new scx::ArgObject(m_article);
    } else {
      return new scx::ArgError("No article");
    }
  }
  if ("root" == name) return new scx::ArgObject(m_profile.get_index());

  return Context::arg_lookup(name);
}

//=========================================================================
scx::Arg* RenderMarkupContext::arg_method(const scx::Auth& auth,const std::string& name,scx::Arg* args)
{
  scx::ArgList* l = dynamic_cast<scx::ArgList*>(args);

  if (name == "print") {
    int n = l->size();
    for (int i=0; i<n; ++i) { 
      m_output.write(l->get(i)->get_string());
    }
    return 0;
  }

  if (name == "print_esc") {
    int n = l->size();
    for (int i=0; i<n; ++i) { 
      m_output.write(scx::escape_html(l->get(i)->get_string()));
    }
    return 0;
  }
  
  if (name == "print_json") {
    scx::ArgStore::store_arg(m_output,l->get(0));
    return 0;
  }

  if (name == "escape") {
    scx::Arg* a_str = l->get(0);
    if (!a_str) {
      return new scx::ArgString("");
    }
    return new scx::ArgString(scx::escape_html(a_str->get_string()));
  }

  if (name == "process_article") {
    if (!auth.trusted()) return new scx::ArgError("Not permitted");

    Article* art = m_article;
    scx::Arg* a_art = l->get(0);
    if (a_art) {
      scx::ArgObject* a_obj = dynamic_cast<scx::ArgObject*>(a_art);
      if (a_obj) {
        art = dynamic_cast<Article*>(a_obj->get_object());
      }
    }
    if (!art) return new scx::ArgError("No article to process");

    scx::Arg* a_section = l->get(1);
    std::string section = "";
    if (a_section) section = a_section->get_string();
    
    // Save current state and setup to process new article
    Article* orig_art = m_article; m_article = art;
    std::string orig_section = m_section; m_section = section;
    bool orig_inhibit = m_inhibit; m_inhibit = (!m_section.empty());
    bool orig_auto_number = m_auto_number;
    
    art->process(*this);

    // Restore previous state
    m_article = orig_art;
    m_section = orig_section;
    m_inhibit = orig_inhibit;
    m_auto_number = orig_auto_number;
    return 0;
  }

  if (name == "edit_article") {
    if (!auth.trusted()) return new scx::ArgError("Not permitted");

    if (m_article) {
      scx::File* file = new scx::File();
      if (scx::Ok == file->open(m_article->get_filepath(),scx::File::Read)) {
        char buffer[1024];
	int na = 0;
        while (scx::Ok == file->read(buffer,1000,na)) {
          std::string str(buffer,na);
          m_output.write(scx::escape_html(str));
        }
      }
      delete file;
    }
    return 0;
  }

  if (name == "template") {
    if (!auth.trusted()) return new scx::ArgError("Not permitted");

    const scx::ArgString* a_tpl = dynamic_cast<const scx::ArgString*>(l->get(0));
    if (!a_tpl) {
      return new scx::ArgError("Template name not specified");
    }
    Template* tpl = m_profile.lookup_template(a_tpl->get_string());
    if (!tpl) {
      return new scx::ArgError("Unknown template");
    }
    tpl->process(*this);
    return 0;
  }

  if (name == "abort") {
    if (!auth.trusted()) return new scx::ArgError("Not permitted");

    //    m_output.close();
    throw std::exception();
    return 0;
  }

  return Context::arg_method(auth,name,args);
}
