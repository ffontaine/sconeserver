/* SconeServer (http://www.sconemad.com)

Transfer stream

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

#include "sconex/StreamTransfer.h"
namespace scx {

// Uncomment to enable debug logging
//#define TRANSFER_DEBUG_LOG(m) STREAM_DEBUG_LOG(m)

#ifndef TRANSFER_DEBUG_LOG
#  define TRANSFER_DEBUG_LOG(m)
#endif

int StreamTransfer::s_tra_count = 0;
  
//=============================================================================
StreamTransfer::StreamTransfer(
  Descriptor* source_des,
  int buffer_size
) : Stream("transfer"),
    m_status(StreamTransfer::Transfer),
    m_buffer(buffer_size),
    m_close_when_finished(false),
    m_uid(++s_tra_count)
{
  DEBUG_COUNT_CONSTRUCTOR(StreamTransfer);

  m_source = new StreamTransferSource(this);
  if (source_des) {
    source_des->add_stream(m_source);
  }

  enable_event(Stream::Opening,true);
}

//=============================================================================
StreamTransfer::~StreamTransfer()
{
  if (m_source) {
    STREAM_DEBUG_LOG("Source stream still exists - sending close");
    m_source->dest_event(Stream::Closing); 
  }

  DEBUG_COUNT_DESTRUCTOR(StreamTransfer);
}

//=============================================================================
Condition StreamTransfer::read(void* buffer,int n,int& na)
{
  return Stream::read(buffer,n,na);
}

//=============================================================================
Condition StreamTransfer::write(const void* buffer,int n,int& na)
{
  return Stream::write(buffer,n,na);
}

//=============================================================================
Condition StreamTransfer::event(Event e)
{
  switch (e) {
  
    case Stream::Opening:
      return scx::Ok;

    case Stream::Writeable:
      switch (transfer()) {
        
        case Transfer:
          return scx::Wait;
          
        case Finished:
          enable_event(Stream::Writeable,false);
          // Close source stream
          if (m_source) {
            m_source->dest_event(Stream::Closing); 
            // Assume source is gone
            m_source = 0;
          }
          if (m_close_when_finished) {
            return scx::Close;
          }
          return scx::End;

        case Read_error:
        case Write_error:
          return scx::Error;
      }
      break;

    default:
      break;
  }
  
  return scx::Ok;
}

//=============================================================================
std::string StreamTransfer::stream_status() const
{
  std::ostringstream oss;
  oss << "<-[" << m_uid << "] buf:" << m_buffer.status_string();
  if (m_close_when_finished) oss << " AUTOCLOSE";
  return oss.str();
}

//=============================================================================
StreamTransfer::Status StreamTransfer::transfer()
{
  if (m_status != StreamTransfer::Transfer) {
    return m_status;
  }

  int bytes_buffered = m_buffer.used();

  if (bytes_buffered==0 && m_source) {
    Condition c_in = m_source->read(
      m_buffer.tail(),m_buffer.free(),bytes_buffered);
    m_buffer.push(bytes_buffered);
    if (c_in==scx::End && bytes_buffered==0) {
      m_status = StreamTransfer::Finished;
      TRANSFER_DEBUG_LOG("read END c_in=" << c_in);
      return m_status;

    } else if (c_in==scx::Error) {
      m_status = StreamTransfer::Read_error;
      STREAM_DEBUG_LOG("read ERROR c_in=" << c_in);
      return m_status;
    }

    TRANSFER_DEBUG_LOG("read " << bytes_buffered << " bytes");
  }

  if (bytes_buffered==0) {
    enable_event(Stream::Writeable,false);
    if (!m_source) {
      m_status = StreamTransfer::Finished;
      TRANSFER_DEBUG_LOG("read END (source gone)");
      return m_status;      
    }
    m_source->dest_event(Stream::Writeable);
    TRANSFER_DEBUG_LOG("wait (buffer empty)");
    return m_status;
  }

  int bytes_written=0;
  Condition c_out = Stream::write(
    m_buffer.head(),bytes_buffered,bytes_written);
  if (c_out==scx::Error) {
    m_status = StreamTransfer::Write_error;
    STREAM_DEBUG_LOG("write ERROR c_out=" << c_out);
    return m_status;
  }
  m_buffer.pop(bytes_written);

  endpoint().reset_timeout();
  TRANSFER_DEBUG_LOG("write " << bytes_written << " bytes");
  return m_status;
}

//=============================================================================
void StreamTransfer::set_close_when_finished(bool onoff)
{
  m_close_when_finished = onoff;
}

//=============================================================================
void StreamTransfer::source_event(Event e)
{
  switch (e) {
    
    case Stream::Readable: {
      enable_event(Stream::Writeable,true);
    } break;
    
    case Stream::Closing: {
      // Source has closed
      m_source = 0;
    } break;

    default:
      break;
  }
}

//=============================================================================
StreamTransferSource::StreamTransferSource(
  StreamTransfer* dest					   
) : Stream("transfer-src"),
    m_dest(dest),
    m_close(false),
    m_dest_uid(0)
{
  DEBUG_COUNT_CONSTRUCTOR(StreamTransferSource);

  if (m_dest) {
    m_dest_uid = m_dest->m_uid;
  }
  
  enable_event(Stream::Readable,true);
  enable_event(Stream::Opening,true);
}

//=============================================================================
StreamTransferSource::~StreamTransferSource()
{
  if (m_dest) {
    STREAM_DEBUG_LOG("Dest stream still exists - sending close");
    m_dest->source_event(Stream::Closing); 
  }

  DEBUG_COUNT_DESTRUCTOR(StreamTransferSource);
}

//=============================================================================
Condition StreamTransferSource::event(Stream::Event e)
{
  if (m_close) {
    return scx::Close;
  }

  switch (e) {
    
    case Stream::Readable:
      enable_event(Stream::Readable,false);
      
    case Stream::Closing: {
      if (m_dest) {
        m_dest->source_event(e);
      } else {
        DEBUG_LOG("StreamTransferSource::event() Dest stream has closed");
      }
    } break;

    default:
      break;
  }

  return scx::Ok;
}

//=============================================================================
std::string StreamTransferSource::stream_status() const
{
  std::ostringstream oss;
  oss << "->[" << m_dest_uid << "]";
  return oss.str();
}

//=============================================================================
void StreamTransferSource::dest_event(Event e)
{
  switch (e) {

    case Stream::Writeable: {
      enable_event(Stream::Readable,true);
    } break;
    
    case Stream::Closing: {
      // Dest has finished reading
      m_close = true;
      m_dest = 0;
      enable_event(Stream::Readable,true);
    } break;

    default:
      break;
  }
}

};
