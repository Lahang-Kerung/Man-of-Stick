#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <string>
#include <cmath>

#define WINDOW_SIZE 900
bool start_flag;
bool end_flag;
bool afterwards;

void line(int x1, int y1, int x2, int y2){
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
}

void rectangle(int x1, int x2, int y1, int y2){
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x1, y2);
	glVertex2f(x2, y2);
	glVertex2f(x2, y1);
	glEnd();
}

void half_crown(){
        glBegin(GL_LINES);
	line(0, 5, 2, 3);
	line(2, 3, 4, 4);
	line(4, 4, 6, 3);
	line(6, 3, 5, 0);
	line(5, 0, 0, 0);
	line(5, 0, 5, -1);
	line(5, -1, 0, -1);
       	glEnd();
}
void crown(float a, float b, float c, float scale1, float scale2=0, int lwd = 5){
	int lw;
	if(!scale2)
		scale2 = scale1;
        glColor3f(a, b, c);
        glLineWidth(lwd);
        glPushMatrix();
        glScalef(scale1, scale2, 1);
        half_crown();
        glPushMatrix();
        glScalef(-1, 1, 1);
        half_crown();
        glPopMatrix();
        glPopMatrix();
}


void display_text(int x, int y, std::string text, float scale = 0.7f){
	glPushMatrix();
	glTranslatef(x,y,0);
	glScalef(scale, scale, 1);
	for(char c : text){
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}

class Fireball{
	int x, y, dir;
       	const float *c;
	public:
	char owner;
	Fireball(int a, int b, const float *color, int d, char own): x(a), y(b), c(color), dir(d), owner(own){}

	~Fireball(){}

	int get_x(){return x;}
	int get_y(){return y;}

	void display_fire(){
		glColor3f(c[0], c[1], c[2]);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y);
		for(int i=0; i<360; i++){
			float theta = i * 3.14159/180;
			glVertex2f(x+20*cos(theta), y+20*sin(theta));
		}
		glEnd();
	}

	void move_x(){x += (dir * 5);}

	bool inWindow(){
		if(x > -20 && x < WINDOW_SIZE+20)
			return true;
		return false;
	}
};

std::vector<Fireball> fireballs;

class Stickman{
	std::string name;
	int life;
	int direction;
	float *c;
	int x, y;
	int up, down, right, left;
	void head(float cx, float cy, float r){
		glPushMatrix();
		glTranslatef(cx, cy, 0);
		glBegin(GL_LINE_LOOP);
		for(float i=0; i< 360; i += 0.5){
			float theta = i*3.14159f/180;
			float a = r * cos(theta);
			float b = r * sin(theta);
			glVertex2f(a, b);
		}
		glEnd();

		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex2f(-r/3, r/3);
		glVertex2f(r/3, r/3);
		glEnd();
		glPopMatrix();
	}
	void body(){
		glBegin(GL_LINES);
		line(0, 40, 0, 10);
		line(0, 10, 15, -10);
		line(0, 10, -15, -10);
		line(0, 35, -15, 25);
		line(0, 35, 15, 25);
		glEnd();
		head(0, 50, 10);
	}

	public:
	Stickman(int pos,  float *color, int d): x(pos), direction(d), c(color){
		y = 200;
		life = 7;
		name="";
	}

	void get_name(){
		std::cout<<"Enter your name: ";
		std::cin >> name;
	}
	std::string ret_name(){return name;}

	bool isAlive(){return life;}

	void attack_right(){direction = 1;}

	void attack_left(){
		direction = -1;
	}

	void inc_x(){
		x += 50;
		calc_AOE();
		if(left > WINDOW_SIZE)
			x -= WINDOW_SIZE;
	}
	void dec_x(){
		x -= 50;
		if(right < 0)
			x += WINDOW_SIZE;
	}
	void inc_y(){y += 150;}
	void dec_y(){y -= 150;}


	void display_health(){
		glPushMatrix();
		glTranslatef(left, up+30, 0);
		int x1=0, x2=7, y1=0, y2=7;
		for(int i=0; i<life; i++){
			rectangle(x1, x2, y1, y2);
			x1 += 14;
			x2 += 14;
		}
		glPopMatrix();
	}

	void display_name(){
		glPushMatrix();
		glTranslatef(left, up+7, 0);
		display_text(0, 0, name, 0.2f);
		glPopMatrix();
	}

	void display_man(){
		glColor3f(c[0], c[1], c[2]);
		glLineWidth(1);
		calc_AOE();
		display_name();
		glLineWidth(3);
		//display_AOE();
		if(!end_flag)
			display_health();
		if(end_flag){
			glPushMatrix();
			glTranslatef((right+left)/2, up+5, 0);
			crown(c[0], c[1], c[2], 4, 7, 3);
			glPopMatrix();
		}
		glPushMatrix();
		glTranslatef(x,y,0);
		glScalef(3,3,0);
		body();
		glPopMatrix();
	}

	void death(){
		x = WINDOW_SIZE/2;
		y = 170;

		glColor3f(c[0], c[1], c[2]);
		glLineWidth(3);
		calc_AOE();
		
		glPushMatrix();
		glTranslatef(x,y,0);
		glRotatef(90, 0, 0, 1);
		glScalef(3,3,1);
		body();

		glColor3f(1,0,0);
		glPushMatrix();
		glTranslatef(0, 45, 0);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0);
		for(int i=160; i<200; i++){
			float theta = i * 3.14159/180;
			glVertex2f(25*cos(theta), 25*sin(theta));
		}
		glEnd();
		glPopMatrix();

