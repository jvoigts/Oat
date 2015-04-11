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

#ifndef MATSERVER_H
#define	MATSERVER_H

#include <string>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <opencv2/core/mat.hpp>

#include "SharedMat.h"

class MatServer {
    
public:
    MatServer(const std::string sink_name);
    MatServer(const MatServer& orig);
    virtual ~MatServer();
    
    void createSharedMat(cv::Mat model); // TODO: encapsulate in the SharedMatHeader object
    
    // Accessors
    void set_shared_mat(cv::Mat mat);
    std::string get_name(void) { return name; }
    
private:
    
    std::string name;
    shmem::SharedMatHeader* shared_mat_header;
    void* shared_mat_data_ptr;
    int data_size; // Size of raw mat data in bytes
    
    std::string shmem_name, shobj_name;
    boost::interprocess::managed_shared_memory shared_memory; // TODO:  encapsulate in the SharedMatHeader object

};

#endif	/* MATSERVER_H */

