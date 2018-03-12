                            STOP PRACTICE
                            =============

The goal of this practice is to implement the logic of a navigation algorithm for 
an automated vehicle. The vehicle must Stop at the T joint, where there is a stop 
sign, wait until there are no cars and pass once the road is clear.

////////////////////////////////////////////////////////////////////////////////
                          E X E C U T I O N 
////////////////////////////////////////////////////////////////////////////////

To launch the example, follow the steps below:
1. Run Gazebo simulator:
 * Execution without seeing the world: 
`$ gzserver stop.world`
 * Execution watching the world: 
`$ gazebo stop.world`
2. Running the practice and the user interface: 
`$ python2 ./stop.py stop_conf.yml`

To simplify the closure of the environment, just close the Stop Component 
window(s). Ctrl + C will give problems.
////////////////////////////////////////////////////////////////////////////////


## How to do the practice
To carry out the practice, you must edit the MyAlgorithm.py file and insert the 
control logic into it.

[MyAlgorithm.py](MyAlgorithm.py#L74)
```
        def execute(self):
        
            # Add your code here
            print "Runing"
            # Getting the images
            input_image = self.cameraC.getImage().data
        
```


### API
* self.cameraC.getImage().data, self.cameraL.getImage().data, self.cameraR.getImage().data
 - to get image from center, left or right camera respectively.
* self.pose3d.getPose3d().x - to obtain the position of the robot
* self.pose3d.getPose3d().y - to obtain the position of the robot
* self.pose3d.getPose3d().yaw - to obtain the position of the robot
* self.motors.sendW() - to set the angular velocity
* self.motors.sendV() - to set the linear velocity


## Demonstrative video
https://www.youtube.com/watch?time_continue=13&v=hF2i0rdlIqE

