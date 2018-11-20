## Task List
* [] Saner build systems for eventual CI
    * [x] File Issue for Docker-based build system
    * [x] Make Ubuntu:12.04 docker container to build Ubuntu & Windows binaries
    * [] Change os-designation to come from Maven rather than intelligently inside the GNU folders.
    * [] Change debug vs not designation to come from Maven
    * [] Change LGPL vs GPL for dependencies to come from Maven
    * [] Change memcheck designation to come from Maven
    * [] Create script to invoke all supported build matrix combinations from Maven
    * [] Remove `humble-video-chef` and update README to require Docker

* [] Upgrade to FFMpeg 4.1 with current dependencies and get all Humble tests to pass
    * [] File Issue for FFmpeg 4.1 support
    * [] Make Ubuntu:18.04 LTS docker container to build Ubuntu & Windows binaries
    * [] Get captive FFmpeg 4.1 to build in that container and on Mac
    * [] Get native to build with FFmpeg 4.1 and all native tests to pass
    * [] Get memcheck to pass
        * This is NOT worth doing on the 2.8 set.
    * [] Get java to build with FFmpeg 4.1 and all tests to pass
    * [] Add additional libraries
        * [] libx265
            * [] add test to native tests
        * [] openh264 (cisco)
            * [] add test to native tests
        * [] opencl?
            * [] add test to native tests
        * [] subtitle libraries?
            * [] add test?
        * [] others?
    * [] Add demo to show what's possible in FFmpeg 4.1
    * [] Close the ticket
* [] Other Potential Ideas
    * [] (stretch) Create CentOS Docker file and get tests to pass