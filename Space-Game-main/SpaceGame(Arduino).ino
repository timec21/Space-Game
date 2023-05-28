#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoSTL.h>

#define _GLIBCXX_USE_NOEXCEPT

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const byte potPin=A0;
const int buzzer = 22;
int potDeger, ct;
int buttonState = 0, buttonPin = 32;
int selectionState = 0, selectionButtonPin = 38;
int downState = 0, downButtonPin = 39;
int upState = 0, upButtonPin = 40;
int leds[] = {46, 45, 44, 47, 48, 49};
#define LATCH 50
#define DATA 51
#define CLK 52
#define MR 53

byte numbers[10]={63, 6, 91, 79, 102, 109, 125, 7, 127, 111};
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

class Plane
{
public:
  int num = 5;
  int loc = 4;
  int health = 3;
  int bullet = 3;
  int score = 0;
  int passedEnemy = 0;
  bool immune = false;
  int a = 1200;
};

class Bullet
{
public:
  int num = 7;
  int x_loc;
  int y_loc;

  Bullet(Plane* plane)
  {
    x_loc = 0;
    y_loc = plane->loc;
  }
};
int ldr()
{
  int sensorValue = analogRead(A3);
  Serial.print("LDR: ");
  Serial.println(sensorValue);
  return sensorValue;
}
void showHealth_Bullet(int health, int bullet)
{
  for(int i = 0; i < 6; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);

  }

  for(int i = 0; i < health; i++)
  {
     digitalWrite(leds[i], HIGH);
  }
  for(int i = 3; i < bullet + 3; i++)
  {
     digitalWrite(leds[i], HIGH);
  }

}

void showScore(int score)
{
  digitalWrite(LATCH, LOW);
  byte first_digit= score/100;
  score = score % 100; 
  byte second_digit = score/10;
  byte third_digit = score%10;
  shiftOut(DATA,CLK,MSBFIRST, numbers[first_digit]);
  shiftOut(DATA,CLK,MSBFIRST, numbers[second_digit]);
  shiftOut(DATA,CLK,MSBFIRST, numbers[third_digit]);
  digitalWrite(LATCH, HIGH);

}

void printMatrix(int rows, int cols, int** matrix, Plane* plane)
{
	matrix[0][plane->loc] = plane->num;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {
            switch(matrix[i][j]){
              case 1:
                display.fillRect(128-(rows-i)*8+3,3+(j*8), 2, 2, WHITE);
                break;
              case 2:
                display.fillRect(129-((rows-i)*8),1+(j*8), 1, 2, WHITE);
                display.fillRect(129-((rows-i)*8),5+(j*8), 1, 2, WHITE);
                display.fillRect(130-((rows-i)*8),1+(j*8), 1, 6, WHITE);
                display.fillRect(131-((rows-i)*8),2+(j*8), 2, 4, WHITE);
                display.fillRect(133-((rows-i)*8),1+(j*8), 1, 6, WHITE);
                display.fillRect(134-((rows-i)*8),1+(j*8), 1, 2, WHITE);
                display.fillRect(134-((rows-i)*8),5+(j*8), 1, 2, WHITE);
                break;
              case 3:
                display.fillRect(129-((rows-i)*8),3+(j*8), 5, 1, WHITE);
                display.fillRect(131-((rows-i)*8),1+(j*8), 1, 5, WHITE);
                break;
              case 5:
                if(plane->immune)
                {
                  display.fillRect(130-((rows-i)*8),2+(j*8), 1, 4, WHITE);
                  display.fillRect(131-((rows-i)*8),1+(j*8), 2, 1, WHITE);
                  display.fillRect(131-((rows-i)*8),6+(j*8), 2, 1, WHITE);
                  display.fillRect(133-((rows-i)*8),2+(j*8), 1, 1, WHITE);
                  display.fillRect(133-((rows-i)*8),5+(j*8), 1, 1, WHITE);
                  display.fillRect(134-((rows-i)*8),3+(j*8), 1, 2, WHITE);
                }else
                {
                  display.fillRect(130-((rows-i)*8),3+(j*8), 1, 2, WHITE);
                  display.fillRect(131-((rows-i)*8),1+(j*8), 2, 6, WHITE);
                  display.fillRect(133-((rows-i)*8),2+(j*8), 1, 4, WHITE);
                  display.fillRect(134-((rows-i)*8),3+(j*8), 1, 2, WHITE);
                }
                break;
              case 7:
                display.fillRect(133-(rows-i)*8,3+(j*8),2,2, WHITE);
                display.fillRect(131-(rows-i)*8,2+(j*8),2,4, WHITE);
                display.fillRect(129-(rows-i)*8,1+(j*8),2,6, WHITE);
                break;
             
              case 8:
                display.fillRect(129-((rows-i)*8),3+(j*8), 1, 2, WHITE);
                display.fillRect(130-((rows-i)*8),2+(j*8), 4, 4, WHITE);
                display.fillRect(134-((rows-i)*8),3+(j*8), 1, 2, WHITE);
                break;
            }

        }
    }
    if(ldr()>180)
      display.invertDisplay(true);
    else
      display.invertDisplay(false);

    display.display();
}


