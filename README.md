# TinyParser-Mitsuba [![GitHub Tag](https://img.shields.io/github/tag/PearCoding/TinyParser-Mitsuba.svg)](https://github.com/PearCoding/TinyParser-Mitsuba/releases) [![Build Status](https://travis-ci.com/PearCoding/TinyParser-Mitsuba.svg?branch=master)](https://travis-ci.com/PearCoding/TinyParser-Mitsuba)

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/PearCoding/TinyParser-Mitsuba/master/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/PearCoding/TinyParser-Mitsuba.svg)](https://github.com/PearCoding/TinyParser-Mitsuba/issues)
[![Coverage Status](https://coveralls.io/repos/github/PearCoding/TinyParser-Mitsuba/badge.svg?branch=master)](https://coveralls.io/github/PearCoding/TinyParser-Mitsuba?branch=master)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/PearCoding/TinyParser-Mitsuba.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/PearCoding/TinyParser-Mitsuba/alerts/)
![CMake 3.9](https://img.shields.io/badge/CMake-3.9+-green.svg)
![Language](https://img.shields.io/badge/language-c++-blue.svg)
![C++ Standard](https://img.shields.io/badge/std-c++11-blue.svg)

A tiny C++11 parser for scene files similar to the ones used by the Mitsuba renderer. This library only parses the XML files, distributes to easy to use Object Oriented structures, but does not generate any images. External resources, like images and shapes, are not loaded, but checked for existence - allowing lazy loading in later stages of the renderer. One exception are spectrum .spd files, which are loaded directly.

This project is not affiliated with the Mitsuba renderer and was developed to allow easy comparisons between different raytracers using one common scene file. This does not necessarily mean a raytracer is capable of rendering all present features however.

Mitsuba renderer:
 - 0.6  <https://github.com/mitsuba-renderer/mitsuba>
 - 2.0+ <https://github.com/mitsuba-renderer/mitsuba2>

## Dependencies

Using the library does not need any external libraries except the standard template library (STL). However, building from source requires the embedded TinyXML2 library, which is taken care of automaticly. For internal testing the Catch2 is required, but also included as a submodule. 

- TinyXML2 <https://github.com/leethomason/tinyxml2> (submodule)
- Catch2 <https://github.com/catchorg/Catch2> (submodule)
