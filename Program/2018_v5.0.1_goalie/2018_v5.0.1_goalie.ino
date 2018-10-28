//#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <math.h>
#define debug
#define goalie

//i2c
#define cmps 0x60
#define nano 0x08
#define ADDRESS 0x60
//movement
#define fldir 7
#define flpwm 3
#define frdir 9
#define frpwm 5
#define bldir 22
#define blpwm 23
#define brdir 6
#define brpwm 2
#define dribDIR 8
#define dribPWM 4

#if defined striker //2nd bot
	
	#define LDR_FL A2
	#define LDR_FR A16
	#define LDR_LI A20
	#define LDR_LM A3
	#define LDR_LO A0
	#define LDR_RI A19
	#define LDR_RM A15
	#define LDR_RO A18
	#define LDR_BIL A6
	#define LDR_BIR A14
	#define LDR_BOL A1
	#define LDR_BOR A17
	#define LDR_GATE A7

	#define GATE_Threshold 200
	#define FL_threshold 260
	#define FR_threshold 260
	#define LI_threshold 220
	#define LM_threshold 180
	#define LO_threshold 120
	#define RI_threshold 150
	#define RM_threshold 120
	#define RO_threshold 120
	#define BIL_threshold 240
	#define BIR_threshold 1000
	#define BOL_threshold 90
	#define BOR_threshold 90	

#elif defined goalie

	#define LDR_FL A2
	#define LDR_FR A20
	#define LDR_LI A16
	#define LDR_LM A19
	#define LDR_LO A15
	#define LDR_RI A18
	#define LDR_RM A14
	#define LDR_RO A17
	#define LDR_BIL A3
	#define LDR_BIR A0
	#define LDR_BOL A6
	#define LDR_BOR A1
	#define LDR_GATE A7

	#define GATE_Threshold 100
	#define FL_threshold 310//200,45
	#define FR_threshold 240//135, 25
	#define LI_threshold 140//200, 40
	#define LM_threshold 210//200, 40
	#define LO_threshold 180//160, 30
	#define RI_threshold 140//160, 30
	#define RM_threshold 90//150, 25
	#define RO_threshold 110//106, 18
	#define BIL_threshold 310//200, 40
	#define BIR_threshold 300//218, 35
	#define BOL_threshold 110//1020, 1000
	#define BOR_threshold 100//123, 25
#else 

#endif	

//LDR
int FR, FL, LI, LM, LO, RI, RM, RO, BIL, BIR, BOL, BOR;
int FL_max=0, FR_max=0, LI_max=0, LM_max=0, LO_max=0, RI_max=0, RM_max=0, RO_max=0, BIL_max=0, BIR_max=0, BOL_max=0, BOR_max=0;
int FL_min=999, FR_min=999, LI_min=999, LM_min=999, LO_min=999, RI_min=999, RM_min=999, RO_min=999, BIL_min=999, BIR_min=999, BOL_min=999, BOR_min=999;
int rfront, rleft, rright, rback, rdir;
bool robot_out = false, prevfront=false, prevback=false, out=false, stall = false;
int lightcnt=0, light_mult = 3, timeLDR, timeStall;
//Cmps
int corrCmps = 0, angleCmps=0, flag = 0, receive, getAngle;
unsigned int angle16;
unsigned char hbyte, lbyte;
//Ultra
int frontUltra, rightUltra, leftUltra, backUltra, tfrontUltra, trightUltra, tleftUltra, tbackUltra, currUltra, thresholdUltra = 35; 
//movement
int fl_pwm, fr_pwm, bl_pwm, br_pwm, fl_dir = 1, fr_dir = 1, bl_dir = 1, br_dir = 1;
const float cmps_mult = 0.5, pi = 3.141592653589793;
float a, b, correction;
float speedDrib, timeKick;
char temp;
//pi xy coords
int side;
float ballx, bally, 
yellowTopx, yellowTopy, yellowBotx, yellowBoty, 
blueTopx, blueTopy, blueBotx, blueBoty, 
angle, break1, tdist, tclr, //side==0 is start at blue side, side==1 is start at yellow side. 
dist, angleBall, angleMove, distBall, 
centerx, centery, angleGoal, distGoal, fieldside, //fieldside==1 is at right, fieldside==2 is at the left
goaly[2][4], goalMult = 1, blueGoaly, yellowGoaly, yCorr, yDiff, yMult = 1, //goal[*][0] is top, goal[*][1] is bottom, goal[*][2] is middle
backUltraCorr, backUltraDiff, backUltraMult = 4, backUltraThreshold = 20,
ballThreshold = 140, goalThreshold = 460,
solenoidMillis, kickcheck = 0;
String incomingByte, tempString;

//LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
	//lcd.begin();
	//lcd.backlight();
	Serial.begin(9600);
	Serial.println("start");
	Wire.begin(0x06);
	Serial4.begin(230400);
	Wire.onReceive(i2c_receive);
	pinMode (fldir, OUTPUT);
	pinMode (flpwm, OUTPUT);
	pinMode (frdir, OUTPUT);
	pinMode (frpwm, OUTPUT);
	pinMode (bldir, OUTPUT);
	pinMode (blpwm, OUTPUT);
	pinMode (brdir, OUTPUT);
	pinMode (brpwm, OUTPUT);
	pinMode (dribDIR, OUTPUT);
	pinMode (dribPWM, OUTPUT);
	delay(2000);
	startCmps();
	initPi();
	solenoidMillis = millis();
	//getAngle = 0;
}

