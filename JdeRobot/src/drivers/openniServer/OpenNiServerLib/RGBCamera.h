//
// Created by frivas on 24/01/17.
//

#ifndef JDEROBOT_RGBCAMERA_DEVICE_H
#define JDEROBOT_RGBCAMERA_DEVICE_H

#include <jderobotutil/interfaceHandlers/CameraHandler.h>
#include <jderobotutil/interfaceHandlers/CameraTask.h>
#include <opencv2/opencv.hpp>
#include "ConcurrentDevice.h"

namespace openniServer {
    class RGBCamera:  public jderobot::CameraHandler {
     public:
        RGBCamera(std::string propertyPrefix, Ice::CommunicatorPtr ic, ConcurrentDevicePtr device);
        ~RGBCamera();

        void getImageData_async(const jderobot::AMD_ImageProvider_getImageDataPtr& cb, const std::string& format, const Ice::Current& c);



     private:
        class ReplyTask: public jderobot::CameraTask {
        public:
            ReplyTask(const jderobot::Camera* camera, int fps,ConcurrentDevicePtr device, bool mirror);

            virtual void createCustomImage(cv::Mat& image);

        private:
            ConcurrentDevicePtr device;
            bool mirror;

        };



        typedef IceUtil::Handle<ReplyTask> ReplyTaskPtr;
        ReplyTaskPtr replyTask;
        ConcurrentDevicePtr device;
        int framerateN;
        IceUtil::ThreadControl control;
        bool mirror;

    };
}


#endif //JDEROBOT_RGBCAMERA_DEVICE_H
