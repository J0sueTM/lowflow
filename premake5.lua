workspace 'lowflow'
  configurations { 'dbg', 'rel' }

project 'lowflow'
  kind 'ConsoleApp'
  language 'C'
  targetdir 'bin/lowflow/%{cfg.buildcfg}'
  files { 'src/**.h', 'src/**.c'}
  links { 'munit' }

  filter 'configurations:dbg'
    defines { 'LF_DBG'}
    symbols 'On'

  filter 'configurations:rel'
  optimize 'On'

project 'lowflow-lib'
  kind 'StaticLib'
  language 'C'
  targetdir 'bin/lowflow-lib/%{cfg.buildcfg}'
  files { 'src/**.h', 'src/**.c' }
  links { 'munit' }

project 'munit'
  kind 'StaticLib'
  language 'C'
  targetdir 'bin/munit/%{cfg.buildcfg}'
  files { 'libs/munit/munit.h', 'libs/munit/munit.c' }

project 'test'
  kind 'ConsoleApp'
  language 'C'
  targetdir 'bin/test/%{cfg.buildcfg}'
  files { 'test/**.c' }
  links { 'munit', 'lowflow-lib' }

  filter 'configurations:dbg'
    symbols 'On'
