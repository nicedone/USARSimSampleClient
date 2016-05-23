/* Auto-generated by genmsg_cpp for file /home/masaru/fuerte_workspace/sandbox/usarsim/usarsim_inf/msg/EffectorStatus.msg */
#ifndef USARSIM_INF_MESSAGE_EFFECTORSTATUS_H
#define USARSIM_INF_MESSAGE_EFFECTORSTATUS_H
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/time.h"

#include "ros/macros.h"

#include "ros/assert.h"

#include "std_msgs/Header.h"

namespace usarsim_inf
{
template <class ContainerAllocator>
struct EffectorStatus_ {
  typedef EffectorStatus_<ContainerAllocator> Type;

  EffectorStatus_()
  : header()
  , state(0)
  {
  }

  EffectorStatus_(const ContainerAllocator& _alloc)
  : header(_alloc)
  , state(0)
  {
  }

  typedef  ::std_msgs::Header_<ContainerAllocator>  _header_type;
   ::std_msgs::Header_<ContainerAllocator>  header;

  typedef uint8_t _state_type;
  uint8_t state;

  enum { OPEN = 0 };
  enum { CLOSE = 1 };

  typedef boost::shared_ptr< ::usarsim_inf::EffectorStatus_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::usarsim_inf::EffectorStatus_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct EffectorStatus
typedef  ::usarsim_inf::EffectorStatus_<std::allocator<void> > EffectorStatus;

typedef boost::shared_ptr< ::usarsim_inf::EffectorStatus> EffectorStatusPtr;
typedef boost::shared_ptr< ::usarsim_inf::EffectorStatus const> EffectorStatusConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::usarsim_inf::EffectorStatus_<ContainerAllocator> & v)
{
  ros::message_operations::Printer< ::usarsim_inf::EffectorStatus_<ContainerAllocator> >::stream(s, "", v);
  return s;}

} // namespace usarsim_inf

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::usarsim_inf::EffectorStatus_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::usarsim_inf::EffectorStatus_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::usarsim_inf::EffectorStatus_<ContainerAllocator> > {
  static const char* value() 
  {
    return "98e54041312d158639ac0a333985e48c";
  }

  static const char* value(const  ::usarsim_inf::EffectorStatus_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x98e54041312d1586ULL;
  static const uint64_t static_value2 = 0x39ac0a333985e48cULL;
};

template<class ContainerAllocator>
struct DataType< ::usarsim_inf::EffectorStatus_<ContainerAllocator> > {
  static const char* value() 
  {
    return "usarsim_inf/EffectorStatus";
  }

  static const char* value(const  ::usarsim_inf::EffectorStatus_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::usarsim_inf::EffectorStatus_<ContainerAllocator> > {
  static const char* value() 
  {
    return "Header header\n\
uint8 state\n\
uint8 OPEN=0\n\
uint8 CLOSE=1\n\
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.secs: seconds (stamp_secs) since epoch\n\
# * stamp.nsecs: nanoseconds since stamp_secs\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
";
  }

  static const char* value(const  ::usarsim_inf::EffectorStatus_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct HasHeader< ::usarsim_inf::EffectorStatus_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct HasHeader< const ::usarsim_inf::EffectorStatus_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::usarsim_inf::EffectorStatus_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.header);
    stream.next(m.state);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct EffectorStatus_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::usarsim_inf::EffectorStatus_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::usarsim_inf::EffectorStatus_<ContainerAllocator> & v) 
  {
    s << indent << "header: ";
s << std::endl;
    Printer< ::std_msgs::Header_<ContainerAllocator> >::stream(s, indent + "  ", v.header);
    s << indent << "state: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.state);
  }
};


} // namespace message_operations
} // namespace ros

#endif // USARSIM_INF_MESSAGE_EFFECTORSTATUS_H