void enemyPassed(int rows, int cols, int** matrix, Plane* plane)
{
        for (int i = 0; i < cols; i++)
        {
                if (matrix[0][i] == 1 || matrix[0][i] == 2)
                {
                        plane->passedEnemy++;
                }   }
        if (plane->passedEnemy >= 14)
        {
                if (plane->health < 3)
                        plane->health++;
                if (plane->bullet < 3)
                        plane->bullet++;
                plane->passedEnemy = 0;
        }
}


void fire(int** matrix, Plane* plane,  std::vector<Bullet>* bullets)
{
        if (plane->bullet <= 3 && plane->bullet > 0)
        {
                plane->bullet--;
                Bullet bullet(plane);
                bullets->push_back(bullet);
        }
}

void createObj(int** matrix, int rows, int cols, Plane* plane, std::vector<Bullet>* bullets)
{
  for (int i = 0; i < bullets->size(); i++)
	{
		if (bullets->at(i).x_loc != 0)
			matrix[bullets->at(i).x_loc][bullets->at(i).y_loc] = bullets->at(i).num;
	} 
	int* newRow;
	int objNum = random(0, cols);// % cols;

	for (int i = 0; i < objNum; i++)
	{
		int objType = random(1, 3);// % 2 + 1;
		int chance = random(0, 100);// % 100;
		int objLoc = random(0, cols);// % cols;
		if (chance > 10 && chance < 20)
			objType = 3;

		if (chance < 10)
			objType = 8;

		if (objLoc == cols - 1 && matrix[rows - 1][objLoc - 1] != 1 && matrix[rows - 1][objLoc - 1] != 2)
			matrix[rows - 1][objLoc] = objType;
		else if (objLoc == 0 && matrix[rows - 1][objLoc + 1] != 1 && matrix[rows - 1][objLoc + 1] != 2)
			matrix[rows - 1][objLoc] = objType;
		else if (matrix[rows - 1][objLoc + 1] != 1 && matrix[rows - 1][objLoc + 1] != 2 && matrix[rows - 1][objLoc - 1] != 1 && matrix[rows - 1][objLoc - 1] != 2)
			matrix[rows - 1][objLoc] = objType;
	}

  ct = analogRead(potPin);

  if(potDeger > ct && plane->loc != 0)
  {
    plane->loc--;
    if (matrix[0][plane->loc] == 1 || matrix[0][plane->loc] == 2)
	  {
		  if (!plane->immune)
		  {
			  plane->health--;
			  plane->immune = true;
        tone(buzzer, 1000); 
        delay(30);
        noTone(buzzer);
		  }
	  }
    if (matrix[0][plane->loc] == 3 && plane->health < 3)
	  	plane->health++;

    if (matrix[0][plane->loc] == 8)
	  {
		  plane->immune = true;
		  plane->a = 1200;
	  } 
  
  } 
  else if(potDeger < ct && plane->loc != cols - 1) 
  {
    plane->loc++; 
    if (matrix[0][plane->loc] == 1 || matrix[0][plane->loc] == 2)
	  {
		  if (!plane->immune)
		  {
			  plane->health--;
        tone(buzzer, 1000);
        delay(30);
        noTone(buzzer);
			  plane->immune = true;
		  }
	  }
    if (matrix[0][plane->loc] == 3 && plane->health < 3)
	  	plane->health++;

    if (matrix[0][plane->loc] == 8)
	  {
		  plane->immune = true;
		  plane->a = 1200;
	  }  
  
  }
  else if(potDeger == ct)
  {
    if (matrix[0][plane->loc] == 1 || matrix[0][plane->loc] == 2)
	  {
		  if (!plane->immune)
		  {
			  plane->health--;
        tone(buzzer, 1000);
        delay(30);
        noTone(buzzer);
			  plane->immune = true;
		  }
	  }
    if (matrix[0][plane->loc] == 3 && plane->health < 3)
	  	plane->health++;

    if (matrix[0][plane->loc] == 8)
	  {
		  plane->immune = true;
		  plane->a = 1200;
	  }   
  }
  buttonState = digitalRead(buttonPin);
     if(buttonState == HIGH)
      fire(matrix, plane, bullets);
      
   
    
  printMatrix(rows, cols, matrix, plane);
  Serial.print("Can: ");Serial.println((int)plane->health);  
  Serial.print( "Dusman: ");Serial.println((int)plane->passedEnemy);
  Serial.print( "Mermi: "); Serial.println((int)plane->bullet);
  Serial.print( "Skor: ");Serial.println((int)plane->score);
  Serial.println();

 
	for (int i = 0; i < bullets->size(); i++)
	{
		if (bullets->at(i).x_loc == rows)
		{
			bullets->erase(bullets->begin() + i);
			i--;
			continue;
		}
		if (matrix[bullets->at(i).x_loc + 1][bullets->at(i).y_loc] == 1 || matrix[bullets->at(i).x_loc + 1][bullets->at(i).y_loc] == 2)
		{
			matrix[bullets->at(i).x_loc + 1][bullets->at(i).y_loc] --;
			matrix[bullets->at(i).x_loc][bullets->at(i).y_loc] = 0;
			bullets->erase(bullets->begin() + i);
			i--;
			continue;
		}
		else if (matrix[bullets->at(i).x_loc + 2][bullets->at(i).y_loc] == 1 || matrix[bullets->at(i).x_loc + 2][bullets->at(i).y_loc] == 2)
		{
			matrix[bullets->at(i).x_loc + 2][bullets->at(i).y_loc] --;
	    matrix[bullets->at(i).x_loc][bullets->at(i).y_loc] = 0;
			bullets->erase(bullets->begin() + i);
			i--;
			continue;
		}

		if (bullets->size() > 0)
			bullets->at(i).x_loc++;
	}
	//scoreee
	plane->score++;

  // health & immune
	if (plane->immune)
		plane->a--;

	if (plane->a < 0)
	{
		plane->immune = false;
		plane->a = 1200;
	}

	for (int i = 0; i < bullets->size(); i++)
	{
		matrix[bullets->at(i).x_loc - 1][bullets->at(i).y_loc] = 0;
	}
	
  enemyPassed(rows, cols, matrix, plane);

	for (int i = 0; i < rows - 1; i++)
	{
		for (int j = 0; j < cols; j++)
			matrix[i][j] = matrix[i + 1][j];
	}

	for (int i = 0; i < cols; i++)
	{
		matrix[rows - 1][i] = 0;
	}

  

}

