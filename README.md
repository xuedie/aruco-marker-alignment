# aruco-marker-alignment

## Detect Markers
This program will take a source image and marker image, detect all the markers in the two images and generate an aligned image of the src.
```
cd detect_markers/build
cmake ../
make

./detect_markers <src_image> <aligned_marker_image>
```

The generated image will be saved in build folder with "generated" prefix.

## Pose Estimation
This program will need the camera calibration first. 
```
cd pose_estimation/build
cmake ../
make

./pose_estimation -ci=<cameraID> -pi=<participantID> -l=<sideLength> -d=<markerLibrary>
```

Note that the side length(-l) is the real marker side length in meters.

## Camera Calibration
Only need to run this program once per camera. Print a marker board and run the following:
```
cd camera_calibration/build
cmake ../
make

./camera_calibration -d=<markerLibrary> -dp=../detector_params.yml -h=2 -w=4 -l=0.02 -s=0.01 ../../calibration_params.yml
```

## Create Markers