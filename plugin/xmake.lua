set_languages("c++26")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "build" })

set_toolchains("clang")
target("viole-plugin")
set_kind("shared")
add_cxflags("-Wall", "-stdlib=libc++", "-std=c++26")
add_ldflags("-stdlib=libc++")
add_files("src/**.cpp")
add_includedirs("$(projectdir)/include/", "$(projectdir)/plugin/include/")
