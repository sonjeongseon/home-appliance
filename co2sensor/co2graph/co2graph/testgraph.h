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
deque<int> dqx; // int 형 deque 선언
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




void getData(void)	//통신으로 받는 데이터 등등... 여기서는 x , y ,z 를(y축) 받을 것... x축은 t시간(셈플링)
{
	while (1) {

		m.lock(); // thread는 lock~ulock 까지 mutex 소유
		/*dqx.push_back((-90) + rand() % 180); // push_back : 뒤 원소 추가
		dqy.push_back((-90) + rand() % 180);
		dqz.push_back((-90) + rand() % 180); */

		CURL* curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, "http://hanbatcom.systems:3001/newEnv");// 접속 url저장, curl_init으로 세션 초기화할때 지정도 가능

		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);//애플리케이션이 호스트 이름확인시 사용할 IP주소 종류

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//함수가 실행될 수 있는 최대 시간 초단위

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);// TRUE -> 서버에서 location:header 응답시 그 주소로 다시 요청 시도, 재귀적

		int httpCode(0);

		std::stringstream httpData;

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);//callback함수 (cUrl resource, 데이터 문자열)

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &httpData);

		curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);//마지막 응답 코드 받기
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
			dqx.pop_front(); // pop_front : 앞 원소 제거
			dqy.pop_front();
			dqz.pop_front();
		}

		m.unlock();
		Sleep(7000);
	}
}


/* Sample func itself 중심선 긋기 용*/
float func(float x)
{
	return x;
}

/* Function plotting func */
void draw(float (*func)(float x), float x1, float x2, float y1, float y2)
{

	glViewport(0, 0, 1000, 100);	//왼쪽하단 1, 2, 넓이, 높이
	glMatrixMode(GL_PROJECTION); // 곱해서 화면에 보일 모습
	glLoadIdentity(); // 단위행렬 초기화
	glOrtho(0, 1, 0, 1, -1, 1); // 뷰포트 변경시 왜곡 현상 막음 왼오아위 가까운깊이 먼깊이
	glMatrixMode(GL_MODELVIEW); // 실제 위치
	glLoadIdentity(); // 초기화 2

	glPushMatrix(); /* GL_MODELVIEW is default */


	glScalef(1.0 / (x2 - x1), 1.0 / (y2 - y1), 1.0); // 현재 행렬에 배율 곱함
	//비율로 들어감...
	glTranslatef(0, 0, 0.0);
	glColor3f(1.0, 0.0, 1.0);//객체 색 지정

	glBegin(GL_LINE_STRIP);//점 연결해서 그리기
	glVertex2f(0, func(90));// 꼭짓점 지정 x y
	glVertex2f(1000, func(90));
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(1, func(0));
	glVertex2f(1, func(180));
	glEnd();

	m.lock();	//데이터를 그릴때 getData에서 데이터를 수집하는걸 방지하기 위해 수집되면 에러
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


	glViewport(0, 100, 1000, 100); //100부터 ~ +100개
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix(); /* GL_MODELVIEW is default */

	glScalef(1.0 / (x2 - x1), 1.0 / (y2 - y1), 1.0);
	//비율로 들어가니깐...
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
	//비율로 들어가니까
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

	// 여기까지 끝
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