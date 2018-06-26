//how to use the ZED SDK with YOLO to find the distance of objects detected by yolo from the camera 
// this code here will be included in the demo.c file in YOLO darknet 
// a JETSON TX1  platform is being used for implementation 

 // ZED includes
#include <sl_zed/Camera.hpp>

// OpenCV includes
#include <opencv2/opencv.hpp>

// Sample includes
#include <SaveDepth.hpp>

using namespace sl;

cv::Mat slMat2cvMat(Mat& input);

//put the following code inside void demo instead of int main 
int main(int argc, char **argv) {

    // Create a ZED camera object
    Camera zed;

    // Set configuration parameters
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION_HD1080;
    init_params.depth_mode = DEPTH_MODE_PERFORMANCE;
    init_params.coordinate_units = UNIT_METER;
 
    //code to use the zed on a file supplied to it instead of using the camera
    if (argc > 1) init_params.svo_input_filename.set(argv[1]);
        
    // Open the camera
    ERROR_CODE err = zed.open(init_params);
    if (err != SUCCESS) {
        printf("%s\n", toString(err).c_str());
        zed.close();
        return 1; // Quit if an error occurred
    }


    // Prepare new image size to retrieve half-resolution images
    Resolution image_size = zed.getResolution();
    int new_width = image_size.width;
    int new_height = image_size.height;

    // To share data between sl::Mat and cv::Mat, use slMat2cvMat()
    // Only the headers and pointer to the sl::Mat are copied, not the data itself
    Mat image_zed(new_width, new_height, MAT_TYPE_8U_C4);
    cv::Mat image_ocv = slMat2cvMat(image_zed);
    Mat depth_image_zed(new_width, new_height, MAT_TYPE_8U_C4);
    cv::Mat depth_image_ocv = slMat2cvMat(depth_image_zed);
    Mat point_cloud;

 /*   // Loop until 'q' is pressed
    char key = ' ';
    while (key != 'q') {*/

        if (zed.grab(runtime_parameters) == SUCCESS) {

            // Retrieve the SIDE-BY-SIDE image, depth image
            zed.retrieveImage(image_zed, VIEW_SIDE_BY_SIDE, MEM_CPU, new_width, new_height);
            zed.retrieveImage(depth_image_zed, VIEW_SIDE_BY_SIDE, MEM_CPU, new_width, new_height);
         
            zed.retrieveMeasure(point_cloud, MEASURE_XYZRGBA, MEM_CPU, new_width, new_height);
         
        }
    zed.close();
    return 0;
}

/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::Mat slMat2cvMat(Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
        case MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
        case MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
        case MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
        case MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
        case MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
        case MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
        case MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
        case MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}

