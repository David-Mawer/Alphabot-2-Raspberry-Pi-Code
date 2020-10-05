#ifndef CAMERA_VIDEO_SERVICE_H
#define CAMERA_VIDEO_SERVICE_H
#include <opencv2/opencv.hpp>   // OpenVC libraries
#include <opencv2/highgui.hpp>


class camera_video_service
{
    public:
        camera_video_service();
        virtual ~camera_video_service();
        bool cameraEnabled();

    protected:

    private:
        cv::VideoCapture myVideo(0);
        bool haveCamera = myVideo.isOpened();

};

#endif // CAMERA_VIDEO_SERVICE_H
