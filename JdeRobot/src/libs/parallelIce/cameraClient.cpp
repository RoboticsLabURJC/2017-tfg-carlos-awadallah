/*
 *  Copyright (C) 1997-2013 JDE Developers TeamkinectViewer.camRGB
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Author : Jose María Cañas <jmplaza@gsyc.es>
			Francisco Miguel Rivas Montero <franciscomiguel.rivas@urjc.es>

 */

#include "cameraClient.h"
#include <zlib.h>
#include <jderobotutil/utils/CameraUtils.h>


namespace jderobot {


cameraClient::cameraClient(Ice::CommunicatorPtr ic, std::string prefix) {

	this->prefix=prefix;
	Ice::PropertiesPtr prop;
	prop = ic->getProperties();
	Ice::ObjectPrx baseCamera;
	this->refreshRate=0;
	this->mImageFormat.empty();
	this->newData=false;

	int fps=prop->getPropertyAsIntWithDefault(prefix+"Fps",30);
	this->cycle=(float)(1/(float)fps)*1000000;
	try{
		baseCamera = ic->propertyToProxy(prefix+"Proxy");
		if (0==baseCamera){
			throw prefix + "Could not create proxy with Camera";
		}
		else {
			this->prx= jderobot::CameraPrx::checkedCast(baseCamera);
			if (0==this->prx)
				throw "Invalid " + prefix + ".Proxy";
		}
	}catch (const Ice::Exception& ex) {
		std::cerr << ex << std::endl;
	}
	catch (const char* msg) {
		std::cerr << msg << std::endl;
		LOG(FATAL) << prefix + " Not camera provided";
	}


	//check if default format is defined
	std::string definedFormat=prop->getProperty(prefix+"ImageFormat");
	this->mImageFormat = CameraUtils::negotiateDefaultFormat(this->prx,definedFormat);


	jderobot::ImageDataPtr data = this->prx->getImageData(this->mImageFormat);

	this->size=cv::Size(data->description->width,data->description->height);
	_done=false;
	this->pauseStatus=false;
}

cameraClient::cameraClient(Ice::CommunicatorPtr ic, std::string prefix, std::string proxy){

	this->newData=false;
	this->prefix=prefix;
	Ice::PropertiesPtr prop;
	prop = ic->getProperties();
	Ice::ObjectPrx baseCamera;
	this->refreshRate=0;
	this->mImageFormat = colorspaces::ImageRGB8::FORMAT_RGB8.get()->name;

	int fps=prop->getPropertyAsIntWithDefault(prefix+"Fps",25);
	this->cycle=(float)(1/(float)fps)*1000000;

	try{
		baseCamera = ic->stringToProxy(proxy);
		if (0==baseCamera){
			throw prefix + "Could not create proxy with Camera";
		}
		else {
			this->prx= jderobot::CameraPrx::checkedCast(baseCamera);
			if (0==this->prx)
				throw "Invalid " + prefix + ".Proxy";
		}

	}catch (const Ice::Exception& ex) {
		std::cerr << ex << std::endl;
	}
	catch (const char* msg) {
		std::cerr << msg << std::endl;
		LOG(FATAL)<< prefix + " Not camera provided" ;
	}

	jderobot::ImageDataPtr data = this->prx->getImageData(this->mImageFormat);

	this->size=cv::Size(data->description->width,data->description->height);
	_done=false;
	this->pauseStatus=false;
}

cameraClient::~cameraClient() {
	_done=true;
}


jderobot::ImageFormat cameraClient::getImageFormat()
{
	return (this->prx->getImageFormat());
}

void cameraClient::setImageFormat (std::string format)
{
	mImageFormat = format;

	LOG(INFO) << "Changed format " + this->mImageFormat + " for camera " + this->prx->getCameraDescription()->name;
};


void cameraClient::reset(){
	this->prx->reset();
}

void cameraClient::pause(){
	this->pauseStatus=true;
}

void cameraClient::resume(){
	this->controlMutex.lock();
	this->pauseStatus=false;
	this->semWait.broadcast();
	this->controlMutex.unlock();
}


void
cameraClient::run(){
	jderobot::ImageDataPtr dataPtr;
	IceUtil::Time last;

	int iterIndex = 0;
	int totalRefreshRate = 0;

	last=IceUtil::Time::now();
	while (!(_done)){

		iterIndex ++;
		if (pauseStatus){
			IceUtil::Mutex::Lock sync(this->controlMutex);
			this->semWait.wait(sync);
		}

		try{

			dataPtr = this->prx->getImageData(this->mImageFormat);
			cv::Mat image = CameraUtils::getImageFromCameraProxy(dataPtr);
			this->controlMutex.lock();
			image.copyTo(this->data);
			this->newData=true;
			this->semBlock.broadcast();
			this->controlMutex.unlock();


		}
		catch(...){
			LOG(WARNING) << prefix +"error during request (connection error)";
			usleep(50000);

		}

		int process = (IceUtil::Time::now().toMicroSeconds() - last.toMicroSeconds());



		if (process > (int)cycle ){
			DLOG(WARNING) << "--------" + prefix + " adquisition timeout-";
		}
		else{
			int delay = (int)cycle - process;
			if (delay <1 || delay > (int)cycle)
				delay = 1;

			usleep(delay);
		}


		int rate =(int)(1000000/(IceUtil::Time::now().toMicroSeconds() - last.toMicroSeconds()));
		totalRefreshRate =  totalRefreshRate + rate;
		this->refreshRate= totalRefreshRate / iterIndex;
		last=IceUtil::Time::now();

		if (iterIndex == INT_MAX) 
		{
			iterIndex = 0;
			DLOG(INFO) << "*** Counter reset";
		}

	}
	this->data.release();
}

void cameraClient::stop_thread()
{
	_done = true;
}

void cameraClient::getImage(cv::Mat& image, bool blocked){
	{
		IceUtil::Mutex::Lock sync(this->controlMutex);
		if (blocked){
			if (!this->newData){
				this->semBlock.wait(sync);
			}
			this->newData=false;
		}
		this->data.copyTo(image);
	}

}

} /* namespace jderobot */
