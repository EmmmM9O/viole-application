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
  local files3 = os.files(path .. "/**.hpp")
    for _,file in ipairs(files3) do
    os.exec("clang-format --style='file:"..os.getenv("PWD").."/.clang-format' -i " .. file)
    print("format ", file)
    end
  local files4 = os.files(path .. "/**.mpp")
    for _,file in ipairs(files4) do
    os.exec("clang-format --style='file:"..os.getenv("PWD").."/.clang-format' -i " .. file)
    print("format ", file)
    end
end

function main(...)
	print("start format")
	local path = os.getenv("PWD")
	traverseDir(path.."/include")
	traverseDir(path.."/src")
	traverseDir(path.."/example")
	print("end format")
end
