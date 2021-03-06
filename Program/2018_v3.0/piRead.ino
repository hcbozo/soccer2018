float findAngle(float x, float y){
	x -= 400;
	y -= 304;
	if(x==0) x = 0.001;
	angle = atanf(y/x);
	angle = angle*180/pi;
	if (x<0){
		if (angle == 0) angle = 0.001;
		angle = (angle/fabsf(angle))*(180-fabsf(angle));
	}
		else angle = -angle;
	return angle;
} 
float findDist(float x, float y){
	x -= 400;
	y -= 304;
	if(x==0) x = 0.001;
	tdist = sqrtf(powf(x,2)+powf(y,2));
	dist = 1.785*(powf(M_E,0.0164*distf));
	return dist; 
}
float findCenter(float x1, float y1, float x2, float y2){
	x1 -= 400;
	x2 -= 400;
	x2 -= 304;
	y2 -= 304;
	centerx = (x1+x2)/2;
	centery = (y1+y2)/2;
}
/* ancient code
void readCamera(){
	if (Serial4.available()>0){
		while (Serial4.available()>0) {
			incomingByte = Serial4.readString();
		}
		end = incomingByte.indexOf('b') + 1;
		if (end==120);
		else incomingByte.remove(0, end);
		incomingByte.remove(5);
		if (incomingByte == "rrrrr") angleBall = 0;
		else if (incomingByte == "0.000") angleBall = 0.001;
		else angleBall = incomingByte.toFloat();
	}
	else angleBall = 0;
}*/
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
		ballx = piSerialCancer();
		bally = piSerialCancer();
		yellowTopx = piSerialCancer();
		yellowTopy = piSerialCancer();
		yellowBotx = piSerialCancer();
		yellowBoty = piSerialCancer();
		blueTopx = piSerialCancer();
		blueTopy = piSerialCancer();
		blueBotx = piSerialCancer();
		blueBoty = piSerialCancer();
		//eg a010a009a008a007a006a005a004a003a002a001ab
	}
}
void initPi(){
	Serial4.write(1);
	while (Serial4.available()==0);
	incomingByte = Serial4.readStringUntil('b');
	side = incomingByte.toFloat();
}
void trackAngle(){
	angleBall = findAngle(ballx, bally);
	distBall = findDist(ballx, bally);
	if (fabsf(angleBall)<101) angleMove = angleBall*(1+20/distball);
	else if (angleBall>100) angleMove = angleBall + (90*20/distball);
	else if (-100>angleBall) angleMove = angleBall - (90*20/distball);
	if (angleMove<0) angleMove = 360+angleMove;
}
void trackGoal(){
	if(side==1) findCenter(blueTopx, blueTopy, blueBotx, blueBoty);
	else findCenter(yellowTopx, yellowTopy, yellowBotx, yellowBoty);
	angleGoal = findAngle(centerx, centery);
	distGoal = findDist(centerx, centery);
}