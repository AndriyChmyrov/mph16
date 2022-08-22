# mph16

This is a Matlab .mex wrapper written in C/C++ (MS Visual Studio) for Thorlabs MPH16 motorized pinhole.

Requires Thorlabs software which can be downloaded from:
https://www.thorlabs.com/software_pages/viewsoftwarepage.cfm?code=MPH16

File "ThorPinholeStepperSettings.xml" with pinhole location parameters has to be located in the current folder of Matlab.

**Example of usage:**
~~~Matlab
numpinholes = mph16();         % initialize and get number of pinholes
position = mph16('position');  % get current pinhole position (indexed from 0)
mph16('position',position);    % set current pinhole position
clear mph16                    % clear pinhole driver
~~~
