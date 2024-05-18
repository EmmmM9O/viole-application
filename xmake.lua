add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
add_rules("mode.debug", "mode.release")
set_toolset("cxx", "clang++")
set_toolset("ld", "clang++")
task("format")
set_category("plugin")
on_run("format")
set_menu({
	usage = "xmake format",
	description = "Format source code",
})
task_end()
includes("third_party/**.lua")
set_languages("c++26")

target("viole")
	set_kind("static")
	add_cxflags("-Wall","-stdlib=libc++")
	add_ldflags("-stdlib=libc++")
	add_includedirs("$(projectdir)/include/")
	add_files("src/**.cpp")

includes("example/**.lua")
