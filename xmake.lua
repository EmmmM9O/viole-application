add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
add_rules("mode.debug", "mode.release")
includes("third_party/**.lua")
set_languages("c++2b")
task("format")
set_category("plugin")
on_run("format")
set_menu({
	usage = "xmake format",
	description = "Format source code",
})
task_end()
target("core")
	set_kind("static")
	add_files("src/**.mpp", { install = true })
	add_files("src/**.cpp")
	set_policy("build.c++.modules", true)

includes("example/**.lua")
