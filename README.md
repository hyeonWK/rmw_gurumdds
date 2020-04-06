# rmw_gurumdds
Implementation of the ROS middleware interface using [GurumNetworks GurumDDS](http://www.gurum.cc).

## Requirements
This project requires `rosidl_typesupport_gurumdds` to be built. For more information, see README.md of the [project](https://github.com/ros2/rosidl_typesupport_gurumdds).

## Packages
This project consists of four packages, `rmw_gurumdds_cpp`, `rmw_gurumdds_dynamic_cpp`, `rmw_gurumdds_shared_cpp` and `demo_nodes_cpp_native_gurumdds`.

### rmw_gurumdds_cpp
`rmw_gurumdds_cpp` is the rmw implementation. You can use this rmw implementation by setting environment variable `RMW_IMPLEMENTATION=rmw_gurumdds_cpp` after installation. For `rmw_gurumdds_cpp` to work properly, make sure to set environment variable `GURUMDDS_CONFIG=$GURUMDDS_HOME/gurumdds.yaml` and set `allow_loopback` variable in `gurumdds.yaml` to `true`. If you are not familiar with [YAML](https://yaml.org/), please note that YAML only supports spaces, not tabs, for indentation.  

```
DATA:
  allow_loopback: true
  dynamic_buffer: true
  mtu: auto # auto | number(1472)
  bitmap_capacity: 256
  buffer_capacity: 512
```

### rmw_gurumdds_dynamic_cpp
`rmw_gurumdds_dynamic_cpp` is another rmw implementation, which uses `rosidl_typesupport_introspection` instead of `rosidl_typesupport_gurumdds`. You can use this rmw implementation with environment variable `RMW_IMPLEMENTATION=rmw_gurumdds_dynamic_cpp`. Other settings and configurations are the same as `rmw_gurumdds_cpp`.

### rmw_gurumdds_shared_cpp
`rmw_gurumdds_shared_cpp` contains some functions used by both `rmw_gurumdds_cpp` and `rmw_gurumdds_dynamic_cpp`.

### demo_nodes_cpp_native_gurumdds
`demo_nodes_cpp_natvie_gurumdds` is similar to `demo_nodes_cpp_native` from ROS2 `demos`. This demo shows how to access the native handles of `rmw_gurumdds_cpp`.

## Branches
There are four branches in this project: master, eloquent, dashing, and crystal.  
New changes made in [ROS2 repository](https://github.com/ros2) will be applied to the master branch, so this branch might be unstable.
If you want to use this project with ROS2 Eloquent Elusor, Dashing Diademata or Crystal Clemmys, please use eloquent, dashing or crystal branch.

## Implementation Status
Currently some features are not fully implemented.
- DDS Security