void loop() {
	#if defined debug
	indiv_debugLDR();
#elif defined striker1
	readPi();
	readUltra();
	trackAngle();
	trackGoal();
	
	//Serial.println(angleCmps);
	/*if(stall){
		if(rdir > 45 && rdir <= 135 && leftUltra > 30){
			stall = false;
			setmove(angleMove, 60, 0, 0);
	  	}
	  	else if(rdir > 135 && rdir <=225 && frontUltra > 30){
			stall = false;
			setmove(angleMove, 60, 0, 0);
	  	}
	  	else if(rdir > 225 && rdir <=315 && rightUltra > 30){
			stall = false;
			setmove(angleMove, 60, 0, 0);
	  	}
	  	else if((rdir > 315 || rdir <=45) && backUltra > 30){
			stall = false;
			setmove(angleMove, 60, 0, 0);
	  	}
		if((millis() - timeStall) > 3000){
			stall = false;
			setmove(angleMove, 60, 0, 0);
		}
		else setmove(0, 0, 0, 0);
	}*/
	/*if(checkLDR()){
		if(distBall < 180){
			/*if(rdir > 45 && rdir <= 135 && leftUltra > 20 && leftUltra < 30){
				timeStall = millis();
				stall = true;
	  		}
	  		else if(rdir > 135 && rdir <=225 && frontUltra > 20 && frontUltra < 30){
				timeStall = millis();
				stall = true;
	  		}
	  		else if(rdir > 225 && rdir <=315 && rightUltra > 20 && rightUltra < 30){
				timeStall = millis();
				stall = true;

	  		}
	  		else if((rdir > 315 || rdir <=45) && backUltra > 20 && rightUltra < 30){
				timeStall = millis();
				stall = true;
	  		}
		}
		else setmove(rdir, 60, 0, 0);
		//Serial.print("OUT: ");
		//Serial.println(rdir);
	}
	}*/
		if(Light_Gate()){
			dribbler(1, 255);
			getAngle = readCmps();
			if(distGoal < 200){
				setmove(-angleCmps, 80, angleGoal, 0.3);
				if(abs(getAngle - angleGoal)<4)
				kick();
			}
			else setmove(-angleCmps, 60, angleGoal, 0.3);
		}
		else if(distBall < 120 && fabsf(angleBall)<50){
			dribbler(1, 255);
			setmove(angleMove, 60, 0, 0);
		}
		else{
			dribbler(0, 0);
			setmove(angleMove, 60, 0, 0);
		}







	#elif defined striker
	readPi();
	getAngle = readCmps();
	readUltra();
	//if balldist < threshold chase ball
	//if goaldist > threshold go back to goal
	//if lightgate turn to closer outside then kick
	//if balldist > threshold but <threshold1 activate dribbler
	goaly[0][0] = blueTopy;
	goaly[0][1] = blueBoty;
	goaly[0][2] = blueTopx;
	goaly[0][3] = blueBotx;
	goaly[1][0] = yellowTopy;
	goaly[1][1] = yellowBoty;
	goaly[1][2] = yellowTopx;
	goaly[1][3] = yellowBotx;
	Serial.print(side);
	Serial.print("  topy:  ");
	Serial.print(goaly[side][0]);
	Serial.print("  boty:  ");
	Serial.print(goaly[side][1]);
	Serial.print("  topx:  ");
	Serial.print(goaly[side][2]);
	Serial.print("  botx:  ");
	Serial.println(goaly[side][3]);

	if (ballx == 0) {
		setmove(0, 0, 0, 0);
		Serial.println("no ball");
	}
	/*else if (Light_Gate()){
		Serial.println("kick");
		if (kickcheck == 0){
			solenoidKick();
			solenoidMillis = millis();
			kickcheck = 2;
		}
		if (kickcheck == 2 && (millis() > (solenoidMillis+250))){
			kickcheck = 0;
		}
	}*/
	else if ((goaly[side][2]+goaly[side][3])/2 > goalThreshold){
		Serial.println("fly too far");
		goalieAlignBall(bally, goaly, backUltra, 20, 3, 20);
	}
	else if (findDist(ballx,bally) < ballThreshold){
		Serial.println("chase ball");
		//insert chaseball program here
	}
	else {
		//Serial.print("bluetopy:   ");
		//Serial.print(blueTopy);
		//Serial.print("   bluebottomy:   ");
		//Serial.print(blueBoty);
		//goaly[0][2] = findGoaly(blueTopy,blueBoty);
		//Serial.print("   yellowtopy:   ");
		//Serial.print(yellowTopy);
		//Serial.print("   yellowbottomy:   ");
		//Serial.print(yellowBoty);
		//goaly[1][2] = findGoaly(yellowTopy,yellowBoty);
		//Serial.println(backUltra);
		Serial.println("normal chase");
		goalieAlignBall(bally, goaly, backUltra, 20, 3, 2);
	}
	
	#endif
}