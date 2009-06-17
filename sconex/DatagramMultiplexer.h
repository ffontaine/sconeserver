/* SconeServer (http://www.sconemad.com)

sconex Datagram Multiplexer

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

#ifndef scxDatagramMultiplexer_h
#define scxDatagramMultiplexer_h

#include "sconex/sconex.h"
#include "sconex/Stream.h"
namespace scx {

class DatagramSocket;
class DatagramChannel;
class Module;

//=============================================================================
class DatagramMultiplexer : public Stream {
public:
  
  DatagramMultiplexer(Module& router,const std::string& chain);
  virtual ~DatagramMultiplexer();
  
  virtual scx::Condition event(scx::Stream::Event e);
  virtual std::string stream_status() const;

  void notify_close(const std::string& addr_remote);

private:

  Module& m_router;
  std::string m_chain;
  std::map<std::string,DatagramChannel*> m_channels;
};


};
#endif