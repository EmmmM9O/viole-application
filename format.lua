function traverseDir(path)
	local files = os.files(path .. "/**.cpp")
	for _, file in ipairs(files) do
		os.exec("clang-format --style='file:"..os.getenv("PWD").."/.clang-format' -i " .. file)
		print("format ", file)
	end
  local files2 = os.files(path .. "/**.h")
    for _,file in ipairs(files2) do
    os.exec("clang-format --style='file:"..os.getenv("PWD").."/.clang-format' -i " .. file)
    print("format ", file)
    end
  local files3 = os.files(path .. "/**.cppm")
    for _,file in ipairs(files3) do
    os.exec("clang-format --style='file:"..os.getenv("PWD").."/.clang-format' -i " .. file)
    print("format ", file)
    end
  local files4 = os.files(path .. "/**.ixx")
    for _,file in ipairs(files4) do
    os.exec("clang-format --style='file:"..os.getenv("PWD").."/.clang-format' -i " .. file)
    print("format ", file)
    end
end

function main(...)
	print("start format")
	local path = os.getenv("PWD")
	traverseDir(path.."/src")
	traverseDir(path.."/include")
	traverseDir(path.."/test")
	traverseDir(path.."/example")
	traverseDir(path.."/third_party/stdmodules/")
	print("end format")
end
