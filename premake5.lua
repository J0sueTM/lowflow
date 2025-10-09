workspace 'lowflow'
  configurations { 'dbg', 'rel' }

project 'lowflow'
  kind 'ConsoleApp'
  language 'C'
  targetdir 'bin/lowflow/%{cfg.buildcfg}'
  files { 'src/**.h', 'src/**.c' }
  buildoptions { "-Wall", "-Werror", "-Wpedantic" }

  filter 'configurations:dbg'
    defines { 'LF_DEBUG_CONFIG' }
    symbols 'On'

  filter 'configurations:rel'
    optimize 'On'


project 'lowflow-lib'
  kind 'StaticLib'
  language 'C'
  targetdir 'lib/lowflow/%{cfg.buildcfg}'
  files { 'src/**.h', 'src/**.c' }
  buildoptions { "-Wall", "-Werror", "-Wpedantic" }

  filter 'configurations:dbg'
    symbols 'On'

  filter 'configurations:rel'
    defines { 'NDEBUG' }
    optimize 'On'


project 'munit-lib'
  kind 'StaticLib'
  language 'C'
  cdialect 'C11'
  targetdir 'lib/lowflow/%{cfg.buildcfg}'
  files { 'vendor/munit/munit.h', 'vendor/munit/munit.c' }

  filter 'configurations:dbg'
    symbols 'On'

  filter 'configurations:rel'
    defines { 'NDEBUG' }
    optimize 'On'


project 'tests'
  kind 'ConsoleApp'
  language 'C'
  targetdir 'bin/tests/%{cfg.buildcfg}'
  files { 'tests/**.c' }
  links { 'munit-lib', 'lowflow-lib' }
  filter 'configurations:dbg'
    symbols 'On'

  filter 'configurations:rel'
    defines { 'NDEBUG' }
    optimize 'On'
