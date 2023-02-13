print("===TESTING gmod_longjmp===")

local _,jit=pcall(require,'jit')
jit=_G.jit or (jit and jit~=true and jit or _G.jit or {status=function() end})
print("lua: INFO",jit.version,jit.arch,jit.status())

jit.on()
jit.on(true,true)


local longjmp=require'longjmp'
longjmp=longjmp~=true and longjmp or _G.longjmp 
assert(longjmp.timed(function() return "hi" end),"expected return")
local OK=true
print("lua: Calling infinite loop")
local function long_running()
	 local a=os.time()
	 while os.time()-a<5 do longjmp.dummy() end
	OK=false
print("lua: SHOULD NOT PRINT!!!!!!!!!!!")
	 return "ret" 
end

local function docall()
	print("lua: pcall of longjmp.timed returned: ",pcall(longjmp.timed,long_running))
end
docall()

if OK then
print("lua: Calling jitted infinite loop")
local function long_running()
	local n=0
	while n<3e9 do n=n+1 end
end

local function docall()
	print("lua: pcall of longjmp.timed returned: ",pcall(longjmp.timed,long_running))
end
docall()



print("lua: Calling complex function")
local function long_running()
	while true do
		if player then player.GetAll()[1]:SteamID() else longjmp.class_cpp() end
	end
end

local function docall()
	print("lua: pcall of longjmp.timed returned: ",pcall(longjmp.timed,long_running))
end
docall()
docall()



end



if os.execute then
	os.execute("sleep 1")
end

print("lua: EOF")
