//******************************************************************************
//* Copyright (c) Jon Newman (jpnewman at mit snail edu) 
//* All right reserved.
//* This file is part of the Simple Tracker project.
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
//******************************************************************************

#ifndef DETECTOR_H
#define	DETECTOR_H

#include <string>
#include <opencv2/opencv.hpp>

#include "../../lib/shmem/Position2D.h"
#include "../../lib/shmem/MatClient.h"
#include "../../lib/shmem/SMServer.h"

/**
 * Abstract base class to be implemented by any object detector within the 
 * Simple Tracker project
 * @param image_source_name Image SOURCE name
 * @param position_sink_name Position SINK name
 */
class Detector {
public:
    
    Detector(std::string image_source_name, std::string position_sink_name) : 
      image_source(image_source_name)
    , position_sink(position_sink_name)
    , tuning_image_title(position_sink_name + "_tuning")
    , slider_title(position_sink_name + "_sliders")
    , tuning_windows_created(false)
    , tuning_on(false) { } 
      
    // Detector must be able to find an object
    virtual void findObject(void) = 0;
    
    // Detector must be able to serve object position
    virtual void servePosition(void) = 0;
    
    // Detectors must be configurable via file
    virtual void configure(std::string file_name, std::string key) = 0;

    // All detectors must be able break through infinite locks on shared
    // resources to exit
    void stop(void) { image_source.notifySelf(); }
    
    void set_tune_mode(bool value) { tuning_on = value; }
    bool get_tune_mode(void) { return tuning_on; }
    
protected:
    
    // Detector must implement method  sifting a threshold image to find objects
    virtual void siftBlobs(void) = 0;
    
    // Detectors must allow manual tuning
    bool tuning_on, tuning_windows_created;
    const std::string tuning_image_title, slider_title;
    cv::Mat tune_image;
    virtual void tune(void) = 0;
    virtual void createTuningWindows(void) = 0;
    
    // The detected object position
    shmem::Position2D object_position;
    
    // The image source (Client side)
    MatClient image_source;
    
    // The detected object position destination (Server side)
    shmem::SMServer<shmem::Position2D> position_sink;
    
};

#endif	/* DETECTOR_H */

