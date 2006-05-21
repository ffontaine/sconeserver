/* SconeServer (http://www.sconemad.com)

Time and TimeZone

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


#include "sconex/Time.h"
#include "sconex/Date.h"
#include "sconex/utils.h"
namespace scx {

//=============================================================================
Time::Time(time_t t)
  : m_time(t)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
}

//=============================================================================
Time::Time(int minutes,int seconds)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
  m_time = (abs(minutes)*SECONDS_PER_MINUTE) + 
           abs(seconds);
  m_time *= (minutes<0) ? -1:1;
}

//=============================================================================
Time::Time(int hours,int minutes,int seconds)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
  m_time = (abs(hours)*SECONDS_PER_HOUR) + 
           (abs(minutes)*SECONDS_PER_MINUTE) + 
           abs(seconds);
  m_time *= (hours<0) ? -1:1;
}

//=============================================================================
Time::Time(int days,int hours,int minutes,int seconds)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
  m_time = (abs(days)*SECONDS_PER_DAY) + 
           (abs(hours)*SECONDS_PER_HOUR) + 
           (abs(minutes)*SECONDS_PER_MINUTE) + 
           abs(seconds);
  m_time *= (days<0) ? -1:1;
}

//=============================================================================
Time::Time(int weeks,int days,int hours,int minutes,int seconds)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
  m_time = (abs(weeks)*SECONDS_PER_WEEK) + 
           (abs(days)*SECONDS_PER_DAY) + 
           (abs(hours)*SECONDS_PER_HOUR) + 
           (abs(minutes)*SECONDS_PER_MINUTE) + 
           abs(seconds);
  m_time *= (weeks<0) ? -1:1;
}

//=============================================================================
Time::Time(const Time& c)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
  m_time=c.m_time;
}

struct time_tok {
  int value;
  std::string sep;
};

//=============================================================================
Time::Time(const std::string& str)
{
  DEBUG_COUNT_CONSTRUCTOR(Time);
  m_time=0;
  std::queue<time_tok> ts;
  std::string::const_iterator it = str.begin();
  time_tok cur;
  cur.value=0;
  int unq=0;
  int sign=1;
  bool got=false;

  while (it < str.end()) {

    if (isdigit(*it)) {
      if (got) {
        ts.push(cur);
        unq += (!isalpha(cur.sep[0]));
      }
      cur.value=0; cur.sep="";
      std::string::const_iterator it1(it);
      do ++it; while (it < str.end() && isdigit(*it));
      std::string token(it1,it);
      cur.value = atoi(token.c_str());
      got=true;

    } else if (isalpha(*it)) {
      std::string::const_iterator it1(it);
      do ++it; while (it < str.end() && isalpha(*it));
      cur.sep = std::string(it1,it);
      strup(cur.sep);
      
    } else if ((*it)=='-') {
      sign = -1;
      ++it;

    } else {
      ++it;
    }
  }
  if (got) {
    ts.push(cur);
    unq += (!isalpha(cur.sep[0]));
  }

  int seq=2;
  if (unq>0 && unq<6) seq = 5-unq;

  while (!ts.empty()) {
    const time_tok& cur = ts.front();
    if (cur.sep=="S" || cur.sep=="SEC" ||
        cur.sep=="SECS" || cur.sep=="SECOND" || cur.sep=="SECONDS") {
      seq=4;
    } else if (cur.sep=="M" || cur.sep=="MIN" ||
               cur.sep=="MINS" || cur.sep=="MINUTE" || cur.sep=="MINUTES") {
      seq=3;
    } else if (cur.sep=="H" || cur.sep=="HR" ||
               cur.sep=="HRS" || cur.sep=="HOUR" || cur.sep=="HOURS") {
      seq=2;
    } else if (cur.sep=="D" || cur.sep=="DY" ||
               cur.sep=="DYS" || cur.sep=="DAY" || cur.sep=="DAYS") {
      seq=1;
    } else if (cur.sep=="W" || cur.sep=="WK" ||
               cur.sep=="WKS" || cur.sep=="WEEK" || cur.sep=="WEEKS") {
      seq=0;
    }
    switch (seq++) {
      case 0: m_time += sign*cur.value*SECONDS_PER_WEEK; break;
      case 1: m_time += sign*cur.value*SECONDS_PER_DAY; break;
      case 2: m_time += sign*cur.value*SECONDS_PER_HOUR; break;
      case 3: m_time += sign*cur.value*SECONDS_PER_MINUTE; break;
      case 4: m_time += sign*cur.value; break;
    }
    ts.pop();
  }
}

//=============================================================================
Time::~Time()
{
  DEBUG_COUNT_DESTRUCTOR(Time);
}

//=============================================================================
int Time::seconds() const
{
  return (int)m_time;
}

//=============================================================================
double Time::to_weeks() const
{
  return m_time / (double)SECONDS_PER_WEEK;
}

//=============================================================================
double Time::to_days() const
{
  return m_time / (double)SECONDS_PER_DAY;
}

//=============================================================================
double Time::to_hours() const
{
  return m_time / (double)SECONDS_PER_HOUR;
}

//=============================================================================
double Time::to_minutes() const
{
  return m_time / (double)SECONDS_PER_MINUTE;
}

//=============================================================================
void Time::get(int& minutes,int& seconds) const
{
  seconds = m_time;
  
  minutes = (int)(seconds / SECONDS_PER_MINUTE);
  seconds -= minutes*SECONDS_PER_MINUTE;
}

//=============================================================================
void Time::get(int& hours,int& minutes,int& seconds) const
{
  seconds = m_time;

  hours = (int)(seconds / SECONDS_PER_HOUR);
  seconds -= hours*SECONDS_PER_HOUR;
  
  minutes = (int)(seconds / SECONDS_PER_MINUTE);
  seconds -= minutes*SECONDS_PER_MINUTE;
}

//=============================================================================
void Time::get(int& days,int& hours,int& minutes,int& seconds) const
{
  seconds = m_time;

  days = (int)(seconds / SECONDS_PER_DAY);
  seconds -= days*SECONDS_PER_DAY;

  hours = (int)(seconds / SECONDS_PER_HOUR);
  seconds -= hours*SECONDS_PER_HOUR;
  
  minutes = (int)(seconds / SECONDS_PER_MINUTE);
  seconds -= minutes*SECONDS_PER_MINUTE;
}

//=============================================================================
void Time::get(int& weeks,int& days,int& hours,int& minutes,int& seconds) const
{
  seconds = m_time;

  weeks = (int)(seconds / SECONDS_PER_WEEK);
  seconds -= weeks*SECONDS_PER_WEEK;

  days = (int)(seconds / SECONDS_PER_DAY);
  seconds -= days*SECONDS_PER_DAY;

  hours = (int)(seconds / SECONDS_PER_HOUR);
  seconds -= hours*SECONDS_PER_HOUR;
  
  minutes = (int)(seconds / SECONDS_PER_MINUTE);
  seconds -= minutes*SECONDS_PER_MINUTE;
}

//=============================================================================
std::string Time::string() const
{
  int weeks,days,hours,minutes,seconds;
  get(weeks,days,hours,minutes,seconds);
  weeks = abs(weeks);
  days = abs(days);
  hours = abs(hours);
  minutes = abs(minutes);
  seconds = abs(seconds);

  std::ostringstream oss;
  oss << (m_time < 0 ? "-" : "");
  
  if (weeks > 0) {
    oss << weeks << " " << (weeks==1 ? "Week" : "Weeks") << " ";
  }

  if (weeks > 0 || days > 0) {
    oss << days << " " << (days==1 ? "Day" : "Days") << " ";
  }
  
  oss << std::setfill('0') << std::setw(2) << hours << ":"
      << std::setfill('0') << std::setw(2) << minutes << ":"
      << std::setfill('0') << std::setw(2) << seconds;
  
  return oss.str();
}

//=============================================================================
Time Time::operator+(const Time& t) const
{
  return Time(m_time+t.m_time);
}

//=============================================================================
Time Time::operator-(const Time& t) const
{
  return Time(m_time-t.m_time);
}

std::map<std::string,int>* TimeZone::s_zone_table = 0;

//=============================================================================
TimeZone::TimeZone(time_t t)
  : Time(t)
{
  DEBUG_COUNT_CONSTRUCTOR(TimeZone);
  init_tables();
}

//=============================================================================
TimeZone::TimeZone(int hours,int minutes)
{
  DEBUG_COUNT_CONSTRUCTOR(TimeZone);
  init_tables();
  m_time = (abs(hours)*SECONDS_PER_HOUR) +
           (abs(minutes)*SECONDS_PER_MINUTE);
  m_time *= (hours<0 || minutes<0) ? -1:1;
}

//=============================================================================
TimeZone::TimeZone(const std::string& str)
{
  DEBUG_COUNT_CONSTRUCTOR(TimeZone);
  init_tables();
  m_time=0;

  // First lookup in zone table
  std::map<std::string,int>::const_iterator zit =
    s_zone_table->find(str);
  if (zit != s_zone_table->end()) {
    m_time = (*zit).second;
    
  } else {
    std::string::const_iterator it = str.begin();
    int sign=1;

    while (it < str.end()) {

      if (isdigit(*it)) {
        std::string::const_iterator it1(it);
        do ++it; while (it < str.end() && isdigit(*it));
        std::string token(it1,it);
        int value = atoi(token.c_str());
        int hours = value/100;
        int minutes = value - (hours*100);
        m_time = (hours*SECONDS_PER_HOUR) + (minutes*SECONDS_PER_MINUTE);
        break;
        
      } else if (isalpha(*it)) {
        std::string::const_iterator it1(it);
        do ++it; while (it < str.end() && isalpha(*it));
        std::string token(it1,it);
        strup(token);
        
      } else if ((*it)=='-') {
        sign = -1;
        ++it;
        
      } else {
        ++it;
      }
    }
    m_time *= sign;
  }

}

//=============================================================================
TimeZone::~TimeZone()
{
  DEBUG_COUNT_DESTRUCTOR(TimeZone);
}

//=============================================================================
TimeZone TimeZone::utc()
{
  return TimeZone(0);
}

//=============================================================================
TimeZone TimeZone::local(const Date& date)
{
  struct tm* tms = 0;
  const time_t t = date.epoch_seconds();

  // Get local time on this date
  tms = localtime(&t);
  if (tms == 0) {
    return TimeZone();
  }
  tms->tm_isdst=-1;
  time_t t_loc = mktime(tms);

  // Get universal time on this date
  tms = gmtime(&t);
  if (tms == 0) {
    return TimeZone();
  }
  tms->tm_isdst=-1;
  time_t t_gmt = mktime(tms);

  // Local timezone is then the difference between these
  return TimeZone(t_loc - t_gmt);
}

//=============================================================================
TimeZone TimeZone::operator+(const Time& t) const
{
  return TimeZone(m_time+t.m_time);
}

//=============================================================================
TimeZone TimeZone::operator-(const Time& t) const
{
  return TimeZone(m_time-t.m_time);
}

//=============================================================================
std::string TimeZone::string() const
{
  int hours,minutes,seconds;
  get(hours,minutes,seconds);
  hours = abs(hours);
  minutes = abs(minutes);

  std::ostringstream oss;
  oss << (m_time < 0 ? "-" : "+")
      << std::setfill('0') << std::setw(2) << hours
      << std::setfill('0') << std::setw(2) << minutes;

  return oss.str();
}

//=============================================================================
void TimeZone::init_tables()
{
  if (s_zone_table == 0) {
    s_zone_table = new std::map<std::string,int>;
#   define ADD_TZ(name,hrs) \
      (*s_zone_table)[name] = (int)((double)SECONDS_PER_HOUR * hrs);
    
    ADD_TZ("AKST",-9);
    ADD_TZ("AKDT",-8);
    ADD_TZ("PST", -8);
    ADD_TZ("PDT", -7);
    ADD_TZ("MST", -7);
    ADD_TZ("MDT", -6);
    ADD_TZ("CST", -6);
    ADD_TZ("CDT", -5);
    ADD_TZ("EST", -5);
    ADD_TZ("EDT", -4);
    ADD_TZ("AST", -4);
    ADD_TZ("NST", -3.5);
    ADD_TZ("ADT", -3);
    ADD_TZ("NDT", -2.5);
    
    ADD_TZ("UT",  0);
    ADD_TZ("UTC", 0);
    ADD_TZ("GMT", 0);
    ADD_TZ("WET", 0);
    ADD_TZ("BST", 1);
    ADD_TZ("IST", 1);
    ADD_TZ("WEDT",1);
    ADD_TZ("CET", 1);
    ADD_TZ("CEDT",2);
    ADD_TZ("EET", 2);
    ADD_TZ("EEDT",3);

    ADD_TZ("CXT", 7);
    ADD_TZ("AWST",8);
    ADD_TZ("JST", 9);
    ADD_TZ("ACST",9.5);
    ADD_TZ("AEST",10);
    ADD_TZ("ACDT",10.5);

    // Military
    ADD_TZ("Y",   -12);
    ADD_TZ("X",   -11);
    ADD_TZ("W",   -10);
    ADD_TZ("V*",  -9.5);
    ADD_TZ("V",   -9);
    ADD_TZ("U",   -8);
    ADD_TZ("T",   -7);
    ADD_TZ("S",   -6);
    ADD_TZ("R",   -5);
    ADD_TZ("Q",   -4);
    ADD_TZ("P*",  -3.5);
    ADD_TZ("P",   -3);
    ADD_TZ("O",   -2);
    ADD_TZ("N",   -1);
    ADD_TZ("Z",   0);
    ADD_TZ("A",   1);
    ADD_TZ("B",   2);
    ADD_TZ("C",   3);
    ADD_TZ("C*",  3.5);
    ADD_TZ("D",   4);
    ADD_TZ("D*",  4.5);
    ADD_TZ("E",   5);
    ADD_TZ("E*",  5.5);
    ADD_TZ("F",   6);
    ADD_TZ("F*",  6.5);
    ADD_TZ("G",   7);
    ADD_TZ("H",   8);
    ADD_TZ("I",   9);
    ADD_TZ("I*",  9.5);
    ADD_TZ("K",   10);
    ADD_TZ("K*",  10.5);
    ADD_TZ("L",   11);
    ADD_TZ("L*",  11.5);
    ADD_TZ("M",   12);
    ADD_TZ("M*" , 13);
    ADD_TZ("M**", 14);
  }
}

};
