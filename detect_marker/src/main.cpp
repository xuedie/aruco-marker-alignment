#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <algorithm>

struct Corner {
    int id;
    std::vector<cv::Point2f> pts;

    bool operator<(const Corner& a) const
    {
        return id < a.id;
    }
};

int main(int argc, char **argv)
{
    std::string src_name = argv[1];
    std::string dst_name = argv[2];
    
    cv::Ptr<cv::aruco::Dictionary> dictionary = 
        cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(6));
    
    cv::Mat src_image, src_image_marker, dst_image, dst_image_marker, generate_image;
    src_image = cv::imread(src_name);
    dst_image = cv::imread(dst_name);
    src_image.copyTo(src_image_marker);
    dst_image.copyTo(dst_image_marker);
    std::vector<int> ids, dst_ids;
    std::vector<std::vector<cv::Point2f> > corners, dstCorners;

    cv::aruco::detectMarkers(src_image, dictionary, corners, ids);
    cv::aruco::detectMarkers(dst_image, dictionary, dstCorners, dst_ids);
    // if at least one marker detected
    if (dst_ids.size() > 0 && ids.size() == dst_ids.size()) {
        cv::aruco::drawDetectedMarkers(src_image_marker, corners, ids);
        cv::aruco::drawDetectedMarkers(dst_image_marker, dstCorners, dst_ids);
    }

    std::vector<Corner> src_corners, dst_corners;
    for (int i = 0; i < corners.size(); ++i) {
        Corner c;
        c.id = ids[i];
        c.pts = corners[i];
        src_corners.push_back(c);
    }

    for (int i = 0; i < dstCorners.size(); ++i) {
        Corner c;
        c.id = dst_ids[i];
        c.pts = dstCorners[i];
        dst_corners.push_back(c);
    }

    std::sort(src_corners.begin(), src_corners.end());
    std::sort(dst_corners.begin(), dst_corners.end());

    std::vector<cv::Point2f> pts_src, pts_dst;
    for (int i = 0; i < src_corners.size(); ++i) {
        Corner c = src_corners[i];
        for (int j = 0; j < c.pts.size(); ++j) {
            pts_src.push_back(c.pts[j]);
        }
    }

    for (int i = 0; i < dst_corners.size(); ++i) {
        Corner c = dst_corners[i];
        for (int j = 0; j < c.pts.size(); ++j) {
            pts_dst.push_back(c.pts[j]);
        }
    }

    cv::Mat affineMatrix = cv::findHomography(pts_src, pts_dst);
    cv::warpPerspective(src_image, generate_image, affineMatrix, dst_image.size());
    cv::imshow("Detected src markers", src_image_marker);
    cv::imshow("Detected dst markers", dst_image_marker);
    cv::imshow("Transformed image", generate_image);
    cv::waitKey(0);

    imwrite("generated_" + src_name, generate_image);
}
