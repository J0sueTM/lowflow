workspace 'lowflow'
configurations { 'dbg', 'rel' }

project 'lowflow'
kind 'ConsoleApp'
language 'C'
targetdir 'bin/lowflow/%{cfg.buildcfg}'
files { 'src/**.h', 'src/**.c' }
-- links { 'munit', 'log.c' }
buildoptions { "-Wall", "-Werror", "-Wpedantic" }

filter 'configurations:dbg'
symbols 'On'

filter 'configurations:rel'
defines { 'NDEBUG' }
optimize 'On'
