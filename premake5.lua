-- Copyright (C) Josué Teodoro Moreira
--  
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--  
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--  
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.

workspace 'lowflow'
  configurations { 'dbg', 'rel' }

project 'lowflow'
  kind 'ConsoleApp'
  language 'C'
  targetdir 'bin/lowflow/%{cfg.buildcfg}'
  files { 'src/**.h', 'src/**.c'}
  links { 'munit' }

  filter 'configurations:dbg'
    symbols 'On'

  filter 'configurations:rel'
  defines { 'NDEBUG'}
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

  filter 'configurations:rel'
  defines { 'NDEBUG'}
  optimize 'On'

project 'test'
  kind 'ConsoleApp'
  language 'C'
  targetdir 'bin/test/%{cfg.buildcfg}'
  files { 'test/**.c' }
  links { 'munit', 'lowflow-lib' }

  filter 'configurations:dbg'
    symbols 'On' 
