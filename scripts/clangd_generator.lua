local lfs = require("lfs")
local cwd = lfs.currentdir()
print("workspace: " .. cwd)
if #arg < 1 then
	print("'gen' to gen from template")
	print("'template' to set template")
	os.exit()
end
function escape_pattern(s)
	local result = ""
	for i = 1, #s do
		local c = s:sub(i, i)
		if
			c == "."
			or c == "%"
			or c == "+"
			or c == "-"
			or c == "*"
			or c == "?"
			or c == "["
			or c == "]"
			or c == "^"
			or c == "$"
		then
			if c == "%" then
				result = result .. "%%"
			else
				result = result .. "%" .. c
			end
		else
			result = result .. c
		end
	end
	return result
end

if arg[1] == "gen" then
	local src_file = assert(io.open(cwd .. "/.clangd_template", "rb"))
	local dst_file = assert(io.open(cwd .. "/.clangd", "wb"))
	local content = src_file:read("*all")
	content = string.gsub(content, "@PROJECT", cwd)
	dst_file:write(content)
	src_file:close()
	dst_file:close()
elseif arg[1] == "template" then
	local src_file = assert(io.open(cwd .. "/.clangd", "rb"))
	local dst_file = assert(io.open(cwd .. "/.clangd_template", "wb"))
	local content = src_file:read("*all")
	content = string.gsub(content, escape_pattern(cwd), "@PROJECT")
	dst_file:write(content)
	src_file:close()
	dst_file:close()
else
	print("error arg")
	print("'gen' to gen from template")
	print("'template' to set template")
end
