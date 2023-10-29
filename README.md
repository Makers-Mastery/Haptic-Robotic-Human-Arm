<p align="center">
  <img width="679" height="677" src="https://github.com/Makers-Mastery/Haptic-Robotic-Human-Arm/blob/main/Media/60437999.png">
</p>

# Introduction
Hello! My name is Elijah, a high school senior going into college and a budding robotics engineer. This is the most advanced project I've made to date, as a way to push myself in learning robotics and electrical engineering. This is the repository for my full scale adaptive human arm, hapic glove, and exo-arm. The project files for the arm, hand, and exo-arm are courtesy of youbionic and the haptic glove (originally designed for VR but repurposed as a control device for my arm) is courtesy of Lucas VRTech on YouTube. The purpose of the haptic glove is to both register the inputs of my fingers as well as recieving data from force sensors I've installed in the tips of the hand's fingers, allowing me to actuate servos on the glove to stop your finger movements and "feel" with the robotic hand. The code is written for the ESP32 Development Board and PCA9685 Servo Controller, using the ESPNow protcol for wireless communication (see Credits for more details). Over the course of this project, I've been recording my progress as I go and once I'm done I'll bee putting together a YouTube video to show off just how far I've come and everything I've learned over the course of this project. Once that's complete, I'll link the video here as well.

<p align="center">
  <img width="1024" height="1400" src="https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/blob/main/Media/7EA858B9-4707-4A26-9FC6-FE2E93865FD5.jpeg">
</p>

<p align="center">
  <img width="1024" height="1400" src="https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/blob/main/Media/Full%20Arm%20Backpack.jpg">
</p>

# Project Update 4/6/23 #
Been a while since I've written an update, been really busy with school and procrastinating. I didn't really touch the project between december and febuary hence the massive gap between updates. The project is now in a more or less funtional state it just requires a LOT more calibration. The functional ranges for the arm, exo arm, hand, preassure sensors, and glove glove are all calculated and the power delivery systems are all hooked up. The hand is being powered by a 6v lead acid battery (I actually built a setup with 6 18650 batteries but it was 8v at full charge with is way too high and bad for the servos) and the arm is being powered off 2 drill batteries being stepped down to 7.3v. I'm planning on still running the glove servos off a battery pack, since it should still work. I haven't gotten to calibrating the haptics in the glove because I've been too focused on trying to make the basic functionality of the arm work. There's been a lot of setbacks along the way, like finding out the servo in the hand was rotational instead of positional or supergluing pins in place only to find out it cut off electrical contact, and I think I might've just broke another servo somehow. I was able to solve the delayed movement of the hand after receiving data by moving moveFingers() from OnDataReceived() to loop(), since they run on different cores which majorly speed up processing power.
## So what's next? ##
I need to do a lot of calibration to make this all run smoothly. What I really should do is impliment some kind of a calibration button so the exoarm will always work no matter how I put it on, but I don't really know how I would impliment that. I of course need to do calibrations for the haptics in the glove, but I expect that to be extremely difficult and time consuming and difficult so I've been saving that for last, focusing on getting everything functional first. I also need to build some kind of a mount/container for the batteries because lugging around 2 drill batteries with step down buck converters is really impractical. ANother problem is the design of the exo arm itself is lacking, particularly in the shoulder, so I may need to redesign it in the future. Other than that, all I really need to do is tune the code and the calibrations and I should finally be done! I'll post a couple youtube shorts here soon and a full youtube video once I have that put together.
# Project Update 12/5/22 #
I'm moving into the final steps of my project as I've been busy assembling the final power delivery system I designed. I'm using both 2 drill batteries to power the arm as well as 6 18650 batteries (2 packs of 3 wired in paralell, with the packs wired together in series) to power the hand. Because I'm now using 2 PCA9685s on one ESP32 to control the arm, I need to reformat the code to make up for this and allow ServoEasing to work with both boards. Once I'm finished updating the code, all that will be left to do is map out functional ranges for the arm and exo arm and I'll finally be finished with the project. I'll post a short Youtube video here showing off the arm once I'm finished, then I'll get to work on finishing the comprehensive build video. 

