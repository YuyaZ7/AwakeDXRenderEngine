BuildProject({
    projectName = "AwakeRenderEngine",
    projectType = "binary",
    debugEvent = function()
        add_defines("_DEBUG")
    end,
    releaseEvent = function()
        add_defines("NDEBUG")
    end,
    exception = true
})
add_defines("_XM_NO_INTRINSICS_=1", "NOMINMAX", "UNICODE", "m128_f32=vector4_f32", "m128_u32=vector4_u32", "IMGUI_DISABLE_SSE", "TINYOBJLOADER_IMPLEMENTATION")
--add_defines("_mm_set_ss", "_mm_rsqrt_ss", "_mm_cvtss_f32")
add_files("**.cpp")
add_files("../vendor/imgui/**.cpp")
add_includedirs("../vendor")
add_includedirs("../res")
add_includedirs("./")
add_includedirs("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um")
add_includedirs("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/ucrt")
add_includedirs("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/winrt")
add_linkdirs("../dependency")
if is_mode("debug") then
add_links("assimp/Debug/assimp-vc143-mtd")
add_links("zlib/Debug/zlibstaticd")
add_linkdirs("../dependency/assimp/Debug")

else
add_links("assimp/Release/assimp-vc143-mt")
add_links("zlib/Release/zlibstatic")
add_linkdirs("../dependency/assimp/Release")
end

add_syslinks("User32", "kernel32", "Gdi32", "Shell32", "DXGI", "D3D12", "D3DCompiler","comdlg32","Advapi32","Comdlg32")

after_build(function(target)
    src_path = "shader/"
    os.cp(src_path .. "*", target:targetdir() .. "/shader/")
end)