--
-- _cmake.lua
-- Define the CMake action(s).
-- Copyright (c) 2015 Miodrag Milanovic
--

local cmake = premake.cmake
local tree = premake.tree

function cmake.list(value)
    if #value > 0 then
        return " " .. table.concat(value, " ")
    else
        return ""
    end
end


function cmake.files(prj)
    local tr = premake.project.buildsourcetree(prj)
    tree.traverse(tr, {
        onbranchenter = function(node, depth)
        end,
        onbranchexit = function(node, depth)
        end,
        onleaf = function(node, depth)
            _p(1, '../%s', node.cfg.name)
        end,
    }, true, 1)
end

function cmake.project(prj)
    io.indent = "  "
    _p('cmake_minimum_required(VERSION 2.8.4)')
    _p('')
    _p('project(%s)', premake.esc(prj.name))
    _p('set(')
    _p('source_list')
    cmake.files(prj)
    _p(')')

    local nativeplatform = iif(os.is64bit(), "x64", "x32")

    local cc = premake.gettool(prj)

    -- build a list of supported target platforms that also includes a generic build
    local platforms = premake.filterplatforms(prj.solution, cc.platforms, "Native")

    -- remove cross-compilers platform
    for i = #platforms, 1, -1 do
        if premake.platforms[platforms[i]].iscrosscompiler then
            table.remove(platforms, i)
        end
    end

    for _, platform in ipairs(platforms) do
        for cfg in premake.eachconfig(prj, platform) do
            if cfg.platform == nativeplatform then
                _p('if(CMAKE_BUILD_TYPE MATCHES \"%s\")', cfg.name)
                for _, v in ipairs(cfg.includedirs) do
                    _p(1, 'include_directories(../%s)', premake.esc(v))
                end
                for _, v in ipairs(cfg.defines) do
                    _p(1, 'add_definitions(-D%s)', v)
                end
                _p('endif()')
            end
        end
    end

    -- Force cpp if needed
    if (prj.options.ForceCPP) then
        _p('set_source_files_properties(${source_list} PROPERTIES LANGUAGE CXX)')
    end

    if (prj.kind == 'StaticLib') then
        _p('add_library(%s STATIC ${source_list})', premake.esc(prj.name))
    end
    if (prj.kind == 'SharedLib') then
        _p('add_library(%s SHARED ${source_list})', premake.esc(prj.name))
    end
    if (prj.kind == 'ConsoleApp' or prj.kind == 'WindowedApp') then
        _p('add_executable(%s ${source_list})', premake.esc(prj.name))
        for _, platform in ipairs(platforms) do
            for cfg in premake.eachconfig(prj, platform) do
                if cfg.platform == nativeplatform then
                    _p('if(CMAKE_BUILD_TYPE MATCHES \"%s\")', cfg.name)
                    _p(1, 'target_link_libraries(%s%s%s)', premake.esc(prj.name)
                        , cmake.list(premake.esc(premake.getlinks(cfg, "siblings", "basename")))
                        , cmake.list(cc.getlinkflags(cfg)))
                    _p('endif()')
                end
            end
        end
    end
end
