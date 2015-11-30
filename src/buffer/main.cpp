//******************************************************************************
//* File:   oat framefilt main.cpp
//* Author: Jon Newman <jpnewman snail mit dot edu>
//
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
//****************************************************************************

#include "OatConfig.h" // Generated by CMake

#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <boost/program_options.hpp>
#include <boost/interprocess/exceptions.hpp>

#include "../../lib/utility/IOFormat.h"
#include "../../lib/datatypes/Position2D.h"

#include "Buffer.h"
#include "TokenBuffer.h"
#include "FrameBuffer.h"

namespace po = boost::program_options;

volatile sig_atomic_t quit = 0;
volatile sig_atomic_t source_eof = 0;

void printUsage(po::options_description options){
    std::cout << "Usage: buffer [INFO]\n"
              << "   or: buffer TYPE SOURCE SINK [CONFIGURATION]\n"
              << "Place tokens from SOURCE into a FIFO. Publish tokens in "
              << "FIFO to SINK.\n\n"
              << "TYPE\n"
              << "  frame: Frame buffer\n"
              << "  pos2D: 2D Position buffer\n"
              << "SOURCE:\n"
              << "  User-supplied name of the memory segment to receive tokens "
              << "from (e.g. input).\n\n"
              << "SINK:\n"
              << "  User-supplied name of the memory segment to publish tokens "
              << "to (e.g. output).\n\n"
              << options << "\n";
}

// Signal handler to ensure shared resources are cleaned on exit due to ctrl-c
void sigHandler(int) {
    quit = 1;
}

// Processing loop
void run(const std::shared_ptr<oat::Buffer>& buffer) {

    try {

        buffer->connectToNode();

        while (!quit && !source_eof) {
            source_eof = buffer->push();
        }

    } catch (const boost::interprocess::interprocess_exception &ex) {

        // Error code 1 indicates a SIGINT during a call to wait(), which
        // is normal behavior
        if (ex.get_error_code() != 1)
            throw;
    }
}

int main(int argc, char *argv[]) {

    std::signal(SIGINT, sigHandler);

    std::string type;
    std::string source;
    std::string sink;
    po::options_description visible_options("OPTIONS");

    std::unordered_map<std::string, char> type_hash;
    type_hash["frame"] = 'a';
    type_hash["pos2D"] = 'b';

    try {

        po::options_description options("INFO");
        options.add_options()
                ("help", "Produce help message.")
                ("version,v", "Print version information.")
                ;

        po::options_description hidden("HIDDEN OPTIONS");
        hidden.add_options()
                ("type", po::value<std::string>(&type),
                "Type of frame filter to use.\n\n"
                "Values:\n"
                "  frame: Frame buffer.\n"
                "  pos2D: 2D position buffer.")
                ("source", po::value<std::string>(&source),
                 "The name of the SOURCE that supplies tokens to buffer.")
                ("sink", po::value<std::string>(&sink),
                 "The name of the SINK to which buffered tokens are published.")
                ;

        po::positional_options_description positional_options;
        positional_options.add("type", 1);
        positional_options.add("source", 1);
        positional_options.add("sink", 1);

        po::options_description all_options("All options");
        all_options.add(options).add(hidden);

        visible_options.add(options);

        po::variables_map variable_map;
        po::store(po::command_line_parser(argc, argv)
                .options(all_options)
                .positional(positional_options)
                .run(),
                variable_map);
        po::notify(variable_map);

        // Use the parsed options
        if (variable_map.count("help")) {
            printUsage(visible_options);
            return 0;
        }

        if (variable_map.count("version")) {
            std::cout << "Oat Buffer version "
                      << Oat_VERSION_MAJOR
                      << "."
                      << Oat_VERSION_MINOR
                      << "\n";
            std::cout << "Written by Jonathan P. Newman in the MWL@MIT.\n";
            std::cout << "Licensed under the GPL3.0.\n";
            return 0;
        }

        if (!variable_map.count("type")) {
            printUsage(visible_options);
            std::cout << oat::Error("A TYPE must be specified.\n");
            return -1;
        }

        if (!variable_map.count("source")) {
            printUsage(visible_options);
            std::cout << oat::Error("A SOURCE must be specified.\n");
            return -1;
        }

        if (!variable_map.count("sink")) {
            printUsage(visible_options);
            std::cerr << oat::Error("A SINK name must be specified.\n");
            return -1;
        }

    } catch (std::exception& e) {
        std::cerr << oat::Error(e.what()) << "\n";
        return -1;
    } catch (...) {
        std::cerr << oat::Error("Exception of unknown type.\n");
        return -1;
    }

    // Create component
    std::shared_ptr<oat::Buffer> buffer;

    // Refine component type
    switch (type_hash[type]) {
        case 'a':
        {
            buffer = std::make_shared<oat::FrameBuffer>(source, sink);
            break;
        }
        case 'b':
        {
            buffer = std::make_shared<oat::TokenBuffer<oat::Position2D>>(source, sink);
            break;
        }
        default:
        {
            printUsage(visible_options);
            std::cerr << oat::Error("Invalid TYPE specified.\n");
            return -1;
        }
    }

    // The business
    try {

        // Tell user
        std::cout << oat::whoMessage(buffer->name(),
                "Listening to source " + oat::sourceText(source) + ".\n")
                << oat::whoMessage(buffer->name(),
                "Steaming to sink " + oat::sinkText(sink) + ".\n")
                << oat::whoMessage(buffer->name(),
                "Press CTRL+C to exit.\n");

        // Infinite loop until ctrl-c or end of stream signal
        run(buffer);

        // Tell user
        std::cout << oat::whoMessage(buffer->name(), "Exiting.\n");

        // Exit success
        return 0;

    } catch (const std::runtime_error ex) {
        std::cerr << oat::whoError(buffer->name(),ex.what()) << "\n";
    } catch (const cv::Exception ex) {
        std::cerr << oat::whoError(buffer->name(), ex.what()) << "\n";
    } catch (...) {
        std::cerr << oat::whoError(buffer->name(), "Unknown exception.\n");
    }

    // Exit failure
    return -1;

}
