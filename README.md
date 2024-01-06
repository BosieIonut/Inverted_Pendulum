<img src="https://i.imgur.com/cLhNwkR.png" height="350"><br>

To accomplish this project, I used the following components:
<ul>
<li>Arduino Mega 2560
<li>MPU6050 accelerometer and gyroscope
<li>L298N Dual Motor Driver
<li>3 lithium batteries of 1200 mAh 3.7V (but in reality, they have more capacity)
<li>Potentiometer
<li>On/Off button
<li>Wires
<li>Manually crafted frame
</ul>

<h2><b>Reading and Filtering Data</h2></b>
<img src="https://i.ebayimg.com/images/g/K20AAOSw3KFWfM7i/s-l500.jpg" width="70"><br>

To read data from the accelerometer, I use the I2C protocol. <br>
Using the accelerometer, I can calculate the angle value quite accurately based on the weight orientation (this method is good at 'low frequencies'; if the robot is in motion or vibrating, we may get poor results). <br>
On the other hand, I can use the gyroscope to integrate angular velocity, obtaining a precise value at high frequencies. However, over time, due to sensor noise, the error will accumulate, causing the angle to deviate.<br>
<br>
The solution is to use a complementary filter that uses accelerometer data at low frequencies and gyroscope data at high frequencies. This way, we obtain a fairly accurate angle value to stabilize the robot.
<br>
<center><img src="https://i.imgur.com/2m3d1Qp.png" width="70%"><br></center>

<h2>Control and Calculation</h2>

The motor command will be given to the H-bridge through 2 pulse width modulation pins.<br>
The motor's VCC is 12V, so the range 0-255 (given on the pwm pin) will be mapped to 0-12V.

The command will be calculated using an optimal Kalmann controller.

Additionally, since the sampling period is small (0.005s), I will apply the derivative component and a Chebyshev2 filter (calculated in MATLAB). This will help us reduce vibrations.

Another issue is that, due to the alignment of components, the center of gravity is not aligned with the geometric center, and consequently, the robot will tilt to one side. To solve this problem, I added a potentiometer that adjusts the SetPoint value.

<h2>Complete Circuit</h2>
<img src="https://i.imgur.com/lTyNOnd.png">

<h2>Graphical Simulation Example</h2>

In the figure below, the command, error, and angle were plotted when a disturbance was applied to the system.

<img src="https://i.imgur.com/mFkkfph.png">
