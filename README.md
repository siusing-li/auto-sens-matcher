# Auto Sensitivity Matcher

* An app that can match a sensitivity from one 3D graphics application to another.
* Calculate increment, and revolution count from a database of back-end api yaw values
* Compares sensitivity from one application to another application through binary search (with continuous context)
* Simulate mouse movement to perform an accurate revolution (360 deg) in the application
* Waits for Alt + Insert to perform the revolution in the application
* Uses computer vision to detect whether current sensitivity’s revolution is too low or high in real time
* Develop support to undo actions and exit safely
* Implementation of threads to run multiple features and functions concurrently