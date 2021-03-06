/* Auto-generated by genmsg_cpp for file /home/masaru/fuerte_workspace/sandbox/usarsim/usarsim_inf/msg/SenseObject.msg */
#ifndef USARSIM_INF_MESSAGE_SENSEOBJECT_H
#define USARSIM_INF_MESSAGE_SENSEOBJECT_H
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
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Pose.h"

namespace usarsim_inf
{
template <class ContainerAllocator>
struct SenseObject_ {
  typedef SenseObject_<ContainerAllocator> Type;

  SenseObject_()
  : header()
  , fov(0.0)
  , object_names()
  , material_names()
  , object_poses()
  , object_hit_locations()
  {
  }

  SenseObject_(const ContainerAllocator& _alloc)
  : header(_alloc)
  , fov(0.0)
  , object_names(_alloc)
  , material_names(_alloc)
  , object_poses(_alloc)
  , object_hit_locations(_alloc)
  {
  }

  typedef  ::std_msgs::Header_<ContainerAllocator>  _header_type;
   ::std_msgs::Header_<ContainerAllocator>  header;

  typedef float _fov_type;
  float fov;

  typedef std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other >  _object_names_type;
  std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other >  object_names;

  typedef std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other >  _material_names_type;
  std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other >  material_names;

  typedef std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other >  _object_poses_type;
  std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other >  object_poses;

  typedef std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other >  _object_hit_locations_type;
  std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other >  object_hit_locations;


  typedef boost::shared_ptr< ::usarsim_inf::SenseObject_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::usarsim_inf::SenseObject_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct SenseObject
typedef  ::usarsim_inf::SenseObject_<std::allocator<void> > SenseObject;

typedef boost::shared_ptr< ::usarsim_inf::SenseObject> SenseObjectPtr;
typedef boost::shared_ptr< ::usarsim_inf::SenseObject const> SenseObjectConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::usarsim_inf::SenseObject_<ContainerAllocator> & v)
{
  ros::message_operations::Printer< ::usarsim_inf::SenseObject_<ContainerAllocator> >::stream(s, "", v);
  return s;}

} // namespace usarsim_inf

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::usarsim_inf::SenseObject_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::usarsim_inf::SenseObject_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::usarsim_inf::SenseObject_<ContainerAllocator> > {
  static const char* value() 
  {
    return "707d7392a86266abad805d802f269544";
  }

  static const char* value(const  ::usarsim_inf::SenseObject_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x707d7392a86266abULL;
  static const uint64_t static_value2 = 0xad805d802f269544ULL;
};

template<class ContainerAllocator>
struct DataType< ::usarsim_inf::SenseObject_<ContainerAllocator> > {
  static const char* value() 
  {
    return "usarsim_inf/SenseObject";
  }

  static const char* value(const  ::usarsim_inf::SenseObject_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::usarsim_inf::SenseObject_<ContainerAllocator> > {
  static const char* value() 
  {
    return "Header header\n\
float32 fov\n\
string[] object_names\n\
string[] material_names\n\
geometry_msgs/Pose[] object_poses\n\
geometry_msgs/Pose[] object_hit_locations\n\
\n\
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
================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
Point position\n\
Quaternion orientation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
";
  }

  static const char* value(const  ::usarsim_inf::SenseObject_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct HasHeader< ::usarsim_inf::SenseObject_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct HasHeader< const ::usarsim_inf::SenseObject_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::usarsim_inf::SenseObject_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.header);
    stream.next(m.fov);
    stream.next(m.object_names);
    stream.next(m.material_names);
    stream.next(m.object_poses);
    stream.next(m.object_hit_locations);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct SenseObject_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::usarsim_inf::SenseObject_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::usarsim_inf::SenseObject_<ContainerAllocator> & v) 
  {
    s << indent << "header: ";
s << std::endl;
    Printer< ::std_msgs::Header_<ContainerAllocator> >::stream(s, indent + "  ", v.header);
    s << indent << "fov: ";
    Printer<float>::stream(s, indent + "  ", v.fov);
    s << indent << "object_names[]" << std::endl;
    for (size_t i = 0; i < v.object_names.size(); ++i)
    {
      s << indent << "  object_names[" << i << "]: ";
      Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.object_names[i]);
    }
    s << indent << "material_names[]" << std::endl;
    for (size_t i = 0; i < v.material_names.size(); ++i)
    {
      s << indent << "  material_names[" << i << "]: ";
      Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.material_names[i]);
    }
    s << indent << "object_poses[]" << std::endl;
    for (size_t i = 0; i < v.object_poses.size(); ++i)
    {
      s << indent << "  object_poses[" << i << "]: ";
      s << std::endl;
      s << indent;
      Printer< ::geometry_msgs::Pose_<ContainerAllocator> >::stream(s, indent + "    ", v.object_poses[i]);
    }
    s << indent << "object_hit_locations[]" << std::endl;
    for (size_t i = 0; i < v.object_hit_locations.size(); ++i)
    {
      s << indent << "  object_hit_locations[" << i << "]: ";
      s << std::endl;
      s << indent;
      Printer< ::geometry_msgs::Pose_<ContainerAllocator> >::stream(s, indent + "    ", v.object_hit_locations[i]);
    }
  }
};


} // namespace message_operations
} // namespace ros

#endif // USARSIM_INF_MESSAGE_SENSEOBJECT_H