void firstScreen()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // P
  display.fillRect(40, 18, 8, 1, WHITE);
  display.fillRect(44, 19, 1, 3, WHITE);
  display.fillRect(47, 19, 1, 3, WHITE);
  display.fillRect(45, 22, 2, 1, WHITE);
  // L
  display.fillRect(40, 24, 1, 5, WHITE);
  display.fillRect(40, 24, 8, 1, WHITE);  
  // A
  display.fillRect(40, 31, 1, 1, WHITE);
  display.fillRect(41, 31, 1, 2, WHITE);
  display.fillRect(42, 32, 2, 6, WHITE);
  display.drawRect(43, 33, 3, 4, WHITE);
  display.fillRect(46, 34, 2, 2, WHITE);
  display.fillRect(40, 38, 1, 1, WHITE);
  display.fillRect(41, 37, 1, 2, WHITE);  
  display.fillRect(43, 34, 1, 2, BLACK);  
  // Y
  display.fillRect(40, 41, 4, 2, WHITE);
  display.fillRect(44, 40, 1, 4, WHITE);
  display.fillRect(45, 39, 3, 1, WHITE);
  display.fillRect(45, 40, 2, 1, WHITE);
  display.fillRect(45, 43, 2, 1, WHITE);
  display.fillRect(45, 44, 3, 1, WHITE);
  /////////////////////////////////////////
  // L
  display.fillRect(100, 18, 1, 5, WHITE);
  display.fillRect(100, 18, 6, 1, WHITE);

  //V
  display.fillRect(103, 23, 3, 1, WHITE);
  display.fillRect(101, 24, 2, 1, WHITE);
  display.fillRect(100, 25, 1, 2, WHITE);
  display.fillRect(101, 27, 2, 1, WHITE);
  display.fillRect(103, 28, 3, 1, WHITE);  
  
  // L
  display.fillRect(100, 30, 1, 5, WHITE);
  display.fillRect(100, 30, 6, 1, WHITE);

  // 1
  display.drawRect(100, 39, 1, 5, WHITE);
  display.drawRect(100, 41, 7, 1, WHITE);
  display.drawRect(104, 39, 1, 1, WHITE);
  display.drawRect(105, 40, 1, 1, WHITE);

  ///////////////////////////////////////////
  // L
  display.fillRect(70, 18, 1, 5, WHITE);
  display.fillRect(70, 18, 6, 1, WHITE);

  //V
  display.fillRect(73, 23, 3, 1, WHITE);
  display.fillRect(71, 24, 2, 1, WHITE);
  display.fillRect(70, 25, 1, 2, WHITE);
  display.fillRect(71, 27, 2, 1, WHITE);
  display.fillRect(73, 28, 3, 1, WHITE);  
  
  // L
  display.fillRect(70, 30, 1, 5, WHITE);
  display.fillRect(70, 30, 6, 1, WHITE);

  // 2
  display.fillRect(70, 39, 1, 5, WHITE);
  display.fillRect(71, 40, 1, 1, WHITE);  
  display.fillRect(72, 41, 1, 1, WHITE);
  display.fillRect(73, 42, 1, 1, WHITE);
  display.fillRect(74, 43, 2, 1, WHITE);
  display.fillRect(76, 40, 1, 3, WHITE);
  display.fillRect(74, 39, 2, 1, WHITE);
  
  int counter = 1, level = 1;
  pinMode(upButtonPin,INPUT);
  pinMode(downButtonPin,INPUT);
  pinMode(selectionButtonPin,INPUT);
  while(true)
  {
    if(counter == 1)
    {
      //1_frame
      display.drawRect(98, 16, 11, 30, WHITE);
      display.display();
      delay(153);
      display.drawRect(98, 16, 11, 30, BLACK);
      display.display();
    }
    if(counter == 2)
    {
      // 2_frame
      display.drawRect(68, 16, 11, 30, WHITE);
      display.display();
      delay(153);
      display.drawRect(68, 16, 11, 30, BLACK);
      display.display();
    }
    if(counter == 3)
    {
      // play_frame
      display.drawRect(38, 16, 12, 31, WHITE);
      display.display();
      delay(153);
      display.drawRect(38, 16, 12, 31, BLACK);
      display.display();
    } 

    if(digitalRead(downButtonPin) == HIGH && counter != 3)
      counter++;
     else if (digitalRead(downButtonPin) == HIGH && counter == 3)
      counter = 1;
    if(digitalRead(upButtonPin) == HIGH  && counter != 1)
      counter--;
    else if (digitalRead(upButtonPin) == HIGH && counter == 1)
      counter = 3;
    if(digitalRead(selectionButtonPin) == HIGH  && counter == 1)
    {
      display.drawRect(98, 16, 11, 30, WHITE);
      display.drawRect(68, 16, 11, 30, BLACK);
      display.display();
      level = 1;
      counter = 3;
    }
    else if (digitalRead(selectionButtonPin) == HIGH && counter == 2)
    {
      display.drawRect(68, 16, 11, 30, WHITE);
      display.drawRect(98, 16, 11, 30, BLACK);
      display.display();
      level = 2;
      counter = 3;
    }
    else if (digitalRead(selectionButtonPin) == HIGH && counter == 3)
    {
      gameScreen(level);
    }
  }
}

