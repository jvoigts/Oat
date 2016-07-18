//******************************************************************************
//* File:   ProgramOptions.h
//* Author: Jon Newman <jpnewman snail mit dot edu>
//*
//* Copyright (c) Jon Newman (jpnewman snail mit dot edu)
//* All right reserved.
//* This file is part of the Oat project.
//* This is free software: you can redistribute it and/or modify
//* it under the terms of the GNU General Public License as published by
//* the Free Software Foundation, either version 3 of the License, or
//* (at your option) any later version.
//* This software is distributed in the hope that it will be useful,
//* but WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//* GNU General Public License for more details.
//* You should have received a copy of the GNU General Public License
//* along with this source code.  If not, see <http://www.gnu.org/licenses/>.
//*******************************************************************************

#ifndef OAT_PROGRAM_OPTIONS
#define	OAT_PROGRAM_OPTIONS

#include "OatConfig.h" // Generated by CMake

#include <memory>
#include <boost/program_options.hpp>

namespace oat {
namespace config {

namespace po = boost::program_options;

static const char * VERSION_STRING =
    Oat_VERSION_MAJOR
    "."
    Oat_VERSION_MINOR
    "\n"
    "Written by Jonathan P. Newman in the MWL@MIT.\n"
    "Licensed under the GPL3.0.\n";

/** 
 * @brief Read only wrapper for program options descriptions common to all oat
 * components (e.g. --help, --version, etc).
 * @note Singleton
 */
class ComponentInfo
{
public:
    static ComponentInfo * instance(); 
    po::options_description get() { return *desc; }

private:
    ComponentInfo();

    static ComponentInfo * inst; 
    std::unique_ptr<po::options_description> desc;
};

/** 
 * @brief Extract a file/key pair from a variable map. 
 * @param map Variable map to search for config file/key option.
 * @param key Key specifying location of file/key pair within the variable map. 
 * @return Extracted config file/key pair. 
 */
std::vector<std::string> 
extractConfigFileKey(po::variables_map map, const char *key="config");

}      /* namespace config */
}      /* namespace oat */
#endif /* OAT_PROGRAM_OPTIONS */