		glPopMatrix();
	}

	void calc_AOE(){
		up = y + 60*3;
		down = y - 10*3;
		right = x + 15*3;
		left = x - 15*3;

	}

	void display_AOE(){
		glBegin(GL_LINES);
		line(right, up, left, up);
		line(right, down, left, down);
		line(right, up, right, down);
		line(left, up, left, down);
		glEnd();
	}

	bool in_AOE(int x, int y){return (x<=right && x>=left && y<=up && y>=down);}

	void isHit(){
		if(!isDead())
			life--;
	}

	bool isDead(){
		if(!life){
			end_flag = true;
			return true;
		}
		return false;
	}

	int get_x(){return x;}
	int get_y(){return y;}
	int get_dir(){return direction;}
	float* get_col(){return c;}

};
float colors[][3] = {
    {1.0f, 0.0f, 0.0f}, // Red
    {0.0f, 1.0f, 0.0f}, // Green
    {0.0f, 0.0f, 1.0f}, // Blue
    {1.0f, 0.0f, 1.0f}, // Magenta
    {0.0f, 1.0f, 1.0f}, // Cyan
    {1.0f, 0.5f, 0.0f}, // Orange
    {0.5f, 0.0f, 0.5f}  // Purple
};

int col1=rand()%7, col2=rand()%7;
Stickman past(150, colors[col1], 1), future(750, colors[col2], -1);

void firetimer(int){
	for(auto it=fireballs.begin(); it!=fireballs.end(); ){
		it->move_x();
		int x = it->get_x(), y=it->get_y();
		if(!it->inWindow()){
			it = fireballs.erase(it);
		}
		else if(it->owner == 'f' && past.in_AOE(x, y)){
			past.isHit();
			it = fireballs.erase(it);
		}
		else if(it->owner == 'p' && future.in_AOE(x, y)){
			future.isHit();
			it = fireballs.erase(it);
		}
		else{
			++it;
		}
	}
	glutPostRedisplay();
	if(!fireballs.empty())
		glutTimerFunc(30, firetimer, 0);
}

void disp_afterward(int){
	afterwards = true;
	glutPostRedisplay();
}

void display1(){
	if(start_flag){
		glColor3f(0,1,0);
		glLineWidth(5);
		display_text(160, 600, "GAME START");
		start_flag = false;
	}
        glPushMatrix();
        glTranslatef(WINDOW_SIZE/2, WINDOW_SIZE/2+100, 0);
	crown(1, 0.8, 0, WINDOW_SIZE/16);
	glPopMatrix();
	glColor3f(0.12,0.64,0.87);
	rectangle(0, WINDOW_SIZE, 0, 170);
	if(!past.isDead())
		past.display_man();
	if(!future.isDead())
		future.display_man();

	for(auto &it : fireballs){
		it.display_fire();
	}

	if(end_flag){
		display_text(160, 600, "GAME OVER");
		std::string winner;
		int draw=0;
		if(!past.isDead())
			winner = past.ret_name();
		else if(!future.isDead())
			winner = future.ret_name();
		else
			draw = 1;
		if(!draw)
			display_text(180, 500, winner+" WINS", 0.5f);
		else
			display_text(180, 500, "DRAW");

		glutTimerFunc(2000, disp_afterward, 0);
	}

}

void display2(){
	glClearColor(0,0,0,0);
	if(past.isDead())
		past.death();
	if(future.isDead())
		future.death();
	display_text(100, WINDOW_SIZE/2, "MURDERER", 1);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	if(!afterwards)
		display1();
	else
		display2();
	glFlush();
}

void goDown_f(int){
	future.dec_y();
	glutPostRedisplay();
}

void goDown_p(int){
	past.dec_y();
	glutPostRedisplay();
}

void keyboard1(int key, int, int){
	if(!future.isDead()){
		switch(key){
			case GLUT_KEY_UP:
				future.inc_y();
				glutTimerFunc(200, goDown_f, 0);
				break;
			case GLUT_KEY_RIGHT:
				future.inc_x();
				future.attack_right();
				break;
			case GLUT_KEY_LEFT:
				future.dec_x();
				future.attack_left();
				break;
			case GLUT_KEY_DOWN:
				fireballs.emplace_back(future.get_x()+future.get_dir()*45, future.get_y()+75, future.get_col(), future.get_dir(), 'f');
				glutTimerFunc(30, firetimer, 0);
				break;
		}
	}
	glutPostRedisplay();
}

void keyboard2(unsigned char key, int, int){
	if(!past.isDead()){
		switch(key){
			case 'w':
				past.inc_y();
				glutTimerFunc(200, goDown_p, 0);
				break;
			case 'd':
				past.inc_x();
				past.attack_right();
				break;
			case 'a':
				past.dec_x();
				past.attack_left();
				break;
			case 's':
				fireballs.emplace_back(past.get_x()+past.get_dir()*45, past.get_y()+75, past.get_col(), past.get_dir(), 'p');
				glutTimerFunc(30, firetimer, 0);
				break;
		}
	}
	glutPostRedisplay();
}

void init(){
        glClearColor(1,1,1,1);
        glColor3f(0,0,0);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(1,WINDOW_SIZE, 0, WINDOW_SIZE);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}


int main(int argc, char** argv){
	start_flag = true;
	end_flag = false;
	afterwards = false;
	std::cout << "PLAYER 1"<< std::endl;
	past.get_name();
	std::cout << "PLAYER 2"<< std::endl;
	future.get_name();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutCreateWindow("Man of Stick");

	init();

	glutDisplayFunc(display);
	glutSpecialFunc(keyboard1);
	glutKeyboardFunc(keyboard2);
	glutMainLoop();
	return 0;
}
