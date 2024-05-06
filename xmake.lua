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

includes("example/**.lua")
