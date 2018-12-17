#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <string>

using namespace std;
using namespace cv;

namespace {
const char* about = "Pose estimation";
const char* keys  =
        "{l        | 0.035      | Marker side lenght (in pixels) }"
        "{d        | 6      | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
        "{ci       | 0     | Camera id if input doesnt come from video (-v) }"
        "{pi       | 99     | Participator id }";
}

void getEulerAngles(Mat &rotCamerMatrix,Vec3d &eulerAngles){

    Mat cameraMatrix,rotMatrix,transVect,rotMatrixX,rotMatrixY,rotMatrixZ;
    double* _r = rotCamerMatrix.ptr<double>();
    double projMatrix[12] = {_r[0],_r[1],_r[2],0,
                          _r[3],_r[4],_r[5],0,
                          _r[6],_r[7],_r[8],0};

    decomposeProjectionMatrix( Mat(3,4,CV_64FC1,projMatrix),
                               cameraMatrix,
                               rotMatrix,
                               transVect,
                               rotMatrixX,
                               rotMatrixY,
                               rotMatrixZ,
                               eulerAngles);
}

int main(int argc, char **argv)
{
    CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    float actual_marker_length = parser.get<float>("l");
    int dictionaryId = parser.get<int>("d");
    int camId = parser.get<int>("ci");
    int pId = parser.get<int>("pi");

    int wait_time = 10;
    //float actual_marker_length = 0.035;  // this should be in meters

    cv::Mat image, image_copy;
    cv::Mat camera_matrix, dist_coeffs;
    std::ostringstream vector_to_marker;
    
    cv::VideoCapture in_video;
    in_video.open(camId);
    cv::Ptr<cv::aruco::Dictionary> dictionary = 
        cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
    
    cv::FileStorage fs("../../calibration_params.yml", cv::FileStorage::READ);

    fs["camera_matrix"] >> camera_matrix;
    fs["distortion_coefficients"] >> dist_coeffs;

    std::cout << "camera_matrix\n" << camera_matrix << std::endl;
    std::cout << "\ndist coeffs\n" << dist_coeffs << std::endl;

    int picNum = 0;
    int posId = 1;

    while (in_video.grab()) 
    {

        in_video.retrieve(image);
        image.copyTo(image_copy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);
        
        // if at least one marker detected
        if (ids.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(image_copy, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, actual_marker_length,
                    camera_matrix, dist_coeffs, rvecs, tvecs);
            
            // draw axis for each marker
            for(int i=0; i < ids.size(); i++)
            {
                cv::aruco::drawAxis(image_copy, camera_matrix, dist_coeffs,
                        rvecs[i], tvecs[i], 0.05);
                
                Mat rotCamerMatrix;
                Rodrigues(rvecs[0],rotCamerMatrix);
                Vec3d eulerAngles;
                getEulerAngles(rotCamerMatrix,eulerAngles);

                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4) 
                                 << "yaw: " << std::setw(8)<<  eulerAngles[1];
                cv::putText(image_copy, vector_to_marker.str(), 
                        Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, 
                        Scalar(0, 0, 124), 2);
                
                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4) 
                                 << "pitch: " << std::setw(8) << eulerAngles[0]; 
                cv::putText(image_copy, vector_to_marker.str(), 
                        Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 1, 
                        Scalar(0, 0, 124), 2);
                
                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4) 
                                 << "roll: " << std::setw(8) << eulerAngles[2];
                cv::putText(image_copy, vector_to_marker.str(), 
                        Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 1, 
                        Scalar(0, 0, 124), 2);
                
            }
        }
        string cur = "Current position: " + std::to_string(posId);
        cv::putText(image_copy, cur, 
                        Point(10, 200), cv::FONT_HERSHEY_SIMPLEX, 1, 
                        Scalar(0, 0, 255), 2);
        
        cv::imshow("Pose estimation", image_copy);
        char key = (char) cv::waitKey(wait_time);
        if (key == 27)
            break;
        if (key == 'n') {
            posId++;
            picNum = 0;
        }
        if (key == 'c') {
            std::cout << "Frame captured" << endl;
            string filename = "P"+std::to_string(pId)+"_C"+
                std::to_string(camId)+"_"+
                std::to_string(posId)+"_"+
                std::to_string(picNum)+
                ".jpg";
            imwrite(filename, image);
            picNum++;
        }
    }

    in_video.release();
}
