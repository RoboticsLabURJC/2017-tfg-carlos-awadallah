//
// Created by frivas on 24/01/17.
//

#include <logger/Logger.h>
#include "DepthCamera.h"
#include <opencv2/opencv.hpp>


namespace openniServer{
    DepthCamera::DepthCamera(std::string propertyPrefix, Ice::CommunicatorPtr ic,ConcurrentDevicePtr device) : CameraHandler(propertyPrefix, ic),device(device) {

        framerateN = prop->getPropertyAsIntWithDefault(prefix+"fps",25);
        mirror = prop->getPropertyAsIntWithDefault(prefix+"Mirror",0);
        std::string fmtStr = prop->getPropertyWithDefault(prefix+"Format","YUY2");//default format YUY2
        imageFmt = colorspaces::Image::Format::searchFormat(fmtStr);
        if (!imageFmt)
            LOG(WARNING) <<"Format " + fmtStr + " unknown" ;
        imageDescription->size = imageDescription->width * imageDescription->height * CV_ELEM_SIZE(imageFmt->cvType);
        imageDescription->format = imageFmt->name;

        LOG(INFO) <<  "Starting thread for camera: " + cameraDescription->name ;
        replyTask = new ReplyTask(this, framerateN, device,mirror);

        this->control=replyTask->start();//my own thread

    }

    void DepthCamera::getImageData_async(const jderobot::AMD_ImageProvider_getImageDataPtr &cb, const std::string &format,
                                       const Ice::Current &c) {
        replyTask->pushJob(cb, format);

    }

    DepthCamera::~DepthCamera() {
        LOG(INFO) <<  "Stopping and joining thread for Depth Camera" ;
        replyTask->destroy();
    }

    DepthCamera::ReplyTask::ReplyTask(const jderobot::Camera *camera, int fps,ConcurrentDevicePtr device,bool mirror) :
            CameraTask(camera, fps),device(device),mirror(mirror) {

    }

    void DepthCamera::ReplyTask::createCustomImage(cv::Mat &image) {
        image=this->device->getDepthImage();
        if (mirror)
            cv::flip(image,image,1);
    }

}