# Project Timeline 11/17/22
At present, the arm is just about complete, this section will outline what I still need to do. 
### Build Power Delivery System ###
#### The Issue ####
The most recent issue I've been encountering with my robotic arm was something called servo twitching, which is exactly what it sounds like. In the process of testing the hand in order to find out the funtional range of the servos I was using as I had them installed, I encountered an issue where in when in the stopped position the servos kept twitching back and fourth about 10 degrees. After looking into the issue more and getting some feedback from the Youbionic telegram (love you Nikolay you the GOAT), I figured out the issue was the power delivery system. I was currently using a 1000 mAh battery bank to power the 6 9g (MG90S) servos in the hand, and after doing the math, I figured out that was drawing ~3.3A of current. Turns out, the battery bank I was using was only rated to draw 2.4A of current. Solution should be simple right? Just buy a bigger battery bank with a higher amp output? Well, the problem was bigger than that, because the upper arm is using 4 25kg servos (DS3225) and 1 15kg servo (MG996R) as opposed to 9g servos. After foing the math for the upper arm, I found out that I had accidently wired the upper arm to be drawing **23 amps** of current (10A is enough to stop your heart) through 20 gauge wire (about 0.8mm thick). It's a good thing I never plugged the arm in! I'm going to have to get pretty creative to power the upper arm.
#### The Solution ####
After talking it over with the Youbionic telegram chat, there are 2 methods I see for powering the arm here. No matter what method I end up choosing, I'm going to get another PCA9685 because that board isn't meant to handle more than ~8A, so I'm going to need to add a second board and power both seperately as well as reformating the code so I can control both boards with 1 ESP32. I also need to get some thicker wire to handle the very high current.
#
The first method of powering the arm would be to buy a bunch of 18650 rechageable batteries and battery holders for them, then wiring them in series until I have ~6v and paralle until I have the necesscary amperage.  <br>
**Pros**-easy recharging, relatively straightforward setup, easy to get the voltage and amperage I want <br>
**Cons**-very expensive, long wait on batteries to arrive from online.
#
The second method method would be to use an 18v power tool battery and use a buck converter to lower the voltage to a managable amount, ~6v. I'm not sure how easily interfaceable power tool batteries are and whether or not I'd have to crack one open to acheive this, so I'm still looking into this method. <br>
**Pros**-I already have power tool batteries and a charging station, significantly cheaper, possibly easier <br>
**Cons**-risk of crappy buck converter, risk of overheating a buck converter<br>
<br>
Whatever method I end up choosing, I'm probably gonna have to spend a lot of money! If I drop $100 this had better work.

### Finish Mapping the Funtional Range of the Arm ### 
Because I dont currently have a way to power the arm, I don't actually have any way to test it at the moment. Once I get the power situation sorted out, the next step would be to find the funtional rang of the servos installed in my arm. Each of the servos I have are rated for 180-270 degrees, but im ususally only using about 100 of that, so I need to figure out what range of 100 degrees that actually is. After that, all that will be left to do is find the same funtional range for the exo-arm and plug those values into the program. Baring any more serious complications, that should be the very last thing I need to do to finish the project.

### Create Wiring Documentation
Due to the "in progress" nature of this project, I'll be making wiring documentation using fritzing a little later down the line once the wiring for the project is a bit more finalized. If you'd like to see a current example of my wiring documentation abilities, you can check out my [Advanced RFID Lock project](https://github.com/The-EAR-Foundation/Advanced-RFID-Lock).

### Finish Editing Youtube Video ###
As I've been building this project, I've been periodically recording my progress as I go with the end goal of eventually putting together a youtube video showing the entire build progress from start to finish. I've been trying to learn adobe premier in my free time in my graphics class so I've been slowly editing this video as I go. Would it have made more sense to make an instagram or tiktok account to post these progress updates as I go? Probably! But a youtube video is the plan for the time being. Once that's done sometime in the next 2 weeks, I'll uplaod here. For any colleges reading this, pay attention to this README file over the coming weeks because I'll be updating it as I go.



# Credits
**[Youbionic](https://www.youbionic.com)** <br>
None of this project would be possible without the ingenius designs of Federico Ciccarese and Christopher Calleja and all the help from the incredible community they've formed over on the Youbionic telegram. The arm and hand, though not originally designed to be used together (my talent with tinkercad solved this), as well as the exoarm were all designed by Youbionic and I followed their project guide in the process of building it, of course adding my own touches along the way.
#
**[Lucas VRTech](https://youtu.be/2yF-SJcg3zQ)** <br>
This haptic glove enabled one of the most difficult parts of this project: the haptic feedback. I'd actually seen the video here about a month before I started this project, and I thought it was super cool and I really wanted to make one even though I didn't have a VR headset at the time (although I later got access to one as a part of my engineering internship with the University of Utah). Once I saw the Youbionic exoarm, I knew immediently I wanted to use this with the project. The glove serves two purposes in my project: meassuring the potentiometer values of each finger to translate into movements of the hand and to receive data from the force sensors on the hand and translate that data into motor movement to serve as haptic feedback by stopping the movement of each finger creating "feeling." None of this would've been possible without the incredible documentation, ease of setup, and open source nature of this fantastic project from Lucas. 
#
**[Random Nerd Tutorials](https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/)** <br>
This website has been indisposable for setting up the ESPNow wireless protocol I'm using for my ESP32 boards, it's by far the best documentation I've found. Credits to this website can be found in comments in my code as well, as their sample for ESP32 2 way communication is what I modified and built my code off.


# Directory
**[Media](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/tree/main/Media)** - This is where you can find all the photos and videos relevant to the project, will be curated as I go. <br>
**[Software](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/tree/main/Software)** - This is where you'll find the code for each of the 4 boards in the project, as well as some of the 3D models I designed or modified for the project. <br>
**[3D Models](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/tree/main/Software/3D%20Models)** <br>
**[Glove Code](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/tree/main/Software/2_way_glove)**<br>
**[Hand Code](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/tree/main/Software/2_way_hand)**<br>
**[Exo-Arm Code](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/blob/main/Software/ExoArm_Code/ExoArm_Code.ino)**<br>
**[Arm Code](https://github.com/The-EAR-Foundation/Haptic-Robotic-Human-Arm/blob/main/Software/Arm_Code/Arm_Code.ino)**<br>
