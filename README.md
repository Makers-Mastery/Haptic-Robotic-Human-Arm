<p align="center">
  <img width="679" height="677" src="https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/blob/main/Media/EAR%20Robotics%20Division%20Logo.jpg">
</p>

# Introduction
Hello! My name is Elijah, a high school senior going into college and a budding robotics engineer. This is the most advanced project I've made to date, as a way to push myself in learning robotics and electrical engineering. This is the repository for my full scale adaptive human arm, hapic glove, and exo-arm. The project files for the arm, hand, and exo-arm are courtesy of youbionic and the haptic glove (originally designed for VR but repurposed as a control device for my arm) is courtesy of Lucas VRTech on YouTube. The code is written for the ESP32 Development Board and PCA9685 Servo Controller, using the ESPNow protcol for wireless communication (see Credits for more details). Over the course of this project, I've been recording my progress as I go and once I'm done I'll bee putting together a YouTube video to show off just how far I've come and everything I've learned over the course of this project. Once that's complete, I'll link the video here as well.

<p align="center">
  <img width="1024" height="1400" src="https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/blob/main/Media/Full%20Arm%20Backpack.jpg">
</p>

# Project Timeline
At present, the arm is just about complete, this section will outline what I still need to do. 
### Build Power Delivery System ###
The most recent issue I've been encountering with my robotic arm was something called servo twitching, which is exactly what it sounds like. In the process of testing the hand in order to find out the funtional range of the servos I was using as I had them installed, I encountered an issue where in when in the stopped position the servos kept twitching back and fourth about 10 degrees. After looking into the issue more and getting some feedback from the Youbionic telegram (love you Nikolay you the GOAT), I figured out the issue was the power delivery system. I was currently using a 1000 mAh battery bank to power the 6 9g (MG90S) servos in the hand, and after doing the math, I figured out that was drawing ~3.3A of current. Turns out, the battery bank I was using was only rated to draw 2.4A of current. Solution should be simple right? Just buy a bigger battery bank with a higher amp output? Well, the problem was bigger than that, because the upper arm is using 4 25kg servos (DS3225) and 1 15kg servo (MG996R) as opposed to 9g servos. After foing the math for the upper arm, I found out that I had accidently wired the upper arm to be drawing 

### Finish Mapping the Funtional Range of the Arm ### 

### Finish Editing Youtube Video ###

# Credits

# Directory
