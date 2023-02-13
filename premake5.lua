	
		
function os.winSdkVersion()
        local reg_arch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
        local sdk_version = os.getWindowsRegistry( "HKLM:SOFTWARE" .. reg_arch .."Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )
        if sdk_version ~= nil then return sdk_version end
end
print"wtf\n\n\n\n"
solution "longjmp"

        language "C++"
        location ( "solutions/" .. os.target() .. "-" .. _ACTION )
        flags { "NoPCH", "NoImportLib"}
        optimize "Off"
        vectorextensions "SSE"
        symbols "On"
        floatingpoint "Fast"
        editandcontinue "Off"
        targetdir ( "out/" .. os.target() .. "/" )
        includedirs {  	"/usr/include/libunwind/", 
						"/usr/include/luajit-2.1/",
						"/usr/include/x86_64-linux-gnu/c++/10/",
						"/usr/include/c++/10/",
						"src/"
                                        }

        if os.target() == "macosx" or os.target() == "linux" then

                buildoptions { "-std=c++11 -fPIC -fpermissive" }
                linkoptions { " -fPIC" }

        end

        configurations { "Release" }
        platforms { "x86", "x86_64" }

        defines { "DEBUG" }
        optimize "Off"
        floatingpoint "Fast"

        if os.target() == "windows" then
                defines{ "WIN32" }
        elseif os.target() == "linux" then
                defines{ "LINUX" }
        end

        local platform
        if os.target() == "windows" then
                platform = "win"
                links { "unwind" }
        elseif os.target() == "macosx" then
                platform = "osx"
                links { "unwind" }
        elseif os.target() == "linux" then
                platform = "linux"
                links { "unwind","iberty","pthread","rt" }
        else
                error "Unsupported platform."
        end



        filter "platforms:x86"
                architecture "x86"
                libdirs { "lib/" .. os.target() }
                if os.target() == "windows" then
                        targetname( "gmsv_longjmp_" .. platform .. "32")
                else
                        targetname( "gmsv_longjmp_" .. platform)
                end
        filter "platforms:x86_64"
                architecture "x86_64"
                libdirs { "lib64/" .. os.target() }
                targetname( "gmsv_longjmp_" .. platform .. "64")

        filter {"system:windows", "action:vs*"}
                systemversion((os.winSdkVersion() or "10.0.16299") .. ".0")
                toolset "v141"
				

        project "longjmp"
                files{ "src/**.c", "src/**.cpp" }
                kind "SharedLib"
                targetprefix ("")
                targetextension (".dll")
                targetdir("out/" .. os.target())
