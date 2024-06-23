target("test-filesystem")
set_kind("binary")
add_cxflags("-Wall", "-stdlib=libc++", "-std=c++23","-fplugin=$(projectdir)/plugin/dist/libviole-plugin.so")
add_ldflags("-stdlib=libc++")
add_includedirs("$(projectdir)/include/")
add_files("*.cpp")
add_deps("viole")
