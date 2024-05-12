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
set_languages("c++2b")

target("viole")
	set_kind("static")
	add_cxflags("-Wall","-stdlib=libc++","-fprebuilt-module-path=build/.gens/viole/linux/arm64-v8a/release/rules/modules/cache/fa597cf7/")
	add_ldflags("-stdlib=libc++")
	add_includedirs("$(projectdir)")
	add_files("src/**.ixx", { install = true })
	add_files("src/**.cpp")
	set_policy("build.c++.modules", true)

includes("example/**.lua")