void gameScreen(int level)
{
  pinMode(buttonPin,INPUT);
  pinMode(buzzer, OUTPUT);

  for(int i = 0; i < 6; i++)
  {
    pinMode(leds[i], OUTPUT);
  }
 
  int rows = 16;
	int cols = 8;
	int** matrix = new int* [rows];
  float milliSec = 400;
  int time = 0;
	for (int i = 0; i < rows; i++) {
		matrix[i] = new int[cols];
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = 0;
		}
	}
	std::vector<Bullet> bullets;
	Plane plane;
  potDeger=analogRead(potPin);
  ct = potDeger;
	while(true)
	{
    if(plane.health == 0)
      break;
    showScore(plane.score);
		createObj(matrix, rows, cols, &plane, &bullets);  
    showHealth_Bullet(plane.health, plane.bullet);
    potDeger=analogRead(potPin);
    delay(milliSec);
    if(plane.immune)
    {
      plane.a -= milliSec;
      Serial.print("immune");Serial.println(plane.a);
    }
    
    if(level == 2 && time > 4000)
    { 
      milliSec *= 0.8;
      
      time -= 4000;
    }
    time +=milliSec;
    Serial.print("Delay: ");
    Serial.println(milliSec);
    Serial.print("Sure: ");
    Serial.println(time);
	}
  firstScreen();
  
}

void setup() {

  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(buzzer, OUTPUT);

  pinMode (LATCH, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(MR, OUTPUT);
  digitalWrite(MR, LOW);
  digitalWrite(MR, HIGH);
}
void loop() {

 firstScreen();

}
