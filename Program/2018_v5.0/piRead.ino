float findAngle(float x, float y){
	x -= 304;
	y -= 228;
	if(x==0) x = 0.001;
	angle = atanf(y/x);
	angle = angle*180/pi;
	angle+=getAngle;
	if (x<0){
		if (angle == 0) angle = 0.001;
		angle = (angle/fabsf(angle))*(180-fabsf(angle));
	}
		else angle = -angle;
	return angle;
} 
float findDist(float x, float y){
	x -= 304;
	y -= 228;
	if(x==0) x = 0.001;
	dist = sqrtf(powf(x,2)+powf(y,2));
	//ancient: dist = 1.785*(powf(M_E,0.0164*tdist));
	//y = 14.08453 - (-0.003564601/-0.02649916)*(1 - e^(+0.02649916*x))
	//y = -(19.90932874x)/(x-292.9796815)
	//dist = 14.08453 - ((0.134517509)*(1-powf(M_E,(0.02649916*tdist))));
	//dist = -(19.90932874*tdist)/(tdist-292.9796815);
	return dist; 
}
void findCenter(float x1, float y1, float x2, float y2){
	centerx = (x1+x2)/2;
	centery = (y1+y2)/2;
}
float piSerialCancer(){
	tempString = incomingByte;
	break1 = incomingByte.indexOf("a");
	tempString.remove(0,(break1+1));
	incomingByte = tempString;
	break1 = incomingByte.indexOf("a");
	tempString.remove(break1);
	return tempString.toFloat();
}
void readPi(){
	if (Serial4.available()>0){
		while (Serial4.available()>0) {
			incomingByte = Serial4.readStringUntil('b');
		}
		tclr = piSerialCancer();
		if(tclr < 2) home();
		else if(tclr != 0) ballx = tclr;
		tclr = piSerialCancer();
		if(tclr < 2) home();
		else if(tclr != 0) bally = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) yellowBotx = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) yellowBoty = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) yellowTopx = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) yellowTopy = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) blueBotx = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) blueBoty = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) blueTopx = tclr;
		tclr = piSerialCancer();
		if(tclr != 0) blueTopy = tclr;
		//eg a010a009a008a007a006a005a004a003a002a001ab
	}
}
void initPi(){
	Serial4.write(1);
	Serial.println("written");
	readPi();
	if (yellowBotx>400){
		side = 0;
	}
	else{
		side = 1;
	}
	Serial.println(side);
}
void trackAngle(){
	angleBall = findAngle(ballx, bally);
	distBall = findDist(ballx, bally);
	if (angleBall<101&&angleBall>=0){
		if(distBall>120) angleMove = angleBall*(1+sqrtf(powf(100, 2)/powf(distBall, 2)));
		else angleMove = angleBall*1.4+(120 - distBall)*0.44;
	}
	else if(angleBall>-100 && angleBall<0){
		if(distBall>100) angleMove = angleBall*(1+sqrtf(powf(100, 2)/powf(distBall, 2)));
		else angleMove = angleBall*1.5+(100 - distBall)*0.4;
	}
	else if (angleBall>100){
		if(distBall>120) angleMove = angleBall + (90*sqrtf(powf(90, 2)/powf(distBall, 2)));
		else angleMove = angleBall + 45 + (140 - distBall)/30*45;
	}
	else if (-100>angleBall){
		if(distBall>95) angleMove = angleBall - (90*sqrtf(powf(90, 2)/powf(distBall, 2)));
		else angleMove = angleBall - 45 - (140 - distBall)/30*45;
	}
	if (angleMove<0) angleMove = 360+angleMove;
}
void trackGoal(){
	if(side==0) findCenter(blueTopx, blueTopy, blueBotx, blueBoty);
	else findCenter(yellowTopx, yellowTopy, yellowBotx, yellowBoty);
	angleGoal = findAngle(centerx, centery);
	distGoal = findDist(centerx, centery);
	if(angleGoal>0) fieldside = 1; //left
	else fieldside = 2; //right
}