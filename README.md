# aruco-marker-alignment

## Detect Markers
This program will take a source image and marker image, detect all the markers in the two images and generate a aligned image of the src.
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

./pose_estimation -ci=<cameraID> -pi=<participantID> -d=<sideLength>
```

Note that the side length is the real marker side length in meters.

## Camera Calibration
Only need to run this program once per camera

## Create Markers