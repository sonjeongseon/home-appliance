#pragma once
#include<stdlib.h>
#include<math.h>
#include<thread>
#include<pthread.h>
#include<mutex>
#include<deque>
#include<io.h>
#include<cstdlib>
#include<iostream>
#include<GL/glut.h>

#include <cstdint>
#include <memory>
#include <string>

#include "curl/curl.h"
#include "json/include/json/json.h"
#pragma comment(lib, "json/jsoncpp.lib")


using namespace std;

static mutex m;
int prex = 90;
int pret = 0;
int i = 1;
int move;
int coinform;
int teminform;
int huminform;
deque<int> dqx; // int �� deque ����
deque<int> dqy;
deque<int> dqz;

namespace
{
	std::size_t callback(
		const char* in,
		std::size_t size,
		std::size_t num,
		char* out)
	{
		std::string data(in, (std::size_t)size * num);
		*((std::stringstream*)out) << data;
		return size * num;
	}
}




void getData(void)	//������� �޴� ������ ���... ���⼭�� x , y ,z ��(y��) ���� ��... x���� t�ð�(���ø�)
{
	while (1) {

		m.lock(); // thread�� lock~ulock ���� mutex ����
		/*dqx.push_back((-90) + rand() % 180); // push_back : �� ���� �߰�
		dqy.push_back((-90) + rand() % 180);
		dqz.push_back((-90) + rand() % 180); */

		CURL* curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, "http://hanbatcom.systems:3001/newEnv");// ���� url����, curl_init���� ���� �ʱ�ȭ�Ҷ� ������ ����

		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);//���ø����̼��� ȣ��Ʈ �̸�Ȯ�ν� ����� IP�ּ� ����

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//�Լ��� ����� �� �ִ� �ִ� �ð� �ʴ���

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);// TRUE -> �������� location:header ����� �� �ּҷ� �ٽ� ��û �õ�, �����

		int httpCode(0);

		std::stringstream httpData;

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);//callback�Լ� (cUrl resource, ������ ���ڿ�)

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &httpData);

		curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);//������ ���� �ڵ� �ޱ�
		curl_easy_cleanup(curl);

		if (httpCode == 200)
		{
			Json::Value jsonData;
			Json::CharReaderBuilder jsonReader;
			std::string errs;


			if (Json::parseFromStream(jsonReader, httpData, &jsonData, &errs))
			{

				for (auto it = jsonData.begin(); it != jsonData.end(); it++)
				{
					if (it->isObject())
					{

						//cout << (*it)["co2_value"] << endl;
						coinform = (*it)["co2_value"].asInt();
						teminform = (*it)["tem_value"].asInt();
						huminform = (*it)["hum_value"].asInt();
					}
				}
			}
		}
		dqx.push_back(coinform/100);
		dqy.push_back(teminform);
		dqz.push_back(huminform);
		if (dqx.size() > 100)
		{
			dqx.pop_front(); // pop_front : �� ���� ����
			dqy.pop_front();
			dqz.pop_front();
		}

		m.unlock();
		Sleep(7000);
	}
}


/* Sample func itself �߽ɼ� �߱� ��*/
float func(float x)
{
	return x;
}

/* Function plotting func */
void draw(float (*func)(float x), float x1, float x2, float y1, float y2)
{

	glViewport(0, 0, 1000, 100);	//�����ϴ� 1, 2, ����, ����
	glMatrixMode(GL_PROJECTION); // ���ؼ� ȭ�鿡 ���� ���
	glLoadIdentity(); // ������� �ʱ�ȭ
	glOrtho(0, 1, 0, 1, -1, 1); // ����Ʈ ����� �ְ� ���� ���� �޿����� �������� �ձ���
	glMatrixMode(GL_MODELVIEW); // ���� ��ġ
	glLoadIdentity(); // �ʱ�ȭ 2

	glPushMatrix(); /* GL_MODELVIEW is default */


	glScalef(1.0 / (x2 - x1), 1.0 / (y2 - y1), 1.0); // ���� ��Ŀ� ���� ����
	//������ ��...
	glTranslatef(0, 0, 0.0);
	glColor3f(1.0, 0.0, 1.0);//��ü �� ����

	glBegin(GL_LINE_STRIP);//�� �����ؼ� �׸���
	glVertex2f(0, func(90));// ������ ���� x y
	glVertex2f(1000, func(90));
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(1, func(0));
	glVertex2f(1, func(180));
	glEnd();

	m.lock();	//�����͸� �׸��� getData���� �����͸� �����ϴ°� �����ϱ� ���� �����Ǹ� ����
	for (deque<int>::iterator iter = dqz.begin(); iter != dqz.end(); ++iter)
	{

		glBegin(GL_LINE_STRIP);
		glVertex2f(pret, func(prex)); //pre
		glVertex2f(i, func(90 + *iter)); //90+@  next -> pre
		glEnd();
		pret = i;
		i += 10;
		prex = 90 + *iter;
		glFlush();
	}
	glPopMatrix();
	m.unlock();

	prex = 90;
	pret = 0;
	i = 1;

	///////////////////////////////////Viewport 1 end


	glViewport(0, 100, 1000, 100); //100���� ~ +100��
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix(); /* GL_MODELVIEW is default */

	glScalef(1.0 / (x2 - x1), 1.0 / (y2 - y1), 1.0);
	//������ ���ϱ�...
	glTranslatef(0, 0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, func(90));
	glVertex2f(1000, func(90));
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(1, func(0));
	glVertex2f(1, func(180));
	glEnd();

	m.lock();
	for (deque<int>::iterator iter = dqy.begin(); iter != dqy.end(); ++iter)
	{


		glBegin(GL_LINE_STRIP);
		glVertex2f(pret, func(prex)); //pre
		glVertex2f(i, func(90 + *iter)); //90+@  next -> pre
		glEnd();
		pret = i;
		i += 10;
		prex = 90 + *iter;
		glFlush();
	}

	glPopMatrix();
	m.unlock();
	///////////////////////////////////Viewport 2 end


	prex = 90;
	pret = 0;
	i = 1;


	glViewport(0, 200, 1000, 100);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix(); /* GL_MODELVIEW is default */

	glScalef(1.0 / (x2 - x1), 1.0 / (y2 - y1), 1.0);
	//������ ���ϱ�
	glTranslatef(0, 0, 0.0);
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, func(90));
	glVertex2f(1000, func(90));
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(1, func(0));
	glVertex2f(1, func(180));
	glEnd();

	m.lock();

	// ������� ��
	for (deque<int>::iterator iter = dqx.begin(); iter != dqx.end(); ++iter)
	{

		glBegin(GL_LINE_STRIP);
		glVertex2f(pret, func(prex)); //pre
		glVertex2f(i, func(90 + *iter)); //90+@  next -> pre
		glEnd();
		pret = i;
		i += 10;
		prex = 90 + *iter;
		glFlush();
	}


	glPopMatrix();
	m.unlock();
	Sleep(500);

	prex = 90;
	pret = 0;
	i = 1;

	///////////////////////////////////Viewport 3 end

};





/* Redrawing func */
void redraw(void)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	draw(func, 0, 1000, 0, 180);

	glutSwapBuffers();
}

/* Idle proc. Redisplays, if called. */
void idle(void)
{
	glutPostRedisplay();
}

/* Key press processing */
void key(unsigned char c, int x, int y)
{
	if (c == 27) exit(0);
}

/* Window reashape */

void reshape(int w, int h)
{


	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}